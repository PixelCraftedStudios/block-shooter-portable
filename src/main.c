#include "raylib.h"
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define SCREEN_W 1280
#define SCREEN_H 720

#define MAX_E 50
#define MAX_B 100

typedef struct {
    Vector2 p;
    Texture2D t;
    bool a;
} E; // enemy struct = pathetic sausage

typedef struct {
    Vector2 p, d;
    float s;
    Texture2D t;
    bool a;
} B; // bullets sausage

int main(void)
{
    InitWindow(SCREEN_W, SCREEN_H, "Block Shooter - Lightweight Port");
    SetTargetFPS(60);
    srand((unsigned int)time(NULL));
    InitAudioDevice();

    // im not paid
    Texture2D P  = LoadTexture("assets/player.png");
    Texture2D G  = LoadTexture("assets/gun1.png");
    Texture2D Bu = LoadTexture("assets/bullet.png");
    Texture2D En = LoadTexture("assets/enemy.png");

    Music bgm   = LoadMusicStream("assets/audio/music/mus.wav");
    Sound crunch = LoadSound("assets/audio/crunch.wav");

    if (bgm.stream.buffer == NULL) {
        TraceLog(LOG_ERROR, "FAILED TO LOAD MUSIC! Crashing Game.");
    } else {
        PlayMusicStream(bgm);
        SetMusicVolume(bgm, 0.95f); // 0.95f is such a strange amount
    }

    float playerSpeed = 2.0f;
    float scale = 0.333f;
    float gunDist = 18.0f;

    Vector2 playerPos = {
        SCREEN_W / 2.0f - (P.width * scale) / 2,
        SCREEN_H - 90.0f
    }; // awfully harcoded numbers

    E enemies[MAX_E] = { 0 };
    B bullets[MAX_B] = { 0 }; // arrays

    float spawnTimer = 0.0f;
    float spawnInterval = 1.5f; // floats :)

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime(); // frame time cuz I'm not smart enough to use fixed timestep
        UpdateMusicStream(bgm);

        // player movement - hardcoded keyboard keys what
        if (IsKeyDown(KEY_W)) playerPos.y -= playerSpeed;
        if (IsKeyDown(KEY_S)) playerPos.y += playerSpeed;
        if (IsKeyDown(KEY_A)) playerPos.x -= playerSpeed;
        if (IsKeyDown(KEY_D)) playerPos.x += playerSpeed;

        Vector2 playerCenter = {
            playerPos.x + (P.width * scale) / 2,
            playerPos.y + (P.height * scale) / 2
        }; // player center = math that i understand but don't know why im using on a hardcoded resolution

        Vector2 mouse = GetMousePosition();
        float gunAngle = atan2f(mouse.y - playerCenter.y, mouse.x - playerCenter.x); // aim at mouse
        float gunOffsetY = 10.0f; // gun offset, // EDIT: Still feels bad after testing

        Vector2 gunPos = {
            playerCenter.x + cosf(gunAngle) * gunDist,
            playerCenter.y + sinf(gunAngle) * gunDist + gunOffsetY
        }; // gun position

        // shoot bullets
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            for (int i = 0; i < MAX_B; i++) {
                if (!bullets[i].a) {
                    bullets[i].a = true;
                    bullets[i].p = gunPos;
                    bullets[i].d = (Vector2){ cosf(gunAngle), sinf(gunAngle) };
                    bullets[i].s = 3.0f;
                    bullets[i].t = Bu;
                    break;
                }
            }
        }

        // move bullets, hope they hit something
        for (int i = 0; i < MAX_B; i++) {
            if (!bullets[i].a) continue;

            bullets[i].p.x += bullets[i].d.x * bullets[i].s;
            bullets[i].p.y += bullets[i].d.y * bullets[i].s;

            if (bullets[i].p.x < 0 || bullets[i].p.x > SCREEN_W ||
                bullets[i].p.y < 0 || bullets[i].p.y > SCREEN_H) {
                bullets[i].a = false; // offscreen
            }
        }

        // nobody gives a sausage meat stuffing about these enemies
        spawnTimer += dt;
        if (spawnTimer >= spawnInterval) {
            for (int i = 0; i < MAX_E; i++) {
                if (!enemies[i].a) {
                    enemies[i].a = true;
                    enemies[i].t = En;
                    enemies[i].p.x = rand() % (SCREEN_W - (int)(En.width * scale));
                    enemies[i].p.y = -En.height;
                    break;
                }
            }
            spawnTimer = 0.0f;
        }

        // move enemies towards, because A* is too hard in C and C++, It was hard enough in C#
        for (int i = 0; i < MAX_E; i++) {
            if (!enemies[i].a) continue;

            Vector2 dir = {
                playerCenter.x - enemies[i].p.x,
                playerCenter.y - enemies[i].p.y
            };

            float len = sqrtf(dir.x * dir.x + dir.y * dir.y);
            if (len != 0) {
                dir.x /= len;
                dir.y /= len;
            }

            enemies[i].p.x += dir.x;
            enemies[i].p.y += dir.y;

            if (enemies[i].p.y > SCREEN_H + 60) {
                enemies[i].a = false; // bro fell off
            }
        }

        // bullet hits enemy
        for (int i = 0; i < MAX_B; i++) {
            if (!bullets[i].a) continue;

            Rectangle bulletRect = {
                bullets[i].p.x,
                bullets[i].p.y,
                Bu.width * 0.25f,
                Bu.height * 0.25f
            };

            for (int j = 0; j < MAX_E; j++) {
                if (!enemies[j].a) continue;

                Rectangle enemyRect = {
                    enemies[j].p.x,
                    enemies[j].p.y,
                    En.width * scale,
                    En.height * scale
                };

                if (CheckCollisionRecs(bulletRect, enemyRect)) {
                    bullets[i].a = false;
                    enemies[j].a = false;
                    PlaySound(crunch); // bullet kill = killemall
                    break;
                }
            }
        }

        // player touch enemy = reset everything, because I can't handle proper death screen
        Rectangle playerRect = {
            playerPos.x,
            playerPos.y,
            P.width * scale,
            P.height * scale
        };

        for (int i = 0; i < MAX_E; i++) {
            if (!enemies[i].a) continue;

            Rectangle enemyRect = {
                enemies[i].p.x,
                enemies[i].p.y,
                En.width * scale,
                En.height * scale
            };

            if (CheckCollisionRecs(playerRect, enemyRect)) {
                // player oh no more
                playerPos = (Vector2){
                    SCREEN_W / 2.0f - (P.width * scale) / 2,
                    SCREEN_H - 90.0f
                };

                for (int j = 0; j < MAX_E; j++) enemies[j].a = false;
                for (int j = 0; j < MAX_B; j++) bullets[j].a = false;
                spawnTimer = 0.0f;
                break;
            }
        }

        BeginDrawing();
        ClearBackground(DARKGREEN);

        DrawTextureEx(P, playerPos, 0.0f, scale, WHITE);

        for (int i = 0; i < MAX_E; i++)
            if (enemies[i].a)
                DrawTextureEx(enemies[i].t, enemies[i].p, 0.0f, scale, WHITE);

        for (int i = 0; i < MAX_B; i++)
            if (bullets[i].a)
                DrawTextureEx(bullets[i].t, bullets[i].p, 0.0f, 0.25f, WHITE);

        // draw gun
        Rectangle src = { 0, 0, G.width, G.height };
        Rectangle dst = { gunPos.x, gunPos.y, G.width * 0.25f, G.height * 0.25f };
        Vector2 origin = { 0, G.height / 2.0f };

        DrawTexturePro(G, src, dst, origin, gunAngle * RAD2DEG, WHITE);

        EndDrawing();
    }

    // what the frick is this chunk of code
    UnloadTexture(P);
    UnloadTexture(G);
    UnloadTexture(Bu);
    UnloadTexture(En);
    UnloadSound(crunch);
    UnloadMusicStream(bgm);
    CloseAudioDevice();
    CloseWindow(); // is the library really that picky?!
    //
    return 0;
}
#include "raylib.h"
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define SCREEN_W 1280
#define SCREEN_H 720

#define MAX_E 50
#define MAX_B 100

typedef struct {
    Vector2 p;
    Texture2D t;
    bool a;
} E; // enemy struct = pathetic sausage

typedef struct {
    Vector2 p, d;
    float s;
    Texture2D t;
    bool a;
} B; // bullets sausage

int main(void)
{
    InitWindow(SCREEN_W, SCREEN_H, "Block Shooter - Lightweight Port");
    SetTargetFPS(60);
    srand((unsigned int)time(NULL));
    InitAudioDevice();

    // im not paid
    Texture2D P  = LoadTexture("assets/player.png");
    Texture2D G  = LoadTexture("assets/gun1.png");
    Texture2D Bu = LoadTexture("assets/bullet.png");
    Texture2D En = LoadTexture("assets/enemy.png");

    Music bgm   = LoadMusicStream("assets/audio/music/mus.wav");
    Sound crunch = LoadSound("assets/audio/crunch.wav");

    if (bgm.stream.buffer == NULL) {
        TraceLog(LOG_ERROR, "FAILED TO LOAD MUSIC! Crashing Game.");
    } else {
        PlayMusicStream(bgm);
        SetMusicVolume(bgm, 0.95f); // 0.95f is such a strange amount
    }

    float playerSpeed = 2.0f;
    float scale = 0.333f;
    float gunDist = 18.0f;

    Vector2 playerPos = {
        SCREEN_W / 2.0f - (P.width * scale) / 2,
        SCREEN_H - 90.0f
    }; // awfully harcoded numbers

    E enemies[MAX_E] = { 0 };
    B bullets[MAX_B] = { 0 }; // arrays

    float spawnTimer = 0.0f;
    float spawnInterval = 1.5f; // floats :)

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime(); // frame time cuz I'm not smart enough to use fixed timestep
        UpdateMusicStream(bgm);

        // player movement - hardcoded keyboard keys what
        if (IsKeyDown(KEY_W)) playerPos.y -= playerSpeed;
        if (IsKeyDown(KEY_S)) playerPos.y += playerSpeed;
        if (IsKeyDown(KEY_A)) playerPos.x -= playerSpeed;
        if (IsKeyDown(KEY_D)) playerPos.x += playerSpeed;

        Vector2 playerCenter = {
            playerPos.x + (P.width * scale) / 2,
            playerPos.y + (P.height * scale) / 2
        }; // player center = math that i understand but don't know why im using on a hardcoded resolution

        Vector2 mouse = GetMousePosition();
        float gunAngle = atan2f(mouse.y - playerCenter.y, mouse.x - playerCenter.x); // aim at mouse
        float gunOffsetY = 10.0f; // gun offset, // EDIT: Still feels bad after testing

        Vector2 gunPos = {
            playerCenter.x + cosf(gunAngle) * gunDist,
            playerCenter.y + sinf(gunAngle) * gunDist + gunOffsetY
        }; // gun position

        // shoot bullets
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            for (int i = 0; i < MAX_B; i++) {
                if (!bullets[i].a) {
                    bullets[i].a = true;
                    bullets[i].p = gunPos;
                    bullets[i].d = (Vector2){ cosf(gunAngle), sinf(gunAngle) };
                    bullets[i].s = 3.0f;
                    bullets[i].t = Bu;
                    break;
                }
            }
        }

        // move bullets, hope they hit something
        for (int i = 0; i < MAX_B; i++) {
            if (!bullets[i].a) continue;

            bullets[i].p.x += bullets[i].d.x * bullets[i].s;
            bullets[i].p.y += bullets[i].d.y * bullets[i].s;

            if (bullets[i].p.x < 0 || bullets[i].p.x > SCREEN_W ||
                bullets[i].p.y < 0 || bullets[i].p.y > SCREEN_H) {
                bullets[i].a = false; // offscreen
            }
        }

        // nobody gives a sausage meat stuffing about these enemies
        spawnTimer += dt;
        if (spawnTimer >= spawnInterval) {
            for (int i = 0; i < MAX_E; i++) {
                if (!enemies[i].a) {
                    enemies[i].a = true;
                    enemies[i].t = En;
                    enemies[i].p.x = rand() % (SCREEN_W - (int)(En.width * scale));
                    enemies[i].p.y = -En.height;
                    break;
                }
            }
            spawnTimer = 0.0f;
        }

        // move enemies towards, because A* is too hard in C and C++, It was hard enough in C#
        for (int i = 0; i < MAX_E; i++) {
            if (!enemies[i].a) continue;

            Vector2 dir = {
                playerCenter.x - enemies[i].p.x,
                playerCenter.y - enemies[i].p.y
            };

            float len = sqrtf(dir.x * dir.x + dir.y * dir.y);
            if (len != 0) {
                dir.x /= len;
                dir.y /= len;
            }

            enemies[i].p.x += dir.x;
            enemies[i].p.y += dir.y;

            if (enemies[i].p.y > SCREEN_H + 60) {
                enemies[i].a = false; // bro fell off
            }
        }

        // bullet hits enemy
        for (int i = 0; i < MAX_B; i++) {
            if (!bullets[i].a) continue;

            Rectangle bulletRect = {
                bullets[i].p.x,
                bullets[i].p.y,
                Bu.width * 0.25f,
                Bu.height * 0.25f
            };

            for (int j = 0; j < MAX_E; j++) {
                if (!enemies[j].a) continue;

                Rectangle enemyRect = {
                    enemies[j].p.x,
                    enemies[j].p.y,
                    En.width * scale,
                    En.height * scale
                };

                if (CheckCollisionRecs(bulletRect, enemyRect)) {
                    bullets[i].a = false;
                    enemies[j].a = false;
                    PlaySound(crunch); // bullet kill = killemall
                    break;
                }
            }
        }

        // player touch enemy = reset everything, because I can't handle proper death screen
        Rectangle playerRect = {
            playerPos.x,
            playerPos.y,
            P.width * scale,
            P.height * scale
        };

        for (int i = 0; i < MAX_E; i++) {
            if (!enemies[i].a) continue;

            Rectangle enemyRect = {
                enemies[i].p.x,
                enemies[i].p.y,
                En.width * scale,
                En.height * scale
            };

            if (CheckCollisionRecs(playerRect, enemyRect)) {
                // player oh no more
                playerPos = (Vector2){
                    SCREEN_W / 2.0f - (P.width * scale) / 2,
                    SCREEN_H - 90.0f
                };

                for (int j = 0; j < MAX_E; j++) enemies[j].a = false;
                for (int j = 0; j < MAX_B; j++) bullets[j].a = false;
                spawnTimer = 0.0f;
                break;
            }
        }

        BeginDrawing();
        ClearBackground(DARKGREEN);

        DrawTextureEx(P, playerPos, 0.0f, scale, WHITE);

        for (int i = 0; i < MAX_E; i++)
            if (enemies[i].a)
                DrawTextureEx(enemies[i].t, enemies[i].p, 0.0f, scale, WHITE);

        for (int i = 0; i < MAX_B; i++)
            if (bullets[i].a)
                DrawTextureEx(bullets[i].t, bullets[i].p, 0.0f, 0.25f, WHITE);

        // draw gun
        Rectangle src = { 0, 0, G.width, G.height };
        Rectangle dst = { gunPos.x, gunPos.y, G.width * 0.25f, G.height * 0.25f };
        Vector2 origin = { 0, G.height / 2.0f };

        DrawTexturePro(G, src, dst, origin, gunAngle * RAD2DEG, WHITE);

        EndDrawing();
    }

    // what the frick is this chunk of code
    UnloadTexture(P);
    UnloadTexture(G);
    UnloadTexture(Bu);
    UnloadTexture(En);
    UnloadSound(crunch);
    UnloadMusicStream(bgm);
    CloseAudioDevice();
    CloseWindow(); // is the library really that picky?!
    //
    return 0;
}
