#include "raylib.h"
#include <vector>
#include <cmath>

enum GameState {
    START,
    PLAYING,
    GAMEOVER,
    LEVELUP,
    VICTORY,
    HIGHSCORES
};

struct Ghost {
    float x;
    float y;
    float dx;
    float dy;
};

struct Particle {
    float x;
    float y;
    float dx;
    float dy;
    float life;
    float maxLife;
    Color color;
};

struct Star {
    float x;
    float y;
    float spd;
    float size;
};

struct Bomb {
    int row;
    int col;
    float timeLeft;
    bool active;
};

GameState state = START;
int currentLevel = 1;
int score = 0;
int highScores[5] = { 0 };
int lives = 3;
float pX = 0.0f;
float pY = 0.0f;
float lastPlayerDx = 0.0f;
float lastPlayerDy = 0.0f;
float pSpdBooster = 1.0f;
int ballsInLevelCollected = 0;
int totalBalls = 0;
bool gameWon = false;
bool isNewHighScore = false;

int currentMap[15][20];
bool balls[15][20];
float ghostSpd = 2.0f;
Ghost ghosts[3];

int pCol = 0;
int pRow = 0;
int menuIdx = 0;
int startLvl = 1;
bool shouldExit = false;
bool playedGameOverSnd = false;

float shakeTime = 0.0f;
float shakeIntensity = 0.0f;
float flashTime = 0.0f;
float levelUpTimer = 0.0f;

int exitRow = 13;
int exitCol = 18;
bool exitUnlocked = false;

Camera2D camera = { 0 };
Particle particles[128];
Star stars[50];
RenderTexture2D screenTarget;

Sound collectSnd;
Sound ghostSnd;
Sound gameOverSnd;
Sound victorySnd;
Sound bgm;
Sound bombSnd;

Bomb bombs[3];
float bombSpawnTimer = 0.0f;

int mapLevel1[15][20] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,1,1,0,1,1,1,1,1,1,0,1,1,1,1,0,1},
    {1,0,1,1,1,1,0,1,1,1,1,1,1,0,1,1,1,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,1,1,0,1,1,0,0,1,1,0,1,1,1,1,0,1},
    {1,0,0,0,0,0,0,1,1,0,0,1,1,0,0,0,0,0,0,1},
    {1,1,1,1,0,1,0,1,1,1,1,1,1,0,1,0,1,1,1,1},
    {1,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,1},
    {1,0,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,1,1,0,1,1,1,1,1,1,0,1,1,1,1,0,1},
    {1,0,1,1,1,1,0,1,1,1,1,1,1,0,1,1,1,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

int mapLevel2[15][20] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,1,0,0,0,0,1,1,0,0,0,0,1,0,0,0,1},
    {1,0,1,0,1,0,1,1,0,1,1,0,1,1,0,1,0,1,0,1},
    {1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1},
    {1,0,1,1,1,1,0,1,1,1,1,1,1,0,1,1,1,1,0,1},
    {1,0,0,0,1,0,0,1,0,0,0,0,1,0,0,1,0,0,0,1},
    {1,1,1,0,1,0,1,1,0,1,1,0,1,1,0,1,0,1,1,1},
    {1,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,1},
    {1,1,1,0,1,0,1,1,1,1,1,1,1,1,0,1,0,1,1,1},
    {1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1},
    {1,0,1,1,1,1,0,1,1,1,1,1,1,0,1,1,1,1,0,1},
    {1,0,1,0,0,0,0,0,0,1,1,0,0,0,0,0,0,1,0,1},
    {1,0,1,0,1,1,1,1,0,1,1,0,1,1,1,1,0,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

int mapLevel3[15][20] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,1},
    {1,0,1,1,0,1,0,1,1,1,1,1,1,0,1,0,1,1,0,1},
    {1,0,1,1,0,1,0,1,0,0,0,0,1,0,1,0,1,1,0,1},
    {1,0,0,0,0,0,0,1,0,1,1,0,1,0,0,0,0,0,0,1},
    {1,1,1,0,1,1,1,1,0,1,1,0,1,1,1,1,0,1,1,1},
    {1,1,1,0,1,0,0,0,0,1,1,0,0,0,0,1,0,1,1,1},
    {1,0,0,0,1,0,1,1,0,1,1,0,1,1,0,1,0,0,0,1},
    {1,1,1,0,1,0,1,1,0,0,0,0,1,1,0,1,0,1,1,1},
    {1,1,1,0,1,0,1,1,1,1,1,1,1,1,0,1,0,1,1,1},
    {1,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,1,1,1,1,0,1,1,0,1,1,1,1,1,1,0,1},
    {1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1},
    {1,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,3,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

int mapLevel4[15][20] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,1},
    {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,1},
    {1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,1},
    {1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,0,1},
    {1,0,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,1},
    {1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,1,0,1},
    {1,0,1,0,1,0,0,0,1,0,1,0,1,0,1,0,1,0,0,1},
    {1,0,1,0,1,1,1,0,1,0,1,0,1,0,1,0,1,1,0,1},
    {1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,1},
    {1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,1,0,1},
    {1,0,0,0,1,0,1,0,0,0,0,0,0,1,0,0,1,0,0,1},
    {1,0,1,0,1,0,1,0,1,1,1,1,0,1,0,1,1,1,0,1},
    {1,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,3,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

bool checkCollision(float x, float y, float r) {
    int minCol = (int)((x - r) / 40.0f);
    int maxCol = (int)((x + r) / 40.0f);
    int minRow = (int)((y - r) / 40.0f);
    int maxRow = (int)((y + r) / 40.0f);

    for (int row = minRow; row <= maxRow; ++row) {
        for (int col = minCol; col <= maxCol; ++col) {
            if (row >= 0 && row < 15 && col >= 0 && col < 20) {
                if (currentMap[row][col] == 1) {
                    return true;
                }
            } else {
                return true;
            }
        }
    }
    return false;
}

void DrawPokeBall(float x, float y, float r) {
    DrawCircleSector(Vector2{x, y}, r, 270.0f, 450.0f, 16, RED);
    DrawCircleSector(Vector2{x, y}, r, 90.0f, 270.0f, 16, WHITE);
    DrawCircleLines(x, y, r, BLACK);
    DrawLine((int)(x - r), (int)y, (int)(x + r), (int)y, BLACK);
    DrawCircle(x, y, r * 0.3f, WHITE);
    DrawCircleLines(x, y, r * 0.3f, BLACK);
}

void DrawPlayer(float x, float y, float r) {
    bool pMoving = IsKeyDown(KEY_UP) || IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_RIGHT);
    float bob = 0.0f;
    if (pMoving || state == START || state == VICTORY) {
        bob = sinf((float)GetTime() * 12.0f) * 2.0f;
    }

    DrawRectangle((int)(x - r * 0.3f), (int)(y + r * 0.2f + bob), (int)(r * 0.6f), (int)(r * 0.5f), BLUE);
    DrawCircle(x, y - r * 0.3f + bob, r * 0.45f, Color{ 253, 218, 181, 255 });
    
    DrawCircleSector(Vector2{x, y - r * 0.35f + bob}, r * 0.5f, 90.0f, 270.0f, 16, RED);
    DrawRectangle((int)(x - r * 0.75f), (int)(y - r * 0.45f + bob), (int)(r * 0.4f), (int)(r * 0.15f), RED);
    DrawCircle(x + r * 0.15f, y - r * 0.35f + bob, r * 0.07f, BLACK);
}

void DrawGhost(float x, float y, float r, int idx) {
    float bounce = sinf((float)GetTime() * 6.0f + x) * 3.0f;
    float gy = y + bounce;

    DrawCircleGradient((int)x, (int)gy, r * 1.8f, Color{ 128, 0, 128, 80 }, Color{ 0, 0, 0, 0 });
    DrawCircle(x, gy - r * 0.1f, r * 0.9f, PURPLE);
    DrawRectangle((int)(x - r * 0.9f), (int)(gy - r * 0.1f), (int)(r * 1.8f), (int)(r * 0.7f), PURPLE);

    float wave1 = gy + r * 0.6f + sinf((float)GetTime() * 10.0f + x) * 3.0f;
    float wave2 = gy + r * 0.6f + sinf((float)GetTime() * 10.0f + x + 1.5f) * 3.0f;
    float wave3 = gy + r * 0.6f + sinf((float)GetTime() * 10.0f + x + 3.0f) * 3.0f;

    DrawTriangle(Vector2{x - r * 0.9f, gy + r * 0.6f}, Vector2{x - r * 0.6f, wave1}, Vector2{x - r * 0.3f, gy + r * 0.6f}, PURPLE);
    DrawTriangle(Vector2{x - r * 0.3f, gy + r * 0.6f}, Vector2{x, wave2}, Vector2{x + r * 0.3f, gy + r * 0.6f}, PURPLE);
    DrawTriangle(Vector2{x + r * 0.3f, gy + r * 0.6f}, Vector2{x + r * 0.6f, wave3}, Vector2{x + r * 0.9f, gy + r * 0.6f}, PURPLE);

    DrawCircle(x - r * 0.35f, gy - r * 0.1f, r * 0.25f, WHITE);
    DrawCircle(x + r * 0.35f, gy - r * 0.1f, r * 0.25f, WHITE);

    float pDx = 0.0f;
    float pDy = 0.0f;
    if (ghosts[idx].dx > 0) pDx = 2.0f;
    else if (ghosts[idx].dx < 0) pDx = -2.0f;
    if (ghosts[idx].dy > 0) pDy = 2.0f;
    else if (ghosts[idx].dy < 0) pDy = -2.0f;

    DrawCircle(x - r * 0.35f + pDx, gy - r * 0.1f + pDy, r * 0.1f, RED);
    DrawCircle(x + r * 0.35f + pDx, gy - r * 0.1f + pDy, r * 0.1f, RED);
}

void DrawBomb(float x, float y, float r, float timeLeft) {
    float pulse = 1.0f + 0.15f * sinf(timeLeft * (timeLeft < 1.5f ? 18.0f : 8.0f));
    DrawCircle((int)x, (int)y, r * pulse, Color{30, 30, 30, 255});
    DrawCircleLines(x, y, r * pulse, RED);
    DrawLineEx(Vector2{x, y - r}, Vector2{x + 8.0f, y - r - 8.0f}, 2.0f, DARKGRAY);
    float sparkPulse = 3.0f + 2.0f * sinf((float)GetTime() * 20.0f);
    DrawCircle(x + 8.0f, y - r - 8.0f, sparkPulse, ORANGE);
    DrawCircle(x + 8.0f, y - r - 8.0f, sparkPulse * 0.5f, YELLOW);
}

void LoadLevel(int level) {
    currentLevel = level;
    pX = 1 * 40.0f + 20.0f;
    pY = 1 * 40.0f + 20.0f;
    pSpdBooster = 1.0f;
    ballsInLevelCollected = 0;
    exitUnlocked = false;
    lastPlayerDx = 0.0f;
    lastPlayerDy = 0.0f;

    for (int r = 0; r < 15; ++r) {
        for (int c = 0; c < 20; ++c) {
            if (level == 1) currentMap[r][c] = mapLevel1[r][c];
            else if (level == 2) currentMap[r][c] = mapLevel2[r][c];
            else if (level == 3) currentMap[r][c] = mapLevel3[r][c];
            else if (level == 4) currentMap[r][c] = mapLevel4[r][c];
        }
    }

    if (level == 1) ghostSpd = 2.0f;
    else if (level == 2) ghostSpd = 2.5f;
    else if (level == 3) ghostSpd = 3.2f;
    else if (level == 4) ghostSpd = 4.0f;

    ghosts[0].x = 18 * 40.0f + 20.0f;
    ghosts[0].y = 3 * 40.0f + 20.0f;
    ghosts[0].dx = 0.0f;
    ghosts[0].dy = 0.0f;

    ghosts[1].x = 3 * 40.0f + 20.0f;
    ghosts[1].y = 13 * 40.0f + 20.0f;
    ghosts[1].dx = 0.0f;
    ghosts[1].dy = 0.0f;

    ghosts[2].x = 17 * 40.0f + 20.0f;
    ghosts[2].y = 13 * 40.0f + 20.0f;
    ghosts[2].dx = 0.0f;
    ghosts[2].dy = 0.0f;

    for (int i = 0; i < 3; ++i) {
        bombs[i].active = false;
    }
    bombSpawnTimer = 3.0f;

    totalBalls = 0;
    for (int r = 0; r < 15; ++r) {
        for (int c = 0; c < 20; ++c) {
            if (currentMap[r][c] == 3) {
                exitRow = r;
                exitCol = c;
            }
            if (currentMap[r][c] == 0) {
                if ((r == 1 && c == 1) || (r == 3 && c == 18) || (r == 13 && c == 3) || (r == 13 && c == 17)) {
                    balls[r][c] = false;
                } else {
                    balls[r][c] = true;
                    totalBalls++;
                }
            } else {
                balls[r][c] = false;
            }
        }
    }
}

void SaveHighScores() {
    char buffer[256] = "";
    int offset = 0;
    for (int i = 0; i < 5; ++i) {
        offset += TextCopy(buffer + offset, TextFormat("%d\n", highScores[i]));
    }
    SaveFileText("highscore.txt", buffer);
}

bool UpdateHighScores(int newScore) {
    bool inserted = false;
    for (int i = 0; i < 5; ++i) {
        if (newScore > highScores[i]) {
            for (int j = 4; j > i; --j) {
                highScores[j] = highScores[j - 1];
            }
            highScores[i] = newScore;
            inserted = true;
            break;
        }
    }
    if (inserted) {
        SaveHighScores();
    }
    return inserted;
}

void InitGame() {
    score = 0;
    lives = 3;
    gameWon = false;
    playedGameOverSnd = false;
    LoadLevel(startLvl);
}

// Menu navigation + background music for the START and HIGHSCORES screens.
void UpdateMenuScreens() {
    if (state == START) {
        if (!IsSoundPlaying(bgm)) {
            PlaySound(bgm);
        }

        for (int i = 0; i < 50; ++i) {
            stars[i].x += stars[i].spd;
            if (stars[i].x > 1000.0f) {
                stars[i].x = -stars[i].size;
                stars[i].y = (float)GetRandomValue(0, 600);
            }
        }

        if (IsKeyPressed(KEY_UP)) {
            menuIdx = (menuIdx - 1 + 5) % 5;
        }
        if (IsKeyPressed(KEY_DOWN)) {
            menuIdx = (menuIdx + 1) % 5;
        }
        
        Rectangle startBtn  = { 380, 245, 240, 35 };
        Rectangle leftArr   = { 340, 305, 30, 35 };
        Rectangle levelBtn  = { 380, 305, 260, 35 };
        Rectangle rightArr  = { 650, 305, 30, 35 };
        Rectangle hsBtn     = { 380, 365, 240, 35 };
        Rectangle fsBtn     = { 380, 425, 220, 35 };
        Rectangle exitBtn   = { 380, 485, 220, 35 };

        Vector2 mouse = GetMousePosition();
        float scale = fminf((float)GetScreenWidth() / 1000.0f, (float)GetScreenHeight() / 600.0f);
        mouse.x = (mouse.x - ((float)GetScreenWidth() - ((float)1000.0f * scale)) * 0.5f) / scale;
        mouse.y = (mouse.y - ((float)GetScreenHeight() - ((float)600.0f * scale)) * 0.5f) / scale;

        static Vector2 lastMousePos = { 0 };
        bool mouseMoved = (mouse.x != lastMousePos.x || mouse.y != lastMousePos.y);
        lastMousePos = mouse;
        
        if (mouseMoved) {
            if (CheckCollisionPointRec(mouse, startBtn)) {
                menuIdx = 0;
            } else if (CheckCollisionPointRec(mouse, leftArr) || CheckCollisionPointRec(mouse, levelBtn) || CheckCollisionPointRec(mouse, rightArr)) {
                menuIdx = 1;
            } else if (CheckCollisionPointRec(mouse, hsBtn)) {
                menuIdx = 2;
            } else if (CheckCollisionPointRec(mouse, fsBtn)) {
                menuIdx = 3;
            } else if (CheckCollisionPointRec(mouse, exitBtn)) {
                menuIdx = 4;
            }
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            if (CheckCollisionPointRec(mouse, startBtn)) { 
                InitGame(); 
                state = PLAYING; 
            } else if (CheckCollisionPointRec(mouse, leftArr)) { 
                startLvl = (startLvl - 2 + 4) % 4 + 1; 
                menuIdx = 1; 
            } else if (CheckCollisionPointRec(mouse, rightArr)) { 
                startLvl = (startLvl % 4) + 1; 
                menuIdx = 1; 
            } else if (CheckCollisionPointRec(mouse, levelBtn)) { 
                menuIdx = 1; 
            } else if (CheckCollisionPointRec(mouse, hsBtn)) { 
                state = HIGHSCORES; 
                menuIdx = 2; 
            } else if (CheckCollisionPointRec(mouse, fsBtn)) { 
                ToggleFullscreen(); 
                menuIdx = 3; 
            } else if (CheckCollisionPointRec(mouse, exitBtn)) { 
                shouldExit = true; 
                menuIdx = 4; 
            }
        }

        if (menuIdx == 1) {
            if (IsKeyPressed(KEY_LEFT)) {
                startLvl = (startLvl - 2 + 4) % 4 + 1;
            }
            if (IsKeyPressed(KEY_RIGHT)) {
                startLvl = (startLvl % 4) + 1;
            }
        }
        if (IsKeyPressed(KEY_ENTER)) {
            if (menuIdx == 0) {
                InitGame();
                state = PLAYING;
            } else if (menuIdx == 2) {
                state = HIGHSCORES;
            } else if (menuIdx == 3) {
                ToggleFullscreen();
            } else if (menuIdx == 4) {
                shouldExit = true;
            }
        }
    } else if (state == HIGHSCORES) {
        if (!IsSoundPlaying(bgm)) {
            PlaySound(bgm);
        }
        for (int i = 0; i < 50; ++i) {
            stars[i].x += stars[i].spd;
            if (stars[i].x > 1000.0f) {
                stars[i].x = -stars[i].size;
                stars[i].y = (float)GetRandomValue(0, 600);
            }
        }
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ESCAPE) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            state = START;
        }
    }
}

// LEVELUP transition timer, GAMEOVER sound/restart handling, and VICTORY input.
void UpdateGameEndStates() {
    if (state == LEVELUP) {
        if (IsKeyPressed(KEY_ESCAPE)) {
            UpdateHighScores(score);
            StopSound(bgm);
            state = START;
        }
        if (!IsSoundPlaying(bgm)) {
            PlaySound(bgm);
        }

        levelUpTimer -= GetFrameTime();
        if (levelUpTimer <= 0.0f) {
            if (currentLevel < 4) {
                LoadLevel(currentLevel + 1);
                state = PLAYING;
            } else {
                bool isNewBest = (score > highScores[0]);
                UpdateHighScores(score);
                isNewHighScore = isNewBest;
                StopSound(bgm);
                PlaySound(victorySnd);
                state = VICTORY;
            }
        }
    } else if (state == GAMEOVER) {
        if (!playedGameOverSnd) {
            StopSound(bgm);
            PlaySound(gameOverSnd);
            playedGameOverSnd = true;
        }

        if (IsKeyPressed(KEY_R)) {
            InitGame();
            state = PLAYING;
        }
        if (IsKeyPressed(KEY_ESCAPE)) {
            state = START;
        }
    } else if (state == VICTORY) {
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ESCAPE)) {
            state = START;
        }
    }
}

// Player input, movement, and bomb spawn/tick/explosion logic for the PLAYING state.
void UpdatePlayerMovementAndBombs() {
    if (state == PLAYING) {
        if (IsKeyPressed(KEY_ESCAPE)) {
            UpdateHighScores(score);
            StopSound(bgm);
            state = START;
        }
        if (!IsSoundPlaying(bgm)) {
            PlaySound(bgm);
        }

        if (shakeTime > 0.0f) {
            shakeTime -= GetFrameTime();
        }
        if (flashTime > 0.0f) {
            flashTime -= GetFrameTime();
        }

        for (int i = 0; i < 128; ++i) {
            if (particles[i].life > 0.0f) {
                particles[i].x += particles[i].dx;
                particles[i].y += particles[i].dy;
                particles[i].life -= GetFrameTime();
            }
        }

        float spd = 3.5f * pSpdBooster;
        float dx = 0.0f;
        float dy = 0.0f;

        if (IsKeyDown(KEY_UP)) dy = -spd;
        if (IsKeyDown(KEY_DOWN)) dy = spd;
        if (IsKeyDown(KEY_LEFT)) dx = -spd;
        if (IsKeyDown(KEY_RIGHT)) dx = spd;

        if (dx != 0.0f || dy != 0.0f) {
            lastPlayerDx = dx > 0.0f ? 1.0f : (dx < 0.0f ? -1.0f : 0.0f);
            lastPlayerDy = dy > 0.0f ? 1.0f : (dy < 0.0f ? -1.0f : 0.0f);
        }

        bool playerMoved = false;
        if (dx != 0.0f) {
            float nextX = pX + dx;
            if (!checkCollision(nextX, pY, 12.0f)) {
                pX = nextX;
                playerMoved = true;
            }
        }
        if (dy != 0.0f) {
            float nextY = pY + dy;
            if (!checkCollision(pX, nextY, 12.0f)) {
                pY = nextY;
                playerMoved = true;
            }
        }

        pCol = (int)(pX / 40.0f);
        pRow = (int)(pY / 40.0f);

        if (playerMoved) {
            bombSpawnTimer -= GetFrameTime();
            if (bombSpawnTimer <= 0.0f) {
                bombSpawnTimer = 3.0f;
                for (int bSpawn = 0; bSpawn < 3; ++bSpawn) {
                    int r = 0, c = 0;
                    bool valid = false;
                    for (int attempts = 0; attempts < 30; ++attempts) {
                        int dr = GetRandomValue(-4, 4);
                        int dc = GetRandomValue(-4, 4);
                        int dist = abs(dr) + abs(dc);
                        if (dist >= 2 && dist <= 5) {
                            int tr = pRow + dr;
                            int tc = pCol + dc;
                            if (tr >= 1 && tr < 14 && tc >= 1 && tc < 19) {
                                if (currentMap[tr][tc] != 1 && currentMap[tr][tc] != 3) {
                                    bool occupied = false;
                                    for (int i = 0; i < 3; ++i) {
                                        if (bombs[i].active && bombs[i].row == tr && bombs[i].col == tc) {
                                            occupied = true;
                                            break;
                                        }
                                    }
                                    if (!occupied) {
                                        r = tr;
                                        c = tc;
                                        valid = true;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                    if (valid) {
                        for (int i = 0; i < 3; ++i) {
                            if (!bombs[i].active) {
                                bombs[i].row = r;
                                bombs[i].col = c;
                                bombs[i].timeLeft = (float)GetRandomValue(3, 6);
                                bombs[i].active = true;
                                break;
                            }
                        }
                    }
                }
            }
        }

        for (int i = 0; i < 3; ++i) {
            if (bombs[i].active) {
                bombs[i].timeLeft -= GetFrameTime();
                if (bombs[i].timeLeft <= 0.0f) {
                    bombs[i].active = false;
                    int spawned = 0;
                    for (int p = 0; p < 128 && spawned < 10; ++p) {
                        if (particles[p].life <= 0.0f) {
                            particles[p].x = bombs[i].col * 40.0f + 20.0f;
                            particles[p].y = bombs[i].row * 40.0f + 20.0f;
                            particles[p].dx = (float)GetRandomValue(-150, 150) / 50.0f;
                            particles[p].dy = (float)GetRandomValue(-150, 150) / 50.0f;
                            particles[p].life = 0.4f;
                            particles[p].maxLife = 0.4f;
                            particles[p].color = ORANGE;
                            spawned++;
                        }
                    }
                } else {
                    float bx = bombs[i].col * 40.0f + 20.0f;
                    float by = bombs[i].row * 40.0f + 20.0f;
                    float distSq = (pX - bx) * (pX - bx) + (pY - by) * (pY - by);
                    if (distSq < 576.0f) {
                        bombs[i].active = false;
                        PlaySound(bombSnd);
                        int spawned = 0;
                        for (int p = 0; p < 128 && spawned < 20; ++p) {
                            if (particles[p].life <= 0.0f) {
                                particles[p].x = bx;
                                particles[p].y = by;
                                particles[p].dx = (float)GetRandomValue(-200, 200) / 40.0f;
                                particles[p].dy = (float)GetRandomValue(-200, 200) / 40.0f;
                                particles[p].life = 0.5f;
                                particles[p].maxLife = 0.5f;
                                particles[p].color = RED;
                                spawned++;
                            }
                        }
                        shakeTime = 0.4f;
                        shakeIntensity = 8.0f;
                        flashTime = 0.3f;
                        lives--;
                        if (lives <= 0) {
                            UpdateHighScores(score);
                            state = GAMEOVER;
                        } else {
                            pX = 1 * 40.0f + 20.0f;
                            pY = 1 * 40.0f + 20.0f;
                            lastPlayerDx = 0.0f;
                            lastPlayerDy = 0.0f;
                            ghosts[0].x = 18 * 40.0f + 20.0f;
                            ghosts[0].y = 3 * 40.0f + 20.0f;
                            ghosts[0].dx = 0.0f;
                            ghosts[0].dy = 0.0f;
                            ghosts[1].x = 3 * 40.0f + 20.0f;
                            ghosts[1].y = 13 * 40.0f + 20.0f;
                            ghosts[1].dx = 0.0f;
                            ghosts[1].dy = 0.0f;
                            ghosts[2].x = 17 * 40.0f + 20.0f;
                            ghosts[2].y = 13 * 40.0f + 20.0f;
                            ghosts[2].dx = 0.0f;
                            ghosts[2].dy = 0.0f;
                            for (int b = 0; b < 3; ++b) bombs[b].active = false;
                        }
                    }
                }
            }
        }
    }
}

// Ball pickup, exit-gate unlock, and ghost AI movement/collision for the PLAYING state.
void UpdateBallsGhostsCollisions() {
    if (state == PLAYING) {
        if (pRow >= 0 && pRow < 15 && pCol >= 0 && pCol < 20) {
            if (balls[pRow][pCol]) {
                balls[pRow][pCol] = false;
                score += 10;
                totalBalls--;
                ballsInLevelCollected++;
                PlaySound(collectSnd);

                int spawned = 0;
                for (int i = 0; i < 128 && spawned < 10; ++i) {
                    if (particles[i].life <= 0.0f) {
                        particles[i].x = pCol * 40.0f + 20.0f;
                        particles[i].y = pRow * 40.0f + 20.0f;
                        particles[i].dx = (float)GetRandomValue(-150, 150) / 50.0f;
                        particles[i].dy = (float)GetRandomValue(-150, 150) / 50.0f;
                        particles[i].life = 0.4f;
                        particles[i].maxLife = 0.4f;
                        particles[i].color = SKYBLUE;
                        spawned++;
                    }
                }

                if (ballsInLevelCollected % 10 == 0) {
                    pSpdBooster += 0.1f;
                }
            }
        }

        if (totalBalls <= 0) {
            exitUnlocked = true;
        }

        if (pCol == exitCol && pRow == exitRow) {
            state = LEVELUP;
            levelUpTimer = 1.5f;
        }

        for (int i = 0; i < 3; ++i) {
            int gCol = (int)(ghosts[i].x / 40.0f);
            int gRow = (int)(ghosts[i].y / 40.0f);
            float centerX = gCol * 40.0f + 20.0f;
            float centerY = gRow * 40.0f + 20.0f;

            bool crossedCenter = false;
            if (ghosts[i].dx > 0.0f && ghosts[i].x < centerX && ghosts[i].x + ghosts[i].dx >= centerX) crossedCenter = true;
            else if (ghosts[i].dx < 0.0f && ghosts[i].x > centerX && ghosts[i].x + ghosts[i].dx <= centerX) crossedCenter = true;
            else if (ghosts[i].dy > 0.0f && ghosts[i].y < centerY && ghosts[i].y + ghosts[i].dy >= centerY) crossedCenter = true;
            else if (ghosts[i].dy < 0.0f && ghosts[i].y > centerY && ghosts[i].y + ghosts[i].dy <= centerY) crossedCenter = true;
            else if (ghosts[i].dx == 0.0f && ghosts[i].dy == 0.0f) crossedCenter = true;

            if (crossedCenter) {
                ghosts[i].x = centerX;
                ghosts[i].y = centerY;

                float dirs[4][2] = { {0, -1}, {0, 1}, {-1, 0}, {1, 0} };
                std::vector<std::pair<float, float>> validDirs;

                for (int d = 0; d < 4; ++d) {
                    int nr = gRow + (int)dirs[d][1];
                    int nc = gCol + (int)dirs[d][0];
                    if (nr >= 0 && nr < 15 && nc >= 0 && nc < 20) {
                        if (currentMap[nr][nc] != 1) {
                            if (dirs[d][0] * ghosts[i].dx + dirs[d][1] * ghosts[i].dy >= 0.0f || (ghosts[i].dx == 0.0f && ghosts[i].dy == 0.0f)) {
                                validDirs.push_back({dirs[d][0], dirs[d][1]});
                            }
                        }
                    }
                }

                if (validDirs.empty()) {
                    for (int d = 0; d < 4; ++d) {
                        int nr = gRow + (int)dirs[d][1];
                        int nc = gCol + (int)dirs[d][0];
                        if (nr >= 0 && nr < 15 && nc >= 0 && nc < 20) {
                            if (currentMap[nr][nc] != 1) {
                                validDirs.push_back({dirs[d][0], dirs[d][1]});
                            }
                        }
                    }
                }

                if (!validDirs.empty()) {
                    float targetX = pX;
                    float targetY = pY;

                    if (i == 0) {
                        targetX = pX;
                        targetY = pY;
                    } else if (i == 1) {
                        float lookAhead = 4 * 40.0f;
                        targetX = pX + lastPlayerDx * lookAhead;
                        targetY = pY + lastPlayerDy * lookAhead;
                    } else {
                        float distToPlayerSq = (pX - centerX) * (pX - centerX) + (pY - centerY) * (pY - centerY);
                        if (distToPlayerSq > (300.0f * 300.0f)) {
                            targetX = centerX + (float)GetRandomValue(-300, 300);
                            targetY = centerY + (float)GetRandomValue(-300, 300);
                        } else {
                            targetX = pX;
                            targetY = pY;
                        }
                    }

                    float bestDx = validDirs[0].first;
                    float bestDy = validDirs[0].second;
                    float bestDistSq = 1e9f;

                    bool wander = (GetRandomValue(0, 100) < 6);
                    if (wander) {
                        int rIdx = GetRandomValue(0, (int)validDirs.size() - 1);
                        bestDx = validDirs[rIdx].first;
                        bestDy = validDirs[rIdx].second;
                    } else {
                        bool allPenalized = true;
                        for (auto& vd : validDirs) {
                            float nextX = centerX + vd.first * 40.0f;
                            float nextY = centerY + vd.second * 40.0f;
                            
                            float penalty = 0.0f;
                            for (int j = 0; j < 3; ++j) {
                                if (j == i) continue;
                                float ghostDistSq = (ghosts[j].x - nextX) * (ghosts[j].x - nextX)
                                                   + (ghosts[j].y - nextY) * (ghosts[j].y - nextY);
                                if (ghostDistSq < (30.0f * 30.0f)) {
                                    penalty = 100000.0f;
                                }
                            }
                            if (penalty == 0.0f) allPenalized = false;

                            float dSq = (targetX - nextX) * (targetX - nextX) + (targetY - nextY) * (targetY - nextY);
                            dSq += penalty;
                            if (dSq < bestDistSq) {
                                bestDistSq = dSq;
                                bestDx = vd.first;
                                bestDy = vd.second;
                            }
                        }

                        if (allPenalized) {
                            bestDistSq = 1e9f;
                            for (auto& vd : validDirs) {
                                float nextX = centerX + vd.first * 40.0f;
                                float nextY = centerY + vd.second * 40.0f;
                                float dSq = (targetX - nextX) * (targetX - nextX) + (targetY - nextY) * (targetY - nextY);
                                if (dSq < bestDistSq) {
                                    bestDistSq = dSq;
                                    bestDx = vd.first;
                                    bestDy = vd.second;
                                }
                            }
                        }
                    }

                    ghosts[i].dx = bestDx * ghostSpd;
                    ghosts[i].dy = bestDy * ghostSpd;
                }
            }

            ghosts[i].x += ghosts[i].dx;
            ghosts[i].y += ghosts[i].dy;

            float distSq = (pX - ghosts[i].x) * (pX - ghosts[i].x) + (pY - ghosts[i].y) * (pY - ghosts[i].y);
            if (distSq < 576.0f) {
                PlaySound(ghostSnd);
                shakeTime = 0.4f;
                shakeIntensity = 6.0f;
                flashTime = 0.3f;
                lives--;
                if (lives <= 0) {
                    UpdateHighScores(score);
                    state = GAMEOVER;
                } else {
                    pX = 1 * 40.0f + 20.0f;
                    pY = 1 * 40.0f + 20.0f;
                    lastPlayerDx = 0.0f;
                    lastPlayerDy = 0.0f;
                    ghosts[0].x = 18 * 40.0f + 20.0f;
                    ghosts[0].y = 3 * 40.0f + 20.0f;
                    ghosts[0].dx = 0.0f;
                    ghosts[0].dy = 0.0f;

                    ghosts[1].x = 3 * 40.0f + 20.0f;
                    ghosts[1].y = 13 * 40.0f + 20.0f;
                    ghosts[1].dx = 0.0f;
                    ghosts[1].dy = 0.0f;

                    ghosts[2].x = 17 * 40.0f + 20.0f;
                    ghosts[2].y = 13 * 40.0f + 20.0f;
                    ghosts[2].dx = 0.0f;
                    ghosts[2].dy = 0.0f;

                    for (int b = 0; b < 3; ++b) bombs[b].active = false;
                }
                break;
            }
        }
    }
}

void UpdateGame() {
    if (IsKeyPressed(KEY_F11)) {
        ToggleFullscreen();
    }

    UpdateMenuScreens();
    UpdateGameEndStates();
    UpdatePlayerMovementAndBombs();
    UpdateBallsGhostsCollisions();
}

void DrawBackgroundPokeBall(float x, float y, float r) {
    DrawCircleSector(Vector2{x, y}, r, 270.0f, 450.0f, 32, Color{255, 0, 0, 40});
    DrawCircleSector(Vector2{x, y}, r, 90.0f, 270.0f, 32, Color{255, 255, 255, 40});
    DrawCircleLines(x, y, r, Color{0, 0, 0, 40});
    DrawLine((int)(x - r), (int)y, (int)(x + r), (int)y, Color{0, 0, 0, 40});
    DrawCircle(x, y, r * 0.3f, Color{255, 255, 255, 60});
    DrawCircleLines(x, y, r * 0.3f, Color{0, 0, 0, 40});
}

void DrawGame() {
    BeginTextureMode(screenTarget);
    ClearBackground(BLACK);

    if (state == START) {
        DrawRectangle(0, 0, 1000, 600, Color{10, 10, 18, 255});
        DrawRectangle(0, 300, 1000, 300, Color{20, 15, 30, 255});

        for (int i = 0; i < 50; ++i) {
            DrawCircle((int)stars[i].x, (int)stars[i].y, stars[i].size, ColorAlpha(WHITE, 0.4f));
        }

        DrawLineEx(Vector2{ 0.0f, 300.0f }, Vector2{ 1000.0f, 300.0f }, 3.0f, SKYBLUE);
        DrawLineEx(Vector2{ 0.0f, 300.0f }, Vector2{ 1000.0f, 300.0f }, 8.0f, ColorAlpha(SKYBLUE, 0.3f));

        for (int i = 0; i <= 20; ++i) {
            float px = i * 50.0f;
            DrawLineEx(Vector2{ 500.0f, 300.0f }, Vector2{ px, 600.0f }, 1.5f, PURPLE);
        }

        float scroll = fmodf((float)GetTime() * 40.0f, 30.0f);
        for (float h = 0.0f; h < 300.0f; h += 30.0f) {
            float y = 300.0f + h + scroll;
            if (y < 600.0f) {
                float pct = (y - 300.0f) / 300.0f;
                DrawLineEx(Vector2{ 500.0f - pct * 500.0f, y }, Vector2{ 500.0f + pct * 500.0f, y }, 1.0f + pct * 2.0f, ColorAlpha(PURPLE, pct * 0.7f));
            }
        }

        DrawBackgroundPokeBall(500.0f, 300.0f, 180.0f);

        float pBounce = sinf((float)GetTime() * 4.0f) * 10.0f;
        DrawPlayer(200.0f, 380.0f + pBounce, 40.0f);

        float gFloat = cosf((float)GetTime() * 3.0f) * 15.0f;
        DrawGhost(800.0f, 300.0f + gFloat, 40.0f, 0);

        float pulseSize = 44.0f + 6.0f * sinf((float)GetTime() * 3.0f);
        int titleWidth = MeasureText("POKÉ-MAZE ADVENTURE", (int)pulseSize);
        int titleX = 500 - titleWidth / 2;
        DrawText("POKÉ-MAZE ADVENTURE", titleX - 3, 82, (int)pulseSize, MAGENTA);
        DrawText("POKÉ-MAZE ADVENTURE", titleX + 3, 82, (int)pulseSize, SKYBLUE);
        DrawText("POKÉ-MAZE ADVENTURE", titleX, 80, (int)pulseSize, YELLOW);

        DrawText("START GAME", 380, 250, menuIdx == 0 ? 28 : 24, menuIdx == 0 ? YELLOW : WHITE);
        DrawText(TextFormat("SELECT LEVEL: < %d >", startLvl), 380, 300, menuIdx == 1 ? 28 : 24, menuIdx == 1 ? YELLOW : WHITE);
        DrawText("HIGHSCORES", 380, 350, menuIdx == 2 ? 28 : 24, menuIdx == 2 ? YELLOW : WHITE);
        DrawText("FULLSCREEN", 380, 400, menuIdx == 3 ? 28 : 24, menuIdx == 3 ? YELLOW : WHITE);
        DrawText("EXIT", 380, 450, menuIdx == 4 ? 28 : 24, menuIdx == 4 ? YELLOW : WHITE);

        if (menuIdx == 0) DrawPokeBall(340.0f, 265.0f, 10.0f);
        else if (menuIdx == 1) DrawPokeBall(340.0f, 315.0f, 10.0f);
        else if (menuIdx == 2) DrawPokeBall(340.0f, 365.0f, 10.0f);
        else if (menuIdx == 3) DrawPokeBall(340.0f, 415.0f, 10.0f);
        else if (menuIdx == 4) DrawPokeBall(340.0f, 465.0f, 10.0f);

        if (((int)(GetTime() * 2) % 2) == 0) {
            DrawText("Press ENTER to Choose", 370, 500, 22, LIGHTGRAY);
        }
    } else if (state == HIGHSCORES) {
        DrawRectangle(0, 0, 1000, 600, Color{10, 10, 18, 255});
        DrawRectangle(0, 300, 1000, 300, Color{20, 15, 30, 255});

        for (int i = 0; i < 50; ++i) {
            DrawCircle((int)stars[i].x, (int)stars[i].y, stars[i].size, ColorAlpha(WHITE, 0.4f));
        }

        int boxWidth = 400;
        int boxHeight = 280;
        int boxX = 500 - boxWidth / 2;
        int boxY = 180;
        
        DrawRectangleRounded(Rectangle{(float)boxX, (float)boxY, (float)boxWidth, (float)boxHeight}, 0.05f, 4, Color{25, 25, 35, 255});
        DrawRectangleRoundedLines(Rectangle{(float)boxX, (float)boxY, (float)boxWidth, (float)boxHeight}, 0.05f, 4, 3.0f, GOLD);
        
        int titleW = MeasureText("TOP 5 HIGH SCORES", 24);
        DrawText("TOP 5 HIGH SCORES", 500 - titleW / 2, 210, 24, GOLD);

        for (int i = 0; i < 5; ++i) {
            DrawText(TextFormat("%d. %05d", i + 1, highScores[i]), boxX + 120, 260 + i * 32, 22, WHITE);
        }

        if (((int)(GetTime() * 2) % 2) == 0) {
            DrawText("Press ESC / ENTER or CLICK to Go Back", 270, 490, 22, LIGHTGRAY);
        }
    } else if (state == GAMEOVER) {
        ClearBackground(Color{40, 10, 10, 255});
        DrawText("GAME OVER", 320, 70, 60, RED);
        DrawText("Caught by Wild Ghosts!", 320, 140, 28, WHITE);
        DrawText(TextFormat("FINAL SCORE: %d", score), 390, 190, 24, LIGHTGRAY);
        DrawText("TOP 5 SCORES", 390, 235, 20, GOLD);
        for (int i = 0; i < 5; ++i) {
            Color color = (score == highScores[i]) ? LIME : LIGHTGRAY;
            DrawText(TextFormat("%d. %05d", i + 1, highScores[i]), 390, 265 + i * 22, 18, color);
        }
        DrawText("Press 'R' to Try Again", 340, 440, 24, YELLOW);
        DrawText("Press ESC for Menu", 360, 480, 20, LIGHTGRAY);
    } else if (state == VICTORY) {
        ClearBackground(Color{10, 40, 30, 255});
        
        DrawCircleGradient(500, 180, 160.0f, Color{ 255, 223, 0, 40 }, Color{ 0, 0, 0, 0 });

        float vPulse = 54.0f + 8.0f * sinf((float)GetTime() * 4.0f);
        int vWidth = MeasureText("YOU WON!", (int)vPulse);
        DrawText("YOU WON!", 500 - vWidth/2 - 3, 132, (int)vPulse, Color{ 0, 100, 80, 255 });
        DrawText("YOU WON!", 500 - vWidth/2 + 3, 132, (int)vPulse, GOLD);
        DrawText("YOU WON!", 500 - vWidth/2, 130, (int)vPulse, YELLOW);

        const char* sub = "All 4 mazes cleared - the wild ghosts retreat!";
        DrawText(sub, 500 - MeasureText(sub, 24)/2, 230, 24, LIGHTGRAY);

        if (isNewHighScore) {
            DrawText("NEW HIGH SCORE!", 500 - MeasureText("NEW HIGH SCORE!", 26)/2, 290, 26, GOLD);
            DrawText(TextFormat("FINAL SCORE: %d", score), 500 - MeasureText(TextFormat("FINAL SCORE: %d", score), 24)/2, 330, 24, LIME);
        } else {
            DrawText(TextFormat("FINAL SCORE: %d", score), 500 - MeasureText(TextFormat("FINAL SCORE: %d", score), 24)/2, 290, 24, WHITE);
            DrawText(TextFormat("HIGH SCORE: %d", highScores[0]), 500 - MeasureText(TextFormat("HIGH SCORE: %d", highScores[0]), 24)/2, 330, 24, LIGHTGRAY);
        }

        for (int i = 0; i < 5; ++i) {
            float bob = sinf((float)GetTime() * 5.0f + i * 1.0f) * 8.0f;
            DrawPokeBall(360.0f + i * 70.0f, 400.0f + bob, 15.0f);
        }

        DrawPlayer(500.0f, 490.0f, 25.0f);

        if (((int)(GetTime() * 2) % 2) == 0) {
            const char* prompt = "Press ENTER or ESC to return to menu";
            DrawText(prompt, 500 - MeasureText(prompt, 20)/2, 540, 20, LIGHTGRAY);
        }
    } else if (state == PLAYING || state == LEVELUP) {
        if (shakeTime > 0.0f) {
            camera.offset.x = (float)GetRandomValue(-shakeIntensity, shakeIntensity);
            camera.offset.y = (float)GetRandomValue(-shakeIntensity, shakeIntensity);
        } else {
            camera.offset.x = 0.0f;
            camera.offset.y = 0.0f;
        }

        BeginMode2D(camera);

        for (int r = 0; r < 15; ++r) {
            for (int c = 0; c < 20; ++c) {
                float x = c * 40.0f;
                float y = r * 40.0f;

                if (currentMap[r][c] == 1) {
                    DrawRectangle((int)x, (int)y, 40, 40, Color{40, 50, 75, 255});
                    Color neonColor = (currentLevel % 2 == 1) ? SKYBLUE : PURPLE;

                    if (c == 0 || currentMap[r][c - 1] != 1) {
                        DrawLineEx(Vector2{x, y}, Vector2{x, y + 40.0f}, 2.0f, neonColor);
                    }
                    if (c == 19 || currentMap[r][c + 1] != 1) {
                        DrawLineEx(Vector2{x + 40.0f, y}, Vector2{x + 40.0f, y + 40.0f}, 2.0f, neonColor);
                    }
                    if (r == 0 || currentMap[r - 1][c] != 1) {
                        DrawLineEx(Vector2{x, y}, Vector2{x + 40.0f, y}, 2.0f, neonColor);
                    }
                    if (r == 14 || currentMap[r + 1][c] != 1) {
                        DrawLineEx(Vector2{x, y + 40.0f}, Vector2{x + 40.0f, y + 40.0f}, 2.0f, neonColor);
                    }
                } else {
                    DrawRectangle((int)x, (int)y, 40, 40, Color{15, 15, 22, 255});

                    if (balls[r][c]) {
                        float pulse = 1.0f + 0.15f * sinf((float)GetTime() * 8.0f);
                        DrawPoly(Vector2{x + 20.0f, y + 20.0f}, 4, 6.0f * pulse, (float)GetTime() * 100.0f, GOLD);
                        DrawPoly(Vector2{x + 20.0f, y + 20.0f}, 4, 6.0f * pulse * 0.5f, (float)GetTime() * 100.0f, WHITE);
                    }
                }
            }
        }

        if (exitRow >= 0 && exitCol >= 0) {
            float ex = exitCol * 40.0f;
            float ey = exitRow * 40.0f;
            if (!exitUnlocked) {
                DrawRectangle((int)ex, (int)ey, 40, 40, ColorAlpha(RED, 0.3f));
                DrawRectangleLinesEx(Rectangle{ex, ey, 40.0f, 40.0f}, 3.0f, RED);
                DrawLine((int)ex, (int)ey, (int)(ex + 40), (int)(ey + 40), RED);
                DrawLine((int)(ex + 40), (int)ey, (int)ex, (int)(ey + 40), RED);
            } else {
                Color gateColor = (((int)(GetTime() * 4) % 2) == 0) ? LIME : GREEN;
                DrawRectangle((int)ex + 8, (int)ey + 8, 24, 24, ColorAlpha(gateColor, 0.4f));
                DrawRectangleLinesEx(Rectangle{ex, ey, 40.0f, 40.0f}, 3.0f, gateColor);
            }
        }

        for (int i = 0; i < 128; ++i) {
            if (particles[i].life > 0.0f) {
                float pct = particles[i].life / particles[i].maxLife;
                DrawCircle((int)particles[i].x, (int)particles[i].y, 4.0f * pct, ColorAlpha(particles[i].color, pct));
            }
        }

        for (int i = 0; i < 3; ++i) {
            if (bombs[i].active) {
                float bx = bombs[i].col * 40.0f + 20.0f;
                float by = bombs[i].row * 40.0f + 20.0f;
                DrawBomb(bx, by, 12.0f, bombs[i].timeLeft);
            }
        }

        DrawPlayer(pX, pY, 12.0f);

        for (int i = 0; i < 3; ++i) {
            DrawGhost(ghosts[i].x, ghosts[i].y, 12.0f, i);
        }

        BeginBlendMode(BLEND_ADDITIVE);
        DrawCircleGradient((int)pX, (int)pY, 180.0f, Color{ 255, 255, 200, 45 }, Color{ 0, 0, 0, 0 });
        EndBlendMode();

        EndMode2D();

        DrawRectangle(800, 0, 200, 600, Color{15, 15, 22, 255});
        DrawRectangle(800, 0, 5, 600, DARKGRAY);

        DrawRectangleRounded(Rectangle{ 820.0f, 25.0f, 170.0f, 550.0f }, 0.05f, 4, Color{ 0, 0, 0, 100 });
        DrawRectangleRounded(Rectangle{ 815.0f, 20.0f, 170.0f, 550.0f }, 0.05f, 4, Color{ 25, 25, 35, 255 });
        DrawRectangleRoundedLines(Rectangle{ 815.0f, 20.0f, 170.0f, 550.0f }, 0.05f, 4, 2.0f, Color{ 60, 70, 90, 255 });

        DrawText("POKÉ-MAZE", 830, 50, 22, GOLD);

        DrawText("LEVEL", 830, 110, 12, LIGHTGRAY);
        DrawText(TextFormat("%d / 4", currentLevel), 830, 130, 24, WHITE);

        DrawText("SCORE", 830, 180, 12, LIGHTGRAY);
        DrawText(TextFormat("%05d", score), 830, 200, 24, LIME);

        DrawText("BOOSTER SPEED", 830, 250, 12, LIGHTGRAY);
        DrawText(TextFormat("%.1fx", pSpdBooster), 830, 270, 24, WHITE);

        DrawText("LIVES", 830, 320, 12, LIGHTGRAY);
        for (int i = 0; i < lives; ++i) {
            float bounce = sinf((float)GetTime() * 5.0f + i) * 3.0f;
            DrawPokeBall(840.0f + i * 35.0f, 360.0f + bounce, 10.0f);
        }

        if (state == LEVELUP) {
            DrawRectangle(0, 200, 1000, 200, ColorAlpha(BLACK, 0.8f));
            DrawText("LEVEL UP!", 360, 240, 50, GOLD);
            DrawText("Get ready for the next challenge...", 310, 310, 20, WHITE);
        }

        if (flashTime > 0.0f) {
            DrawRectangle(0, 0, 1000, 600, ColorAlpha(RED, (flashTime / 0.3f) * 0.4f));
        }
    }

    EndTextureMode();

    BeginDrawing();
    ClearBackground(BLACK);
    float scale = fminf((float)GetScreenWidth() / 1000.0f, (float)GetScreenHeight() / 600.0f);
    DrawTexturePro(screenTarget.texture,
                   Rectangle{ 0, 0, (float)screenTarget.texture.width, (float)-screenTarget.texture.height },
                   Rectangle{ ((float)GetScreenWidth() - ((float)1000.0f * scale)) * 0.5f,
                              ((float)GetScreenHeight() - ((float)600.0f * scale)) * 0.5f,
                              (float)1000.0f * scale, (float)600.0f * scale },
                   Vector2{ 0, 0 }, 0.0f, WHITE);
    EndDrawing();
}

int main() {
    InitWindow(1000, 600, "Poke-Maze Adventure");
    SetExitKey(KEY_NULL);
    InitAudioDevice();
    SetTargetFPS(60);

    screenTarget = LoadRenderTexture(1000, 600);
    SetTextureFilter(screenTarget.texture, TEXTURE_FILTER_BILINEAR);

    if (FileExists("highscore.txt")) {
        char *text = LoadFileText("highscore.txt");
        if (text) {
            int idx = 0;
            char *line = text;
            while (*line && idx < 5) {
                highScores[idx] = TextToInteger(line);
                idx++;
                while (*line && *line != '\n') line++;
                if (*line == '\n') line++;
            }
            UnloadFileText(text);
        }
    }

    for (int i = 0; i < 50; ++i) {
        stars[i].x = (float)GetRandomValue(0, 1000);
        stars[i].y = (float)GetRandomValue(0, 600);
        stars[i].spd = (float)GetRandomValue(20, 80) / 100.0f;
        stars[i].size = (float)GetRandomValue(2, 6);
    }

    for (int i = 0; i < 128; ++i) {
        particles[i].life = 0.0f;
    }

    camera.target = Vector2{ 0.0f, 0.0f };
    camera.offset = Vector2{ 0.0f, 0.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    collectSnd = LoadSound("audio/collect.wav");
    ghostSnd = LoadSound("audio/ghost.wav");
    gameOverSnd = LoadSound("audio/gameover.wav");
    victorySnd = LoadSound("audio/victory.wav");
    bgm = LoadSound("audio/bgm.wav");
    bombSnd = LoadSound("audio/bomb.wav");

    InitGame();

    while (!WindowShouldClose() && !shouldExit) {
        UpdateGame();
        DrawGame();
    }

    UnloadSound(collectSnd);
    UnloadSound(ghostSnd);
    UnloadSound(gameOverSnd);
    UnloadSound(victorySnd);
    UnloadSound(bgm);
    UnloadSound(bombSnd);

    UnloadRenderTexture(screenTarget);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
