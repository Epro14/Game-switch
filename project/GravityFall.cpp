#include "GravityFall.hpp"
#include <switch.h>
#include <SDL2/SDL_image.h>
#include <vector>
#include <random> // Per generare ostacoli casuali

// Struttura per gli ostacoli fluttuanti
struct FloatingObstacle {
    float x, y;
    SDL_Texture* texture;
    SDL_Rect rect;

    FloatingObstacle(float startX, float startY, SDL_Texture* tex) 
        : x(startX), y(startY), texture(tex) {
        SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
        rect.x = (int)x;
        rect.y = (int)y;
    }

    void update(float scrollSpeed) {
        y += scrollSpeed;
        rect.y = (int)y;
    }

    void render(SDL_Renderer* renderer) {
        SDL_RenderCopy(renderer, texture, NULL, &rect);
    }
};

// Struttura per i pickup
struct Pickup {
    float x, y;
    SDL_Texture* texture;
    SDL_Rect rect;

    Pickup(float startX, float startY, SDL_Texture* tex) 
        : x(startX), y(startY), texture(tex) {
        SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
        rect.x = (int)x;
        rect.y = (int)y;
    }

    void update(float scrollSpeed) {
        y += scrollSpeed;
        rect.y = (int)y;
    }

    void render(SDL_Renderer* renderer) {
        SDL_RenderCopy(renderer, texture, NULL, &rect);
    }
};

void startGravityFall(SDL_Renderer* renderer) {
    bool inGame = true;
    
    // Generatore numeri casuali
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist_x(50, SCREEN_W - 150); // Posizione X degli ostacoli
    std::uniform_int_distribution<> dist_gap(100, 300); // Distanza tra gli ostacoli
    std::uniform_int_distribution<> dist_pickup_x(50, SCREEN_W - 80);

    // Caricamento Texture
    SDL_Texture* playerTex = IMG_LoadTexture(renderer, "romfs:/gfx/stella_player_gf.png");
    SDL_Texture* obstacleTex = IMG_LoadTexture(renderer, "romfs:/gfx/ostacolo_floating.png");
    SDL_Texture* bgTex = IMG_LoadTexture(renderer, "romfs:/gfx/sfondo_gravity_fall.png");
    SDL_Texture* pickupTex = IMG_LoadTexture(renderer, "romfs:/gfx/pickup_star.png");

    if (!playerTex || !obstacleTex || !bgTex || !pickupTex) {
        // Gestione errore caricamento immagini
        return; 
    }

    // Variabili Giocatore
    float playerX = SCREEN_W / 2 - 32; // Centrato
    float playerY = 50; // Inizia in alto
    float playerVelX = 0;
    const float playerSpeed = 8.0f;
    SDL_Rect playerRect = { (int)playerX, (int)playerY, 64, 64 }; // Dimensione stella

    // Variabili Gioco
    float scrollSpeed = 5.0f; // Velocità di scorrimento degli ostacoli (e quindi della caduta)
    int score = 0;
    
    std::vector<FloatingObstacle> obstacles;
    std::vector<Pickup> pickups;
    float nextObstacleY = playerY + 200; // Posizione Y del prossimo ostacolo

    while (appletMainLoop() && inGame) {
        hidScanInput();
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
        u64 kHeld = hidKeysHeld(CONTROLLER_P1_AUTO);

        if (kDown & KEY_PLUS) inGame = false; // Esci dal gioco
        if (kDown & KEY_B) inGame = false; // Torna al menu

        // Movimento orizzontale del giocatore
        playerVelX = 0;
        if (kHeld & KEY_DRIGHT) playerVelX = playerSpeed;
        if (kHeld & KEY_DLEFT) playerVelX = -playerSpeed;
        playerX += playerVelX;

        // Limita il giocatore allo schermo
        if (playerX < 0) playerX = 0;
        if (playerX > SCREEN_W - playerRect.w) playerX = SCREEN_W - playerRect.w;
        playerRect.x = (int)playerX;

        // --- Gestione Ostacoli ---
        // Genera nuovi ostacoli se necessario
        while (nextObstacleY < SCREEN_H + 100) { // Genera anche fuori dallo schermo per un effetto migliore
            obstacles.emplace_back(dist_x(gen), nextObstacleY - SCREEN_H, obstacleTex); // Genera sopra lo schermo
            
            // Possibilità di aggiungere un pickup vicino all'ostacolo
            if (std::uniform_int_distribution<>(0,1)(gen) == 0) { // 50% di possibilità
                pickups.emplace_back(dist_pickup_x(gen), nextObstacleY - SCREEN_H + 50, pickupTex);
            }
            nextObstacleY += dist_gap(gen);
        }

        // Aggiorna e pulisci ostacoli e pickup
        for (auto it = obstacles.begin(); it != obstacles.end(); ) {
            it->update(scrollSpeed);
            if (it->y > SCREEN_H) { // Se l'ostacolo è uscito dallo schermo
                it = obstacles.erase(it);
            } else {
                ++it;
            }
        }
        for (auto it = pickups.begin(); it != pickups.end(); ) {
            it->update(scrollSpeed);
            if (it->y > SCREEN_H) {
                it = pickups.erase(it);
            } else {
                ++it;
            }
        }

        // --- Collisioni ---
        // Con ostacoli
        for (const auto& obs : obstacles) {
            if (SDL_HasIntersection(&playerRect, &obs.rect)) {
                inGame = false; // Game Over
            }
        }
        // Con pickup
        for (auto it = pickups.begin(); it != pickups.end(); ) {
            if (SDL_HasIntersection(&playerRect, &it->rect)) {
                score += 10; // Aumenta il punteggio
                it = pickups.erase(it); // Rimuovi il pickup
            } else {
                ++it;
            }
        }

        // --- RENDERING ---
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, bgTex, NULL, NULL); // Sfondo

        // Disegna ostacoli
        for (const auto& obs : obstacles) {
            obs.render(renderer);
        }
        // Disegna pickup
        for (const auto& p : pickups) {
            p.render(renderer);
        }
        
        // Disegna giocatore
        SDL_RenderCopy(renderer, playerTex, NULL, &playerRect);

        // Renderizza il punteggio (richiede SDL_ttf, per ora solo un segnaposto)
        // std::string scoreText = "Score: " + std::to_string(score);
        // ... (SDL_ttf code for rendering text) ...

        SDL_RenderPresent(renderer);
    }

    // Pulizia risorse
    SDL_DestroyTexture(playerTex);
    SDL_DestroyTexture(obstacleTex);
    SDL_DestroyTexture(bgTex);
    SDL_DestroyTexture(pickupTex);
}