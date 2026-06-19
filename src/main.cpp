#include "game.h"
#include "renderer.h"
#include "raylib.h"

int main() {
    // ──────────────────────────────────────────
    // Window setup
    // ──────────────────────────────────────────
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
    InitWindow(700, 820, "Sudoku");
    SetTargetFPS(60);

    // ──────────────────────────────────────────
    // Create game & renderer
    // ──────────────────────────────────────────
    Game game;
    Renderer renderer;

    // ──────────────────────────────────────────
    // Main loop
    // ──────────────────────────────────────────
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        // ─── Input ───────────────────────────
        switch (game.getScreen()) {
            case GameScreen::Menu:
                // Menu input is handled in renderer (button clicks)
                break;

            case GameScreen::Playing: {
                // Mouse click → select cell
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    Vector2 mp = GetMousePosition();
                    int gridX = (int)mp.x - 50;   // GRID_OFFSET_X
                    int gridY = (int)mp.y - 120;   // GRID_OFFSET_Y

                    if (gridX >= 0 && gridX < 66 * 9 && gridY >= 0 && gridY < 66 * 9) {
                        int col = gridX / 66;
                        int row = gridY / 66;
                        game.selectCell(row, col);
                    }

                    // Number palette click (bottom bar)
                    int paletteY = 120 + 66 * 9 + 25;
                    int paletteStartX = 50;
                    for (int n = 1; n <= 9; ++n) {
                        float px = (float)(paletteStartX + (n - 1) * 66);
                        float py = (float)paletteY;
                        Rectangle r = {px, py, 62, 50};
                        if (CheckCollisionPointRec(mp, r)) {
                            game.inputNumber(n);
                        }
                    }
                }

                // Keyboard numbers 1-9
                for (int n = 1; n <= 9; ++n) {
                    if (IsKeyPressed(KEY_ONE + (n - 1)) || IsKeyPressed(KEY_KP_1 + (n - 1))) {
                        game.inputNumber(n);
                    }
                }

                // Arrow keys for cell navigation
                if (IsKeyPressed(KEY_UP)    || IsKeyPressed(KEY_W)) {
                    int r = game.getSelectedRow();
                    int c = game.getSelectedCol();
                    if (r < 0) game.selectCell(0, 0);
                    else game.selectCell((r - 1 + 9) % 9, c < 0 ? 0 : c);
                }
                if (IsKeyPressed(KEY_DOWN)  || IsKeyPressed(KEY_S)) {
                    int r = game.getSelectedRow();
                    int c = game.getSelectedCol();
                    if (r < 0) game.selectCell(0, 0);
                    else game.selectCell((r + 1) % 9, c < 0 ? 0 : c);
                }
                if (IsKeyPressed(KEY_LEFT)  || IsKeyPressed(KEY_A)) {
                    int r = game.getSelectedRow();
                    int c = game.getSelectedCol();
                    if (c < 0) game.selectCell(0, 0);
                    else game.selectCell(r < 0 ? 0 : r, (c - 1 + 9) % 9);
                }
                if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
                    int r = game.getSelectedRow();
                    int c = game.getSelectedCol();
                    if (c < 0) game.selectCell(0, 0);
                    else game.selectCell(r < 0 ? 0 : r, (c + 1) % 9);
                }

                // Delete / Backspace → clear cell
                if (IsKeyPressed(KEY_DELETE) || IsKeyPressed(KEY_BACKSPACE)) {
                    game.clearSelected();
                }

                // H → hint
                if (IsKeyPressed(KEY_H)) {
                    game.requestHint();
                }

                // Ctrl+Z or U → undo
                if (IsKeyPressed(KEY_U) || 
                    (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_Z))) {
                    game.requestUndo();
                }

                // Escape → back to menu
                if (IsKeyPressed(KEY_ESCAPE)) {
                    game.reset();
                }

                break;
            }

            case GameScreen::Won:
            case GameScreen::TimeUp:
            case GameScreen::TooManyMistakes: {
                // Click handlers are in the overlay renderer
                // We need to check for "Play Again" and "Menu" button clicks
                Vector2 mp = GetMousePosition();

                float cardW = 460;
                float cardH = 370;
                float cardX = (700 - cardW) / 2;
                float cardY = (820 - cardH) / 2 - 20;

                Rectangle playBtn = {cardX + 40, cardY + cardH - 90, (cardW - 100) / 2, 55};
                Rectangle menuBtn = {cardX + 60 + (cardW - 100) / 2, cardY + cardH - 90, (cardW - 100) / 2, 55};

                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    if (CheckCollisionPointRec(mp, playBtn)) {
                        game.startGame(game.getDifficulty());
                    } else if (CheckCollisionPointRec(mp, menuBtn)) {
                        game.reset();
                    }
                }

                // Escape → menu
                if (IsKeyPressed(KEY_ESCAPE)) {
                    game.reset();
                }
                break;
            }
        }

        // ─── Update ──────────────────────────
        game.update(dt);

        // ─── Draw ────────────────────────────
        renderer.draw(game);
    }

    // ──────────────────────────────────────────
    // Cleanup
    // ──────────────────────────────────────────
    CloseWindow();
    return 0;
}
