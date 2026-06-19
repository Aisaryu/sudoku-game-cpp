#pragma once

#include "sudoku.h"
#include <string>

// ──────────────────────────────────────────────
// Game states
// ──────────────────────────────────────────────
enum class GameScreen {
    Menu,
    Playing,
    Won,
    TimeUp,
    TooManyMistakes
};

// ──────────────────────────────────────────────
// Floating text popup (brief feedback messages)
// ──────────────────────────────────────────────
struct FloatingText {
    std::string text;
    float x, y;
    float life;       // remaining seconds
    float maxLife;
    unsigned char r, g, b;  // color
};

// ──────────────────────────────────────────────
// Completion flash (row/col/box completed)
// ──────────────────────────────────────────────
struct CompletionFlash {
    enum Type { Row, Col, Box };
    Type type;
    int index;        // row/col index, or box index (0-8)
    float life;
};

// ──────────────────────────────────────────────
// Game — manages the play session
// ──────────────────────────────────────────────
class Game {
public:
    Game();

    void update(float dt);          // Call every frame
    void startGame(Difficulty diff);
    void reset();                   // Back to menu

    // Input handlers
    void selectCell(int row, int col);
    void inputNumber(int num);      // 1-9
    void clearSelected();
    void requestHint();
    void requestUndo();

    // Getters
    GameScreen     getScreen()     const { return screen_; }
    const Sudoku&  getSudoku()     const { return sudoku_; }
    int            getSelectedRow() const { return selRow_; }
    int            getSelectedCol() const { return selCol_; }
    int            getMoveCount()  const { return moveCount_; }
    float          getTimeLeft()   const { return timeLeft_; }
    float          getTotalTime()  const { return totalTime_; }
    Difficulty     getDifficulty() const { return difficulty_; }

    // Hint state
    bool  isHintActive()   const { return hintActive_; }
    int   getHintRow()     const { return hintRow_; }
    int   getHintCol()     const { return hintCol_; }
    float getHintTimer()   const { return hintTimer_; }
    int   getHintsUsed()   const { return hintsUsed_; }
    int   getMaxHints()    const { return maxHints_; }
    float getHintCooldown() const { return hintCooldown_; }
    float getHintCooldownMax() const { return HINT_COOLDOWN; }

    // Streak & score
    int   getStreak()      const { return streak_; }
    int   getBestStreak()  const { return bestStreak_; }
    int   getScore()       const { return score_; }

    // Mistakes
    int   getMistakes()    const { return mistakes_; }
    int   getMaxMistakes() const { return maxMistakes_; }

    // Floating text popups
    const std::vector<FloatingText>& getFloatingTexts() const { return floatingTexts_; }

    // Completion flashes
    const std::vector<CompletionFlash>& getCompletionFlashes() const { return completionFlashes_; }

    // Menu hover state (for button highlighting)
    int   getMenuHover()   const { return menuHover_; }
    void  setMenuHover(int h)    { menuHover_ = h; }

private:
    void addFloatingText(const std::string& text, float x, float y,
                         unsigned char r, unsigned char g, unsigned char b);
    void checkCompletions(int row, int col);
    bool isRowComplete(int row) const;
    bool isColComplete(int col) const;
    bool isBoxComplete(int boxRow, int boxCol) const;

    GameScreen screen_;
    Sudoku     sudoku_;
    Difficulty difficulty_;

    int   selRow_, selCol_;       // Selected cell (-1 = none)
    int   moveCount_;
    float timeLeft_;              // Countdown timer
    float totalTime_;             // Time taken to solve

    // Hint system (anti-spam)
    bool  hintActive_;
    int   hintRow_, hintCol_;
    float hintTimer_;
    int   hintsUsed_;
    int   maxHints_;              // per-difficulty limit
    float hintCooldown_;          // seconds until next hint allowed

    // Streak
    int   streak_;
    int   bestStreak_;

    // Score
    int   score_;

    // Mistakes
    int   mistakes_;
    int   maxMistakes_;           // -1 = unlimited

    // Floating text popups
    std::vector<FloatingText> floatingTexts_;

    // Completion flashes
    std::vector<CompletionFlash> completionFlashes_;

    // Menu
    int   menuHover_;             // -1 = none, 0/1/2 = Easy/Medium/Hard

    static constexpr float TIME_LIMIT      = 900.0f;   // 15 minutes
    static constexpr float HINT_DURATION   = 2.0f;     // hint flash seconds
    static constexpr float HINT_COOLDOWN   = 20.0f;    // seconds between hints
    static constexpr float HINT_PENALTY_BASE = 15.0f;  // base time penalty per hint
    static constexpr float FLOATING_TEXT_LIFE = 1.5f;   // popup duration
    static constexpr float COMPLETION_FLASH_LIFE = 1.0f;
};
