#include "GameMap.h"
#include "../gamestate/GameState.h"
#include <algorithm>
#include <cmath>

void ResourceMine::onVisit(HeroID heroId) {
    // Implementation would check hero ownership and transfer mine control
}

bool ResourceMine::canVisit(HeroID heroId) const {
    // Implementation would check if hero can capture this mine
    return true;
}

void MonsterGroup::onVisit(HeroID heroId) {
    // Implementation would initiate battle with the monster group
}

GameMap::GameMap(int w, int h, int l) : width(w), height(h), levels(l) {
    initializeTiles();
}

MapTile& GameMap::getTile(int x, int y, int z) {
    if (!isPositionInBounds(x, y, z)) {
        static MapTile invalidTile;
        return invalidTile;
    }
    return tiles[z][y][x];
}

const MapTile& GameMap::getTile(int x, int y, int z) const {
    if (!isPositionInBounds(x, y, z)) {
        static MapTile invalidTile;
        return invalidTile;
    }
    return tiles[z][y][x];
}

bool GameMap::isValidPosition(int x, int y, int z) const {
    return isPositionInBounds(x, y, z);
}

bool GameMap::isValidPosition(const Position& pos) const {
    return isValidPosition(pos.x, pos.y, pos.z);
}

bool GameMap::isPassable(const Position& pos) const {
    if (!isValidPosition(pos)) {
        return false;
    }
    
    const MapTile& tile = getTile(pos);
    return tile.passable;
}

int GameMap::getMovementCost(const Position& pos) const {
    if (!isValidPosition(pos)) {
        return 999; // Very high cost for invalid positions
    }
    
    const MapTile& tile = getTile(pos);
    return tile.movementCost;
}

void GameMap::addObject(std::unique_ptr<MapObject> object) {
    if (!object) {
        return;
    }
    
    Position pos = object->getPosition();
    if (isValidPosition(pos)) {
        MapTile& tile = getTile(pos);
        tile.object = object->getType();
        tile.objectId = object->getId();
        
        if (object->blocksMovement()) {
            tile.passable = false;
        }
    }
    
    objects.push_back(std::move(object));
}

MapObject* GameMap::getObject(uint32_t objectId) {
    auto it = std::find_if(objects.begin(), objects.end(),
        [objectId](const std::unique_ptr<MapObject>& obj) {
            return obj->getId() == objectId;
        });
    
    return (it != objects.end()) ? it->get() : nullptr;
}

const MapObject* GameMap::getObject(uint32_t objectId) const {
    auto it = std::find_if(objects.begin(), objects.end(),
        [objectId](const std::unique_ptr<MapObject>& obj) {
            return obj->getId() == objectId;
        });
    
    return (it != objects.end()) ? it->get() : nullptr;
}

void GameMap::removeObject(uint32_t objectId) {
    auto it = std::find_if(objects.begin(), objects.end(),
        [objectId](const std::unique_ptr<MapObject>& obj) {
            return obj->getId() == objectId;
        });
    
    if (it != objects.end()) {
        Position pos = (*it)->getPosition();
        if (isValidPosition(pos)) {
            MapTile& tile = getTile(pos);
            tile.object = ObjectType::None;
            tile.objectId = 0;
            tile.passable = true; // Reset passability
        }
        
        objects.erase(it);
    }
}

std::vector<MapObject*> GameMap::getObjectsAt(const Position& pos) {
    std::vector<MapObject*> result;
    
    for (auto& obj : objects) {
        if (obj->getPosition() == pos) {
            result.push_back(obj.get());
        }
    }
    
    return result;
}

bool GameMap::canHeroMoveTo(HeroID heroId, const Position& pos) const {
    if (!isPassable(pos)) {
        return false;
    }
    
    // Check for blocking objects
    const MapTile& tile = getTile(pos);
    if (tile.object != ObjectType::None) {
        const MapObject* obj = getObject(tile.objectId);
        if (obj && obj->blocksMovement() && !obj->canVisit(heroId)) {
            return false;
        }
    }
    
    return true;
}

void GameMap::moveHero(HeroID heroId, const Position& from, const Position& to) {
    // Clear hero from old position
    if (isValidPosition(from)) {
        MapTile& fromTile = getTile(from);
        if (fromTile.object == ObjectType::Hero) {
            fromTile.object = ObjectType::None;
            fromTile.objectId = 0;
        }
    }
    
    // Place hero at new position
    if (isValidPosition(to)) {
        MapTile& toTile = getTile(to);
        
        // Don't overwrite existing objects - heroes can stand on objects
        // The client will handle interactions separately
        // Only set hero as object if the tile was empty
        if (toTile.object == ObjectType::None) {
            toTile.object = ObjectType::Hero;
            toTile.objectId = heroId;
        }
    }
}

std::vector<Position> GameMap::getAdjacentPositions(const Position& pos) const {
    std::vector<Position> adjacent;
    
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dx == 0 && dy == 0) continue;
            
            Position newPos(pos.x + dx, pos.y + dy, pos.z);
            if (isValidPosition(newPos)) {
                adjacent.push_back(newPos);
            }
        }
    }
    
    return adjacent;
}

int GameMap::calculateDistance(const Position& from, const Position& to) const {
    int dx = std::abs(from.x - to.x);
    int dy = std::abs(from.y - to.y);
    int dz = std::abs(from.z - to.z);
    
    return dx + dy + dz; // Manhattan distance
}

void GameMap::initializeTiles() {
    tiles.resize(levels);
    for (int z = 0; z < levels; z++) {
        tiles[z].resize(height);
        for (int y = 0; y < height; y++) {
            tiles[z][y].resize(width);
            for (int x = 0; x < width; x++) {
                tiles[z][y][x] = MapTile(TerrainType::Grass);
            }
        }
    }
}

bool GameMap::isPositionInBounds(int x, int y, int z) const {
    return x >= 0 && x < width &&
           y >= 0 && y < height &&
           z >= 0 && z < levels;
}