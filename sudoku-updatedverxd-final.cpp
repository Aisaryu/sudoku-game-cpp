#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <stack>

const int SIZE = 9;
const int EMPTY = 0;
const double TIME_LIMIT = 900.0;  // Time limit for the game in seconds

enum Difficulty {
    EASY,
    MEDIUM,
    HARD
};

// Function to print the Sudoku grid
void printGrid(const std::vector<std::vector<int>>& grid) {
    for (int i = 0; i < SIZE; ++i) {
        for (int j = 0; j < SIZE; ++j) {
            if (grid[i][j] == EMPTY) {
                std::cout << "  "; // Print empty cells as spaces
            } else {
                std::cout << grid[i][j] << " ";
            }
            if ((j + 1) % 3 == 0 && j != SIZE - 1) {
                std::cout << "| ";
            }
        }
        std::cout << std::endl;
        if ((i + 1) % 3 == 0 && i != SIZE - 1) {
            std::cout << "------+-------+------" << std::endl;
        }
    }
}

// Function to check if a number can be placed at a given position
bool canPlace(const std::vector<std::vector<int>>& grid, int row, int col, int num) {
    // Check if the number exists in the current row or column
    for (int i = 0; i < SIZE; ++i) {
        if (grid[row][i] == num || grid[i][col] == num) {
            return false;
        }
    }

    // Check if the number exists in the current 3x3 box
    int startRow = (row / 3) * 3;
    int startCol = (col / 3) * 3;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (grid[startRow + i][startCol + j] == num) {
                return false;
            }
        }
    }

    return true;
}

// Function to solve the Sudoku puzzle using backtracking
bool solveSudoku(std::vector<std::vector<int>>& grid) {
    for (int row = 0; row < SIZE; ++row) {
        for (int col = 0; col < SIZE; ++col) {
            if (grid[row][col] == EMPTY) {
                for (int num = 1; num <= SIZE; ++num) {
                    if (canPlace(grid, row, col, num)) {
                        grid[row][col] = num;
                        if (solveSudoku(grid)) {
                            return true;
                        }
                        grid[row][col] = EMPTY;  // Undo the choice if it leads to a dead end
                    }
                }
                return false;  // No number can be placed at this position
            }
        }
    }
    return true;  // The grid has been filled completely
}

// Function to generate a random Sudoku puzzle
void generateSudoku(std::vector<std::vector<int>>& grid, Difficulty difficulty) {
    std::srand(std::time(0));  // Seed the random number generator

    // Clear the grid
    grid.clear();
    for (int i = 0; i < SIZE; ++i) {
        grid.push_back(std::vector<int>(SIZE, EMPTY));
    }

    solveSudoku(grid);

    // Remove some numbers to create a puzzle based on the difficulty
    int numToRemove = 0;
    switch (difficulty) {
        case EASY:
            numToRemove = 40;
            break;
        case MEDIUM:
            numToRemove = 50;
            break;
        case HARD:
            numToRemove = 60;
            break;
    }

    while (numToRemove > 0) {
        int row = std::rand() % SIZE;
        int col = std::rand() % SIZE;
        if (grid[row][col] != EMPTY) {
            grid[row][col] = EMPTY;
            numToRemove--;
        }
    }
}

// Function to get user input for a move
void getUserMove(int& row, int& col, int& num, bool& hintRequested) {
    std::cout << "Enter row, column, and number (1-9) separated by spaces, or enter 'h' for a hint, or enter 'u' to undo: ";
    char input;
    std::cin >> input;

    if (input == 'h' || input == 'H') {
        hintRequested = true;
        return;
    }

    if (input == 'u' || input == 'U') {
        row = -1; // Undo move flag
        return;
    }

    row = input - '0';  // Convert character input to integer
    std::cin >> col >> num;
    row--;  // Adjust for 0-based indexing
    col--;
}

// Function to check if the Sudoku puzzle has been solved
bool isSudokuSolved(const std::vector<std::vector<int>>& grid) {
    for (int row = 0; row < SIZE; ++row) {
        for (int col = 0; col < SIZE; ++col) {
            if (grid[row][col] == EMPTY) {
                return false;
            }
        }
    }
    return true;
}

// Function to display a fancy introduction
void displayIntroduction() {
    std::cout << "+-----------------------------------------------------+" << std::endl;
    std::cout << "|                 *   Sudoku Game  *                  |" << std::endl;
    std::cout << "+-----------------------------------------------------+" << std::endl;
    std::cout << "|                 Welcome to Sudoku!                  |" << std::endl;
    std::cout << "|   Fill in the empty cells with numbers from 1 to 9. |" << std::endl;
    std::cout << "|                                                     |" << std::endl;
    std::cout << "|        Make sure that no row, column, or 3x3 box    |" << std::endl;
    std::cout << "|             contains the same number twice.         |" << std::endl;
    std::cout << "|                                                     |" << std::endl; 
    std::cout << "|     You have 15 Minutes to solve the sudoku puzzle. |" << std::endl;
    std::cout << "|                                                     |" << std::endl;
    std::cout << "|                 Good luck and have fun!             |" << std::endl;
    std::cout << "|                                                     |" << std::endl;
    std::cout << "|                                                     |" << std::endl;
    std::cout << "+-----------------------------------------------------+" << std::endl;
    std::cout << std::endl;
}

int main() {
    // Declare and initialize the grid
    std::vector<std::vector<int>> grid;
    Difficulty difficulty;

    // Display the introduction
    displayIntroduction();

    // Get user input for difficulty
    std::cout << "Select the difficulty level (0 = Easy, 1 = Medium, 2 = Hard): ";
    int level;
    std::cin >> level;

    switch (level) {
        case 0:
            difficulty = EASY;
            break;
        case 1:
            difficulty = MEDIUM;
            break;
        case 2:
            difficulty = HARD;
            break;
        default:
            std::cout << "Invalid difficulty level. Choosing Easy by default." << std::endl;
            difficulty = EASY;
            break;
    }

    // Generate the Sudoku puzzle
    generateSudoku(grid, difficulty);

    // Main game loop
    int moveCounter = 0;  // Move counter
    std::chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();
    std::vector<std::vector<int>> previousGrid = grid;
    while (true) {
        // Print the Sudoku grid
        std::cout << "Sudoku Puzzle:" << std::endl;
        printGrid(grid);

        // Calculate and print the elapsed time
        std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();
        std::chrono::duration<double> timeElapsed = std::chrono::duration_cast<std::chrono::duration<double>>(currentTime - startTime);
        std::cout << "Time Elapsed: " << timeElapsed.count() << " seconds" << std::endl;

        // Print the move counter
        std::cout << "Moves: " << moveCounter << std::endl;

        // Get user move or request a hint
        int row, col, num;
        bool hintRequested = false;
        getUserMove(row, col, num, hintRequested);

        // Check if the user requested a hint
        if (hintRequested) {
            // Find the first empty cell and provide a hint
            bool hintGiven = false;
            for (int i = 0; i < SIZE; ++i) {
                for (int j = 0; j < SIZE; ++j) {
                    if (grid[i][j] == EMPTY) {
                        // Check if a valid number can be placed in the empty cell
                        if (canPlace(grid, i, j, num)) {
                            hintGiven = true;
                            std::cout << "Hint: Try placing the number " << num << " in row " << (i + 1) << ", column " << (j + 1) << "." << std::endl;
                            break;
                        }
                    }
                }
                if (hintGiven) {
                    break;
                }
            }
            if (!hintGiven) {
                std::cout << "No valid hint available." << std::endl;
            }
            continue;
        }

        // Check if the user wants to undo the move
        if (row == -1) {
            grid = previousGrid; // Restore previous grid
            std::cout << "You undid your last move" << std::endl;
            continue;
        }

        // Check if the move is valid
        if (row >= 0 && row < SIZE && col >= 0 && col < SIZE && num >= 1 && num <= SIZE && grid[row][col] == EMPTY) {
            if (canPlace(grid, row, col, num)) {
                previousGrid = grid; // Store previous grid
                grid[row][col] = num;
                moveCounter++;  // Increment move counter

                // Check if the puzzle is solved
                if (isSudokuSolved(grid)) {
                    // Stop the timer
                    std::chrono::steady_clock::time_point endTime = std::chrono::steady_clock::now();
                    std::chrono::duration<double> totalTime = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime);
                    std::cout << "Congratulations! You solved the Sudoku puzzle." << std::endl;
                    std::cout << "Total time taken: " << totalTime.count() << " seconds" << std::endl;
                    break;
                }
            } else {
                std::cout << "Invalid move! Try again." << std::endl;
            }
        } else {
            std::cout << "Invalid move! Try again." << std::endl;
        }
    }

    return 0;
}

