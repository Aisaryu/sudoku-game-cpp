#include "renderer.h"
#include <cstdio>
#include <cmath>

// ──────────────────────────────────────────────
// Color Palette (Dark Theme)
// ──────────────────────────────────────────────
static const Color BG_DARK       = {18, 18, 24, 255};
static const Color BG_CARD       = {28, 28, 40, 255};
static const Color GRID_LINE     = {55, 55, 75, 255};
static const Color GRID_THICK    = {100, 100, 140, 255};
static const Color CELL_SELECTED = {60, 80, 140, 120};
static const Color CELL_SAME_NUM = {45, 55, 90, 80};
static const Color CELL_HIGHLIGHT= {38, 42, 60, 100};
static const Color TEXT_GIVEN    = {220, 220, 235, 255};
static const Color TEXT_USER     = {100, 200, 255, 255};
static const Color TEXT_ERROR    = {255, 80, 80, 255};
static const Color TEXT_HINT     = {120, 255, 160, 255};
static const Color ACCENT_BLUE  = {70, 130, 240, 255};
static const Color ACCENT_CYAN  = {0, 210, 230, 255};
static const Color ACCENT_GREEN = {60, 220, 140, 255};
static const Color ACCENT_GOLD  = {255, 200, 60, 255};
static const Color ACCENT_RED   = {240, 70, 80, 255};
static const Color ACCENT_ORANGE = {255, 140, 60, 255};
static const Color BTN_BASE     = {40, 45, 65, 255};
static const Color BTN_HOVER    = {55, 65, 100, 255};
static const Color TEXT_DIM      = {120, 120, 150, 255};
static const Color TEXT_WHITE    = {240, 240, 250, 255};
static const Color COMPLETION_GLOW = {120, 255, 160, 40};

// ──────────────────────────────────────────────
// Constructor / Destructor
// ──────────────────────────────────────────────
Renderer::Renderer()
    : fontsLoaded_(false)
    , animTime_(0.0f)
{
    mainFont_ = GetFontDefault();
    numberFont_ = GetFontDefault();
    fontsLoaded_ = false;
}

Renderer::~Renderer() {}

// ──────────────────────────────────────────────
// Main draw dispatch
// ──────────────────────────────────────────────
void Renderer::draw(Game& game) {
    animTime_ += GetFrameTime();

    BeginDrawing();
    ClearBackground(BG_DARK);

    switch (game.getScreen()) {
        case GameScreen::Menu:             drawMenu(game);             break;
        case GameScreen::Playing:          drawPlaying(game);          break;
        case GameScreen::Won:              drawWon(game);              break;
        case GameScreen::TimeUp:           drawTimeUp(game);           break;
        case GameScreen::TooManyMistakes:  drawTooManyMistakes(game);  break;
    }

    EndDrawing();
}

// ──────────────────────────────────────────────
// Menu Screen
// ──────────────────────────────────────────────
void Renderer::drawMenu(Game& game) {
    // Animated gradient accent line at top
    float wave = sinf(animTime_ * 1.5f) * 0.5f + 0.5f;
    Color gradLeft = {(unsigned char)(70 + wave * 30), 130, 240, 255};
    Color gradRight = {0, (unsigned char)(180 + wave * 40), 230, 255};
    DrawRectangleGradientH(0, 0, WINDOW_W, 4, gradLeft, gradRight);

    // Title
    const char* title = "SUDOKU";
    int titleSize = 72;
    int titleW = MeasureText(title, titleSize);
    DrawText(title, (WINDOW_W - titleW) / 2, 100, titleSize, TEXT_WHITE);

    // Subtitle with glow effect
    const char* subtitle = "A Classic Puzzle Game";
    int subSize = 22;
    int subW = MeasureText(subtitle, subSize);
    unsigned char alpha = (unsigned char)(140 + sinf(animTime_ * 2.0f) * 40);
    DrawText(subtitle, (WINDOW_W - subW) / 2, 180, subSize, {ACCENT_CYAN.r, ACCENT_CYAN.g, ACCENT_CYAN.b, alpha});

    // Decorative line
    DrawRectangleGradientH(100, 225, 500, 2, {ACCENT_BLUE.r, ACCENT_BLUE.g, ACCENT_BLUE.b, 60}, 
                           {ACCENT_CYAN.r, ACCENT_CYAN.g, ACCENT_CYAN.b, 60});

    // Difficulty label
    const char* chooseText = "SELECT DIFFICULTY";
    int chooseW = MeasureText(chooseText, 20);
    DrawText(chooseText, (WINDOW_W - chooseW) / 2, 255, 20, TEXT_DIM);

    // Difficulty buttons with more detail
    const char* labels[] = {"Easy", "Medium", "Hard"};
    const char* descriptions[] = {
        "41 clues  |  5 hints  |  No mistake limit",
        "31 clues  |  3 hints  |  5 mistakes max",
        "23 clues  |  1 hint   |  3 mistakes max"
    };
    Color accents[] = {ACCENT_GREEN, ACCENT_GOLD, ACCENT_RED};

    Vector2 mousePos = GetMousePosition();
    int newHover = -1;

    for (int i = 0; i < 3; ++i) {
        float btnY = 300.0f + i * 105.0f;
        Rectangle btnRect = {150, btnY, 400, 85};

        bool hovered = CheckCollisionPointRec(mousePos, btnRect);
        if (hovered) newHover = i;

        Color bg = hovered ? BTN_HOVER : BTN_BASE;
        DrawRectangleRounded(btnRect, 0.15f, 8, bg);
        DrawRectangleRounded({btnRect.x, btnRect.y, 5, btnRect.height}, 0.5f, 4, accents[i]);

        if (hovered) {
            DrawRectangleRoundedLinesEx(btnRect, 0.15f, 8, 2.0f, 
                {accents[i].r, accents[i].g, accents[i].b, 100});
        }

        DrawText(labels[i], (int)(btnRect.x + 30), (int)(btnRect.y + 18), 28, TEXT_WHITE);
        DrawText(descriptions[i], (int)(btnRect.x + 30), (int)(btnRect.y + 52), 14, TEXT_DIM);

        if (hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            game.startGame(static_cast<Difficulty>(i));
        }
    }

    game.setMenuHover(newHover);

    // Feature highlights
    DrawRectangleGradientH(100, 630, 500, 1, {60, 60, 80, 0}, {60, 60, 80, 120});
    DrawRectangleGradientH(100, 630, 500, 1, {60, 60, 80, 120}, {60, 60, 80, 0});

    const char* features[] = {
        "Streak Bonuses  ·  Score System  ·  Completion Effects",
        "Click cell + press 1-9  |  H: Hint  |  Ctrl+Z: Undo  |  Del: Clear"
    };
    for (int i = 0; i < 2; ++i) {
        int fw = MeasureText(features[i], 13);
        DrawText(features[i], (WINDOW_W - fw) / 2, 645 + i * 22, 13, 
                 {80, 80, 110, 255});
    }

    DrawText("v1.1", WINDOW_W - 50, WINDOW_H - 30, 12, {80, 80, 100, 255});
}

// ──────────────────────────────────────────────
// Playing Screen
// ──────────────────────────────────────────────
void Renderer::drawPlaying(const Game& game) {
    drawHUD(game);
    drawCompletionFlashes(game);
    drawGrid(game);
    drawNumbers(game);
    drawFloatingTexts(game);
}

// ──────────────────────────────────────────────
// HUD (timer, moves, streak, score, hints, mistakes)
// ──────────────────────────────────────────────
void Renderer::drawHUD(const Game& game) {
    // Top accent bar
    DrawRectangleGradientH(0, 0, WINDOW_W, 3, ACCENT_BLUE, ACCENT_CYAN);

    // Difficulty badge
    const char* diffNames[] = {"EASY", "MEDIUM", "HARD"};
    Color diffColors[] = {ACCENT_GREEN, ACCENT_GOLD, ACCENT_RED};
    int diffIdx = static_cast<int>(game.getDifficulty());

    int badgeW = MeasureText(diffNames[diffIdx], 14) + 20;
    DrawRectangleRounded({16, 14, (float)badgeW, 26}, 0.4f, 6, 
                         {diffColors[diffIdx].r, diffColors[diffIdx].g, diffColors[diffIdx].b, 40});
    DrawText(diffNames[diffIdx], 26, 19, 14, diffColors[diffIdx]);

    // Timer (center)
    float tl = game.getTimeLeft();
    int minutes = (int)tl / 60;
    int seconds = (int)tl % 60;
    char timerStr[16];
    snprintf(timerStr, sizeof(timerStr), "%02d:%02d", minutes, seconds);

    Color timerColor = TEXT_WHITE;
    if (tl < 60.0f) {
        unsigned char a = (unsigned char)(200 + sinf(animTime_ * 6.0f) * 55);
        timerColor = {255, 70, 70, a};
    } else if (tl < 180.0f) {
        timerColor = ACCENT_GOLD;
    }

    int timerW = MeasureText(timerStr, 32);
    DrawText(timerStr, (WINDOW_W - timerW) / 2, 10, 32, timerColor);

    const char* timeLabel = "TIME";
    int tlW = MeasureText(timeLabel, 10);
    DrawText(timeLabel, (WINDOW_W - tlW) / 2, 43, 10, TEXT_DIM);

    // ── Left side: Score ──
    char scoreStr[32];
    snprintf(scoreStr, sizeof(scoreStr), "%d", game.getScore());
    DrawText(scoreStr, 18, 48, 18, ACCENT_CYAN);
    DrawText("SCORE", 18, 67, 10, TEXT_DIM);

    // ── Right side: Moves ──
    char moveStr[32];
    snprintf(moveStr, sizeof(moveStr), "%d", game.getMoveCount());
    int moveW = MeasureText(moveStr, 18);
    DrawText(moveStr, WINDOW_W - moveW - 18, 48, 18, TEXT_DIM);
    int moveLabelW = MeasureText("MOVES", 10);
    DrawText("MOVES", WINDOW_W - moveLabelW - 18, 67, 10, {80, 80, 110, 255});

    // ── Separator ──
    DrawRectangleGradientH(20, 82, WINDOW_W - 40, 1, {60, 60, 80, 0}, {60, 60, 80, 160});
    DrawRectangleGradientH(20, 82, WINDOW_W - 40, 1, {60, 60, 80, 160}, {60, 60, 80, 0});

    // ── Info bar (streak, hints, mistakes) ──
    int infoY = 88;

    // Streak indicator
    int streak = game.getStreak();
    if (streak >= 2) {
        char streakStr[32];
        snprintf(streakStr, sizeof(streakStr), "x%d", streak);
        
        // Streak glow effect
        float pulse = sinf(animTime_ * 4.0f) * 0.3f + 0.7f;
        Color streakColor;
        if (streak >= 10)     streakColor = ACCENT_ORANGE;
        else if (streak >= 5) streakColor = ACCENT_GOLD;
        else                  streakColor = ACCENT_CYAN;
        
        streakColor.a = (unsigned char)(255 * pulse);
        DrawText(streakStr, 20, infoY, 18, streakColor);
        
        int sw = MeasureText(streakStr, 18);
        DrawText("STREAK", 20 + sw + 6, infoY + 4, 11, TEXT_DIM);
    }

    // Hint counter (center-left)
    {
        int hintsLeft = game.getMaxHints() - game.getHintsUsed();
        char hintStr[32];
        snprintf(hintStr, sizeof(hintStr), "HINTS: %d/%d", hintsLeft, game.getMaxHints());
        
        Color hintColor = TEXT_DIM;
        if (hintsLeft == 0) hintColor = {80, 80, 100, 255};
        else if (hintsLeft == 1) hintColor = ACCENT_GOLD;
        
        int hw = MeasureText(hintStr, 12);
        DrawText(hintStr, (WINDOW_W - hw) / 2 - 80, infoY + 3, 12, hintColor);

        // Cooldown bar
        float cd = game.getHintCooldown();
        if (cd > 0.0f) {
            float cdRatio = cd / game.getHintCooldownMax();
            int barX = (WINDOW_W - hw) / 2 - 80;
            int barW = hw;
            DrawRectangle(barX, infoY + 18, barW, 3, {40, 40, 55, 255});
            DrawRectangle(barX, infoY + 18, (int)(barW * cdRatio), 3, ACCENT_GOLD);
        }
    }

    // Mistake counter (right side)
    {
        char mistStr[32];
        if (game.getMaxMistakes() > 0) {
            snprintf(mistStr, sizeof(mistStr), "MISTAKES: %d/%d", game.getMistakes(), game.getMaxMistakes());
        } else {
            snprintf(mistStr, sizeof(mistStr), "MISTAKES: %d", game.getMistakes());
        }
        
        Color mistColor = TEXT_DIM;
        if (game.getMaxMistakes() > 0) {
            float ratio = (float)game.getMistakes() / game.getMaxMistakes();
            if (ratio >= 0.66f) mistColor = ACCENT_RED;
            else if (ratio >= 0.33f) mistColor = ACCENT_GOLD;
        }
        
        int mw = MeasureText(mistStr, 12);
        DrawText(mistStr, WINDOW_W - mw - 18, infoY + 3, 12, mistColor);
    }

    // Controls reminder (compact)
    DrawText("H: Hint  |  Ctrl+Z: Undo  |  Del: Clear  |  Esc: Menu", 
             110, 108, 11, {65, 65, 90, 255});

    // ── Number palette at bottom ──
    int paletteY = GRID_OFFSET_Y + CELL_SIZE * GRID_SIZE + 25;
    int paletteStartX = GRID_OFFSET_X;
    int palCellW = CELL_SIZE;

    for (int n = 1; n <= 9; ++n) {
        Rectangle r = {(float)(paletteStartX + (n - 1) * palCellW), (float)paletteY, 
                       (float)(palCellW - 4), 50};

        bool hovered = CheckCollisionPointRec(GetMousePosition(), r);
        DrawRectangleRounded(r, 0.2f, 6, hovered ? BTN_HOVER : BTN_BASE);

        char numStr[4];
        snprintf(numStr, sizeof(numStr), "%d", n);
        int nw = MeasureText(numStr, 28);
        DrawText(numStr, (int)(r.x + (r.width - nw) / 2), (int)(r.y + 10), 28, 
                 hovered ? ACCENT_CYAN : TEXT_DIM);
    }
}

// ──────────────────────────────────────────────
// Completion flashes
// ──────────────────────────────────────────────
void Renderer::drawCompletionFlashes(const Game& game) {
    int ox = GRID_OFFSET_X;
    int oy = GRID_OFFSET_Y;

    for (const auto& cf : game.getCompletionFlashes()) {
        float alpha = cf.life * 0.8f;  // fade out
        unsigned char a = (unsigned char)(alpha * 50);
        Color glowColor = {ACCENT_GREEN.r, ACCENT_GREEN.g, ACCENT_GREEN.b, a};

        switch (cf.type) {
            case CompletionFlash::Row: {
                DrawRectangle(ox, oy + cf.index * CELL_SIZE, 
                              CELL_SIZE * GRID_SIZE, CELL_SIZE, glowColor);
                break;
            }
            case CompletionFlash::Col: {
                DrawRectangle(ox + cf.index * CELL_SIZE, oy,
                              CELL_SIZE, CELL_SIZE * GRID_SIZE, glowColor);
                break;
            }
            case CompletionFlash::Box: {
                int br = (cf.index / BOX_SIZE) * BOX_SIZE;
                int bc = (cf.index % BOX_SIZE) * BOX_SIZE;
                DrawRectangle(ox + bc * CELL_SIZE, oy + br * CELL_SIZE,
                              CELL_SIZE * BOX_SIZE, CELL_SIZE * BOX_SIZE, glowColor);
                break;
            }
        }
    }
}

// ──────────────────────────────────────────────
// Grid
// ──────────────────────────────────────────────
void Renderer::drawGrid(const Game& game) {
    int ox = GRID_OFFSET_X;
    int oy = GRID_OFFSET_Y;
    int gridPx = CELL_SIZE * GRID_SIZE;

    // Grid background (card)
    DrawRectangleRounded({(float)(ox - 8), (float)(oy - 8), (float)(gridPx + 16), (float)(gridPx + 16)},
                         0.02f, 8, BG_CARD);

    int sr = game.getSelectedRow();
    int sc = game.getSelectedCol();
    int selectedVal = (sr >= 0 && sc >= 0) ? game.getSudoku().getCell(sr, sc) : 0;

    // Cell backgrounds
    for (int r = 0; r < GRID_SIZE; ++r) {
        for (int c = 0; c < GRID_SIZE; ++c) {
            Rectangle cellRect = {(float)(ox + c * CELL_SIZE), (float)(oy + r * CELL_SIZE),
                                  (float)CELL_SIZE, (float)CELL_SIZE};

            bool sameRow = (r == sr);
            bool sameCol = (c == sc);
            bool sameBox = (sr >= 0 && sc >= 0 &&
                           (r / BOX_SIZE == sr / BOX_SIZE) && (c / BOX_SIZE == sc / BOX_SIZE));

            if (r == sr && c == sc) {
                DrawRectangleRec(cellRect, CELL_SELECTED);
            } else if (selectedVal != EMPTY_CELL && game.getSudoku().getCell(r, c) == selectedVal) {
                DrawRectangleRec(cellRect, CELL_SAME_NUM);
            } else if (sameRow || sameCol || sameBox) {
                DrawRectangleRec(cellRect, CELL_HIGHLIGHT);
            }

            // Hint flash
            if (game.isHintActive() && r == game.getHintRow() && c == game.getHintCol()) {
                float pulse = sinf(game.getHintTimer() * 8.0f) * 0.5f + 0.5f;
                unsigned char alpha = (unsigned char)(60 + pulse * 80);
                DrawRectangleRec(cellRect, {TEXT_HINT.r, TEXT_HINT.g, TEXT_HINT.b, alpha});
            }
        }
    }

    // Thin grid lines
    for (int i = 0; i <= GRID_SIZE; ++i) {
        DrawRectangle(ox, oy + i * CELL_SIZE, gridPx, THIN_LINE, GRID_LINE);
        DrawRectangle(ox + i * CELL_SIZE, oy, THIN_LINE, gridPx, GRID_LINE);
    }

    // Thick lines for 3x3 boxes
    for (int i = 0; i <= BOX_SIZE; ++i) {
        DrawRectangle(ox - 1, oy + i * BOX_SIZE * CELL_SIZE - THICK_LINE / 2,
                      gridPx + 2, THICK_LINE, GRID_THICK);
        DrawRectangle(ox + i * BOX_SIZE * CELL_SIZE - THICK_LINE / 2, oy - 1,
                      THICK_LINE, gridPx + 2, GRID_THICK);
    }
}

// ──────────────────────────────────────────────
// Numbers
// ──────────────────────────────────────────────
void Renderer::drawNumbers(const Game& game) {
    int ox = GRID_OFFSET_X;
    int oy = GRID_OFFSET_Y;
    const Sudoku& sudoku = game.getSudoku();

    for (int r = 0; r < GRID_SIZE; ++r) {
        for (int c = 0; c < GRID_SIZE; ++c) {
            int val = sudoku.getCell(r, c);
            if (val == EMPTY_CELL) continue;

            char numStr[4];
            snprintf(numStr, sizeof(numStr), "%d", val);

            Color col;
            if (sudoku.isGiven(r, c)) {
                col = TEXT_GIVEN;
            } else if (sudoku.hasConflict(r, c)) {
                col = TEXT_ERROR;
            } else if (game.isHintActive() && r == game.getHintRow() && c == game.getHintCol()) {
                col = TEXT_HINT;
            } else {
                col = TEXT_USER;
            }

            int fontSize = 30;
            int textW = MeasureText(numStr, fontSize);
            int textX = ox + c * CELL_SIZE + (CELL_SIZE - textW) / 2;
            int textY = oy + r * CELL_SIZE + (CELL_SIZE - fontSize) / 2 + 2;

            DrawText(numStr, textX, textY, fontSize, col);
        }
    }
}

// ──────────────────────────────────────────────
// Floating text popups
// ──────────────────────────────────────────────
void Renderer::drawFloatingTexts(const Game& game) {
    for (const auto& ft : game.getFloatingTexts()) {
        float alpha = ft.life / ft.maxLife;  // fade out
        unsigned char a = (unsigned char)(255 * alpha);
        Color col = {ft.r, ft.g, ft.b, a};

        int fontSize = 18;
        int tw = MeasureText(ft.text.c_str(), fontSize);
        DrawText(ft.text.c_str(), (int)(ft.x - tw / 2), (int)ft.y, fontSize, col);
    }
}

// ──────────────────────────────────────────────
// Overlay screen (Win / Time Up / Too Many Mistakes)
// ──────────────────────────────────────────────
void Renderer::drawOverlayScreen(const char* title, const char* subtitle,
                                  const char* detail, const char* detail2,
                                  Color accentColor, const Game& game) {
    // Dim the background
    DrawRectangle(0, 0, WINDOW_W, WINDOW_H, {0, 0, 0, 180});

    // Card
    float cardW = 460;
    float cardH = 370;
    float cardX = (WINDOW_W - cardW) / 2;
    float cardY = (WINDOW_H - cardH) / 2 - 20;
    DrawRectangleRounded({cardX, cardY, cardW, cardH}, 0.06f, 8, BG_CARD);

    // Accent top border
    DrawRectangleRounded({cardX, cardY, cardW, 5}, 0.5f, 4, accentColor);

    // Title
    int titleSize = 44;
    int titleW = MeasureText(title, titleSize);
    DrawText(title, (WINDOW_W - titleW) / 2, (int)(cardY + 35), titleSize, accentColor);

    // Subtitle
    int subSize = 18;
    int subW = MeasureText(subtitle, subSize);
    DrawText(subtitle, (WINDOW_W - subW) / 2, (int)(cardY + 90), subSize, TEXT_DIM);

    // Detail line 1
    if (detail[0] != '\0') {
        int detSize = 16;
        int detW = MeasureText(detail, detSize);
        DrawText(detail, (WINDOW_W - detW) / 2, (int)(cardY + 125), detSize, TEXT_WHITE);
    }

    // Detail line 2
    if (detail2[0] != '\0') {
        int detSize = 15;
        int detW = MeasureText(detail2, detSize);
        DrawText(detail2, (WINDOW_W - detW) / 2, (int)(cardY + 155), detSize, ACCENT_CYAN);
    }

    // "Play Again" button
    Rectangle btnRect = {cardX + 40, cardY + cardH - 90, (cardW - 100) / 2, 55};
    bool hoverPlay = CheckCollisionPointRec(GetMousePosition(), btnRect);
    DrawRectangleRounded(btnRect, 0.2f, 6, hoverPlay ? BTN_HOVER : BTN_BASE);
    DrawRectangleRoundedLinesEx(btnRect, 0.2f, 6, 1.5f, 
                                 hoverPlay ? accentColor : GRID_LINE);
    const char* playText = "Play Again";
    int playW = MeasureText(playText, 20);
    DrawText(playText, (int)(btnRect.x + (btnRect.width - playW) / 2), (int)(btnRect.y + 16), 20,
             hoverPlay ? accentColor : TEXT_DIM);

    // "Menu" button
    Rectangle menuRect = {cardX + 60 + (cardW - 100) / 2, cardY + cardH - 90, (cardW - 100) / 2, 55};
    bool hoverMenu = CheckCollisionPointRec(GetMousePosition(), menuRect);
    DrawRectangleRounded(menuRect, 0.2f, 6, hoverMenu ? BTN_HOVER : BTN_BASE);
    DrawRectangleRoundedLinesEx(menuRect, 0.2f, 6, 1.5f,
                                 hoverMenu ? ACCENT_BLUE : GRID_LINE);
    const char* menuText = "Menu";
    int menuW = MeasureText(menuText, 20);
    DrawText(menuText, (int)(menuRect.x + (menuRect.width - menuW) / 2), (int)(menuRect.y + 16), 20,
             hoverMenu ? ACCENT_BLUE : TEXT_DIM);
}

// ──────────────────────────────────────────────
// Won screen
// ──────────────────────────────────────────────
void Renderer::drawWon(const Game& game) {
    drawHUD(game);
    drawGrid(game);
    drawNumbers(game);

    float t = game.getTotalTime();
    int min = (int)t / 60;
    int sec = (int)t % 60;
    char detail[128];
    snprintf(detail, sizeof(detail), "Time: %02d:%02d   |   Moves: %d   |   Mistakes: %d",
             min, sec, game.getMoveCount(), game.getMistakes());

    char detail2[128];
    snprintf(detail2, sizeof(detail2), "Score: %d   |   Best Streak: x%d   |   Hints Used: %d",
             game.getScore(), game.getBestStreak(), game.getHintsUsed());

    drawOverlayScreen("YOU WIN!", "Puzzle completed successfully", detail, detail2, ACCENT_GREEN, game);
}

// ──────────────────────────────────────────────
// Time Up screen
// ──────────────────────────────────────────────
void Renderer::drawTimeUp(const Game& game) {
    drawHUD(game);
    drawGrid(game);
    drawNumbers(game);

    char detail[128];
    snprintf(detail, sizeof(detail), "Score: %d   |   Moves: %d   |   Best Streak: x%d",
             game.getScore(), game.getMoveCount(), game.getBestStreak());

    drawOverlayScreen("TIME'S UP!", "The 15 minute limit has been reached", detail, "", ACCENT_RED, game);
}

// ──────────────────────────────────────────────
// Too Many Mistakes screen
// ──────────────────────────────────────────────
void Renderer::drawTooManyMistakes(const Game& game) {
    drawHUD(game);
    drawGrid(game);
    drawNumbers(game);

    char detail[128];
    snprintf(detail, sizeof(detail), "Mistakes: %d/%d   |   Score: %d",
             game.getMistakes(), game.getMaxMistakes(), game.getScore());

    drawOverlayScreen("GAME OVER", "Too many mistakes!", detail, "", ACCENT_RED, game);
}
