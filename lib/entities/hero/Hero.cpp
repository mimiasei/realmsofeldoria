#include "Hero.h"
#include <algorithm>

bool Army::addCreatures(CreatureID creatureId, int count) {
    // Try to find existing slot with same creature
    for (auto& slot : slots) {
        if (slot.creatureId == creatureId) {
            slot.count += count;
            return true;
        }
    }
    
    // Try to find empty slot
    for (auto& slot : slots) {
        if (slot.isEmpty()) {
            slot.creatureId = creatureId;
            slot.count = count;
            return true;
        }
    }
    
    return false; // No space
}

bool Army::removeCreatures(int slotIndex, int count) {
    if (slotIndex < 0 || slotIndex >= MAX_SLOTS) {
        return false;
    }
    
    ArmySlot& slot = slots[slotIndex];
    if (slot.count < count) {
        return false;
    }
    
    slot.count -= count;
    if (slot.count == 0) {
        slot.creatureId = 0;
    }
    
    return true;
}

int Army::getTotalCreatureCount() const {
    int total = 0;
    for (const auto& slot : slots) {
        total += slot.count;
    }
    return total;
}

bool Army::isEmpty() const {
    for (const auto& slot : slots) {
        if (!slot.isEmpty()) {
            return false;
        }
    }
    return true;
}

bool Army::isFull() const {
    for (const auto& slot : slots) {
        if (slot.isEmpty()) {
            return false;
        }
    }
    return true;
}

Hero::Hero(HeroID id, const std::string& name, HeroClass hClass, Gender g)
    : id(id), name(name), heroClass(hClass), gender(g), position(0, 0, 0),
      movementPoints(0), maxMovementPoints(1000), attack(0), defense(0),
      spellPower(0), knowledge(0), mana(0), maxMana(0), experience(0), level(1) {
    calculateMaxMana();
    calculateMaxMovement();
    mana = maxMana;
}

void Hero::setPrimaryStats(int att, int def, int sp, int know) {
    attack = att;
    defense = def;
    spellPower = sp;
    knowledge = know;
    calculateMaxMana();
    calculateMaxMovement();
}

void Hero::increasePrimaryStat(SkillType stat, int amount) {
    switch (stat) {
        case SkillType::Attack:
            attack += amount;
            break;
        case SkillType::Defense:
            defense += amount;
            break;
        case SkillType::SpellPower:
            spellPower += amount;
            calculateMaxMana();
            break;
        case SkillType::Knowledge:
            knowledge += amount;
            calculateMaxMana();
            break;
        default:
            break;
    }
}

int Hero::getSkillLevel(SkillType skill) const {
    auto it = skills.find(skill);
    return (it != skills.end()) ? it->second : 0;
}

void Hero::setSkill(SkillType skill, int skillLevel) {
    skills[skill] = skillLevel;
}

void Hero::increaseSkill(SkillType skill) {
    skills[skill] = std::min(3, getSkillLevel(skill) + 1);
}

void Hero::learnSpell(SpellID spellId) {
    if (!knowsSpell(spellId)) {
        knownSpells.push_back(spellId);
    }
}

bool Hero::knowsSpell(SpellID spellId) const {
    return std::find(knownSpells.begin(), knownSpells.end(), spellId) != knownSpells.end();
}

void Hero::equipArtifact(ArtifactID artifactId) {
    if (!hasArtifact(artifactId)) {
        artifacts.push_back(artifactId);
    }
}

void Hero::removeArtifact(ArtifactID artifactId) {
    artifacts.erase(std::remove(artifacts.begin(), artifacts.end(), artifactId), artifacts.end());
}

bool Hero::hasArtifact(ArtifactID artifactId) const {
    return std::find(artifacts.begin(), artifacts.end(), artifactId) != artifacts.end();
}

void Hero::gainExperience(int exp) {
    experience += exp;
    while (canLevelUp()) {
        levelUp();
    }
}

bool Hero::canLevelUp() const {
    return experience >= getExperienceForLevel(level + 1);
}

void Hero::levelUp() {
    if (!canLevelUp()) {
        return;
    }
    
    level++;
    // Simple leveling - in a full implementation, this would offer choices
    attack += 1;
    defense += 1;
    
    calculateMaxMana();
    calculateMaxMovement();
}

void Hero::calculateMaxMana() {
    maxMana = knowledge * 10 + spellPower * 5;
}

void Hero::calculateMaxMovement() {
    maxMovementPoints = 1000 + getSkillLevel(SkillType::Logistics) * 200;
}

int Hero::getExperienceForLevel(int targetLevel) const {
    // Simple exponential experience curve
    return (targetLevel - 1) * (targetLevel - 1) * 1000;
}