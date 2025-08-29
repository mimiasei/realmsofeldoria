#include "GameState.h"
#include <algorithm>

void Player::removeHero(HeroID heroId) {
    heroes.erase(std::remove(heroes.begin(), heroes.end(), heroId), heroes.end());
}

void Player::removeTown(TownID townId) {
    towns.erase(std::remove(towns.begin(), towns.end(), townId), towns.end());
}

Player::Player(PlayerID id, const std::string& name, Faction faction, bool human)
    : id(id), name(name), faction(faction), isHuman(human), isActive(true) {
}

TurnManager::TurnManager() : currentPlayerIndex(0), turnNumber(1), dayNumber(1) {
}

void TurnManager::initializeTurnOrder(const std::vector<PlayerID>& players) {
    playerOrder = players;
    currentPlayerIndex = 0;
}

PlayerID TurnManager::getCurrentPlayer() const {
    if (playerOrder.empty()) {
        return 0;
    }
    return playerOrder[currentPlayerIndex];
}

void TurnManager::nextPlayer() {
    if (playerOrder.empty()) {
        return;
    }
    
    currentPlayerIndex = (currentPlayerIndex + 1) % playerOrder.size();
    
    // If we've cycled through all players, it's a new turn
    if (currentPlayerIndex == 0) {
        nextTurn();
    }
}

void TurnManager::nextTurn() {
    turnNumber++;
    dayNumber++;
}

std::map<CreatureID, std::unique_ptr<Creature>> GameState::creatureDatabase;

GameState::GameState() : gameRunning(false), gameWon(false), winner(0), difficulty(GameDifficulty::Normal) {
}

GameState::~GameState() = default;

void GameState::startGame() {
    if (players.empty()) {
        return;
    }
    
    // Initialize turn order
    std::vector<PlayerID> playerIds;
    for (const auto& [id, player] : players) {
        playerIds.push_back(id);
    }
    turnManager.initializeTurnOrder(playerIds);
    
    gameRunning = true;
    gameWon = false;
    
    // Load creature database if not already loaded
    if (creatureDatabase.empty()) {
        loadCreatureDatabase();
    }
}

void GameState::endGame(PlayerID winnerPlayer) {
    gameRunning = false;
    gameWon = true;
    winner = winnerPlayer;
}

Player* GameState::getPlayer(PlayerID id) {
    auto it = players.find(id);
    return (it != players.end()) ? it->second.get() : nullptr;
}

const Player* GameState::getPlayer(PlayerID id) const {
    auto it = players.find(id);
    return (it != players.end()) ? it->second.get() : nullptr;
}

void GameState::addPlayer(std::unique_ptr<Player> player) {
    if (player) {
        players[player->getId()] = std::move(player);
    }
}

Hero* GameState::getHero(HeroID id) {
    auto it = heroes.find(id);
    return (it != heroes.end()) ? it->second.get() : nullptr;
}

const Hero* GameState::getHero(HeroID id) const {
    auto it = heroes.find(id);
    return (it != heroes.end()) ? it->second.get() : nullptr;
}

void GameState::addHero(std::unique_ptr<Hero> hero) {
    if (hero) {
        heroes[hero->getId()] = std::move(hero);
    }
}

const Creature* GameState::getCreatureData(CreatureID id) {
    auto it = creatureDatabase.find(id);
    return (it != creatureDatabase.end()) ? it->second.get() : nullptr;
}

void GameState::loadCreatureDatabase() {
    // Create some basic creatures for testing
    auto peasant = std::make_unique<Creature>(1, "Peasant", Faction::Castle, CreatureTier::Tier1);
    peasant->setStats(1, 1, 1, 1, 1, 3);
    peasant->setCost(Resources{});
    peasant->setAiValue(15);
    creatureDatabase[1] = std::move(peasant);
    
    auto archer = std::make_unique<Creature>(2, "Archer", Faction::Castle, CreatureTier::Tier2);
    archer->setStats(6, 3, 2, 3, 10, 4);
    archer->addAbility(CreatureAbility::Shooting);
    Resources archerCost;
    archerCost.gold = 100;
    archerCost.wood = 5;
    archer->setCost(archerCost);
    archer->setAiValue(126);
    creatureDatabase[2] = std::move(archer);
}

void GameState::processDailyEvents() {
    generateDailyResources();
    resetHeroMovement();
}

void GameState::processWeeklyEvents() {
    // Weekly creature growth in towns would go here
}

void GameState::processMonthlyEvents() {
    // Monthly events would go here
}

void GameState::generateDailyResources() {
    for (auto& [id, player] : players) {
        Resources dailyIncome;
        dailyIncome.gold = 1000; // Base daily gold income
        player->addResources(dailyIncome);
    }
}

void GameState::resetHeroMovement() {
    for (auto& [id, hero] : heroes) {
        hero->resetMovementPoints();
    }
}