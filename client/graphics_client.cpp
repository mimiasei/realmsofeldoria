/*
 * graphics_client.cpp - Full graphical client with UI
 * Realms of Eldoria
 */
#include <SDL2/SDL.h>
#include <iostream>
#include <memory>
#include "../include/GameTypes.h"
#include "../lib/gamestate/GameState.h"
#include "../lib/map/GameMap.h"
#include "../lib/entities/hero/Hero.h"
#include "../lib/battle/Battle.h"
#include "../lib/render/Canvas.h"
#include "../lib/render/Font.h"
#include "render/MapView.h"
#include "ui/ResourceBar.h"
#include "ui/HeroPanel.h"
#include "ui/BattleWindow.h"

const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;
const int MAP_WIDTH = 40;
const int MAP_HEIGHT = 25;

class GraphicsClient {
private:
    SDL_Window* window;
    SDL_Surface* screenSurface;
    std::unique_ptr<GameState> gameState;
    std::unique_ptr<MapView> mapView;
    std::unique_ptr<ResourceBar> resourceBar;
    std::unique_ptr<HeroPanel> heroPanel;
    std::unique_ptr<BattleWindow> battleWindow;

    bool running;
    Hero* selectedHero;
    bool inBattle;

    void initializeGameState() {
        // Create player
        auto player = std::make_unique<Player>(1, "Player 1", Faction::Castle);
        player->addResources(Resources{10, 5, 10, 5, 5, 5, 20000}); // Starting resources

        // Create first hero
        auto hero1 = std::make_unique<Hero>(1, "Sir Aldric", HeroClass::Knight, Gender::Male);
        hero1->setPrimaryStats(5, 4, 2, 2);
        hero1->setPosition(Position(5, 7, 0));
        hero1->setSkill(SkillType::Leadership, 2);
        hero1->setSkill(SkillType::Attack, 1);

        // Give Sir Aldric starting troops
        hero1->getArmy().addCreatures(1, 10); // 10 peasants
        hero1->getArmy().addCreatures(2, 5);  // 5 archers
        hero1->resetMovementPoints();

        // Create second hero
        auto hero2 = std::make_unique<Hero>(2, "Lady Morgana", HeroClass::Wizard, Gender::Female);
        hero2->setPrimaryStats(3, 4, 8, 7);
        hero2->setPosition(Position(12, 8, 0));
        hero2->setSkill(SkillType::Wisdom, 2);
        hero2->setSkill(SkillType::Mysticism, 1);

        // Give Lady Morgana starting troops
        hero2->getArmy().addCreatures(1, 8); // 8 peasants
        hero2->getArmy().addCreatures(2, 7); // 7 archers
        hero2->resetMovementPoints();

        player->addHero(1);
        player->addHero(2);
        gameState->addHero(std::move(hero1));
        gameState->addHero(std::move(hero2));
        gameState->addPlayer(std::move(player));

        // Create and initialize map
        auto map = std::make_unique<GameMap>(MAP_WIDTH, MAP_HEIGHT, 1);
        map->setName("Tutorial Valley");

        // Add resource mines
        auto goldMine = std::make_unique<ResourceMine>(1, Position(3, 3, 0), ResourceType::Gold, 1000);
        auto woodMine = std::make_unique<ResourceMine>(2, Position(15, 10, 0), ResourceType::Wood, 2);
        auto oreMine = std::make_unique<ResourceMine>(3, Position(17, 3, 0), ResourceType::Ore, 2);

        // Add monster groups
        auto weakMonsters = std::make_unique<MonsterGroup>(4, Position(8, 5, 0), 1, 3);
        auto mediumMonsters = std::make_unique<MonsterGroup>(5, Position(14, 7, 0), 2, 4);
        auto strongMonsters = std::make_unique<MonsterGroup>(6, Position(10, 12, 0), 1, 8);
        auto eliteMonsters = std::make_unique<MonsterGroup>(7, Position(6, 2, 0), 2, 6);

        // Mine guards
        auto mineGuard1 = std::make_unique<MonsterGroup>(8, Position(4, 3, 0), 1, 4);
        auto mineGuard2 = std::make_unique<MonsterGroup>(9, Position(16, 10, 0), 2, 3);
        auto mineGuard3 = std::make_unique<MonsterGroup>(10, Position(18, 3, 0), 1, 6);

        // Roaming groups
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

        gameState->setMap(std::move(map));
        gameState->startGame();
    }

    BattleResult conductBattle(Hero* hero, MonsterGroup* monsters) {
        // Create battle engine
        BattleEngine battle(hero);

        // Add hero's army to battle
        const Army& army = hero->getArmy();
        for (int i = 0; i < 7; i++) {
            const ArmySlot& slot = army.getSlot(i);
            if (!slot.isEmpty()) {
                battle.addPlayerUnit(slot.creatureId, slot.count);
            }
        }

        // Add monster units to battle
        battle.addEnemyUnit(monsters->getCreatureType(), monsters->getCount());

        // Execute auto-battle
        BattleResult result = battle.executeAutoBattle();

        // Update hero's army based on battle results
        if (result == BattleResult::Victory || result == BattleResult::Defeat) {
            updateHeroArmyAfterBattle(hero, battle.getPlayerUnits());
        }

        // Award experience for victory
        if (result == BattleResult::Victory) {
            int expGained = battle.calculateExperienceGained();
            hero->gainExperience(expGained);
        }

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

    void handleEvents() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            else if (event.type == SDL_KEYDOWN) {
                handleKeyPress(event.key.keysym.sym);
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN) {
                handleMouseClick(event.button.x, event.button.y, event.button.button);
            }
            else if (event.type == SDL_MOUSEMOTION) {
                handleMouseMove(event.motion.x, event.motion.y);
            }
            else if (event.type == SDL_MOUSEWHEEL) {
                handleMouseWheel(event.wheel.y);
            }
        }
    }

    void handleKeyPress(SDL_Keycode key) {
        switch (key) {
            case SDLK_ESCAPE:
            case SDLK_q:
                running = false;
                break;

            case SDLK_UP:
                mapView->moveCamera(Point(0, -1));
                break;
            case SDLK_DOWN:
                mapView->moveCamera(Point(0, 1));
                break;
            case SDLK_LEFT:
                mapView->moveCamera(Point(-1, 0));
                break;
            case SDLK_RIGHT:
                mapView->moveCamera(Point(1, 0));
                break;

            case SDLK_PLUS:
            case SDLK_EQUALS:
                mapView->zoomIn();
                break;
            case SDLK_MINUS:
                mapView->zoomOut();
                break;

            case SDLK_n:
                // Next turn
                gameState->nextTurn();
                refreshUI();
                break;

            case SDLK_TAB:
                // Switch hero
                selectNextHero();
                break;

            case SDLK_SPACE:
                // Center on selected hero
                if (selectedHero) {
                    const Position& pos = selectedHero->getPosition();
                    mapView->centerOn(Point(pos.x, pos.y));
                }
                break;
        }
    }

    void handleMouseClick(int x, int y, int button) {
        Point clickPos(x, y);

        // If in battle, handle battle window clicks first
        if (inBattle && battleWindow && battleWindow->visible) {
            if (battleWindow->onClick(clickPos)) {
                return;
            }
        }

        // Check UI elements first
        if (heroPanel && heroPanel->onClick(clickPos)) {
            refreshUI();
            return;
        }

        if (button == SDL_BUTTON_LEFT) {
            // Click on map - try to select hero or move selected hero
            Point tilePos = mapView->screenToTile(clickPos);

            // Check if clicking on a hero
            for (const auto& [playerId, player] : gameState->getAllPlayers()) {
                for (HeroID heroId : player->getHeroes()) {
                    Hero* hero = gameState->getHero(heroId);
                    if (hero) {
                        const Position& heroPos = hero->getPosition();
                        if (heroPos.x == tilePos.x && heroPos.y == tilePos.y) {
                            selectedHero = hero;
                            heroPanel->setHero(selectedHero);
                            mapView->centerOn(tilePos);
                            return;
                        }
                    }
                }
            }

            // Try to move selected hero to this position
            if (selectedHero) {
                // Check if adjacent and has movement points
                const Position& heroPos = selectedHero->getPosition();
                int dx = std::abs(tilePos.x - heroPos.x);
                int dy = std::abs(tilePos.y - heroPos.y);

                if ((dx == 1 && dy == 0) || (dx == 0 && dy == 1)) {
                    if (selectedHero->getMovementPoints() > 0) {
                        // Check for objects at target position
                        Position targetPos(tilePos.x, tilePos.y, heroPos.z);
                        auto objects = gameState->getMap()->getObjectsAt(targetPos);

                        MapObject* monsterObj = nullptr;
                        for (auto* obj : objects) {
                            if (obj->getType() == ObjectType::Monster) {
                                monsterObj = obj;
                                break;
                            }
                        }

                        if (monsterObj) {
                            // Monster encounter - start battle UI!
                            MonsterGroup* monsters = static_cast<MonsterGroup*>(monsterObj);

                            // Create battle engine
                            BattleEngine* battle = new BattleEngine(selectedHero);

                            // Add hero's army to battle
                            Army& army = selectedHero->getArmy();
                            for (int i = 0; i < 7; i++) {
                                const ArmySlot& slot = army.getSlot(i);
                                if (slot.count > 0) {
                                    battle->addPlayerUnit(slot.creatureId, slot.count);
                                }
                            }

                            // Add monster units to battle
                            battle->addEnemyUnit(monsters->getCreatureType(), monsters->getCount());

                            // Show battle window
                            inBattle = true;
                            battleWindow->startBattle(battle, gameState.get());
                            battleWindow->setOnBattleComplete([this, battle, selectedHero = selectedHero,
                                                              monsters, monsterObj, targetPos]() {
                                BattleResult result = battleWindow->getBattleResult();

                                // Update hero's army based on battle results
                                updateHeroArmyAfterBattle(selectedHero, battle->getPlayerUnits());

                                if (result == BattleResult::Victory) {
                                    // Remove the monster group from map
                                    gameState->getMap()->removeObject(monsterObj->getId());
                                    // Move hero to the position
                                    selectedHero->setPosition(targetPos);
                                    selectedHero->setMovementPoints(selectedHero->getMovementPoints() - 1);
                                    // Award experience
                                    int expGained = battle->calculateExperienceGained();
                                    selectedHero->gainExperience(expGained);
                                } else if (result == BattleResult::Defeat) {
                                    // Hero loses movement points but doesn't move
                                    selectedHero->setMovementPoints(0);
                                }

                                delete battle;
                                inBattle = false;
                                refreshUI();
                            });
                        } else {
                            // Normal movement
                            selectedHero->setPosition(targetPos);
                            selectedHero->setMovementPoints(selectedHero->getMovementPoints() - 1);
                            refreshUI();
                        }
                    }
                }
            }
        }
    }

    void handleMouseMove(int x, int y) {
        Point hoverPos(x, y);

        if (heroPanel) {
            heroPanel->onHover(hoverPos);
        }
    }

    void handleMouseWheel(int delta) {
        if (delta > 0) {
            mapView->zoomIn();
        } else if (delta < 0) {
            mapView->zoomOut();
        }
    }

    void selectNextHero() {
        Player* currentPlayer = gameState->getPlayer(gameState->getCurrentPlayer());
        if (!currentPlayer) {
            selectedHero = nullptr;
            heroPanel->setHero(nullptr);
            return;
        }

        const std::vector<HeroID>& heroIds = currentPlayer->getHeroes();
        if (heroIds.empty()) {
            selectedHero = nullptr;
            heroPanel->setHero(nullptr);
            return;
        }

        if (!selectedHero) {
            selectedHero = gameState->getHero(heroIds[0]);
        } else {
            // Find current hero and select next
            for (size_t i = 0; i < heroIds.size(); ++i) {
                if (gameState->getHero(heroIds[i]) == selectedHero) {
                    selectedHero = gameState->getHero(heroIds[(i + 1) % heroIds.size()]);
                    break;
                }
            }
        }

        if (selectedHero) {
            heroPanel->setHero(selectedHero);
            const Position& pos = selectedHero->getPosition();
            mapView->centerOn(Point(pos.x, pos.y));
        }
    }

    void refreshUI() {
        if (resourceBar) {
            resourceBar->refresh();
        }
        if (heroPanel) {
            heroPanel->refresh();
        }
    }

    void render() {
        // Create canvas from window surface for this frame
        Canvas canvas = Canvas::createFromSurface(screenSurface);

        // Clear screen
        canvas.fill(ColorRGBA(20, 20, 30));

        // Render map (takes most of the screen, with space for UI)
        if (mapView && gameState && gameState->getMap()) {
            mapView->render(canvas, *gameState->getMap(), *gameState);
        }

        // Render UI elements (if not in battle)
        if (!inBattle) {
            if (resourceBar) {
                resourceBar->render(canvas);
            }

            if (heroPanel) {
                heroPanel->render(canvas);
            }
        }

        // Render battle window if active
        if (inBattle && battleWindow && battleWindow->visible) {
            battleWindow->render(canvas);
        }

        // Update screen
        SDL_UpdateWindowSurface(window);
    }

public:
    GraphicsClient()
        : window(nullptr)
        , screenSurface(nullptr)
        , running(false)
        , selectedHero(nullptr)
        , inBattle(false)
    {
    }

    ~GraphicsClient() {
        cleanup();
    }

    bool initialize() {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
            return false;
        }

        window = SDL_CreateWindow(
            "Realms of Eldoria",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            SCREEN_WIDTH,
            SCREEN_HEIGHT,
            SDL_WINDOW_SHOWN
        );

        if (!window) {
            std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
            return false;
        }

        screenSurface = SDL_GetWindowSurface(window);
        if (!screenSurface) {
            std::cerr << "Failed to get window surface: " << SDL_GetError() << std::endl;
            return false;
        }

        // Initialize game state
        gameState = std::make_unique<GameState>();

        // Initialize game with test data (similar to ASCII client)
        initializeGameState();

        // Create map view with viewport size
        mapView = std::make_unique<MapView>(Point(SCREEN_WIDTH, SCREEN_HEIGHT));

        // Create UI components
        resourceBar = std::make_unique<ResourceBar>(gameState.get());
        heroPanel = std::make_unique<HeroPanel>(gameState.get());

        // Create battle window (centered on screen, 1920x1080)
        battleWindow = std::make_unique<BattleWindow>(Point(0, 0), Point(1920, 1080));
        battleWindow->setVisible(false); // Hidden by default

        // Select first hero if any exist
        Player* currentPlayer = gameState->getPlayer(gameState->getCurrentPlayer());
        if (currentPlayer && !currentPlayer->getHeroes().empty()) {
            selectedHero = gameState->getHero(currentPlayer->getHeroes()[0]);
            if (selectedHero) {
                heroPanel->setHero(selectedHero);
                const Position& pos = selectedHero->getPosition();
                mapView->centerOn(Point(pos.x, pos.y));
            }
        }

        refreshUI();

        std::cout << "Graphics client initialized successfully!" << std::endl;
        std::cout << "\nControls:" << std::endl;
        std::cout << "  Arrow Keys: Scroll map" << std::endl;
        std::cout << "  Mouse Wheel / +/-: Zoom in/out" << std::endl;
        std::cout << "  Left Click: Select hero or move" << std::endl;
        std::cout << "  TAB: Switch between heroes" << std::endl;
        std::cout << "  SPACE: Center on selected hero" << std::endl;
        std::cout << "  N: Next turn" << std::endl;
        std::cout << "  ESC/Q: Quit" << std::endl;

        return true;
    }

    void run() {
        running = true;

        while (running) {
            handleEvents();
            render();
            SDL_Delay(16);  // ~60 FPS
        }
    }

    void cleanup() {
        battleWindow.reset();
        heroPanel.reset();
        resourceBar.reset();
        mapView.reset();
        gameState.reset();

        if (window) {
            SDL_DestroyWindow(window);
            window = nullptr;
        }

        SDL_Quit();
    }
};

int main(int argc, char* argv[]) {
    GraphicsClient client;

    if (!client.initialize()) {
        std::cerr << "Failed to initialize graphics client" << std::endl;
        return 1;
    }

    client.run();

    return 0;
}
