#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <sstream>
#include <termios.h>
#include <unistd.h>
#include <algorithm>
#include "../lib/gamestate/GameState.h"
#include "../lib/entities/hero/Hero.h"
#include "../lib/map/GameMap.h"
#include "../lib/battle/Battle.h"

class AsciiGameClient {
private:
    GameState gameState;
    bool running;
    enum class GameScreen {
        MainMenu,
        Game,
        HeroInfo,
        Exit
    };
    
    GameScreen currentScreen;
    int selectedHero;
    const int MAP_WIDTH = 20;
    const int MAP_HEIGHT = 15;
    
public:
    AsciiGameClient() : running(false), currentScreen(GameScreen::MainMenu), selectedHero(1) {}
    
    void run() {
        initialize();
        running = true;
        
        while (running) {
            clearScreen();
            
            switch (currentScreen) {
                case GameScreen::MainMenu:
                    showMainMenu();
                    break;
                case GameScreen::Game:
                    showGameScreen();
                    break;
                case GameScreen::HeroInfo:
                    showHeroInfo();
                    break;
                case GameScreen::Exit:
                    running = false;
                    break;
            }
            
            handleInput();
        }
    }
    
private:
    void initialize() {
        initializeGame();
    }
    
    void initializeGame() {
        // Create a test player
        auto player = std::make_unique<Player>(1, "Player 1", Faction::Castle, true);
        player->getResources().gold = 10000;
        player->getResources().wood = 20;
        player->getResources().ore = 20;
        
        // Create test heroes
        auto hero1 = std::make_unique<Hero>(1, "Sir Arthur", HeroClass::Knight, Gender::Male);
        hero1->setPrimaryStats(8, 6, 2, 3);
        hero1->setPosition(Position(5, 7, 0));
        hero1->setSkill(SkillType::Leadership, 2);
        hero1->setSkill(SkillType::Attack, 1);
        
        // Give Sir Arthur some starting troops
        hero1->getArmy().addCreatures(1, 10); // 10 peasants
        hero1->getArmy().addCreatures(2, 5);  // 5 archers
        
        auto hero2 = std::make_unique<Hero>(2, "Lady Morgana", HeroClass::Wizard, Gender::Female);
        hero2->setPrimaryStats(3, 4, 8, 7);
        hero2->setPosition(Position(12, 8, 0));
        hero2->setSkill(SkillType::Wisdom, 2);
        hero2->setSkill(SkillType::Mysticism, 1);
        
        // Give Lady Morgana some starting troops
        hero2->getArmy().addCreatures(1, 8);  // 8 peasants
        hero2->getArmy().addCreatures(2, 7);  // 7 archers
        
        player->addHero(1);
        player->addHero(2);
        gameState.addHero(std::move(hero1));
        gameState.addHero(std::move(hero2));
        gameState.addPlayer(std::move(player));
        
        // Create and initialize map
        auto map = std::make_unique<GameMap>(MAP_WIDTH, MAP_HEIGHT, 1);
        map->setName("Tutorial Valley");
        
        // Add some objects to the map
        auto goldMine = std::make_unique<ResourceMine>(1, Position(3, 3, 0), ResourceType::Gold, 1000);
        auto woodMine = std::make_unique<ResourceMine>(2, Position(15, 10, 0), ResourceType::Wood, 2);
        auto monsters = std::make_unique<MonsterGroup>(3, Position(8, 5, 0), 1, 5); // 5 peasants
        
        map->addObject(std::move(goldMine));
        map->addObject(std::move(woodMine));
        map->addObject(std::move(monsters));
        
        gameState.setMap(std::move(map));
        gameState.startGame();
    }
    
    void clearScreen() {
        system("clear");
    }
    
    char getChar() {
        char ch;
        struct termios oldt, newt;
        
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        
        ch = getchar();
        
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        
        return ch;
    }
    
    void showMainMenu() {
        std::cout << "═══════════════════════════════════════════════════════════════\n";
        std::cout << "                    REALMS OF ELDORIA                          \n";
        std::cout << "═══════════════════════════════════════════════════════════════\n\n";
        std::cout << "    ⚔️  A Heroes of Might & Magic III Inspired Game ⚔️       \n\n";
        std::cout << "                        [1] New Game                           \n";
        std::cout << "                        [2] Exit                              \n\n";
        std::cout << "                   Press 1 or 2 to select                     \n";
        std::cout << "═══════════════════════════════════════════════════════════════\n";
    }
    
    void showGameScreen() {
        const GameMap* map = gameState.getMap();
        const Player* player = gameState.getPlayer(1);
        const Hero* hero = gameState.getHero(selectedHero);
        
        std::cout << "═══════════════════════════════════════════════════════════════\n";
        std::cout << "  REALMS OF ELDORIA - " << map->getName() << "  Day: " << gameState.getTurnManager().getDayNumber() << "\n";
        std::cout << "═══════════════════════════════════════════════════════════════\n\n";
        
        // Show player resources
        const Resources& res = player->getResources();
        std::cout << "💰 Gold: " << res.gold << "  🪵 Wood: " << res.wood << "  ⛏️  Ore: " << res.ore;
        std::cout << "  💎 Gems: " << res.gems << "  🔮 Crystal: " << res.crystal << "\n\n";
        
        // Show map
        drawMap();
        
        std::cout << "\n";
        std::cout << "═══════════════════════════════════════════════════════════════\n";
        std::cout << "Current Hero: " << hero->getName() << " (Level " << hero->getLevel() << ")\n";
        std::cout << "Location: (" << hero->getPosition().x << ", " << hero->getPosition().y << ")";
        std::cout << "  Movement: " << hero->getMovementPoints() << "/" << hero->getMaxMovementPoints() << "\n";
        std::cout << "ATT: " << hero->getAttack() << "  DEF: " << hero->getDefense();
        std::cout << "  SP: " << hero->getSpellPower() << "  KN: " << hero->getKnowledge() << "\n";
        std::cout << "═══════════════════════════════════════════════════════════════\n";
        std::cout << "Controls: [WASD] Move  [H] Hero Info  [TAB] Switch Hero  [N] Next Turn  [Q] Quit\n";
    }
    
    void drawMap() {
        const GameMap* map = gameState.getMap();
        const Hero* hero1 = gameState.getHero(1);
        const Hero* hero2 = gameState.getHero(2);
        
        // Top border
        std::cout << "┌";
        for (int x = 0; x < MAP_WIDTH; x++) {
            std::cout << "─";
        }
        std::cout << "┐\n";
        
        // Map content
        for (int y = 0; y < MAP_HEIGHT; y++) {
            std::cout << "│";
            for (int x = 0; x < MAP_WIDTH; x++) {
                Position pos(x, y, 0);
                
                // Check for heroes
                if (hero1 && hero1->getPosition() == pos) {
                    if (selectedHero == 1) {
                        std::cout << "🗡️"; // Selected hero
                    } else {
                        std::cout << "⚔️"; // Other hero
                    }
                } else if (hero2 && hero2->getPosition() == pos) {
                    if (selectedHero == 2) {
                        std::cout << "🔮"; // Selected wizard
                    } else {
                        std::cout << "🧙"; // Other wizard
                    }
                } else {
                    // Check for objects
                    const MapTile& tile = map->getTile(pos);
                    switch (tile.object) {
                        case ObjectType::Mine:
                            std::cout << "⛏️";
                            break;
                        case ObjectType::Monster:
                            std::cout << "👹";
                            break;
                        default:
                            // Terrain
                            switch (tile.terrain) {
                                case TerrainType::Grass:
                                    std::cout << ".";
                                    break;
                                case TerrainType::Water:
                                    std::cout << "~";
                                    break;
                                case TerrainType::Sand:
                                    std::cout << "▒";
                                    break;
                                case TerrainType::Snow:
                                    std::cout << "*";
                                    break;
                                default:
                                    std::cout << ".";
                                    break;
                            }
                            break;
                    }
                }
            }
            std::cout << "│\n";
        }
        
        // Bottom border
        std::cout << "└";
        for (int x = 0; x < MAP_WIDTH; x++) {
            std::cout << "─";
        }
        std::cout << "┘\n";
    }
    
    void showHeroInfo() {
        const Hero* hero = gameState.getHero(selectedHero);
        
        std::cout << "═══════════════════════════════════════════════════════════════\n";
        std::cout << "                        HERO INFORMATION                       \n";
        std::cout << "═══════════════════════════════════════════════════════════════\n\n";
        
        std::cout << "Name: " << hero->getName() << "\n";
        std::cout << "Class: ";
        switch (hero->getHeroClass()) {
            case HeroClass::Knight: std::cout << "Knight"; break;
            case HeroClass::Wizard: std::cout << "Wizard"; break;
            case HeroClass::Cleric: std::cout << "Cleric"; break;
            default: std::cout << "Unknown"; break;
        }
        std::cout << "\n";
        std::cout << "Level: " << hero->getLevel() << "\n";
        std::cout << "Experience: " << hero->getExperience() << "\n\n";
        
        std::cout << "Primary Attributes:\n";
        std::cout << "  Attack: " << hero->getAttack() << "\n";
        std::cout << "  Defense: " << hero->getDefense() << "\n";
        std::cout << "  Spell Power: " << hero->getSpellPower() << "\n";
        std::cout << "  Knowledge: " << hero->getKnowledge() << "\n\n";
        
        std::cout << "Secondary Skills:\n";
        const auto& skills = hero->getAllSkills();
        if (skills.empty()) {
            std::cout << "  None\n";
        } else {
            for (const auto& [skill, level] : skills) {
                std::cout << "  ";
                switch (skill) {
                    case SkillType::Leadership: std::cout << "Leadership"; break;
                    case SkillType::Attack: std::cout << "Attack"; break;
                    case SkillType::Wisdom: std::cout << "Wisdom"; break;
                    case SkillType::Mysticism: std::cout << "Mysticism"; break;
                    default: std::cout << "Unknown Skill"; break;
                }
                std::cout << ": " << level << "\n";
            }
        }
        
        std::cout << "\nMana: " << hero->getMana() << "/" << hero->getMaxMana() << "\n";
        std::cout << "Movement: " << hero->getMovementPoints() << "/" << hero->getMaxMovementPoints() << "\n\n";
        
        std::cout << "Press any key to return to game...\n";
    }
    
    void handleInput() {
        char key = getChar();
        
        switch (currentScreen) {
            case GameScreen::MainMenu:
                handleMainMenuInput(key);
                break;
            case GameScreen::Game:
                handleGameInput(key);
                break;
            case GameScreen::HeroInfo:
                currentScreen = GameScreen::Game;
                break;
            default:
                break;
        }
    }
    
    void handleMainMenuInput(char key) {
        switch (key) {
            case '1':
                currentScreen = GameScreen::Game;
                break;
            case '2':
                currentScreen = GameScreen::Exit;
                break;
        }
    }
    
    void handleGameInput(char key) {
        Hero* hero = gameState.getHero(selectedHero);
        GameMap* map = gameState.getMap();
        
        switch (key) {
            case 'w':
            case 'W':
                moveHero(hero, map, 0, -1);
                break;
            case 's':
            case 'S':
                moveHero(hero, map, 0, 1);
                break;
            case 'a':
            case 'A':
                moveHero(hero, map, -1, 0);
                break;
            case 'd':
            case 'D':
                moveHero(hero, map, 1, 0);
                break;
            case '\t': // Tab key
                switchHero();
                break;
            case 'h':
            case 'H':
                currentScreen = GameScreen::HeroInfo;
                break;
            case 'n':
            case 'N':
                nextTurn();
                break;
            case 'q':
            case 'Q':
                currentScreen = GameScreen::Exit;
                break;
        }
    }
    
    void moveHero(Hero* hero, GameMap* map, int dx, int dy) {
        if (!hero) {
            return;
        }
        
        // Check if hero is exhausted
        if (hero->getMovementPoints() <= 0) {
            std::cout << "\n>>> " << hero->getName() << " is exhausted and must rest until the next day! <<<\n";
            std::cout << "Press any key to continue...\n";
            getChar();
            return;
        }
        
        Position currentPos = hero->getPosition();
        Position newPos(currentPos.x + dx, currentPos.y + dy, currentPos.z);
        
        if (map->isValidPosition(newPos) && map->canHeroMoveTo(hero->getId(), newPos)) {
            map->moveHero(hero->getId(), currentPos, newPos);
            hero->setPosition(newPos);
            hero->setMovementPoints(hero->getMovementPoints() - 100);
            
            // Check for objects at new position
            checkForInteraction(hero, newPos);
        }
    }
    
    void checkForInteraction(Hero* hero, const Position& pos) {
        GameMap* map = gameState.getMap();
        const MapTile& tile = map->getTile(pos);
        
        // Only interact with objects that the hero steps directly on
        if (tile.object != ObjectType::None && tile.objectId != 0) {
            MapObject* obj = map->getObject(tile.objectId);
            if (obj) {
                std::cout << "\n>>> ";
                switch (obj->getType()) {
                    case ObjectType::Mine: {
                        ResourceMine* mine = static_cast<ResourceMine*>(obj);
                        std::cout << hero->getName() << " found a ";
                        switch (mine->getResourceType()) {
                            case ResourceType::Gold:
                                std::cout << "Gold Mine! (+1000 gold/day)";
                                break;
                            case ResourceType::Wood:
                                std::cout << "Sawmill! (+2 wood/day)";
                                break;
                            default:
                                std::cout << "Mine!";
                                break;
                        }
                        
                        // Claim the mine for the player
                        Player* player = gameState.getPlayer(1);
                        if (player && mine->getOwner() == 0) {
                            mine->setOwner(1);
                            std::cout << " " << hero->getName() << " claims it!";
                        } else if (mine->getOwner() == 1) {
                            std::cout << " (Already controlled by you)";
                        }
                        break;
                    }
                    case ObjectType::Monster: {
                        MonsterGroup* monsters = static_cast<MonsterGroup*>(obj);
                        std::cout << hero->getName() << " encounters ";
                        std::cout << monsters->getCount() << " creatures!\n";
                        std::cout << "Prepare for battle!\n\n";
                        
                        // Initiate battle
                        BattleResult result = conductBattle(hero, monsters);
                        
                        if (result == BattleResult::Victory) {
                            // Remove the monster group
                            map->removeObject(tile.objectId);
                            
                            // Grant experience
                            int expGained = monsters->getCount() * 75; // More exp for actual battle
                            hero->gainExperience(expGained);
                            std::cout << "\n" << hero->getName() << " gains " << expGained << " experience!";
                            
                            // Check for level up
                            if (hero->canLevelUp()) {
                                std::cout << "\n*** " << hero->getName() << " has gained a level! ***\n";
                                hero->levelUp();
                                std::cout << hero->getName() << " is now level " << hero->getLevel() << "!\n";
                            }
                        } else {
                            std::cout << "\nThe monsters remain on the map...\n";
                        }
                        break;
                    }
                    default:
                        std::cout << hero->getName() << " found something interesting!";
                        break;
                }
                std::cout << " <<<\nPress any key to continue...\n";
                getChar();
            }
        }
    }
    
    void switchHero() {
        const Player* player = gameState.getPlayer(1);
        const auto& heroIds = player->getHeroes();
        
        auto it = std::find(heroIds.begin(), heroIds.end(), selectedHero);
        if (it != heroIds.end()) {
            ++it;
            if (it == heroIds.end()) {
                it = heroIds.begin();
            }
            selectedHero = *it;
        }
    }
    
    void nextTurn() {
        gameState.nextTurn();
        gameState.processDailyEvents();
        
        // Reset hero movement
        for (const auto& [id, hero] : gameState.getAllHeroes()) {
            hero->resetMovementPoints();
        }
    }
    
    BattleResult conductBattle(Hero* hero, MonsterGroup* monsters) {
        // Create battle engine
        BattleEngine battle(hero);
        
        // Add hero's army to battle
        const Army& army = hero->getArmy();
        for (int i = 0; i < 7; i++) { // MAX_SLOTS = 7
            const ArmySlot& slot = army.getSlot(i);
            if (!slot.isEmpty()) {
                battle.addPlayerUnit(slot.creatureId, slot.count);
            }
        }
        
        // Add monster units to battle
        battle.addEnemyUnit(monsters->getCreatureType(), monsters->getCount());
        
        // Show battle start
        AsciiBattleDisplay::showBattleStart(hero, battle.getEnemyUnits());
        
        std::cout << "Press any key to begin battle...\n";
        getChar();
        
        // Execute battle
        BattleResult result = battle.executeAutoBattle();
        
        // Show result
        int expGained = battle.calculateExperienceGained();
        AsciiBattleDisplay::showBattleResult(result, expGained);
        
        // Update hero's army based on battle results
        if (result == BattleResult::Victory || result == BattleResult::Defeat) {
            updateHeroArmyAfterBattle(hero, battle.getPlayerUnits());
        }
        
        getChar(); // Wait for user input
        return result;
    }
    
    void updateHeroArmyAfterBattle(Hero* hero, const std::vector<BattleUnit>& survivingUnits) {
        Army& army = hero->getArmy();
        
        // Clear current army
        for (int i = 0; i < 7; i++) {
            army.getSlot(i) = ArmySlot(); // Empty slot
        }
        
        // Add surviving units back to army
        int slotIndex = 0;
        for (const auto& unit : survivingUnits) {
            if (unit.count > 0 && slotIndex < 7) {
                army.getSlot(slotIndex) = ArmySlot(unit.creatureId, unit.count);
                slotIndex++;
            }
        }
    }
};

int main() {
    AsciiGameClient client;
    client.run();
    
    std::cout << "\nThanks for playing Realms of Eldoria!\n";
    return 0;
}