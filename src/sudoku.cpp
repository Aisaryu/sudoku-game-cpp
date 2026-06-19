#include "sudoku.h"
#include <algorithm>
#include <numeric>

// ──────────────────────────────────────────────
// Constructor
// ──────────────────────────────────────────────
Sudoku::Sudoku()
    : rng_(std::random_device{}())
{
    for (auto& row : grid_)      row.fill(EMPTY_CELL);
    for (auto& row : givenMask_) row.fill(0);
    for (auto& row : solution_)  row.fill(EMPTY_CELL);
}

// ──────────────────────────────────────────────
// Solver (backtracking)
// ──────────────────────────────────────────────
bool Sudoku::solve(Grid& g) const {
    for (int row = 0; row < GRID_SIZE; ++row) {
        for (int col = 0; col < GRID_SIZE; ++col) {
            if (g[row][col] == EMPTY_CELL) {
                for (int num = 1; num <= GRID_SIZE; ++num) {
                    // Check row
                    bool ok = true;
                    for (int i = 0; i < GRID_SIZE && ok; ++i)
                        if (g[row][i] == num) ok = false;
                    // Check col
                    for (int i = 0; i < GRID_SIZE && ok; ++i)
                        if (g[i][col] == num) ok = false;
                    // Check box
                    int br = (row / BOX_SIZE) * BOX_SIZE;
                    int bc = (col / BOX_SIZE) * BOX_SIZE;
                    for (int i = 0; i < BOX_SIZE && ok; ++i)
                        for (int j = 0; j < BOX_SIZE && ok; ++j)
                            if (g[br + i][bc + j] == num) ok = false;

                    if (ok) {
                        g[row][col] = num;
                        if (solve(g)) return true;
                        g[row][col] = EMPTY_CELL;
                    }
                }
                return false;
            }
        }
    }
    return true;
}

// ──────────────────────────────────────────────
// Shuffle a solved grid to create variety
// ──────────────────────────────────────────────
void Sudoku::shuffleGrid(Grid& g) {
    // Shuffle numbers (relabel 1-9 randomly)
    std::array<int, GRID_SIZE> perm;
    std::iota(perm.begin(), perm.end(), 1);
    std::shuffle(perm.begin(), perm.end(), rng_);

    for (auto& row : g)
        for (auto& cell : row)
            if (cell != EMPTY_CELL)
                cell = perm[cell - 1];

    // Shuffle rows within each band (3 bands of 3 rows)
    for (int band = 0; band < BOX_SIZE; ++band) {
        std::array<int, BOX_SIZE> idx = {0, 1, 2};
        std::shuffle(idx.begin(), idx.end(), rng_);
        Grid tmp = g;
        for (int i = 0; i < BOX_SIZE; ++i)
            g[band * BOX_SIZE + i] = tmp[band * BOX_SIZE + idx[i]];
    }

    // Shuffle columns within each stack (3 stacks of 3 cols)
    for (int stack = 0; stack < BOX_SIZE; ++stack) {
        std::array<int, BOX_SIZE> idx = {0, 1, 2};
        std::shuffle(idx.begin(), idx.end(), rng_);
        Grid tmp = g;
        for (int row = 0; row < GRID_SIZE; ++row)
            for (int i = 0; i < BOX_SIZE; ++i)
                g[row][stack * BOX_SIZE + i] = tmp[row][stack * BOX_SIZE + idx[i]];
    }

    // Shuffle bands (groups of 3 rows)
    {
        std::array<int, BOX_SIZE> idx = {0, 1, 2};
        std::shuffle(idx.begin(), idx.end(), rng_);
        Grid tmp = g;
        for (int b = 0; b < BOX_SIZE; ++b)
            for (int r = 0; r < BOX_SIZE; ++r)
                g[b * BOX_SIZE + r] = tmp[idx[b] * BOX_SIZE + r];
    }

    // Shuffle stacks (groups of 3 cols)
    {
        std::array<int, BOX_SIZE> idx = {0, 1, 2};
        std::shuffle(idx.begin(), idx.end(), rng_);
        Grid tmp = g;
        for (int row = 0; row < GRID_SIZE; ++row)
            for (int s = 0; s < BOX_SIZE; ++s)
                for (int c = 0; c < BOX_SIZE; ++c)
                    g[row][s * BOX_SIZE + c] = tmp[row][idx[s] * BOX_SIZE + c];
    }
}

// ──────────────────────────────────────────────
// Generate a new puzzle
// ──────────────────────────────────────────────
void Sudoku::generate(Difficulty diff) {
    // Clear everything
    for (auto& row : grid_)      row.fill(EMPTY_CELL);
    for (auto& row : givenMask_) row.fill(0);
    while (!undoStack_.empty()) undoStack_.pop();

    // Solve an empty grid to get a valid complete grid
    solve(grid_);

    // Shuffle to add randomness
    shuffleGrid(grid_);

    // Store the solution
    solution_ = grid_;

    // Determine how many cells to remove
    int toRemove = 0;
    switch (diff) {
        case Difficulty::Easy:   toRemove = 40; break;
        case Difficulty::Medium: toRemove = 50; break;
        case Difficulty::Hard:   toRemove = 58; break;
    }

    // Remove cells randomly
    std::vector<int> positions(GRID_SIZE * GRID_SIZE);
    std::iota(positions.begin(), positions.end(), 0);
    std::shuffle(positions.begin(), positions.end(), rng_);

    int removed = 0;
    for (int pos : positions) {
        if (removed >= toRemove) break;
        int r = pos / GRID_SIZE;
        int c = pos % GRID_SIZE;
        grid_[r][c] = EMPTY_CELL;
        removed++;
    }

    // Mark remaining cells as given
    for (int r = 0; r < GRID_SIZE; ++r)
        for (int c = 0; c < GRID_SIZE; ++c)
            givenMask_[r][c] = (grid_[r][c] != EMPTY_CELL) ? 1 : 0;
}

// ──────────────────────────────────────────────
// Cell access
// ──────────────────────────────────────────────
int Sudoku::getCell(int row, int col) const {
    return grid_[row][col];
}

bool Sudoku::isGiven(int row, int col) const {
    return givenMask_[row][col] == 1;
}

bool Sudoku::setCell(int row, int col, int value) {
    if (isGiven(row, col)) return false;
    Move m{row, col, grid_[row][col], value};
    undoStack_.push(m);
    grid_[row][col] = value;
    return true;
}

bool Sudoku::clearCell(int row, int col) {
    if (isGiven(row, col)) return false;
    if (grid_[row][col] == EMPTY_CELL) return false;
    Move m{row, col, grid_[row][col], EMPTY_CELL};
    undoStack_.push(m);
    grid_[row][col] = EMPTY_CELL;
    return true;
}

// ──────────────────────────────────────────────
// Validation
// ──────────────────────────────────────────────
bool Sudoku::canPlace(int row, int col, int num) const {
    for (int i = 0; i < GRID_SIZE; ++i) {
        if (i != col && grid_[row][i] == num) return false;
        if (i != row && grid_[i][col] == num) return false;
    }
    int br = (row / BOX_SIZE) * BOX_SIZE;
    int bc = (col / BOX_SIZE) * BOX_SIZE;
    for (int i = 0; i < BOX_SIZE; ++i)
        for (int j = 0; j < BOX_SIZE; ++j)
            if ((br + i != row || bc + j != col) && grid_[br + i][bc + j] == num)
                return false;
    return true;
}

bool Sudoku::hasConflict(int row, int col) const {
    int val = grid_[row][col];
    if (val == EMPTY_CELL) return false;
    return !canPlace(row, col, val);
}

// ──────────────────────────────────────────────
// Hint — use the stored solution
// ──────────────────────────────────────────────
bool Sudoku::getHint(int& hintRow, int& hintCol, int& hintValue) const {
    // Find the first empty cell and reveal its solution
    for (int r = 0; r < GRID_SIZE; ++r) {
        for (int c = 0; c < GRID_SIZE; ++c) {
            if (grid_[r][c] == EMPTY_CELL) {
                hintRow = r;
                hintCol = c;
                hintValue = solution_[r][c];
                return true;
            }
        }
    }
    return false;
}

// ──────────────────────────────────────────────
// Undo
// ──────────────────────────────────────────────
void Sudoku::pushMove(const Move& m) {
    undoStack_.push(m);
}

bool Sudoku::undo() {
    if (undoStack_.empty()) return false;
    Move m = undoStack_.top();
    undoStack_.pop();
    grid_[m.row][m.col] = m.prevValue;
    return true;
}

// ──────────────────────────────────────────────
// Check if puzzle is solved
// ──────────────────────────────────────────────
bool Sudoku::isSolved() const {
    for (int r = 0; r < GRID_SIZE; ++r)
        for (int c = 0; c < GRID_SIZE; ++c)
            if (grid_[r][c] == EMPTY_CELL || hasConflict(r, c))
                return false;
    return true;
}
