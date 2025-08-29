#include "../include/GameTypes.h"

int& Resources::operator[](ResourceType type) {
    switch (type) {
        case ResourceType::Wood: return wood;
        case ResourceType::Mercury: return mercury;
        case ResourceType::Ore: return ore;
        case ResourceType::Sulfur: return sulfur;
        case ResourceType::Crystal: return crystal;
        case ResourceType::Gems: return gems;
        case ResourceType::Gold: return gold;
    }
    return gold; // fallback
}

const int& Resources::operator[](ResourceType type) const {
    switch (type) {
        case ResourceType::Wood: return wood;
        case ResourceType::Mercury: return mercury;
        case ResourceType::Ore: return ore;
        case ResourceType::Sulfur: return sulfur;
        case ResourceType::Crystal: return crystal;
        case ResourceType::Gems: return gems;
        case ResourceType::Gold: return gold;
    }
    return gold; // fallback
}

Resources Resources::operator+(const Resources& other) const {
    Resources result = *this;
    result.wood += other.wood;
    result.mercury += other.mercury;
    result.ore += other.ore;
    result.sulfur += other.sulfur;
    result.crystal += other.crystal;
    result.gems += other.gems;
    result.gold += other.gold;
    return result;
}

Resources Resources::operator-(const Resources& other) const {
    Resources result = *this;
    result.wood -= other.wood;
    result.mercury -= other.mercury;
    result.ore -= other.ore;
    result.sulfur -= other.sulfur;
    result.crystal -= other.crystal;
    result.gems -= other.gems;
    result.gold -= other.gold;
    return result;
}

bool Resources::canAfford(const Resources& cost) const {
    return wood >= cost.wood &&
           mercury >= cost.mercury &&
           ore >= cost.ore &&
           sulfur >= cost.sulfur &&
           crystal >= cost.crystal &&
           gems >= cost.gems &&
           gold >= cost.gold;
}