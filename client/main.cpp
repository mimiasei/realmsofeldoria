#include <iostream>
#include <SDL2/SDL.h>
#include "../lib/gamestate/GameState.h"
#include "../lib/entities/hero/Hero.h"

class GameClient {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    GameState gameState;
    bool running;
    
public:
    GameClient() : window(nullptr), renderer(nullptr), running(false) {}
    
    ~GameClient() {
        cleanup();
    }
    
    bool initialize() {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
            return false;
        }
        
        window = SDL_CreateWindow(
            "Realms of Eldoria",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            1024, 768,
            SDL_WINDOW_SHOWN
        );
        
        if (!window) {
            std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
            return false;
        }
        
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (!renderer) {
            std::cerr << "Renderer creation failed: " << SDL_GetError() << std::endl;
            return false;
        }
        
        // Initialize game state
        initializeGame();
        
        running = true;
        return true;
    }
    
    void initializeGame() {
        // Create a test player
        auto player = std::make_unique<Player>(1, "Player 1", Faction::Castle, true);
        player->getResources().gold = 10000;
        player->getResources().wood = 20;
        player->getResources().ore = 20;
        
        // Create a test hero
        auto hero = std::make_unique<Hero>(1, "Test Hero", HeroClass::Knight, Gender::Male);
        hero->setPrimaryStats(5, 5, 1, 1);
        hero->setPosition(Position(10, 10, 0));
        
        player->addHero(1);
        gameState.addHero(std::move(hero));
        gameState.addPlayer(std::move(player));
        
        gameState.startGame();
    }
    
    void run() {
        if (!initialize()) {
            return;
        }
        
        while (running) {
            handleEvents();
            update();
            render();
            
            SDL_Delay(16); // ~60 FPS
        }
    }
    
    void handleEvents() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_KEYDOWN:
                    handleKeyPress(event.key.keysym.sym);
                    break;
            }
        }
    }
    
    void handleKeyPress(SDL_Keycode key) {
        switch (key) {
            case SDLK_ESCAPE:
                running = false;
                break;
            case SDLK_SPACE:
                gameState.nextTurn();
                std::cout << "Turn " << gameState.getTurnManager().getTurnNumber() 
                         << ", Day " << gameState.getTurnManager().getDayNumber() << std::endl;
                break;
        }
    }
    
    void update() {
        // Game logic updates would go here
        if (gameState.getTurnManager().isNewWeek()) {
            gameState.processWeeklyEvents();
        }
    }
    
    void render() {
        // Clear screen with dark blue
        SDL_SetRenderDrawColor(renderer, 0, 0, 64, 255);
        SDL_RenderClear(renderer);
        
        // Simple rendering placeholder
        renderUI();
        
        SDL_RenderPresent(renderer);
    }
    
    void renderUI() {
        // Simple UI placeholder - draw a white rectangle
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_Rect rect = {50, 50, 200, 100};
        SDL_RenderFillRect(renderer, &rect);
        
        // Draw some debug info areas
        SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
        SDL_Rect infoRect = {300, 50, 300, 200};
        SDL_RenderDrawRect(renderer, &infoRect);
    }
    
    void cleanup() {
        if (renderer) {
            SDL_DestroyRenderer(renderer);
            renderer = nullptr;
        }
        
        if (window) {
            SDL_DestroyWindow(window);
            window = nullptr;
        }
        
        SDL_Quit();
    }
};

int main(int argc, char* argv[]) {
    std::cout << "Starting Realms of Eldoria..." << std::endl;
    
    GameClient client;
    client.run();
    
    std::cout << "Game ended." << std::endl;
    return 0;
}