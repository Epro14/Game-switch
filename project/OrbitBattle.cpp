#include "OrbitBattle.hpp"
#include <switch.h>
#include <SDL2/SDL_image.h>
#include <vector>
#include <cmath>

struct Bullet {
    float x, y, vx, vy;
};

void startOrbitBattle(SDL_Renderer* renderer) {
    bool inGame = true;

    // Caricamento Texture
    SDL_Texture* p1Tex = IMG_LoadTexture(renderer, "romfs:/gfx/stella_p1.png");
    SDL_Texture* p2Tex = IMG_LoadTexture(renderer, "romfs:/gfx/stella_p2.png");
    SDL_Texture* planetTex = IMG_LoadTexture(renderer, "romfs:/gfx/pianeta_centrale.png");
    SDL_Texture* bulletTex = IMG_LoadTexture(renderer, "romfs:/gfx/proiettile_scia.png");

    // Posizioni iniziali
    float p1X = 300, p1Y = 360;
    float p2X = 980, p2Y = 360;
    std::vector<Bullet> bullets;

    while (appletMainLoop() && inGame) {
        hidScanInput();
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
        u64 kHeld = hidKeysHeld(CONTROLLER_P1_AUTO);

        if (kDown & KEY_B) inGame = false;

        // --- MOVIMENTO P1 (Esempio semplificato con levette/frecce) ---
        if (kHeld & KEY_DUP)    p1Y -= 5;
        if (kHeld & KEY_DDOWN)  p1Y += 5;
        if (kHeld & KEY_DLEFT)  p1X -= 5;
        if (kHeld & KEY_DRIGHT) p1X += 5;

        // Sparo P1 (Tasto A)
        if (kDown & KEY_A) {
            bullets.push_back({p1X + 32, p1Y + 16, 10, 0});
        }

        // --- LOGICA PROIETTILI ---
        for (auto& b : bullets) {
            b.x += b.vx;
            // Qui potresti aggiungere la gravità del pianeta che devia i colpi!
        }

        // --- DISEGNO ---
        SDL_RenderClear(renderer);
        
        // Disegna Pianeta al centro
        SDL_Rect planetRect = { 1280/2 - 128, 720/2 - 128, 256, 256 };
        SDL_RenderCopy(renderer, planetTex, NULL, &planetRect);

        // Disegna Giocatori
        SDL_Rect p1Rect = { (int)p1X, (int)p1Y, 64, 64 };
        SDL_Rect p2Rect = { (int)p2X, (int)p2Y, 64, 64 };
        SDL_RenderCopy(renderer, p1Tex, NULL, &p1Rect);
        SDL_RenderCopy(renderer, p2Tex, NULL, &p2Rect);

        // Disegna Proiettili
        for (const auto& b : bullets) {
            SDL_Rect bRect = { (int)b.x, (int)b.y, 24, 24 };
            SDL_RenderCopy(renderer, bulletTex, NULL, &bRect);
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(p1Tex);
    SDL_DestroyTexture(p2Tex);
    SDL_DestroyTexture(planetTex);
    SDL_DestroyTexture(bulletTex);
}