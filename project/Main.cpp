#include <switch.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>
#include <string>
#include <cmath>

// Inclusione degli header dei giochi pronti
#include "StarDash.hpp" 
#include "GravityFall.hpp"
#include "OrbitBattle.hpp"

const int SCREEN_W = 1280;
const int SCREEN_H = 720;

int main(int argc, char* argv[]) {
    romfsInit();
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);
    IMG_Init(IMG_INIT_PNG);

    SDL_Window* window = SDL_CreateWindow("Star Clash", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_W, SCREEN_H, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    // Caricamento Sfondo Menu
    SDL_Texture* bgTex = IMG_LoadTexture(renderer, "romfs:/gfx/bg_menu.png");

    // Caricamento dei 5 loghi
    std::vector<std::string> paths = {
        "romfs:/gfx/logo-star-dash.png",
        "romfs:/gfx/logo-gravity-fall.png",
        "romfs:/gfx/logo-orbit-battle.png",
        "romfs:/gfx/logo-light-up.png",
        "romfs:/gfx/logo-meteor-shower.png"
    };
    
    std::vector<SDL_Texture*> logos;
    for (const auto& p : paths) {
        logos.push_back(IMG_LoadTexture(renderer, p.c_str()));
    }

    int selezione = 0;
    float pulseTimer = 0; 
    int errorTimer = 0; // Timer per mostrare l'errore
    bool running = true;

    while (appletMainLoop() && running) {
        hidScanInput();
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

        if (kDown & KEY_PLUS) running = false;
        if (kDown & KEY_DRIGHT) selezione = (selezione + 1) % 5;
        if (kDown & KEY_DLEFT)  selezione = (selezione - 1 + 5) % 5;

        // --- GESTIONE AVVIO E ERRORI ---
        if (kDown & KEY_A) {
            if (selezione == 0) {
                startStarDash(renderer);
            } else if (selezione == 1) {
                startGravityFall(renderer);
            } else if (selezione == 2) {
                startOrbitBattle(renderer);
            } else {
                // Se selezioni il 4° o il 5° gioco, attiva il timer di errore
                errorTimer = 60; 
            }
        }

        // --- DISEGNO ---
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, bgTex, NULL, NULL);

        pulseTimer += 0.08f;
        int pulse = (int)(sin(pulseTimer) * 15); 

        // Se c'è un errore, il bordo diventa ROSSO, altrimenti GIALLO
        if (errorTimer > 0) {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Rosso
            errorTimer--;
        } else {
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Giallo Neon
        }

        // Disegno rettangolo selezione
        SDL_Rect selRect = { (SCREEN_W/2 - 200) - pulse/2, (SCREEN_H/2 - 200) - pulse/2, 400 + pulse, 400 + pulse };
        SDL_RenderDrawRect(renderer, &selRect);
        
        // Disegno Logo
        if (logos[selezione]) {
            SDL_Rect logoRect = { SCREEN_W/2 - 150, SCREEN_H/2 - 150, 300, 300 };
            SDL_RenderCopy(renderer, logos[selezione], NULL, &logoRect);
        }

        // Messaggio di errore testuale semplificato (Rettangolo rosso in basso)
        if (errorTimer > 0) {
            SDL_Rect errBar = { SCREEN_W/2 - 200, 600, 400, 50 };
            SDL_SetRenderDrawColor(renderer, 200, 0, 0, 255);
            SDL_RenderFillRect(renderer, &errBar);
            // Nota: Se hai SDL_ttf, qui scriveresti "GIOCO NON DISPONIBILE"
        }

        SDL_RenderPresent(renderer);
    }

    // Pulizia
    for (auto t : logos) if(t) SDL_DestroyTexture(t);
    SDL_DestroyTexture(bgTex);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    romfsExit();
    SDL_Quit();
    return 0;
}