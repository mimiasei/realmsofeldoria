#include <iostream>
#include "../lib/gamestate/GameState.h"

int main(int argc, char* argv[]) {
    std::cout << "Realms of Eldoria Server starting..." << std::endl;
    
    GameState gameState;
    
    // Create a simple server game state
    auto player1 = std::make_unique<Player>(1, "Server Player 1", Faction::Castle, false);
    auto player2 = std::make_unique<Player>(2, "Server Player 2", Faction::Rampart, false);
    
    gameState.addPlayer(std::move(player1));
    gameState.addPlayer(std::move(player2));
    
    gameState.startGame();
    
    std::cout << "Server initialized with " << gameState.getAllPlayers().size() << " players." << std::endl;
    std::cout << "Current player: " << gameState.getCurrentPlayer() << std::endl;
    
    // Simple server loop (placeholder)
    for (int turn = 0; turn < 5; turn++) {
        std::cout << "Processing turn " << gameState.getTurnManager().getTurnNumber() << std::endl;
        gameState.nextTurn();
        gameState.processDailyEvents();
    }
    
    std::cout << "Server shutting down." << std::endl;
    return 0;
}