/*
 * MapView.cpp - Adventure map renderer for Realms of Eldoria
 * Based on VCMI map rendering system
 *
 * License: GNU General Public License v2.0 or later
 */
#include "MapView.h"
#include "../../lib/render/Canvas.h"
#include "../../lib/render/Image.h"
#include "../../lib/gamestate/GameState.h"
#include "../../lib/entities/hero/Hero.h"
#include <iostream>
#include <algorithm>

MapView::MapView(const Point & viewportSize)
	: cameraPos(0, 0)
	, viewportSize(viewportSize)
	, currentTileSize(64)  // Start at 64px zoom level
{
	loadTerrainTiles();
}

void MapView::loadTerrainTiles()
{
	std::cout << "Loading terrain tiles..." << std::endl;

	const std::map<TerrainType, std::string> terrainNames = {
		{TerrainType::Dirt, "dirt"},
		{TerrainType::Sand, "sand"},
		{TerrainType::Grass, "grass"},
		{TerrainType::Snow, "snow"},
		{TerrainType::Swamp, "swamp"},
		{TerrainType::Rough, "rough"},
		{TerrainType::Lava, "lava"},
		{TerrainType::Water, "water"}
	};

	for (const auto & [type, name] : terrainNames)
	{
		try
		{
			// Try relative path from binary location first (when run from build/bin/)
			std::string path = "../../assets/tiles/" + name + ".bmp";
			terrainTiles[type] = std::make_unique<Image>(path);
			std::cout << "  Loaded: " << name << ".bmp" << std::endl;
		}
		catch (const std::exception & e)
		{
			std::cerr << "  Failed to load " << name << ".bmp: " << e.what() << std::endl;
			// Create placeholder colored tile
			ColorRGBA color(128, 128, 128, 255);  // Default gray
			terrainTiles[type] = std::make_unique<Image>(Point(BASE_TILE_SIZE, BASE_TILE_SIZE), color);
		}
	}
}

void MapView::render(Canvas & canvas, const GameMap & map, const GameState & state)
{
	Rect visibleTiles = getVisibleTiles();

	// Clamp to map bounds
	visibleTiles = visibleTiles.intersect(Rect(0, 0, map.getWidth(), map.getHeight()));

	// Render layers
	renderTerrain(canvas, map, visibleTiles);
	renderObjects(canvas, map, state, visibleTiles);
	renderHeroes(canvas, state, visibleTiles);
}

void MapView::setCameraPos(const Point & pos)
{
	cameraPos = pos;
}

void MapView::moveCamera(const Point & delta)
{
	cameraPos += delta;
}

void MapView::centerOn(const Point & tilePos)
{
	// Center camera on tile position
	int tilesX = viewportSize.x / currentTileSize;
	int tilesY = viewportSize.y / currentTileSize;
	cameraPos = Point(tilePos.x - tilesX / 2, tilePos.y - tilesY / 2);
}

Point MapView::screenToTile(const Point & screenPos) const
{
	return Point(
		(screenPos.x / currentTileSize) + cameraPos.x,
		(screenPos.y / currentTileSize) + cameraPos.y
	);
}

Point MapView::tileToScreen(const Point & tilePos) const
{
	return Point(
		(tilePos.x - cameraPos.x) * currentTileSize,
		(tilePos.y - cameraPos.y) * currentTileSize
	);
}

Rect MapView::getVisibleTiles() const
{
	// Calculate tiles needed based on current zoom
	int tilesX = (viewportSize.x + currentTileSize - 1) / currentTileSize;
	int tilesY = (viewportSize.y + currentTileSize - 1) / currentTileSize;

	return Rect(cameraPos.x, cameraPos.y, tilesX, tilesY);
}

void MapView::setZoom(int tileSize)
{
	// Clamp to valid range
	currentTileSize = std::max(MIN_TILE_SIZE, std::min(MAX_TILE_SIZE, tileSize));
}

void MapView::zoomIn()
{
	// Double the tile size (32 -> 64 -> 128)
	if (currentTileSize < MAX_TILE_SIZE)
	{
		currentTileSize *= 2;
		std::cout << "Zoomed in to " << currentTileSize << "px tiles" << std::endl;
	}
}

void MapView::zoomOut()
{
	// Halve the tile size (128 -> 64 -> 32)
	if (currentTileSize > MIN_TILE_SIZE)
	{
		currentTileSize /= 2;
		std::cout << "Zoomed out to " << currentTileSize << "px tiles" << std::endl;
	}
}

void MapView::renderTerrain(Canvas & canvas, const GameMap & map, const Rect & visibleTiles)
{
	for (int y = visibleTiles.y; y < visibleTiles.y + visibleTiles.h; y++)
	{
		for (int x = visibleTiles.x; x < visibleTiles.x + visibleTiles.w; x++)
		{
			if (!map.isValidPosition(x, y, 0))
				continue;

			const MapTile & tile = map.getTile(x, y, 0);
			const Image * tileImg = getTerrainTile(tile.terrain);

			if (tileImg)
			{
				Point screenPos = tileToScreen(Point(x, y));
				// Scale tile from base 128x128 to current zoom level
				canvas.drawScaled(*tileImg, screenPos, Point(currentTileSize, currentTileSize));
			}
		}
	}
}

void MapView::renderObjects(Canvas & canvas, const GameMap & map, const GameState & state, const Rect & visibleTiles)
{
	// Get all objects on the map
	const auto & objects = map.getAllObjects();

	for (const auto & obj : objects)
	{
		const Position & pos = obj->getPosition();

		// Check if object is in visible range
		if (pos.x < visibleTiles.x || pos.x >= visibleTiles.x + visibleTiles.w ||
		    pos.y < visibleTiles.y || pos.y >= visibleTiles.y + visibleTiles.h)
			continue;

		Point screenPos = tileToScreen(Point(pos.x, pos.y));

		// Draw simple colored rectangle for objects
		ColorRGBA color;
		switch (obj->getType())
		{
			case ObjectType::Mine:
				color = ColorRGBA(192, 192, 0, 255);  // Yellow-ish
				break;
			case ObjectType::Monster:
				color = ColorRGBA(255, 0, 0, 255);    // Red
				break;
			case ObjectType::Town:
				color = ColorRGBA(128, 128, 255, 255); // Light blue
				break;
			default:
				color = ColorRGBA(255, 255, 255, 255); // White
				break;
		}

		// Draw simple object marker (scaled with zoom)
		int markerSize = currentTileSize / 4;
		int markerOffset = currentTileSize / 8;
		Rect objectRect(screenPos.x + markerOffset, screenPos.y + markerOffset, markerSize, markerSize);
		canvas.drawRect(objectRect, color);
		canvas.drawBorder(objectRect, ColorRGBA(0, 0, 0, 255), 1);
	}
}

void MapView::renderHeroes(Canvas & canvas, const GameState & state, const Rect & visibleTiles)
{
	// Get all heroes from all players
	// Since we don't have getPlayers(), we'll iterate through known heroes
	for (HeroID heroId = 1; heroId < 100; heroId++)  // Arbitrary upper limit
	{
		const Hero * hero = state.getHero(heroId);
		if (!hero)
			continue;

		const Position & pos = hero->getPosition();

		// Check if hero is in visible range
		if (pos.x < visibleTiles.x || pos.x >= visibleTiles.x + visibleTiles.w ||
		    pos.y < visibleTiles.y || pos.y >= visibleTiles.y + visibleTiles.h)
			continue;

		Point screenPos = tileToScreen(Point(pos.x, pos.y));

		// Draw hero as bright colored circle (scaled with zoom)
		ColorRGBA heroColor(0, 255, 255, 255);  // Cyan
		int heroSize = currentTileSize / 5;
		int heroOffset = currentTileSize / 4;
		Rect heroRect(screenPos.x + heroOffset, screenPos.y + heroOffset, heroSize, heroSize);
		canvas.drawRect(heroRect, heroColor);
		canvas.drawBorder(heroRect, ColorRGBA(255, 255, 255, 255), 1);
	}
}

const Image * MapView::getTerrainTile(TerrainType type) const
{
	auto it = terrainTiles.find(type);
	if (it != terrainTiles.end())
		return it->second.get();
	return nullptr;
}
