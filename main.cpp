#include <SFML/Graphics.hpp>

#include <cstdlib>
#include <utility>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

constexpr unsigned WINDOW_WIDTH = 600;
constexpr unsigned WINDOW_HEIGHT = 680;
constexpr unsigned FRAME_LIMIT = 60;
constexpr int ROWS = 8;
constexpr int COLS = 8;
constexpr float CELL_SIZE = 70.f;
constexpr float BOARD_X = 20.f;
constexpr float BOARD_Y = 100.f;
constexpr int MENU = 0;
constexpr int PLAYING = 1;

// Functions Prototypes(2630)
bool loadLevel(int grid[ROWS][COLS], int &movesLeft, int &target, std::string &errorMessage, const std::filesystem::path &levelPath);

struct InputEvent
{
    bool isMouseClick = false;
    int mouseX = 0;
    int mouseY = 0;
    sf::Keyboard::Key key = sf::Keyboard::Key::Unknown;
};
bool saveGame(const std::filesystem::path &savePath,
              const int grid[ROWS][COLS],
              int score, int movesLeft, int target);

bool loadGame(const std::filesystem::path &savePath,
              int grid[ROWS][COLS],
              int &score, int &movesLeft, int &target);
int calculateScore(int currentScore, int candiesCleared);

// Collect raw events. Mouse coordinates are in window pixels.
void handleInput(sf::RenderWindow &window, std::vector<InputEvent> &input)
{
    input.clear();

    while (const auto event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            window.close();
            return;
        }

        if (const auto *mouse = event->getIf<sf::Event::MouseButtonPressed>())
        {
            if (mouse->button == sf::Mouse::Button::Left)
                input.push_back({true, mouse->position.x, mouse->position.y});
        }
        else if (const auto *keyboard = event->getIf<sf::Event::KeyPressed>())
        {
            input.push_back({false, 0, 0, keyboard->code});
        }
    }
}

bool cellFromClick(int mouseX, int mouseY, int &row, int &col)
{
    if (mouseX < 20 || mouseX >= 580 || mouseY < 100 || mouseY >= 660)
    {
        return false;
    }

    row = (mouseY - 100) / 70;
    col = (mouseX - 20) / 70;

    return true;
}

void markHorizontalMatches(const int grid[ROWS][COLS], bool marked[ROWS][COLS])
{
    for (int row = 0; row < ROWS; row++)
    {
        for (int col = 0; col < COLS; col++)
        {
            marked[row][col] = false;
        }
    }

    for (int row = 0; row < ROWS; row++)
    {
        for (int col = 0; col + 2 < COLS; col++)
        {
            int candy = grid[row][col];

            if (candy != 0 && candy == grid[row][col + 1] && candy == grid[row][col + 2])
            {
                marked[row][col] = true;
                marked[row][col + 1] = true;
                marked[row][col + 2] = true;
            }
        }
    }
}

void markVerticalMatches(const int grid[ROWS][COLS], bool marked[ROWS][COLS])
{
    for (int col = 0; col < COLS; ++col)
    {
        for (int row = 0; row + 2 < ROWS; ++row)
        {
            const int candy = grid[row][col];

            if (candy != 0 &&
                candy == grid[row + 1][col] &&
                candy == grid[row + 2][col])
            {
                marked[row][col] = true;
                marked[row + 1][col] = true;
                marked[row + 2][col] = true;
            }
        }
    }
}

bool isAdjacent(int r1, int c1, int r2, int c2)
{
    return std::abs(r1 - r2) + std::abs(c1 - c2) == 1;
}

bool anyMarked(const bool marked[ROWS][COLS])
{
    for (int r = 0; r < ROWS; ++r)
        for (int c = 0; c < COLS; ++c)
            if (marked[r][c])
                return true;
    return false;
}

bool trySwapCandies(int grid[ROWS][COLS], int r1, int c1, int r2, int c2)
{
    if (!isAdjacent(r1, c1, r2, c2))
        return false;
    if (grid[r1][c1] == 0 || grid[r2][c2] == 0)
        return false;

    std::swap(grid[r1][c1], grid[r2][c2]);

    bool marked[ROWS][COLS]{};
    markHorizontalMatches(grid, marked);
    bool found = anyMarked(marked);

    markVerticalMatches(grid, marked);
    if (anyMarked(marked))
        found = true;

    if (!found)
        std::swap(grid[r1][c1], grid[r2][c2]);

    return found;
}

int clearMarked(int grid[ROWS][COLS], const bool marked[ROWS][COLS])
{
    int candiesCleared = 0;

    for (int row = 0; row < ROWS; row++)
    {
        for (int col = 0; col < COLS; col++)
        {
            if (marked[row][col] && grid[row][col] != 0)
            {
                grid[row][col] = 0;
                candiesCleared++;
            }
        }
    }

    return candiesCleared;
}

void updateGame(const std::vector<InputEvent> &input, int &selectedMenuOption, sf::RenderWindow &window, int &gameState, int grid[ROWS][COLS], int &movesLeft,
                int &target, std::string &errorMessage, const std::filesystem::path &levelPath, int &selectedRow, int &selectedCol, int &score, const std::filesystem::path &savePath)
{
    for (const InputEvent &event : input)
    {
        if (gameState == PLAYING)
        {
            if (event.key == sf::Keyboard::Key::Escape)
            {
                if (saveGame(savePath, grid, score, movesLeft, target))
                {
                    errorMessage.clear();
                    selectedMenuOption = 1;
                    std::cout << "Saved to: " << savePath.string() << std::endl;
                    gameState = MENU;
                }
                else
                {
                    std::cerr << "Could not save the game.\n";
                }
                return;
            }
            if (event.isMouseClick)
            {
                int row = 0;
                int col = 0;

                if (cellFromClick(event.mouseX, event.mouseY, row, col))
                {
                    if (selectedRow == -1) // first click
                    {
                        selectedRow = row;
                        selectedCol = col;
                    }
                    else if (isAdjacent(selectedRow, selectedCol, row, col)) // second click, neighbour
                    {
                        if (trySwapCandies(grid, selectedRow, selectedCol, row, col))
                            movesLeft--; // spend a move only on a kept swap
                        selectedRow = -1;
                        selectedCol = -1;
                    }
                    else // not a neighbour: move the selection
                    {
                        selectedRow = row;
                        selectedCol = col;
                    }
                }
            }

            continue;
        }

        if (event.key == sf::Keyboard::Key::Down && selectedMenuOption < 2)
        {
            ++selectedMenuOption;
        }
        else if (event.key == sf::Keyboard::Key::Up && selectedMenuOption > 0)
        {
            --selectedMenuOption;
        }
        else if (event.key == sf::Keyboard::Key::Enter && selectedMenuOption == 2)
        {
            window.close();
            return;
        }
        else if (event.key == sf::Keyboard::Key::Enter && selectedMenuOption == 0)
        {
            errorMessage.clear();
            if (loadLevel(grid, movesLeft, target, errorMessage, levelPath))
            {
                gameState = PLAYING;
                selectedRow = -1;
                selectedCol = -1;
                score = 0;
            }
        }
        else if (event.key == sf::Keyboard::Key::Enter && selectedMenuOption == 1)
        {
            errorMessage.clear();
            if (loadGame(savePath, grid, score, movesLeft, target))
            {
                selectedRow = -1;
                selectedCol = -1;
                gameState = PLAYING;
                return;
            }

            errorMessage = "Could not load savegame.txt";
        }
    }
}

void drawText(sf::RenderWindow &window, const sf::Font &font,
              const std::string &words, unsigned size, float x, float y)
{
    sf::Text text(font, words, size);
    text.setPosition({x, y});
    text.setFillColor(sf::Color(237, 246, 240));
    window.draw(text);
}

void drawHud(sf::RenderWindow &window, const sf::Font &font,
             const std::string &score, const std::string &moves, const std::string &target)
{
    sf::RectangleShape strip({static_cast<float>(WINDOW_WIDTH), 80.f});
    strip.setFillColor(sf::Color(23, 45, 50));
    window.draw(strip);

    drawText(window, font, "Score", 15, 20.f, 9.f);
    drawText(window, font, "Moves left", 15, 220.f, 9.f);
    drawText(window, font, "Target", 15, 420.f, 9.f);
    drawText(window, font, score, 25, 20.f, 30.f);
    drawText(window, font, moves, 25, 220.f, 30.f);
    drawText(window, font, target, 25, 420.f, 30.f);
}

sf::Vector2f cellPosition(int row, int col)
{
    return {BOARD_X + col * CELL_SIZE, BOARD_Y + row * CELL_SIZE};
}

void drawGrid(sf::RenderWindow &window)
{
    sf::RectangleShape tile({CELL_SIZE - 4.f, CELL_SIZE - 4.f});
    tile.setFillColor(sf::Color(23, 45, 50));
    tile.setOutlineColor(sf::Color(49, 79, 81));
    tile.setOutlineThickness(1.f);

    for (int row = 0; row < ROWS; ++row)
    {
        for (int col = 0; col < COLS; ++col)
        {
            tile.setPosition(cellPosition(row, col) + sf::Vector2f(2.f, 2.f));
            window.draw(tile);
        }
    }
}

// Draw one supplied candy. The caller decides which cells to highlight.
void drawCandy(sf::RenderWindow &window, int row, int col, int candyId, bool highlighted)
{
    const sf::Color palette[] = {
        sf::Color::Transparent, sf::Color(255, 106, 121), sf::Color(244, 157, 94),
        sf::Color(247, 210, 92), sf::Color(107, 220, 162), sf::Color(106, 183, 247),
        sf::Color(184, 150, 246)};
    if (candyId < 1 || candyId > 6)
        return;

    constexpr float radius = 25.f;
    constexpr float inset = CELL_SIZE / 2.f - radius;
    sf::CircleShape candy(radius);
    candy.setPosition(cellPosition(row, col) + sf::Vector2f(inset, inset));
    candy.setFillColor(palette[candyId]);
    if (highlighted)
    {
        candy.setOutlineColor(sf::Color::White);
        candy.setOutlineThickness(3.f);
    }
    window.draw(candy);
}

void drawMenu(sf::RenderWindow &window, const sf::Font &font, int selectedMenuOption)
{
    drawText(window, font, "Candy Crush", 42, 160.f, 160.f);
    drawText(window, font, selectedMenuOption == 0 ? "> New Game" : "  New Game", 24, 200.f, 260.f);
    drawText(window, font, selectedMenuOption == 1 ? "> Continue" : "  Continue", 24, 200.f, 310.f);
    drawText(window, font, selectedMenuOption == 2 ? "> Exit" : "  Exit", 24, 200.f, 360.f);
}

// Draw the entire board. The caller decides which cells to highlight.
void drawBoard(sf::RenderWindow &window, int grid[ROWS][COLS], int selectedRow, int selectedCol)
{
    for (int r = 0; r < ROWS; ++r)
        for (int c = 0; c < COLS; ++c)
            drawCandy(window, r, c, grid[r][c], r == selectedRow && c == selectedCol);
}

void drawGame(sf::RenderWindow &window, const sf::Font &font, int selectedMenuOption, int gameState, int grid[ROWS][COLS], int movesLeft,
              int target, const std::string &errorMessage, int selectedRow, int selectedCol, int score)
{
    window.clear(sf::Color(12, 27, 32));
    // drawHud(window, font, "-", "-", "-");
    // drawGrid(window);

    // Static graphics preview. Replace this with calls from your board drawing code.
    // for (int col = 0; col < 6; ++col)
    //  drawCandy(window, 0, col, col + 1, col == 0);

    // drawText(window, font, "SFML graphics preview", 13, 20.f, 661.f);
    switch (gameState)
    {
    case MENU:
        drawMenu(window, font, selectedMenuOption);
        if (!errorMessage.empty())
        {
            drawText(window, font, errorMessage, 18, 140.f, 420.f);
        }
        break;

    case PLAYING:
        drawHud(window, font, std::to_string(score), std::to_string(movesLeft),
                std::to_string(target));
        drawGrid(window);
        drawBoard(window, grid, selectedRow, selectedCol);
        break;
    }
    window.display();
}

bool loadLevel(int grid[ROWS][COLS], int &movesLeft, int &target, std::string &errorMessage, const std::filesystem::path &levelPath)
{
    std::ifstream file(levelPath);
    if (!file.is_open())
    {
        errorMessage = "Could not open level.txt";
        return false;
    }

    int rows, cols;
    file >> rows >> cols;        // 8 8
    file >> movesLeft >> target; // 20 800

    if (!file || rows != ROWS || cols != COLS)
    {
        errorMessage = "level.txt has a bad header or wrong board size";
        return false;
    }

    for (int r = 0; r < ROWS; ++r)
        for (int c = 0; c < COLS; ++c)
            file >> grid[r][c];

    if (!file)
    {
        errorMessage = "level.txt is missing candy values";
        return false;
    }

    return true;
}

bool saveGame(const std::filesystem::path &savePath, const int grid[ROWS][COLS], int score, int movesLeft, int target)
{
    std::ofstream file(savePath);
    if (!file)
    {
        return false;
    }

    file << score << ' ' << movesLeft << ' ' << target << '\n';

    for (int row = 0; row < ROWS; ++row)
    {
        for (int col = 0; col < COLS; ++col)
        {
            file << grid[row][col] << ' ';
        }
        file << '\n';
    }

    file.close();
    return !file.fail();
}

bool loadGame(const std::filesystem::path &savePath,
              int grid[ROWS][COLS],
              int &score, int &movesLeft, int &target)
{
    std::ifstream file(savePath);
    if (!file)
    {
        return false;
    }

    int loadedScore = 0;
    int loadedMoves = 0;
    int loadedTarget = 0;

    if (!(file >> loadedScore >> loadedMoves >> loadedTarget))
    {
        return false;
    }

    int loadedGrid[ROWS][COLS]{};

    for (int row = 0; row < ROWS; ++row)
    {
        for (int col = 0; col < COLS; ++col)
        {
            int candy = 0;
            if (!(file >> candy) || candy < 1 || candy > 6)
            {
                return false;
            }

            loadedGrid[row][col] = candy;
        }
    }

    for (int row = 0; row < ROWS; ++row)
    {
        for (int col = 0; col < COLS; ++col)
        {
            grid[row][col] = loadedGrid[row][col];
        }
    }

    score = loadedScore;
    movesLeft = loadedMoves;
    target = loadedTarget;

    return true;
}

int main(int argc, char *argv[])
{
    int selectedRow = -1;
    int selectedCol = -1;

    // CMake copies the font next to the executable.
    const auto folder = std::filesystem::absolute(argc > 0 ? argv[0] : "CandyCrushStarter").parent_path();
    sf::Font font;
    if (!font.openFromFile(folder / "assets" / "DejaVuSans.ttf"))
    {
        std::cerr << "Could not open assets/DejaVuSans.ttf." << std::endl;
        return 1;
    }

    sf::RenderWindow window(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "Candy Crush",
                            sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(FRAME_LIMIT);
    window.setKeyRepeatEnabled(false);

    std::vector<InputEvent> input;
    int selectedMenuOption = 0;
    int gameState = MENU;
    int grid[ROWS][COLS]{};
    int movesLeft = 0;
    int score = 0;
    int target = 0;
    std::string errorMessage;
    while (window.isOpen())
    {
        handleInput(window, input);
        if (!window.isOpen())
            break;

        updateGame(input, selectedMenuOption, window, gameState, grid, movesLeft, target, errorMessage, folder / "level.txt", selectedRow, selectedCol,
                   score, folder / "savegame.txt");

        if (!window.isOpen())
        {
            break;
        }

        drawGame(window, font, selectedMenuOption, gameState, grid, movesLeft, target, errorMessage,
                 selectedRow, selectedCol, score);
    }
}
