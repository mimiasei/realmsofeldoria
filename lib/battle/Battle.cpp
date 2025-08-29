#include "Battle.h"
#include "../gamestate/GameState.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <climits>

BattleEngine::BattleEngine(Hero* hero) 
    : attackingHero(hero), battleActive(false), rng(std::random_device{}()) {
}

void BattleEngine::addPlayerUnit(CreatureID creatureId, int count) {
    const Creature* creatureData = GameState::getCreatureData(creatureId);
    if (creatureData && count > 0) {
        playerUnits.emplace_back(creatureId, count, creatureData->getHitPoints(), true);
    }
}

void BattleEngine::addEnemyUnit(CreatureID creatureId, int count) {
    const Creature* creatureData = GameState::getCreatureData(creatureId);
    if (creatureData && count > 0) {
        enemyUnits.emplace_back(creatureId, count, creatureData->getHitPoints(), false);
    }
}

BattleResult BattleEngine::executeBattle() {
    return executeAutoBattle(); // For now, always auto-battle
}

BattleResult BattleEngine::executeAutoBattle() {
    initializeBattle();
    
    int roundNumber = 1;
    const int MAX_ROUNDS = 20; // Prevent infinite battles
    
    while (battleActive && roundNumber <= MAX_ROUNDS) {
        AsciiBattleDisplay::showBattleRound(playerUnits, enemyUnits, roundNumber);
        
        executeRound();
        
        if (checkBattleEnd()) {
            break;
        }
        
        roundNumber++;
        
        // Brief pause for readability
        std::cout << "\nPress any key to continue to next round...\n";
        std::cin.get();
    }
    
    endBattle();
    return determineBattleResult();
}

void BattleEngine::initializeBattle() {
    battleActive = true;
    
    // Remove dead units
    playerUnits.erase(
        std::remove_if(playerUnits.begin(), playerUnits.end(),
            [](const BattleUnit& unit) { return unit.count <= 0; }),
        playerUnits.end()
    );
    
    enemyUnits.erase(
        std::remove_if(enemyUnits.begin(), enemyUnits.end(),
            [](const BattleUnit& unit) { return unit.count <= 0; }),
        enemyUnits.end()
    );
}

void BattleEngine::executeRound() {
    // Simple turn-based combat: all player units attack, then all enemy units
    
    // Player units attack
    for (auto& playerUnit : playerUnits) {
        if (playerUnit.count <= 0) continue;
        
        // Find best enemy target
        int targetIndex = selectBestTarget(enemyUnits);
        if (targetIndex >= 0 && targetIndex < static_cast<int>(enemyUnits.size())) {
            auto& target = enemyUnits[targetIndex];
            if (target.count > 0) {
                int damage = calculateDamage(playerUnit, target);
                AsciiBattleDisplay::showDamage(playerUnit, target, damage);
                
                // Apply damage
                int killedUnits = damage / GameState::getCreatureData(target.creatureId)->getHitPoints();
                target.count -= killedUnits;
                
                // Handle partial damage to remaining unit
                int remainingDamage = damage % GameState::getCreatureData(target.creatureId)->getHitPoints();
                if (remainingDamage > 0 && target.count > 0) {
                    target.currentHealth -= remainingDamage;
                    if (target.currentHealth <= 0) {
                        target.count--;
                        target.currentHealth = GameState::getCreatureData(target.creatureId)->getHitPoints();
                    }
                }
                
                target.count = std::max(0, target.count);
            }
        }
    }
    
    // Remove dead enemy units
    enemyUnits.erase(
        std::remove_if(enemyUnits.begin(), enemyUnits.end(),
            [](const BattleUnit& unit) { return unit.count <= 0; }),
        enemyUnits.end()
    );
    
    // Enemy units attack (if any remain)
    for (auto& enemyUnit : enemyUnits) {
        if (enemyUnit.count <= 0) continue;
        
        // Find best player target
        int targetIndex = selectBestTarget(playerUnits);
        if (targetIndex >= 0 && targetIndex < static_cast<int>(playerUnits.size())) {
            auto& target = playerUnits[targetIndex];
            if (target.count > 0) {
                int damage = calculateDamage(enemyUnit, target);
                AsciiBattleDisplay::showDamage(enemyUnit, target, damage);
                
                // Apply damage
                int killedUnits = damage / GameState::getCreatureData(target.creatureId)->getHitPoints();
                target.count -= killedUnits;
                
                // Handle partial damage
                int remainingDamage = damage % GameState::getCreatureData(target.creatureId)->getHitPoints();
                if (remainingDamage > 0 && target.count > 0) {
                    target.currentHealth -= remainingDamage;
                    if (target.currentHealth <= 0) {
                        target.count--;
                        target.currentHealth = GameState::getCreatureData(target.creatureId)->getHitPoints();
                    }
                }
                
                target.count = std::max(0, target.count);
            }
        }
    }
    
    // Remove dead player units
    playerUnits.erase(
        std::remove_if(playerUnits.begin(), playerUnits.end(),
            [](const BattleUnit& unit) { return unit.count <= 0; }),
        playerUnits.end()
    );
}

int BattleEngine::calculateDamage(const BattleUnit& attacker, const BattleUnit& defender) {
    const Creature* attackerCreature = GameState::getCreatureData(attacker.creatureId);
    const Creature* defenderCreature = GameState::getCreatureData(defender.creatureId);
    
    if (!attackerCreature || !defenderCreature) {
        return 0;
    }
    
    // Base damage calculation
    int baseDamage = attackerCreature->calculateDamageAgainst(*defenderCreature);
    
    // Apply hero bonuses if attacking hero exists
    if (attacker.isPlayerControlled && attackingHero) {
        float attackBonus = 1.0f + (attackingHero->getAttack() * 0.05f);
        baseDamage = static_cast<int>(baseDamage * attackBonus);
    }
    
    // Multiply by unit count
    int totalDamage = baseDamage * attacker.count;
    
    // Add some randomness (±20%)
    std::uniform_real_distribution<float> variance(0.8f, 1.2f);
    totalDamage = static_cast<int>(totalDamage * variance(rng));
    
    return std::max(1, totalDamage);
}

int BattleEngine::selectBestTarget(const std::vector<BattleUnit>& targets) {
    if (targets.empty()) {
        return -1;
    }
    
    // Simple AI: target the weakest unit first
    int bestIndex = 0;
    int lowestHealth = INT_MAX;
    
    for (int i = 0; i < static_cast<int>(targets.size()); i++) {
        if (targets[i].count <= 0) continue;
        
        const Creature* creature = GameState::getCreatureData(targets[i].creatureId);
        if (creature) {
            int totalHealth = creature->getHitPoints() * targets[i].count;
            if (totalHealth < lowestHealth) {
                lowestHealth = totalHealth;
                bestIndex = i;
            }
        }
    }
    
    return bestIndex;
}

int BattleEngine::selectBestAttacker(const std::vector<BattleUnit>& attackers) {
    if (attackers.empty()) {
        return -1;
    }
    
    // Simple AI: use strongest attacker first
    int bestIndex = 0;
    int highestAttack = 0;
    
    for (int i = 0; i < static_cast<int>(attackers.size()); i++) {
        if (attackers[i].count <= 0) continue;
        
        const Creature* creature = GameState::getCreatureData(attackers[i].creatureId);
        if (creature && creature->getAttack() > highestAttack) {
            highestAttack = creature->getAttack();
            bestIndex = i;
        }
    }
    
    return bestIndex;
}

bool BattleEngine::checkBattleEnd() {
    // Battle ends when one side has no units left
    bool playerHasUnits = std::any_of(playerUnits.begin(), playerUnits.end(),
        [](const BattleUnit& unit) { return unit.count > 0; });
    
    bool enemyHasUnits = std::any_of(enemyUnits.begin(), enemyUnits.end(),
        [](const BattleUnit& unit) { return unit.count > 0; });
    
    return !playerHasUnits || !enemyHasUnits;
}

BattleResult BattleEngine::determineBattleResult() {
    bool playerHasUnits = std::any_of(playerUnits.begin(), playerUnits.end(),
        [](const BattleUnit& unit) { return unit.count > 0; });
    
    return playerHasUnits ? BattleResult::Victory : BattleResult::Defeat;
}

int BattleEngine::calculateExperienceGained() const {
    int totalExperience = 0;
    
    // Experience is based on enemy units that were defeated
    for (const auto& unit : enemyUnits) {
        const Creature* creature = GameState::getCreatureData(unit.creatureId);
        if (creature) {
            // Experience = creature AI value * defeated count
            totalExperience += creature->getAiValue() * std::max(0, 5 - unit.count); // Assuming started with 5
        }
    }
    
    return totalExperience;
}

// ASCII Battle Display Implementation
void AsciiBattleDisplay::showBattleStart(const Hero* hero, const std::vector<BattleUnit>& enemies) {
    std::cout << "\n╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                        BATTLE BEGINS!                       ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n\n";
    
    std::cout << hero->getName() << " encounters enemy forces!\n\n";
    
    std::cout << "Enemy Forces:\n";
    for (const auto& unit : enemies) {
        const Creature* creature = GameState::getCreatureData(unit.creatureId);
        if (creature) {
            std::cout << "  " << unit.count << "x " << creature->getName() << "\n";
        }
    }
    std::cout << "\n";
}

void AsciiBattleDisplay::showBattleRound(const std::vector<BattleUnit>& playerUnits, 
                                       const std::vector<BattleUnit>& enemyUnits, 
                                       int roundNumber) {
    std::cout << "\n═══ ROUND " << roundNumber << " ═══\n\n";
    
    std::cout << "Your Forces:\n";
    for (const auto& unit : playerUnits) {
        const Creature* creature = GameState::getCreatureData(unit.creatureId);
        if (creature && unit.count > 0) {
            std::cout << "  " << unit.count << "x " << creature->getName();
            if (unit.currentHealth < creature->getHitPoints()) {
                std::cout << " (wounded: " << unit.currentHealth << "/" << creature->getHitPoints() << " HP)";
            }
            std::cout << "\n";
        }
    }
    
    std::cout << "\nEnemy Forces:\n";
    for (const auto& unit : enemyUnits) {
        const Creature* creature = GameState::getCreatureData(unit.creatureId);
        if (creature && unit.count > 0) {
            std::cout << "  " << unit.count << "x " << creature->getName();
            if (unit.currentHealth < creature->getHitPoints()) {
                std::cout << " (wounded: " << unit.currentHealth << "/" << creature->getHitPoints() << " HP)";
            }
            std::cout << "\n";
        }
    }
    std::cout << "\n";
}

void AsciiBattleDisplay::showDamage(const BattleUnit& attacker, const BattleUnit& defender, int damage) {
    const Creature* attackerCreature = GameState::getCreatureData(attacker.creatureId);
    const Creature* defenderCreature = GameState::getCreatureData(defender.creatureId);
    
    if (attackerCreature && defenderCreature) {
        std::cout << attackerCreature->getName() << " attacks " 
                  << defenderCreature->getName() << " for " << damage << " damage!\n";
    }
}

void AsciiBattleDisplay::showBattleResult(BattleResult result, int experienceGained) {
    std::cout << "\n╔══════════════════════════════════════════════════════════════╗\n";
    
    switch (result) {
        case BattleResult::Victory:
            std::cout << "║                         VICTORY!                           ║\n";
            std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
            std::cout << "\nYou have defeated the enemy forces!\n";
            if (experienceGained > 0) {
                std::cout << "Experience gained: " << experienceGained << "\n";
            }
            break;
            
        case BattleResult::Defeat:
            std::cout << "║                         DEFEAT!                            ║\n";
            std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
            std::cout << "\nYour forces have been defeated!\n";
            break;
            
        case BattleResult::Flee:
            std::cout << "║                       RETREATED!                          ║\n";
            std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
            std::cout << "\nYou have successfully retreated from battle!\n";
            break;
    }
    
    std::cout << "\nPress any key to continue...\n";
}