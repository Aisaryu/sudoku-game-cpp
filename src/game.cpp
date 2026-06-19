#include "game.h"
#include <cstdio>
#include <algorithm>

// ──────────────────────────────────────────────
// Constructor
// ──────────────────────────────────────────────
Game::Game()
    : screen_(GameScreen::Menu)
    , difficulty_(Difficulty::Easy)
    , selRow_(-1), selCol_(-1)
    , moveCount_(0)
    , timeLeft_(TIME_LIMIT)
    , totalTime_(0.0f)
    , hintActive_(false)
    , hintRow_(-1), hintCol_(-1)
    , hintTimer_(0.0f)
    , hintsUsed_(0), maxHints_(5)
    , hintCooldown_(0.0f)
    , streak_(0), bestStreak_(0)
    , score_(0)
    , mistakes_(0), maxMistakes_(-1)
    , menuHover_(-1)
{}

// ──────────────────────────────────────────────
// Update — called each frame
// ──────────────────────────────────────────────
void Game::update(float dt) {
    if (screen_ == GameScreen::Playing) {
        // Count down timer
        timeLeft_ -= dt;
        if (timeLeft_ <= 0.0f) {
            timeLeft_ = 0.0f;
            screen_ = GameScreen::TimeUp;
        }

        // Update hint flash
        if (hintActive_) {
            hintTimer_ -= dt;
            if (hintTimer_ <= 0.0f) {
                hintActive_ = false;
            }
        }

        // Update hint cooldown
        if (hintCooldown_ > 0.0f) {
            hintCooldown_ -= dt;
            if (hintCooldown_ < 0.0f) hintCooldown_ = 0.0f;
        }

        // Update floating texts
        for (auto& ft : floatingTexts_) {
            ft.life -= dt;
            ft.y -= dt * 30.0f;  // float upward
        }
        floatingTexts_.erase(
            std::remove_if(floatingTexts_.begin(), floatingTexts_.end(),
                [](const FloatingText& ft) { return ft.life <= 0.0f; }),
            floatingTexts_.end()
        );

        // Update completion flashes
        for (auto& cf : completionFlashes_) {
            cf.life -= dt;
        }
        completionFlashes_.erase(
            std::remove_if(completionFlashes_.begin(), completionFlashes_.end(),
                [](const CompletionFlash& cf) { return cf.life <= 0.0f; }),
            completionFlashes_.end()
        );
    }
}

// ──────────────────────────────────────────────
// Start a new game
// ──────────────────────────────────────────────
void Game::startGame(Difficulty diff) {
    difficulty_ = diff;
    sudoku_.generate(diff);
    screen_ = GameScreen::Playing;
    selRow_ = -1;
    selCol_ = -1;
    moveCount_ = 0;
    timeLeft_ = TIME_LIMIT;
    totalTime_ = 0.0f;
    hintActive_ = false;
    hintsUsed_ = 0;
    hintCooldown_ = 0.0f;
    streak_ = 0;
    bestStreak_ = 0;
    score_ = 0;
    mistakes_ = 0;
    floatingTexts_.clear();
    completionFlashes_.clear();

    // Set limits per difficulty
    switch (diff) {
        case Difficulty::Easy:
            maxHints_ = 5;
            maxMistakes_ = -1;   // unlimited (but still tracked)
            break;
        case Difficulty::Medium:
            maxHints_ = 3;
            maxMistakes_ = 5;
            break;
        case Difficulty::Hard:
            maxHints_ = 1;
            maxMistakes_ = 3;
            break;
    }
}

// ──────────────────────────────────────────────
// Reset to menu
// ──────────────────────────────────────────────
void Game::reset() {
    screen_ = GameScreen::Menu;
    menuHover_ = -1;
}

// ──────────────────────────────────────────────
// Cell selection
// ──────────────────────────────────────────────
void Game::selectCell(int row, int col) {
    if (row >= 0 && row < GRID_SIZE && col >= 0 && col < GRID_SIZE) {
        selRow_ = row;
        selCol_ = col;
    }
}

// ──────────────────────────────────────────────
// Place a number in the selected cell
// ──────────────────────────────────────────────
void Game::inputNumber(int num) {
    if (screen_ != GameScreen::Playing) return;
    if (selRow_ < 0 || selCol_ < 0) return;
    if (sudoku_.isGiven(selRow_, selCol_)) return;

    // If cell already has this value, ignore
    if (sudoku_.getCell(selRow_, selCol_) == num) return;

    sudoku_.setCell(selRow_, selCol_, num);
    moveCount_++;

    // Check if the placement is correct (matches solution)
    bool correct = !sudoku_.hasConflict(selRow_, selCol_);

    if (correct) {
        // Increase streak
        streak_++;
        if (streak_ > bestStreak_) bestStreak_ = streak_;

        // Score: base points × streak multiplier × difficulty bonus
        int diffBonus = 1;
        switch (difficulty_) {
            case Difficulty::Easy:   diffBonus = 1; break;
            case Difficulty::Medium: diffBonus = 2; break;
            case Difficulty::Hard:   diffBonus = 3; break;
        }
        int points = 10 * std::min(streak_, 5) * diffBonus;
        score_ += points;

        // Floating text for streaks
        // Cell center position (approximate screen coords)
        float fx = 50.0f + selCol_ * 66.0f + 33.0f;
        float fy = 120.0f + selRow_ * 66.0f;

        if (streak_ == 3) {
            addFloatingText("Nice!", fx, fy, 100, 200, 255);
        } else if (streak_ == 5) {
            addFloatingText("Great!", fx, fy, 120, 255, 160);
        } else if (streak_ == 8) {
            addFloatingText("Amazing!", fx, fy, 255, 200, 60);
        } else if (streak_ >= 10 && streak_ % 5 == 0) {
            char buf[32];
            snprintf(buf, sizeof(buf), "x%d Streak!", streak_);
            addFloatingText(buf, fx, fy, 255, 140, 60);
        }

        // Check row/col/box completions
        checkCompletions(selRow_, selCol_);

        // Check win
        if (sudoku_.isSolved()) {
            totalTime_ = TIME_LIMIT - timeLeft_;
            screen_ = GameScreen::Won;
        }
    } else {
        // Wrong placement — break streak, count mistake
        streak_ = 0;
        mistakes_++;

        float fx = 50.0f + selCol_ * 66.0f + 33.0f;
        float fy = 120.0f + selRow_ * 66.0f;
        addFloatingText("Wrong!", fx, fy, 255, 80, 80);

        // Check if too many mistakes (game over)
        if (maxMistakes_ > 0 && mistakes_ >= maxMistakes_) {
            totalTime_ = TIME_LIMIT - timeLeft_;
            screen_ = GameScreen::TooManyMistakes;
        }
    }
}

// ──────────────────────────────────────────────
// Clear selected cell
// ──────────────────────────────────────────────
void Game::clearSelected() {
    if (screen_ != GameScreen::Playing) return;
    if (selRow_ < 0 || selCol_ < 0) return;

    if (sudoku_.clearCell(selRow_, selCol_)) {
        moveCount_++;
    }
}

// ──────────────────────────────────────────────
// Hint (with anti-spam)
// ──────────────────────────────────────────────
void Game::requestHint() {
    if (screen_ != GameScreen::Playing) return;

    // Check if hints exhausted
    if (hintsUsed_ >= maxHints_) {
        addFloatingText("No hints left!", 350.0f, 400.0f, 255, 80, 80);
        return;
    }

    // Check cooldown
    if (hintCooldown_ > 0.0f) {
        char buf[32];
        snprintf(buf, sizeof(buf), "Wait %.0fs", hintCooldown_);
        addFloatingText(buf, 350.0f, 400.0f, 255, 200, 60);
        return;
    }

    int hr, hc, hv;
    if (sudoku_.getHint(hr, hc, hv)) {
        hintRow_ = hr;
        hintCol_ = hc;
        hintActive_ = true;
        hintTimer_ = HINT_DURATION;

        // Auto-place the hint
        sudoku_.setCell(hr, hc, hv);
        moveCount_++;
        hintsUsed_++;

        // Apply escalating time penalty
        float penalty = HINT_PENALTY_BASE * hintsUsed_;
        timeLeft_ -= penalty;
        if (timeLeft_ < 0.0f) timeLeft_ = 0.0f;

        // Start cooldown
        hintCooldown_ = HINT_COOLDOWN;

        // Reset streak (using hints breaks streaks)
        streak_ = 0;

        // Show penalty text
        char buf[32];
        snprintf(buf, sizeof(buf), "-%ds penalty", (int)penalty);
        float fx = 50.0f + hc * 66.0f + 33.0f;
        float fy = 120.0f + hr * 66.0f;
        addFloatingText(buf, fx, fy, 255, 140, 60);

        // Select the hinted cell
        selRow_ = hr;
        selCol_ = hc;

        // Check completions
        checkCompletions(hr, hc);

        // Check win
        if (sudoku_.isSolved()) {
            totalTime_ = TIME_LIMIT - timeLeft_;
            screen_ = GameScreen::Won;
        }
    }
}

// ──────────────────────────────────────────────
// Undo
// ──────────────────────────────────────────────
void Game::requestUndo() {
    if (screen_ != GameScreen::Playing) return;
    sudoku_.undo();
}

// ──────────────────────────────────────────────
// Floating text helper
// ──────────────────────────────────────────────
void Game::addFloatingText(const std::string& text, float x, float y,
                            unsigned char r, unsigned char g, unsigned char b) {
    floatingTexts_.push_back({text, x, y, FLOATING_TEXT_LIFE, FLOATING_TEXT_LIFE, r, g, b});
}

// ──────────────────────────────────────────────
// Completion checks
// ──────────────────────────────────────────────
bool Game::isRowComplete(int row) const {
    for (int c = 0; c < GRID_SIZE; ++c) {
        if (sudoku_.getCell(row, c) == EMPTY_CELL) return false;
        if (sudoku_.hasConflict(row, c)) return false;
    }
    return true;
}

bool Game::isColComplete(int col) const {
    for (int r = 0; r < GRID_SIZE; ++r) {
        if (sudoku_.getCell(r, col) == EMPTY_CELL) return false;
        if (sudoku_.hasConflict(r, col)) return false;
    }
    return true;
}

bool Game::isBoxComplete(int boxRow, int boxCol) const {
    for (int i = 0; i < BOX_SIZE; ++i)
        for (int j = 0; j < BOX_SIZE; ++j) {
            int r = boxRow * BOX_SIZE + i;
            int c = boxCol * BOX_SIZE + j;
            if (sudoku_.getCell(r, c) == EMPTY_CELL) return false;
            if (sudoku_.hasConflict(r, c)) return false;
        }
    return true;
}

void Game::checkCompletions(int row, int col) {
    if (isRowComplete(row)) {
        completionFlashes_.push_back({CompletionFlash::Row, row, COMPLETION_FLASH_LIFE});
        score_ += 50;
        float fx = 50.0f + 4 * 66.0f + 33.0f;  // center of row
        float fy = 120.0f + row * 66.0f;
        addFloatingText("Row complete! +50", fx, fy, 120, 255, 160);
    }
    if (isColComplete(col)) {
        completionFlashes_.push_back({CompletionFlash::Col, col, COMPLETION_FLASH_LIFE});
        score_ += 50;
        float fx = 50.0f + col * 66.0f + 33.0f;
        float fy = 120.0f + 4 * 66.0f;  // center of col
        addFloatingText("Col complete! +50", fx, fy, 100, 200, 255);
    }
    int boxIdx = (row / BOX_SIZE) * BOX_SIZE + (col / BOX_SIZE);
    if (isBoxComplete(row / BOX_SIZE, col / BOX_SIZE)) {
        completionFlashes_.push_back({CompletionFlash::Box, boxIdx, COMPLETION_FLASH_LIFE});
        score_ += 100;
        float fx = 50.0f + ((col / BOX_SIZE) * BOX_SIZE + 1) * 66.0f + 33.0f;
        float fy = 120.0f + ((row / BOX_SIZE) * BOX_SIZE + 1) * 66.0f;
        addFloatingText("Box complete! +100", fx, fy, 255, 200, 60);
    }
}
