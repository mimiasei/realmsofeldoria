#pragma once

#include "../../../include/GameTypes.h"
#include <string>
#include <vector>

enum class CreatureTier {
    Tier1 = 1,
    Tier2,
    Tier3,
    Tier4,
    Tier5,
    Tier6,
    Tier7
};

enum class CreatureAbility {
    Flying,
    Shooting,
    DoubleAttack,
    NoMeleeRetaliation,
    MagicResistance,
    Regeneration,
    Undead,
    FireImmunity,
    WaterImmunity,
    EarthImmunity,
    AirImmunity
};

class Creature {
private:
    CreatureID id;
    std::string name;
    Faction faction;
    CreatureTier tier;
    
    // Combat stats
    int attack;
    int defense;
    int minDamage;
    int maxDamage;
    int hitPoints;
    int speed;
    
    // Economic
    Resources cost;
    int aiValue;
    
    // Special properties
    std::vector<CreatureAbility> abilities;
    bool canUpgrade;
    CreatureID upgradeTarget;
    
public:
    Creature(CreatureID id, const std::string& name, Faction faction, CreatureTier tier);
    
    // Getters
    CreatureID getId() const { return id; }
    const std::string& getName() const { return name; }
    Faction getFaction() const { return faction; }
    CreatureTier getTier() const { return tier; }
    
    int getAttack() const { return attack; }
    int getDefense() const { return defense; }
    int getMinDamage() const { return minDamage; }
    int getMaxDamage() const { return maxDamage; }
    int getHitPoints() const { return hitPoints; }
    int getSpeed() const { return speed; }
    
    const Resources& getCost() const { return cost; }
    int getAiValue() const { return aiValue; }
    
    // Setters
    void setStats(int att, int def, int minDmg, int maxDmg, int hp, int spd);
    void setCost(const Resources& resources);
    void setAiValue(int value) { aiValue = value; }
    
    // Abilities
    void addAbility(CreatureAbility ability);
    bool hasAbility(CreatureAbility ability) const;
    const std::vector<CreatureAbility>& getAbilities() const { return abilities; }
    
    // Upgrade system
    void setUpgrade(CreatureID target) { upgradeTarget = target; canUpgrade = true; }
    bool canBeUpgraded() const { return canUpgrade; }
    CreatureID getUpgradeTarget() const { return upgradeTarget; }
    
    // Combat calculations
    int calculateDamage() const;
    int calculateDamageAgainst(const Creature& target) const;
};