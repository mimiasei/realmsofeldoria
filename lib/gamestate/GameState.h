#pragma once

#include "../../include/GameTypes.h"
#include "../entities/hero/Hero.h"
#include "../map/GameMap.h"
#include <vector>
#include <map>
#include <memory>

enum class GameDifficulty {
    Easy,
    Normal,
    Hard,
    Expert,
    Impossible
};

class Player {
private:
    PlayerID id;
    std::string name;
    Faction faction;
    Resources resources;
    std::vector<HeroID> heroes;
    std::vector<TownID> towns;
    bool isHuman;
    bool isActive;
    
public:
    Player(PlayerID id, const std::string& name, Faction faction, bool human = true);
    
    // Basic info
    PlayerID getId() const { return id; }
    const std::string& getName() const { return name; }
    Faction getFaction() const { return faction; }
    bool isHumanPlayer() const { return isHuman; }
    bool isActivePlayer() const { return isActive; }
    void setActive(bool active) { isActive = active; }
    
    // Resources
    Resources& getResources() { return resources; }
    const Resources& getResources() const { return resources; }
    void addResources(const Resources& res) { resources = resources + res; }
    bool canAfford(const Resources& cost) const { return resources.canAfford(cost); }
    void spendResources(const Resources& cost) { resources = resources - cost; }
    
    // Heroes and towns
    const std::vector<HeroID>& getHeroes() const { return heroes; }
    const std::vector<TownID>& getTowns() const { return towns; }
    void addHero(HeroID heroId) { heroes.push_back(heroId); }
    void addTown(TownID townId) { towns.push_back(townId); }
    void removeHero(HeroID heroId);
    void removeTown(TownID townId);
};

class TurnManager {
private:
    std::vector<PlayerID> playerOrder;
    int currentPlayerIndex;
    int turnNumber;
    int dayNumber;
    
public:
    TurnManager();
    
    void initializeTurnOrder(const std::vector<PlayerID>& players);
    PlayerID getCurrentPlayer() const;
    void nextPlayer();
    void nextTurn();
    
    int getTurnNumber() const { return turnNumber; }
    int getDayNumber() const { return dayNumber; }
    bool isNewWeek() const { return dayNumber % 7 == 1; }
    bool isNewMonth() const { return dayNumber % 28 == 1; }
};

class GameState {
private:
    // Core game data
    std::map<HeroID, std::unique_ptr<Hero>> heroes;
    std::map<PlayerID, std::unique_ptr<Player>> players;
    std::unique_ptr<GameMap> gameMap;
    
    // Game flow
    TurnManager turnManager;
    GameDifficulty difficulty;
    bool gameRunning;
    bool gameWon;
    PlayerID winner;
    
    // Static data repositories
    static std::map<CreatureID, std::unique_ptr<Creature>> creatureDatabase;
    
public:
    GameState();
    ~GameState();
    
    // Game flow
    void startGame();
    void endGame(PlayerID winnerPlayer);
    bool isGameRunning() const { return gameRunning; }
    bool isGameWon() const { return gameWon; }
    PlayerID getWinner() const { return winner; }
    
    // Turn management
    TurnManager& getTurnManager() { return turnManager; }
    const TurnManager& getTurnManager() const { return turnManager; }
    PlayerID getCurrentPlayer() const { return turnManager.getCurrentPlayer(); }
    void nextTurn() { turnManager.nextTurn(); }
    
    // Players
    Player* getPlayer(PlayerID id);
    const Player* getPlayer(PlayerID id) const;
    void addPlayer(std::unique_ptr<Player> player);
    const std::map<PlayerID, std::unique_ptr<Player>>& getAllPlayers() const { return players; }
    
    // Heroes
    Hero* getHero(HeroID id);
    const Hero* getHero(HeroID id) const;
    void addHero(std::unique_ptr<Hero> hero);
    const std::map<HeroID, std::unique_ptr<Hero>>& getAllHeroes() const { return heroes; }
    
    // Map
    GameMap* getMap() { return gameMap.get(); }
    const GameMap* getMap() const { return gameMap.get(); }
    void setMap(std::unique_ptr<GameMap> map) { gameMap = std::move(map); }
    
    // Static creature database
    static const Creature* getCreatureData(CreatureID id);
    static void loadCreatureDatabase();
    
    // Game settings
    GameDifficulty getDifficulty() const { return difficulty; }
    void setDifficulty(GameDifficulty diff) { difficulty = diff; }
    
    // Daily/weekly events
    void processDailyEvents();
    void processWeeklyEvents();
    void processMonthlyEvents();
    
private:
    void generateDailyResources();
    void resetHeroMovement();
};