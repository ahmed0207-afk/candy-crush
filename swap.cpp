#include <cstdlib>
using namespace std;
constexpr int ROWS = 8;
constexpr int COLS = 8;
static int countSame(int grid[ROWS][COLS], int r, int c, int dr, int dc)
{
    int id = grid[r][c];
    int count = 0;
    r += dr;
    c += dc;
    while (r >= 0 && r < ROWS && c >= 0 && c < COLS && grid[r][c] == id)
    {
        count++;
        r += dr;
        c += dc;
    }
    return count;
}

// True if the candy at (r, c) is part of a horizontal or vertical run of 3+.
static bool createsMatchAt(int grid[ROWS][COLS], int r, int c)
{
    if (grid[r][c] < 1)
        return false;

    int horizontal = 1 + countSame(grid, r, c, 0, 1) + countSame(grid, r, c, 0, -1);
    int vertical = 1 + countSame(grid, r, c, 1, 0) + countSame(grid, r, c, -1, 0);
    return horizontal >= 3 || vertical >= 3;
}

// True if the two cells are direct neighbours (up, down, left, right).
static bool isAdjacent(int r1, int c1, int r2, int c2)
{
    return abs(r1 - r2) + abs(c1 - c2) == 1;
}

// Exchanges two candies in the grid.
static void swapCandies(int grid[ROWS][COLS], int r1, int c1, int r2, int c2)
{
    int temp = grid[r1][c1];
    grid[r1][c1] = grid[r2][c2];
    grid[r2][c2] = temp;
}

// Swaps two cells only if the move is legal and creates a match of 3+.
// If there is no match, the swap is undone and false is returned.
static bool trySwap(int grid[ROWS][COLS], int r1, int c1, int r2, int c2)
{
    // stay inside the board
    if (r1 < 0 || r1 >= ROWS || c1 < 0 || c1 >= COLS ||
        r2 < 0 || r2 >= ROWS || c2 < 0 || c2 >= COLS)
        return false;

    if (!isAdjacent(r1, c1, r2, c2))
        return false;

    // do not swap with empty cells
    if (grid[r1][c1] < 1 || grid[r2][c2] < 1)
        return false;

    swapCandies(grid, r1, c1, r2, c2);

    if (createsMatchAt(grid, r1, c1) || createsMatchAt(grid, r2, c2))
        return true; // valid move, keep the swap

    swapCandies(grid, r1, c1, r2, c2); // no match, undo it
    return false;
}

// Handles one click on a board cell (select, deselect, or swap).
// selR / selC hold the first selected cell (-1 = nothing selected).
// Returns true only when a valid swap was performed.
bool handleCellClick(int grid[ROWS][COLS], int row, int col, int &selR, int &selC)
{
    if (row < 0 || row >= ROWS || col < 0 || col >= COLS)
        return false;

    if (selR == -1) // first click: select
    {
        selR = row;
        selC = col;
        return false;
    }

    if (selR == row && selC == col) // same cell: deselect
    {
        selR = -1;
        selC = -1;
        return false;
    }

    if (!isAdjacent(selR, selC, row, col)) // far away: select the new cell
    {
        selR = row;
        selC = col;
        return false;
    }

    bool swapped = trySwap(grid, selR, selC, row, col);
    selR = -1;
    selC = -1; // selection ends either way
    return swapped;
}