#include <SFML/Graphics.hpp>

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

// Functions Prototypes(2630)
bool loadLevel(int grid[ROWS][COLS], int &movesLeft, int &target, std::string &errorMessage);

struct InputEvent
{
    bool isMouseClick = false;
    int mouseX = 0;
    int mouseY = 0;
    sf::Keyboard::Key key = sf::Keyboard::Key::Unknown;
};

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

void updateGame(const std::vector<InputEvent> & /* input */)
{
    // Team TODO: Add your own game update code here.
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

void drawGame(sf::RenderWindow &window, const sf::Font &font)
{
    window.clear(sf::Color(12, 27, 32));
    drawHud(window, font, "-", "-", "-");
    drawGrid(window);

    // Static graphics preview. Replace this with calls from your board drawing code.
    for (int col = 0; col < 6; ++col)
        drawCandy(window, 0, col, col + 1, col == 0);

    drawText(window, font, "SFML graphics preview", 13, 20.f, 661.f);
    window.display();
}

bool loadLevel(int grid[ROWS][COLS], int &movesLeft, int &target, std::string &errorMessage)
{
    std::ifstream file("level.txt");
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

int main(int argc, char *argv[])
{
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
    while (window.isOpen())
    {
        handleInput(window, input);
        if (!window.isOpen())
            break;

        updateGame(input);
        drawGame(window, font);
    }
}
