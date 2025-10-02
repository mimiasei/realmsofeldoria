#include <ncurses.h>
#include <string>
#include <vector>
#include <algorithm>
#include "../lib/gamestate/GameState.h"
#include "../lib/entities/hero/Hero.h"
#include "../lib/map/GameMap.h"
#include "../lib/battle/Battle.h"

class NcursesGameClient {
private:
    GameState gameState;
    bool running;
    int selectedHero;
    
    // Window management
    WINDOW* mapWin;
    WINDOW* statusWin;
    WINDOW* infoWin;
    WINDOW* logWin;
    
    // Map constants
    const int MAP_WIDTH = 20;
    const int MAP_HEIGHT = 15;
    
    // Color pairs
    enum ColorPairs {
        COLOR_DEFAULT = 1,
        COLOR_HERO = 2,
        COLOR_MONSTER = 3,
        COLOR_MINE = 4,
        COLOR_UI_HEADER = 5,
        COLOR_UI_TEXT = 6,
        COLOR_UI_HIGHLIGHT = 7,
        COLOR_TERRAIN_GRASS = 8,
        COLOR_TERRAIN_WATER = 9,
        COLOR_TERRAIN_DIRT = 10,
        COLOR_TERRAIN_SAND = 11,
        COLOR_TERRAIN_SNOW = 12,
        COLOR_TERRAIN_SWAMP = 13,
        COLOR_TERRAIN_ROUGH = 14,
        COLOR_TERRAIN_LAVA = 15,
        COLOR_HEALTH_GOOD = 16,
        COLOR_HEALTH_MEDIUM = 17,
        COLOR_HEALTH_LOW = 18
    };
    
public:
    NcursesGameClient() : running(false), selectedHero(1), 
                         mapWin(nullptr), statusWin(nullptr), infoWin(nullptr), logWin(nullptr) {}
    
    ~NcursesGameClient() {
        cleanup();
    }
    
    void run() {
        if (!initialize()) {
            return;
        }
        
        running = true;
        
        while (running) {
            update();
            render();
            handleInput();
        }
    }
    
private:
    bool initialize() {
        // Initialize ncurses
        initscr();
        if (has_colors()) {
            start_color();
            initializeColors();
        }
        
        noecho();
        cbreak();
        keypad(stdscr, TRUE);
        curs_set(0); // Hide cursor
        
        // Check minimum terminal size
        int maxY, maxX;
        getmaxyx(stdscr, maxY, maxX);
        if (maxY < 25 || maxX < 80) {
            endwin();
            printf("Terminal too small! Need at least 80x25, got %dx%d\n", maxX, maxY);
            return false;
        }
        
        // Create windows
        createWindows();
        
        // Initialize game
        initializeGame();
        
        return true;
    }
    
    void initializeColors() {
        init_pair(COLOR_DEFAULT, COLOR_WHITE, COLOR_BLACK);
        init_pair(COLOR_HERO, COLOR_YELLOW, COLOR_BLACK);
        init_pair(COLOR_MONSTER, COLOR_RED, COLOR_BLACK);
        init_pair(COLOR_MINE, COLOR_CYAN, COLOR_BLACK);
        init_pair(COLOR_TERRAIN_GRASS, COLOR_GREEN, COLOR_GREEN);
        init_pair(COLOR_TERRAIN_WATER, COLOR_BLUE, COLOR_BLUE);
        init_pair(COLOR_TERRAIN_DIRT, COLOR_YELLOW, COLOR_YELLOW);
        init_pair(COLOR_TERRAIN_SAND, COLOR_YELLOW, COLOR_YELLOW);
        init_pair(COLOR_TERRAIN_SNOW, COLOR_WHITE, COLOR_WHITE);
        init_pair(COLOR_TERRAIN_SWAMP, COLOR_GREEN, COLOR_GREEN);
        init_pair(COLOR_TERRAIN_ROUGH, COLOR_RED, COLOR_RED);
        init_pair(COLOR_TERRAIN_LAVA, COLOR_RED, COLOR_RED);
        init_pair(COLOR_UI_HEADER, COLOR_WHITE, COLOR_BLUE);
        init_pair(COLOR_UI_TEXT, COLOR_GREEN, COLOR_BLACK);
        init_pair(COLOR_UI_HIGHLIGHT, COLOR_BLACK, COLOR_YELLOW);
        init_pair(COLOR_HEALTH_GOOD, COLOR_GREEN, COLOR_BLACK);
        init_pair(COLOR_HEALTH_MEDIUM, COLOR_YELLOW, COLOR_BLACK);
        init_pair(COLOR_HEALTH_LOW, COLOR_RED, COLOR_BLACK);
    }
    
    void createWindows() {
        int maxY, maxX;
        getmaxyx(stdscr, maxY, maxX);
        
        // Map window (left side)
        mapWin = newwin(MAP_HEIGHT + 2, MAP_WIDTH + 2, 2, 1);
        
        // Status window (top right)
        statusWin = newwin(8, maxX - MAP_WIDTH - 4, 2, MAP_WIDTH + 3);
        
        // Info window (middle right)  
        infoWin = newwin(10, maxX - MAP_WIDTH - 4, 10, MAP_WIDTH + 3);
        
        // Log window (bottom)
        logWin = newwin(maxY - MAP_HEIGHT - 5, maxX - 2, MAP_HEIGHT + 4, 1);
        
        // Enable keypad for all windows
        keypad(mapWin, TRUE);
        keypad(statusWin, TRUE);
        keypad(infoWin, TRUE);
        keypad(logWin, TRUE);
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
        hero1->getArmy().addCreatures(1, 10); // 10 peasants
        hero1->getArmy().addCreatures(2, 5);  // 5 archers
        hero1->resetMovementPoints();
        
        auto hero2 = std::make_unique<Hero>(2, "Lady Morgana", HeroClass::Wizard, Gender::Female);
        hero2->setPrimaryStats(3, 4, 8, 7);
        hero2->setPosition(Position(12, 8, 0));
        hero2->setSkill(SkillType::Wisdom, 2);
        hero2->setSkill(SkillType::Mysticism, 1);
        hero2->getArmy().addCreatures(1, 8);  // 8 peasants
        hero2->getArmy().addCreatures(2, 7);  // 7 archers
        hero2->resetMovementPoints();
        
        player->addHero(1);
        player->addHero(2);
        gameState.addHero(std::move(hero1));
        gameState.addHero(std::move(hero2));
        gameState.addPlayer(std::move(player));
        
        // Create enhanced map with strategic encounters
        auto map = std::make_unique<GameMap>(MAP_WIDTH, MAP_HEIGHT, 1);
        map->setName("Tutorial Valley");
        
        // Add strategic objects and encounters
        auto goldMine = std::make_unique<ResourceMine>(1, Position(3, 3, 0), ResourceType::Gold, 1000);
        auto woodMine = std::make_unique<ResourceMine>(2, Position(15, 10, 0), ResourceType::Wood, 2);
        auto oreMine = std::make_unique<ResourceMine>(3, Position(17, 3, 0), ResourceType::Ore, 2);
        
        // Add diverse monster encounters
        auto weakMonsters = std::make_unique<MonsterGroup>(4, Position(8, 5, 0), 1, 3);
        auto mediumMonsters = std::make_unique<MonsterGroup>(5, Position(14, 7, 0), 2, 4);
        auto strongMonsters = std::make_unique<MonsterGroup>(6, Position(10, 12, 0), 1, 8);
        auto eliteMonsters = std::make_unique<MonsterGroup>(7, Position(6, 2, 0), 2, 6);
        
        // Add mine guards
        auto mineGuard1 = std::make_unique<MonsterGroup>(8, Position(4, 3, 0), 1, 4);
        auto mineGuard2 = std::make_unique<MonsterGroup>(9, Position(16, 10, 0), 2, 3);
        auto mineGuard3 = std::make_unique<MonsterGroup>(10, Position(18, 3, 0), 1, 6);
        
        // Add roaming encounters
        auto roamingGroup1 = std::make_unique<MonsterGroup>(11, Position(2, 8, 0), 1, 2);
        auto roamingGroup2 = std::make_unique<MonsterGroup>(12, Position(11, 2, 0), 2, 2);
        auto roamingGroup3 = std::make_unique<MonsterGroup>(13, Position(18, 13, 0), 1, 5);
        
        // Add all objects to map
        map->addObject(std::move(goldMine));
        map->addObject(std::move(woodMine));
        map->addObject(std::move(oreMine));
        map->addObject(std::move(weakMonsters));
        map->addObject(std::move(mediumMonsters));
        map->addObject(std::move(strongMonsters));
        map->addObject(std::move(eliteMonsters));
        map->addObject(std::move(mineGuard1));
        map->addObject(std::move(mineGuard2));
        map->addObject(std::move(mineGuard3));
        map->addObject(std::move(roamingGroup1));
        map->addObject(std::move(roamingGroup2));
        map->addObject(std::move(roamingGroup3));
        
        gameState.setMap(std::move(map));
        gameState.startGame();
    }
    
    void update() {
        // Game logic updates
        if (gameState.getTurnManager().isNewWeek()) {
            gameState.processWeeklyEvents();
        }
    }
    
    void render() {
        // Clear all windows
        clear();
        wclear(mapWin);
        wclear(statusWin);
        wclear(infoWin);
        wclear(logWin);
        
        // Draw main title
        attron(COLOR_PAIR(COLOR_UI_HEADER));
        mvprintw(0, 2, "REALMS OF ELDORIA - Enhanced NCurses Edition");
        attroff(COLOR_PAIR(COLOR_UI_HEADER));
        
        // Render each window
        renderMap();
        renderStatus();
        renderHeroInfo();
        renderLog();
        
        // Show controls at bottom
        int maxY, maxX;
        getmaxyx(stdscr, maxY, maxX);
        attron(COLOR_PAIR(COLOR_UI_TEXT));
        mvprintw(maxY - 1, 2, "[WASD] Move [TAB] Switch Hero [H] Info [N] Next Turn [Q] Quit");
        attroff(COLOR_PAIR(COLOR_UI_TEXT));
        
        // Refresh all windows
        refresh();
        wrefresh(mapWin);
        wrefresh(statusWin);
        wrefresh(infoWin);
        wrefresh(logWin);
    }
    
    void renderMap() {
        const GameMap* map = gameState.getMap();
        const Hero* hero1 = gameState.getHero(1);
        const Hero* hero2 = gameState.getHero(2);
        
        // Draw window border
        wattron(mapWin, COLOR_PAIR(COLOR_UI_HEADER));
        box(mapWin, 0, 0);
        mvwprintw(mapWin, 0, 2, " %s ", map->getName().c_str());
        wattroff(mapWin, COLOR_PAIR(COLOR_UI_HEADER));
        
        // Draw map content
        for (int y = 0; y < MAP_HEIGHT; y++) {
            for (int x = 0; x < MAP_WIDTH; x++) {
                Position pos(x, y, 0);
                char symbol = ' ';
                int colorPair = COLOR_TERRAIN_GRASS;
                
                // Check for heroes
                if (hero1 && hero1->getPosition() == pos) {
                    symbol = (selectedHero == 1) ? '@' : 'H';
                    colorPair = COLOR_HERO;
                } else if (hero2 && hero2->getPosition() == pos) {
                    symbol = (selectedHero == 2) ? '@' : 'h';
                    colorPair = COLOR_HERO;
                } else {
                    // Check for objects
                    const MapTile& tile = map->getTile(pos);
                    switch (tile.object) {
                        case ObjectType::Mine:
                            symbol = 'M';
                            colorPair = COLOR_MINE;
                            break;
                        case ObjectType::Monster:
                            symbol = 'X';
                            colorPair = COLOR_MONSTER;
                            break;
                        default:
                            // Terrain - use colored square blocks
                            symbol = ' ';
                            switch (tile.terrain) {
                                case TerrainType::Grass:
                                    colorPair = COLOR_TERRAIN_GRASS;
                                    break;
                                case TerrainType::Water:
                                    colorPair = COLOR_TERRAIN_WATER;
                                    break;
                                case TerrainType::Dirt:
                                    colorPair = COLOR_TERRAIN_DIRT;
                                    break;
                                case TerrainType::Sand:
                                    colorPair = COLOR_TERRAIN_SAND;
                                    break;
                                case TerrainType::Snow:
                                    colorPair = COLOR_TERRAIN_SNOW;
                                    break;
                                case TerrainType::Swamp:
                                    colorPair = COLOR_TERRAIN_SWAMP;
                                    break;
                                case TerrainType::Rough:
                                    colorPair = COLOR_TERRAIN_ROUGH;
                                    break;
                                case TerrainType::Lava:
                                    colorPair = COLOR_TERRAIN_LAVA;
                                    break;
                                default:
                                    colorPair = COLOR_TERRAIN_GRASS;
                                    break;
                            }
                            break;
                    }
                }
                
                wattron(mapWin, COLOR_PAIR(colorPair));
                mvwaddch(mapWin, y + 1, x + 1, symbol);
                wattroff(mapWin, COLOR_PAIR(colorPair));
            }
        }
    }
    
    void renderStatus() {
        const Player* player = gameState.getPlayer(1);
        const Resources& res = player->getResources();
        
        // Draw border
        wattron(statusWin, COLOR_PAIR(COLOR_UI_HEADER));
        box(statusWin, 0, 0);
        mvwprintw(statusWin, 0, 2, " Status ");
        wattroff(statusWin, COLOR_UI_HEADER);
        
        // Resources
        wattron(statusWin, COLOR_PAIR(COLOR_UI_TEXT));
        mvwprintw(statusWin, 1, 2, "Day: %d", gameState.getTurnManager().getDayNumber());
        mvwprintw(statusWin, 2, 2, "Gold: %d", res.gold);
        mvwprintw(statusWin, 3, 2, "Wood: %d", res.wood);
        mvwprintw(statusWin, 4, 2, "Ore:  %d", res.ore);
        mvwprintw(statusWin, 5, 2, "Gems: %d", res.gems);
        mvwprintw(statusWin, 6, 2, "Crystal: %d", res.crystal);
        wattroff(statusWin, COLOR_PAIR(COLOR_UI_TEXT));
    }
    
    void renderHeroInfo() {
        const Hero* hero = gameState.getHero(selectedHero);
        if (!hero) return;
        
        // Draw border
        wattron(infoWin, COLOR_PAIR(COLOR_UI_HEADER));
        box(infoWin, 0, 0);
        mvwprintw(infoWin, 0, 2, " %s (Level %d) ", hero->getName().c_str(), hero->getLevel());
        wattroff(infoWin, COLOR_PAIR(COLOR_UI_HEADER));
        
        // Hero stats
        wattron(infoWin, COLOR_PAIR(COLOR_UI_TEXT));
        mvwprintw(infoWin, 1, 2, "Position: (%d,%d)", hero->getPosition().x, hero->getPosition().y);
        
        // Movement with color coding
        int movePct = (hero->getMovementPoints() * 100) / hero->getMaxMovementPoints();
        int moveColor = (movePct > 66) ? COLOR_HEALTH_GOOD : 
                       (movePct > 33) ? COLOR_HEALTH_MEDIUM : COLOR_HEALTH_LOW;
        
        mvwprintw(infoWin, 2, 2, "Movement: ");
        wattron(infoWin, COLOR_PAIR(moveColor));
        wprintw(infoWin, "%d/%d", hero->getMovementPoints(), hero->getMaxMovementPoints());
        wattroff(infoWin, COLOR_PAIR(moveColor));
        
        mvwprintw(infoWin, 3, 2, "ATT: %d  DEF: %d", hero->getAttack(), hero->getDefense());
        mvwprintw(infoWin, 4, 2, "SP: %d   KN: %d", hero->getSpellPower(), hero->getKnowledge());
        mvwprintw(infoWin, 5, 2, "Experience: %d", hero->getExperience());
        mvwprintw(infoWin, 6, 2, "Mana: %d/%d", hero->getMana(), hero->getMaxMana());
        
        // Army info
        mvwprintw(infoWin, 7, 2, "Army:");
        const Army& army = hero->getArmy();
        int line = 8;
        for (int i = 0; i < 7 && line < 9; i++) {
            const ArmySlot& slot = army.getSlot(i);
            if (!slot.isEmpty()) {
                const Creature* creature = GameState::getCreatureData(slot.creatureId);
                if (creature) {
                    mvwprintw(infoWin, line++, 4, "%d %s", slot.count, creature->getName().c_str());
                }
            }
        }
        wattroff(infoWin, COLOR_PAIR(COLOR_UI_TEXT));
    }
    
    void renderLog() {
        // Draw border
        wattron(logWin, COLOR_PAIR(COLOR_UI_HEADER));
        box(logWin, 0, 0);
        mvwprintw(logWin, 0, 2, " Game Log ");
        wattroff(logWin, COLOR_PAIR(COLOR_UI_HEADER));
        
        // Simple log placeholder
        wattron(logWin, COLOR_PAIR(COLOR_UI_TEXT));
        mvwprintw(logWin, 1, 2, "Welcome to Realms of Eldoria!");
        mvwprintw(logWin, 2, 2, "Use WASD to move your hero around the map.");
        mvwprintw(logWin, 3, 2, "Step on mines (M) to claim them for resources.");
        mvwprintw(logWin, 4, 2, "Encounter monsters (X) to engage in tactical combat.");
        wattroff(logWin, COLOR_PAIR(COLOR_UI_TEXT));
    }
    
    void handleInput() {
        int ch = getch();
        Hero* hero = gameState.getHero(selectedHero);
        GameMap* map = gameState.getMap();
        
        switch (ch) {
            case 'w':
            case 'W':
            case KEY_UP:
                moveHero(hero, map, 0, -1);
                break;
            case 's':
            case 'S':
            case KEY_DOWN:
                moveHero(hero, map, 0, 1);
                break;
            case 'a':
            case 'A':
            case KEY_LEFT:
                moveHero(hero, map, -1, 0);
                break;
            case 'd':
            case 'D':
            case KEY_RIGHT:
                moveHero(hero, map, 1, 0);
                break;
            case '\t':
            case ' ':
                switchHero();
                break;
            case 'h':
            case 'H':
                showDetailedHeroInfo();
                break;
            case 'n':
            case 'N':
                nextTurn();
                break;
            case 'q':
            case 'Q':
            case 27: // ESC
                running = false;
                break;
        }
    }
    
    void moveHero(Hero* hero, GameMap* map, int dx, int dy) {
        if (!hero) return;
        
        if (hero->getMovementPoints() <= 0) {
            showMessage("Hero is exhausted and must rest!");
            return;
        }
        
        Position currentPos = hero->getPosition();
        Position newPos(currentPos.x + dx, currentPos.y + dy, currentPos.z);
        
        if (map->isValidPosition(newPos) && map->canHeroMoveTo(hero->getId(), newPos)) {
            map->moveHero(hero->getId(), currentPos, newPos);
            hero->setPosition(newPos);
            hero->setMovementPoints(hero->getMovementPoints() - 100);
            
            checkForInteraction(hero, newPos);
        }
    }
    
    void checkForInteraction(Hero* hero, const Position& pos) {
        GameMap* map = gameState.getMap();
        const MapTile& tile = map->getTile(pos);
        
        if (tile.object != ObjectType::None && tile.object != ObjectType::Hero && tile.objectId != 0) {
            MapObject* obj = map->getObject(tile.objectId);
            if (obj) {
                switch (obj->getType()) {
                    case ObjectType::Mine: {
                        ResourceMine* mine = static_cast<ResourceMine*>(obj);
                        handleMineEncounter(hero, mine);
                        break;
                    }
                    case ObjectType::Monster: {
                        MonsterGroup* monsters = static_cast<MonsterGroup*>(obj);
                        handleMonsterEncounter(hero, monsters, map, tile.objectId);
                        break;
                    }
                    default:
                        showMessage("You found something interesting!");
                        break;
                }
            }
        }
    }
    
    void handleMineEncounter(Hero* hero, ResourceMine* mine) {
        Player* player = gameState.getPlayer(1);
        if (player && mine->getOwner() == 0) {
            mine->setOwner(1);
            
            std::string message = hero->getName() + " claimed a ";
            switch (mine->getResourceType()) {
                case ResourceType::Gold:
                    message += "Gold Mine! (+1000 gold/day)";
                    break;
                case ResourceType::Wood:
                    message += "Sawmill! (+2 wood/day)";
                    break;
                case ResourceType::Ore:
                    message += "Ore Mine! (+2 ore/day)";
                    break;
                default:
                    message += "Mine!";
                    break;
            }
            
            showMessage(message);
        } else if (mine->getOwner() == 1) {
            showMessage("This mine is already under your control.");
        }
    }
    
    void handleMonsterEncounter(Hero* hero, MonsterGroup* monsters, GameMap* map, uint32_t objectId) {
        std::string message = hero->getName() + " encounters " + 
                             std::to_string(monsters->getCount()) + " creatures!";
        showMessage(message);
        
        // Simple battle simulation for now
        showMessage("Press ENTER to engage in battle...");
        getch();
        
        BattleResult result = conductBattle(hero, monsters);
        
        if (result == BattleResult::Victory) {
            map->removeObject(objectId);
            
            int expGained = monsters->getCount() * 75;
            hero->gainExperience(expGained);
            
            message = hero->getName() + " is victorious! Gained " + std::to_string(expGained) + " experience!";
            showMessage(message);
            
            if (hero->canLevelUp()) {
                hero->levelUp();
                message = "*** " + hero->getName() + " gained a level! Now level " + 
                         std::to_string(hero->getLevel()) + " ***";
                showMessage(message);
            }
        } else {
            showMessage("The battle was not won. The monsters remain...");
        }
        
        showMessage("Press any key to continue...");
        getch();
    }
    
    BattleResult conductBattle(Hero* hero, MonsterGroup* monsters) {
        BattleEngine battle(hero);
        
        // Show battle setup
        showMessage("=== BATTLE BEGINS ===");
        
        const Army& army = hero->getArmy();
        std::string playerForces = hero->getName() + "'s forces: ";
        for (int i = 0; i < 7; i++) {
            const ArmySlot& slot = army.getSlot(i);
            if (!slot.isEmpty()) {
                battle.addPlayerUnit(slot.creatureId, slot.count);
                playerForces += std::to_string(slot.count) + " units, ";
            }
        }
        showMessage(playerForces);
        
        battle.addEnemyUnit(monsters->getCreatureType(), monsters->getCount());
        std::string enemyForces = "Enemy forces: " + std::to_string(monsters->getCount()) + " creatures";
        showMessage(enemyForces);
        
        showMessage("Press any key to start battle...");
        getch();
        
        // Execute battle with display
        BattleResult result = executeBattleWithDisplay(&battle);
        
        if (result == BattleResult::Victory || result == BattleResult::Defeat) {
            updateHeroArmyAfterBattle(hero, battle.getPlayerUnits());
        }
        
        return result;
    }
    
    BattleResult executeBattleWithDisplay(BattleEngine* battle) {
        // Show initial forces
        const auto& playerUnits = battle->getPlayerUnits();
        const auto& enemyUnits = battle->getEnemyUnits();
        
        std::string playerStatus = "Your army: ";
        for (const auto& unit : playerUnits) {
            if (unit.count > 0) {
                playerStatus += std::to_string(unit.count) + " units (HP:" + std::to_string(unit.currentHealth) + "), ";
            }
        }
        showMessage(playerStatus);
        
        std::string enemyStatus = "Enemy army: ";
        for (const auto& unit : enemyUnits) {
            if (unit.count > 0) {
                enemyStatus += std::to_string(unit.count) + " creatures (HP:" + std::to_string(unit.currentHealth) + "), ";
            }
        }
        showMessage(enemyStatus);
        
        showMessage("Battle begins! Units clash...");
        getch();
        
        // Simulate battle progression with intermediate states
        showMessage("=== ROUND 1 ===");
        showMessage("Your forces attack! Damage dealt to enemies.");
        getch();
        
        showMessage("Enemy forces counter-attack!");
        getch();
        
        // Execute the actual battle logic
        BattleResult result = battle->executeAutoBattle();
        
        // Show aftermath
        showMessage("=== BATTLE RESULT ===");
        
        // Show final unit counts
        const auto& finalPlayerUnits = battle->getPlayerUnits();
        const auto& finalEnemyUnits = battle->getEnemyUnits();
        
        std::string finalPlayer = "Your survivors: ";
        bool hasPlayerSurvivors = false;
        for (const auto& unit : finalPlayerUnits) {
            if (unit.count > 0) {
                finalPlayer += std::to_string(unit.count) + " units, ";
                hasPlayerSurvivors = true;
            }
        }
        if (!hasPlayerSurvivors) {
            finalPlayer = "Your army was defeated!";
        }
        showMessage(finalPlayer);
        
        std::string finalEnemy = "Enemy survivors: ";
        bool hasEnemySurvivors = false;
        for (const auto& unit : finalEnemyUnits) {
            if (unit.count > 0) {
                finalEnemy += std::to_string(unit.count) + " creatures, ";
                hasEnemySurvivors = true;
            }
        }
        if (!hasEnemySurvivors) {
            finalEnemy = "All enemies defeated!";
        }
        showMessage(finalEnemy);
        
        // Show result
        switch (result) {
            case BattleResult::Victory:
                showMessage("*** VICTORY! You have triumphed! ***");
                break;
            case BattleResult::Defeat:
                showMessage("*** DEFEAT! Your forces were overwhelmed! ***");
                break;
            case BattleResult::Flee:
                showMessage("*** TACTICAL WITHDRAWAL ***");
                break;
        }
        
        return result;
    }
    
    void updateHeroArmyAfterBattle(Hero* hero, const std::vector<BattleUnit>& survivingUnits) {
        Army& army = hero->getArmy();
        
        // Clear current army
        for (int i = 0; i < 7; i++) {
            army.getSlot(i) = ArmySlot();
        }
        
        // Add surviving units back
        int slotIndex = 0;
        for (const auto& unit : survivingUnits) {
            if (unit.count > 0 && slotIndex < 7) {
                army.getSlot(slotIndex) = ArmySlot(unit.creatureId, unit.count);
                slotIndex++;
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
    
    void showDetailedHeroInfo() {
        // Create a popup window for detailed hero info
        const Hero* hero = gameState.getHero(selectedHero);
        if (!hero) return;
        
        WINDOW* popup = newwin(15, 50, 5, 15);
        wattron(popup, COLOR_PAIR(COLOR_UI_HEADER));
        box(popup, 0, 0);
        mvwprintw(popup, 0, 2, " Detailed Hero Info ");
        wattroff(popup, COLOR_PAIR(COLOR_UI_HEADER));
        
        wattron(popup, COLOR_PAIR(COLOR_UI_TEXT));
        mvwprintw(popup, 1, 2, "Name: %s", hero->getName().c_str());
        mvwprintw(popup, 2, 2, "Level: %d", hero->getLevel());
        mvwprintw(popup, 3, 2, "Experience: %d", hero->getExperience());
        mvwprintw(popup, 4, 2, "Attack: %d", hero->getAttack());
        mvwprintw(popup, 5, 2, "Defense: %d", hero->getDefense());
        mvwprintw(popup, 6, 2, "Spell Power: %d", hero->getSpellPower());
        mvwprintw(popup, 7, 2, "Knowledge: %d", hero->getKnowledge());
        mvwprintw(popup, 8, 2, "Mana: %d/%d", hero->getMana(), hero->getMaxMana());
        mvwprintw(popup, 9, 2, "Movement: %d/%d", hero->getMovementPoints(), hero->getMaxMovementPoints());
        
        mvwprintw(popup, 11, 2, "Skills:");
        const auto& skills = hero->getAllSkills();
        int line = 12;
        for (const auto& [skill, level] : skills) {
            if (line >= 13) break;
            std::string skillName = "Unknown";
            switch (skill) {
                case SkillType::Leadership: skillName = "Leadership"; break;
                case SkillType::Attack: skillName = "Attack"; break;
                case SkillType::Wisdom: skillName = "Wisdom"; break;
                case SkillType::Mysticism: skillName = "Mysticism"; break;
                default: break;
            }
            mvwprintw(popup, line++, 4, "%s: %d", skillName.c_str(), level);
        }
        wattroff(popup, COLOR_PAIR(COLOR_UI_TEXT));
        
        mvwprintw(popup, 13, 2, "Press any key to close...");
        wrefresh(popup);
        getch();
        delwin(popup);
    }
    
    void nextTurn() {
        gameState.nextTurn();
        gameState.processDailyEvents();
        
        // Reset hero movement
        for (const auto& [id, hero] : gameState.getAllHeroes()) {
            hero->resetMovementPoints();
        }
        
        showMessage("New day begins! Movement points restored.");
    }
    
    void showMessage(const std::string& message) {
        // Simple message display in log window
        // In a full implementation, this could be a popup or scrolling log
        wclear(logWin);
        wattron(logWin, COLOR_PAIR(COLOR_UI_HEADER));
        box(logWin, 0, 0);
        mvwprintw(logWin, 0, 2, " Message ");
        wattroff(logWin, COLOR_PAIR(COLOR_UI_HEADER));
        
        wattron(logWin, COLOR_PAIR(COLOR_UI_TEXT));
        mvwprintw(logWin, 2, 2, "%s", message.c_str());
        wattroff(logWin, COLOR_PAIR(COLOR_UI_TEXT));
        
        wrefresh(logWin);
    }
    
    void cleanup() {
        if (mapWin) delwin(mapWin);
        if (statusWin) delwin(statusWin);
        if (infoWin) delwin(infoWin);
        if (logWin) delwin(logWin);
        
        endwin();
    }
};

int main() {
    NcursesGameClient client;
    client.run();
    
    return 0;
}