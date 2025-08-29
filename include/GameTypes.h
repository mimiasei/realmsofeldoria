#pragma once

#include <cstdint>
#include <string>
#include <vector>

// Core type definitions
using HeroID = uint32_t;
using CreatureID = uint32_t;
using TownID = uint32_t;
using SpellID = uint32_t;
using ArtifactID = uint32_t;
using PlayerID = uint8_t;

// Position on game map
struct Position {
    int x, y, z;
    
    Position(int x = 0, int y = 0, int z = 0) : x(x), y(y), z(z) {}
    
    bool operator==(const Position& other) const {
        return x == other.x && y == other.y && z == other.z;
    }
};

// Resource types
enum class ResourceType {
    Wood,
    Mercury,
    Ore,
    Sulfur,
    Crystal,
    Gems,
    Gold
};

// Skill types
enum class SkillType {
    Attack,
    Defense,
    SpellPower,
    Knowledge,
    Archery,
    Leadership,
    Logistics,
    Luck,
    Mysticism,
    Navigation,
    Necromancy,
    Resistance,
    Scholar,
    Scouting,
    Tactics,
    Wisdom
};

// Faction types
enum class Faction {
    Castle,
    Rampart,
    Tower,
    Inferno,
    Necropolis,
    Dungeon,
    Stronghold,
    Fortress,
    Neutral
};

// Battle hex coordinate
struct BattleHex {
    int x, y;
    bool occupied = false;
    
    BattleHex(int x = 0, int y = 0) : x(x), y(y) {}
};

// Resource collection
class Resources {
public:
    int wood = 0;
    int mercury = 0;
    int ore = 0;
    int sulfur = 0;
    int crystal = 0;
    int gems = 0;
    int gold = 0;
    
    int& operator[](ResourceType type);
    const int& operator[](ResourceType type) const;
    
    Resources operator+(const Resources& other) const;
    Resources operator-(const Resources& other) const;
    bool canAfford(const Resources& cost) const;
};