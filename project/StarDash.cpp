#include <switch.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>

// Struttura per gestire gli ostacoli
struct Obstacle {
    float x, y;
};

void startStarDash(SDL_Renderer* renderer) {
    bool inGame = true;
    
    // Caricamento Texture
    SDL_Surface* starSurf = IMG_Load("romfs:/gfx/stella_player.png");
    SDL_Texture* starTex = SDL_CreateTextureFromSurface(renderer, starSurf);
    SDL_FreeSurface(starSurf);

    SDL_Surface* floorSurf = IMG_Load("romfs:/gfx/pavimento_spaziale.png");
    SDL_Texture* floorTex = SDL_CreateTextureFromSurface(renderer, floorSurf);
    SDL_FreeSurface(floorSurf);

    // Variabili Giocatore
    float playerY = 500.0f;
    float velocityY = 0.0f;
    bool isJumping = false;
    float gravity = 0.8f;

    // Gestione Ostacoli
    std::vector<Obstacle> obstacles;
    int spawnTimer = 0;

    while (appletMainLoop() && inGame) {
        hidScanInput();
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

        if (kDown & KEY_B) inGame = false; // Torna al menu

        // Salto
        if ((kDown & KEY_A) && !isJumping) {
            velocityY = -15.0f;
            isJumping = true;
        }

        // Fisica Gravità
        playerY += velocityY;
        if (playerY < 500.0f) {
            velocityY += gravity;
        } else {
            playerY = 500.0f;
            isJumping = false;
            velocityY = 0;
        }

        // Generazione Ostacoli
        spawnTimer++;
        if (spawnTimer > 100) {
            obstacles.push_back({1300, 500});
            spawnTimer = 0;
        }

        // Muovi Ostacoli
        for (int i = 0; i < obstacles.size(); i++) {
            obstacles[i].x -= 8.0f; // Velocità della corsa
            // Controllo collisione semplificato
            if (obstacles[i].x < 100 && obstacles[i].x > 40 && playerY > 450) {
                inGame = false; // Game Over (torna al menu)
            }
        }

        // --- DISEGNO ---
        SDL_SetRenderDrawColor(renderer, 10, 10, 30, 255);
        SDL_RenderClear(renderer);

        // Disegna Pavimento (ripetuto)
        for(int i=0; i<10; i++) {
            SDL_Rect fRect = { i*128, 564, 128, 128 };
            SDL_RenderCopy(renderer, floorTex, NULL, &fRect);
        }

        // Disegna Giocatore
        SDL_Rect playerRect = { 64, (int)playerY, 64, 64 };
        SDL_RenderCopy(renderer, starTex, NULL, &playerRect);

        // Disegna Ostacoli
        for (const auto& o : obstacles) {
            SDL_Rect oRect = { (int)o.x, (int)o.y, 64, 64 };
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Rosso se manca l'immagine
            SDL_RenderFillRect(renderer, &oRect); 
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(starTex);
    SDL_DestroyTexture(floorTex);
}