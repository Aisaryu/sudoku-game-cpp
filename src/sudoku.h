#pragma once

#include <array>
#include <vector>
#include <stack>
#include <random>

// ──────────────────────────────────────────────
// Constants
// ──────────────────────────────────────────────
constexpr int GRID_SIZE  = 9;
constexpr int BOX_SIZE   = 3;
constexpr int EMPTY_CELL = 0;

// ──────────────────────────────────────────────
// Types
// ──────────────────────────────────────────────
using Grid = std::array<std::array<int, GRID_SIZE>, GRID_SIZE>;

enum class Difficulty { Easy, Medium, Hard };

// Represents a single move (for undo)
struct Move {
    int row;
    int col;
    int prevValue;
    int newValue;
};

// ──────────────────────────────────────────────
// Sudoku Engine
// ──────────────────────────────────────────────
class Sudoku {
public:
    Sudoku();

    // Puzzle lifecycle
    void generate(Difficulty diff);
    bool isSolved() const;

    // Cell access
    int  getCell(int row, int col) const;
    bool isGiven(int row, int col) const;
    bool setCell(int row, int col, int value);   // returns false if cell is a given
    bool clearCell(int row, int col);             // returns false if cell is a given

    // Validation
    bool canPlace(int row, int col, int num) const;
    bool hasConflict(int row, int col) const;

    // Hint: returns true if a hint was found, sets hintRow/hintCol/hintValue
    bool getHint(int& hintRow, int& hintCol, int& hintValue) const;

    // Undo
    void pushMove(const Move& m);
    bool undo();  // returns false if no moves to undo

    // Grid access
    const Grid& getGrid() const { return grid_; }
    const Grid& getGivenMask() const { return givenMask_; }

private:
    bool solve(Grid& g) const;
    void shuffleGrid(Grid& g);

    Grid grid_;
    Grid givenMask_;   // 1 = given (immutable), 0 = user-editable
    Grid solution_;    // the full solution (for hints)
    std::stack<Move> undoStack_;
    mutable std::mt19937 rng_;
};
