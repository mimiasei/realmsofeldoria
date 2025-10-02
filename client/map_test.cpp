/*
 * map_test.cpp - Test map rendering with tiles
 * Part of Realms of Eldoria
 */
#include <iostream>
#include <SDL2/SDL.h>
#include "../lib/render/Canvas.h"
#include "../lib/gamestate/GameState.h"
#include "../lib/entities/hero/Hero.h"
#include "../lib/map/GameMap.h"
#include "render/MapView.h"

int main(int argc, char* argv[])
{
	std::cout << "Starting Realms of Eldoria Map Renderer Test..." << std::endl;

	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
		return 1;
	}

	// Create window
	const int WINDOW_WIDTH = 1920;
	const int WINDOW_HEIGHT = 1080;

	SDL_Window* window = SDL_CreateWindow(
		"Realms of Eldoria - Map View Test",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		WINDOW_WIDTH, WINDOW_HEIGHT,
		SDL_WINDOW_SHOWN
	);

	if (!window)
	{
		std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}

	// Get window surface
	SDL_Surface* windowSurface = SDL_GetWindowSurface(window);

	// Create game state with map
	GameState gameState;

	// Create player
	auto player = std::make_unique<Player>(1, "Player 1", Faction::Castle, true);
	player->getResources().gold = 10000;

	// Create heroes
	auto hero1 = std::make_unique<Hero>(1, "Sir Arthur", HeroClass::Knight, Gender::Male);
	hero1->setPrimaryStats(8, 6, 2, 3);
	hero1->setPosition(Position(5, 7, 0));
	hero1->getArmy().addCreatures(1, 10);  // Peasants
	hero1->getArmy().addCreatures(2, 5);   // Archers

	auto hero2 = std::make_unique<Hero>(2, "Lady Morgana", HeroClass::Wizard, Gender::Female);
	hero2->setPrimaryStats(3, 4, 8, 7);
	hero2->setPosition(Position(12, 8, 0));
	hero2->getArmy().addCreatures(1, 8);
	hero2->getArmy().addCreatures(2, 7);

	player->addHero(1);
	player->addHero(2);
	gameState.addHero(std::move(hero1));
	gameState.addHero(std::move(hero2));
	gameState.addPlayer(std::move(player));

	// Create and initialize map - 40x25 to fill 30x16 viewport with room to scroll
	auto map = std::make_unique<GameMap>(40, 25);
	map->setName("Test Adventure Map");

	// Add some varied terrain
	for (int y = 0; y < 25; y++)
	{
		for (int x = 0; x < 40; x++)
		{
			TerrainType terrain = TerrainType::Grass;

			// Create interesting terrain patterns
			if (y < 4) terrain = TerrainType::Snow;
			else if (y > 20) terrain = TerrainType::Swamp;
			else if (x < 5) terrain = TerrainType::Water;
			else if (x > 35) terrain = TerrainType::Lava;
			else if ((x + y) % 7 == 0) terrain = TerrainType::Rough;
			else if ((x * y) % 11 == 0) terrain = TerrainType::Sand;
			else if ((x - y) % 5 == 0) terrain = TerrainType::Dirt;

			map->getTile(x, y, 0).terrain = terrain;
		}
	}

	// Add some objects scattered across the larger map
	auto mine1 = std::make_unique<ResourceMine>(1, Position(8, 5, 0), ResourceType::Gold, 1000);
	auto mine2 = std::make_unique<ResourceMine>(2, Position(25, 15, 0), ResourceType::Wood, 5);
	auto mine3 = std::make_unique<ResourceMine>(5, Position(35, 8, 0), ResourceType::Ore, 10);
	auto monster1 = std::make_unique<MonsterGroup>(3, Position(10, 7, 0), 3, 10);
	auto monster2 = std::make_unique<MonsterGroup>(4, Position(20, 12, 0), 4, 15);
	auto monster3 = std::make_unique<MonsterGroup>(6, Position(30, 18, 0), 5, 20);

	map->addObject(std::move(mine1));
	map->addObject(std::move(mine2));
	map->addObject(std::move(mine3));
	map->addObject(std::move(monster1));
	map->addObject(std::move(monster2));
	map->addObject(std::move(monster3));

	gameState.setMap(std::move(map));
	gameState.startGame();

	// Create map view
	MapView mapView(Point(WINDOW_WIDTH, WINDOW_HEIGHT));
	mapView.centerOn(Point(20, 12));  // Center on middle of map

	std::cout << "Map initialized. Rendering..." << std::endl;

	// Create canvas from window surface
	Canvas canvas = Canvas::createFromSurface(windowSurface);

	// Render the map
	mapView.render(canvas, *gameState.getMap(), gameState);

	// Update window
	SDL_UpdateWindowSurface(window);

	std::cout << "Rendering complete. Controls:" << std::endl;
	std::cout << "  Arrow keys: Scroll map" << std::endl;
	std::cout << "  +/=: Zoom in" << std::endl;
	std::cout << "  -: Zoom out" << std::endl;
	std::cout << "  HOME: Center on map" << std::endl;
	std::cout << "  ESC/Q: Exit" << std::endl;

	// Event loop with camera controls
	bool running = true;
	SDL_Event event;
	while (running)
	{
		bool needsRedraw = false;

		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				running = false;
			}
			else if (event.type == SDL_KEYDOWN)
			{
				switch (event.key.keysym.sym)
				{
					case SDLK_ESCAPE:
					case SDLK_q:
						running = false;
						break;

					case SDLK_LEFT:
						mapView.moveCamera(Point(-1, 0));
						needsRedraw = true;
						break;

					case SDLK_RIGHT:
						mapView.moveCamera(Point(1, 0));
						needsRedraw = true;
						break;

					case SDLK_UP:
						mapView.moveCamera(Point(0, -1));
						needsRedraw = true;
						break;

					case SDLK_DOWN:
						mapView.moveCamera(Point(0, 1));
						needsRedraw = true;
						break;

					case SDLK_HOME:
						mapView.centerOn(Point(20, 12));
						needsRedraw = true;
						break;

					case SDLK_EQUALS:
					case SDLK_PLUS:
						mapView.zoomIn();
						needsRedraw = true;
						break;

					case SDLK_MINUS:
						mapView.zoomOut();
						needsRedraw = true;
						break;
				}
			}
			else if (event.type == SDL_MOUSEWHEEL)
			{
				if (event.wheel.y > 0)  // Scroll up = zoom in
				{
					mapView.zoomIn();
					needsRedraw = true;
				}
				else if (event.wheel.y < 0)  // Scroll down = zoom out
				{
					mapView.zoomOut();
					needsRedraw = true;
				}
			}
		}

		if (needsRedraw)
		{
			canvas.fill(ColorRGBA(0, 0, 0, 255));
			mapView.render(canvas, *gameState.getMap(), gameState);
			SDL_UpdateWindowSurface(window);
		}

		SDL_Delay(16); // ~60 FPS
	}

	// Cleanup
	SDL_DestroyWindow(window);
	SDL_Quit();

	std::cout << "Map test ended successfully." << std::endl;
	return 0;
}
