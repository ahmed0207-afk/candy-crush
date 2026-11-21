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
