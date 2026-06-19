#pragma once

#include "game.h"
#include "raylib.h"

// ──────────────────────────────────────────────
// Renderer — all drawing code
// ──────────────────────────────────────────────
class Renderer {
public:
    Renderer();
    ~Renderer();

    void draw(Game& game);   // Main draw dispatch

private:
    // Screen renderers
    void drawMenu(Game& game);
    void drawPlaying(const Game& game);
    void drawWon(const Game& game);
    void drawTimeUp(const Game& game);
    void drawTooManyMistakes(const Game& game);

    // Components
    void drawGrid(const Game& game);
    void drawNumbers(const Game& game);
    void drawHUD(const Game& game);
    void drawFloatingTexts(const Game& game);
    void drawCompletionFlashes(const Game& game);
    void drawOverlayScreen(const char* title, const char* subtitle, 
                           const char* detail, const char* detail2,
                           Color accentColor, const Game& game);

    // Layout constants
    static constexpr int WINDOW_W     = 700;
    static constexpr int WINDOW_H     = 820;
    static constexpr int GRID_OFFSET_X = 50;
    static constexpr int GRID_OFFSET_Y = 120;
    static constexpr int CELL_SIZE     = 66;
    static constexpr int THICK_LINE    = 4;
    static constexpr int THIN_LINE     = 1;

    // Fonts
    Font mainFont_;
    Font numberFont_;
    bool fontsLoaded_;

    // Animation state
    float animTime_;
};
