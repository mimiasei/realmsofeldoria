#pragma once

#include "../../../include/GameTypes.h"
#include "../creature/Creature.h"
#include <string>
#include <map>
#include <vector>
#include <memory>

enum class HeroClass {
    Knight,
    Cleric,
    Ranger,
    Druid,
    Alchemist,
    Wizard,
    Demoniac,
    Heretic,
    DeathKnight,
    Necromancer,
    Overlord,
    Warlock,
    Barbarian,
    BattleMage,
    Beastmaster,
    Witch
};

enum class Gender {
    Male,
    Female
};

// Army slot containing creatures
struct ArmySlot {
    CreatureID creatureId;
    int count;
    
    ArmySlot(CreatureID id = 0, int cnt = 0) : creatureId(id), count(cnt) {}
    bool isEmpty() const { return count == 0; }
};

// Hero's army (7 slots max)
class Army {
private:
    static const int MAX_SLOTS = 7;
    std::array<ArmySlot, MAX_SLOTS> slots;
    
public:
    ArmySlot& getSlot(int index) { return slots[index]; }
    const ArmySlot& getSlot(int index) const { return slots[index]; }
    
    bool addCreatures(CreatureID creatureId, int count);
    bool removeCreatures(int slotIndex, int count);
    int getTotalCreatureCount() const;
    bool isEmpty() const;
    bool isFull() const;
};

class Hero {
private:
    HeroID id;
    std::string name;
    HeroClass heroClass;
    Gender gender;
    
    // Position and movement
    Position position;
    int movementPoints;
    int maxMovementPoints;
    
    // Primary attributes
    int attack;
    int defense;
    int spellPower;
    int knowledge;
    
    // Secondary skills (skill -> level)
    std::map<SkillType, int> skills;
    
    // Spells and magic
    std::vector<SpellID> knownSpells;
    int mana;
    int maxMana;
    
    // Army and artifacts
    Army army;
    std::vector<ArtifactID> artifacts;
    
    // Experience and leveling
    int experience;
    int level;
    
public:
    Hero(HeroID id, const std::string& name, HeroClass hClass, Gender g = Gender::Male);
    
    // Basic info
    HeroID getId() const { return id; }
    const std::string& getName() const { return name; }
    HeroClass getHeroClass() const { return heroClass; }
    Gender getGender() const { return gender; }
    
    // Position and movement
    const Position& getPosition() const { return position; }
    void setPosition(const Position& pos) { position = pos; }
    int getMovementPoints() const { return movementPoints; }
    int getMaxMovementPoints() const { return maxMovementPoints; }
    void setMovementPoints(int points) { movementPoints = points; }
    void resetMovementPoints() { movementPoints = maxMovementPoints; }
    bool canMove() const { return movementPoints > 0; }
    
    // Primary attributes
    int getAttack() const { return attack; }
    int getDefense() const { return defense; }
    int getSpellPower() const { return spellPower; }
    int getKnowledge() const { return knowledge; }
    
    void setPrimaryStats(int att, int def, int sp, int know);
    void increasePrimaryStat(SkillType stat, int amount);
    
    // Secondary skills
    int getSkillLevel(SkillType skill) const;
    void setSkill(SkillType skill, int level);
    void increaseSkill(SkillType skill);
    const std::map<SkillType, int>& getAllSkills() const { return skills; }
    
    // Magic system
    const std::vector<SpellID>& getKnownSpells() const { return knownSpells; }
    void learnSpell(SpellID spellId);
    bool knowsSpell(SpellID spellId) const;
    int getMana() const { return mana; }
    int getMaxMana() const { return maxMana; }
    void setMana(int m) { mana = m; }
    void restoreMana() { mana = maxMana; }
    
    // Army management
    Army& getArmy() { return army; }
    const Army& getArmy() const { return army; }
    
    // Artifacts
    void equipArtifact(ArtifactID artifactId);
    void removeArtifact(ArtifactID artifactId);
    bool hasArtifact(ArtifactID artifactId) const;
    const std::vector<ArtifactID>& getArtifacts() const { return artifacts; }
    
    // Experience and leveling
    int getExperience() const { return experience; }
    int getLevel() const { return level; }
    void gainExperience(int exp);
    bool canLevelUp() const;
    void levelUp();
    
private:
    void calculateMaxMana();
    void calculateMaxMovement();
    int getExperienceForLevel(int targetLevel) const;
};