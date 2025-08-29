#pragma once

#include "../../include/GameTypes.h"
#include "../entities/hero/Hero.h"
#include "../entities/creature/Creature.h"
#include <vector>
#include <memory>
#include <random>

enum class BattleResult {
    Victory,
    Defeat,
    Flee
};

struct BattleUnit {
    CreatureID creatureId;
    int count;
    int currentHealth;
    bool isPlayerControlled;
    
    BattleUnit(CreatureID id, int cnt, int health, bool player) 
        : creatureId(id), count(cnt), currentHealth(health), isPlayerControlled(player) {}
};

class BattleEngine {
private:
    Hero* attackingHero;
    std::vector<BattleUnit> playerUnits;
    std::vector<BattleUnit> enemyUnits;
    bool battleActive;
    std::mt19937 rng;
    
public:
    BattleEngine(Hero* hero);
    ~BattleEngine() = default;
    
    // Battle setup
    void addPlayerUnit(CreatureID creatureId, int count);
    void addEnemyUnit(CreatureID creatureId, int count);
    
    // Battle execution
    BattleResult executeBattle();
    BattleResult executeAutoBattle();
    
    // Battle flow
    bool isBattleActive() const { return battleActive; }
    void endBattle() { battleActive = false; }
    
    // Unit management
    const std::vector<BattleUnit>& getPlayerUnits() const { return playerUnits; }
    const std::vector<BattleUnit>& getEnemyUnits() const { return enemyUnits; }
    
    // Experience calculation
    int calculateExperienceGained() const;
    
private:
    void initializeBattle();
    int calculateDamage(const BattleUnit& attacker, const BattleUnit& defender);
    void executeRound();
    bool checkBattleEnd();
    BattleResult determineBattleResult();
    
    // AI decision making
    int selectBestTarget(const std::vector<BattleUnit>& targets);
    int selectBestAttacker(const std::vector<BattleUnit>& attackers);
};

// Simple ASCII battle display
class AsciiBattleDisplay {
public:
    static void showBattleStart(const Hero* hero, const std::vector<BattleUnit>& enemies);
    static void showBattleRound(const std::vector<BattleUnit>& playerUnits, 
                               const std::vector<BattleUnit>& enemyUnits, 
                               int roundNumber);
    static void showBattleResult(BattleResult result, int experienceGained);
    static void showDamage(const BattleUnit& attacker, const BattleUnit& defender, int damage);
};