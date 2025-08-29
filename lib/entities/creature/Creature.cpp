#include "Creature.h"
#include <algorithm>
#include <random>

Creature::Creature(CreatureID id, const std::string& name, Faction faction, CreatureTier tier)
    : id(id), name(name), faction(faction), tier(tier), attack(0), defense(0),
      minDamage(0), maxDamage(0), hitPoints(0), speed(0), aiValue(0),
      canUpgrade(false), upgradeTarget(0) {
}

void Creature::setStats(int att, int def, int minDmg, int maxDmg, int hp, int spd) {
    attack = att;
    defense = def;
    minDamage = minDmg;
    maxDamage = maxDmg;
    hitPoints = hp;
    speed = spd;
}

void Creature::setCost(const Resources& resources) {
    cost = resources;
}

void Creature::addAbility(CreatureAbility ability) {
    if (std::find(abilities.begin(), abilities.end(), ability) == abilities.end()) {
        abilities.push_back(ability);
    }
}

bool Creature::hasAbility(CreatureAbility ability) const {
    return std::find(abilities.begin(), abilities.end(), ability) != abilities.end();
}

int Creature::calculateDamage() const {
    if (minDamage == maxDamage) {
        return minDamage;
    }
    
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(minDamage, maxDamage);
    return dis(gen);
}

int Creature::calculateDamageAgainst(const Creature& target) const {
    int baseDamage = calculateDamage();
    
    // Simple damage calculation with attack vs defense
    float attackDefenseRatio = static_cast<float>(attack) / static_cast<float>(target.defense + 1);
    
    if (attackDefenseRatio > 1.0f) {
        baseDamage = static_cast<int>(baseDamage * (1.0f + (attackDefenseRatio - 1.0f) * 0.1f));
    } else if (attackDefenseRatio < 1.0f) {
        baseDamage = static_cast<int>(baseDamage * attackDefenseRatio);
    }
    
    return std::max(1, baseDamage);
}