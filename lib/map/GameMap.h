#pragma once

#include "../../include/GameTypes.h"
#include <vector>
#include <memory>

enum class TerrainType {
    Dirt,
    Sand,
    Grass,
    Snow,
    Swamp,
    Rough,
    Lava,
    Water
};

enum class ObjectType {
    None,
    Hero,
    Town,
    Mine,
    Dwelling,
    Artifact,
    Resource,
    Monster,
    Treasure,
    Shrine,
    Library,
    Tree,
    Rock,
    Decoration
};

struct MapTile {
    TerrainType terrain;
    ObjectType object;
    uint32_t objectId;  // ID of specific object instance
    bool passable;
    int movementCost;
    
    MapTile(TerrainType t = TerrainType::Grass) 
        : terrain(t), object(ObjectType::None), objectId(0), passable(true), movementCost(1) {}
};

class MapObject {
protected:
    uint32_t id;
    ObjectType type;
    Position position;
    bool blocksTile;
    
public:
    MapObject(uint32_t id, ObjectType type, const Position& pos, bool blocks = true)
        : id(id), type(type), position(pos), blocksTile(blocks) {}
    
    virtual ~MapObject() = default;
    
    uint32_t getId() const { return id; }
    ObjectType getType() const { return type; }
    const Position& getPosition() const { return position; }
    void setPosition(const Position& pos) { position = pos; }
    bool blocksMovement() const { return blocksTile; }
    
    virtual void onVisit(HeroID heroId) {}
    virtual bool canVisit(HeroID heroId) const { return true; }
};

class ResourceMine : public MapObject {
private:
    ResourceType resourceType;
    int dailyProduction;
    PlayerID owner;  // 0 = neutral
    
public:
    ResourceMine(uint32_t id, const Position& pos, ResourceType type, int production)
        : MapObject(id, ObjectType::Mine, pos), resourceType(type), dailyProduction(production), owner(0) {}
    
    ResourceType getResourceType() const { return resourceType; }
    int getDailyProduction() const { return dailyProduction; }
    PlayerID getOwner() const { return owner; }
    void setOwner(PlayerID playerId) { owner = playerId; }
    
    void onVisit(HeroID heroId) override;
    bool canVisit(HeroID heroId) const override;
};

class MonsterGroup : public MapObject {
private:
    CreatureID creatureType;
    int count;
    bool neverFlees;
    Resources reward;
    
public:
    MonsterGroup(uint32_t id, const Position& pos, CreatureID creature, int cnt)
        : MapObject(id, ObjectType::Monster, pos), creatureType(creature), count(cnt), neverFlees(false) {}
    
    CreatureID getCreatureType() const { return creatureType; }
    int getCount() const { return count; }
    void setCount(int cnt) { count = cnt; }
    
    const Resources& getReward() const { return reward; }
    void setReward(const Resources& res) { reward = res; }
    
    void onVisit(HeroID heroId) override;
    bool canVisit(HeroID heroId) const override { return true; }
};

class GameMap {
private:
    int width, height, levels;
    std::vector<std::vector<std::vector<MapTile>>> tiles;
    std::vector<std::unique_ptr<MapObject>> objects;
    std::string mapName;
    std::string description;
    
public:
    GameMap(int w, int h, int l = 1);
    ~GameMap() = default;
    
    // Map dimensions
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    int getLevels() const { return levels; }
    
    // Tile access
    MapTile& getTile(int x, int y, int z = 0);
    const MapTile& getTile(int x, int y, int z = 0) const;
    MapTile& getTile(const Position& pos) { return getTile(pos.x, pos.y, pos.z); }
    const MapTile& getTile(const Position& pos) const { return getTile(pos.x, pos.y, pos.z); }
    
    // Position validation
    bool isValidPosition(int x, int y, int z = 0) const;
    bool isValidPosition(const Position& pos) const;
    bool isPassable(const Position& pos) const;
    int getMovementCost(const Position& pos) const;
    
    // Object management
    void addObject(std::unique_ptr<MapObject> object);
    MapObject* getObject(uint32_t objectId);
    const MapObject* getObject(uint32_t objectId) const;
    void removeObject(uint32_t objectId);
    std::vector<MapObject*> getObjectsAt(const Position& pos);
    const std::vector<std::unique_ptr<MapObject>>& getAllObjects() const { return objects; }
    
    // Hero movement
    bool canHeroMoveTo(HeroID heroId, const Position& pos) const;
    void moveHero(HeroID heroId, const Position& from, const Position& to);
    
    // Map info
    const std::string& getName() const { return mapName; }
    void setName(const std::string& name) { mapName = name; }
    const std::string& getDescription() const { return description; }
    void setDescription(const std::string& desc) { description = desc; }
    
    // Pathfinding helpers
    std::vector<Position> getAdjacentPositions(const Position& pos) const;
    int calculateDistance(const Position& from, const Position& to) const;
    
private:
    void initializeTiles();
    bool isPositionInBounds(int x, int y, int z) const;
};