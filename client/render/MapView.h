/*
 * MapView.h - Adventure map renderer for Realms of Eldoria
 * Based on VCMI map rendering system
 *
 * License: GNU General Public License v2.0 or later
 */
#pragma once

#include "../../lib/geometry/Point.h"
#include "../../lib/geometry/Rect.h"
#include "../../lib/map/GameMap.h"
#include "../../lib/render/Image.h"
#include <memory>
#include <map>

class Canvas;
class GameState;

/// Renders the game map with tiles, objects, and heroes
class MapView
{
private:
	/// Base tile size in asset files (128x128)
	static constexpr int BASE_TILE_SIZE = 128;

	/// Minimum zoom level (tiles rendered at 32px)
	static constexpr int MIN_TILE_SIZE = 32;

	/// Maximum zoom level (tiles rendered at 128px)
	static constexpr int MAX_TILE_SIZE = 128;

	/// Loaded terrain tile images (at base 128x128 resolution)
	std::map<TerrainType, std::unique_ptr<Image>> terrainTiles;

	/// Camera position (in tile coordinates)
	Point cameraPos;

	/// Viewport size (in pixels)
	Point viewportSize;

	/// Current zoom level (tile size in pixels: 32, 64, or 128)
	int currentTileSize;

	/// Load all terrain tiles from assets directory
	void loadTerrainTiles();

public:
	/// Create map view with specified viewport size
	MapView(const Point & viewportSize);

	/// Render the visible portion of the map
	void render(Canvas & canvas, const GameMap & map, const GameState & state);

	/// Set camera position (in tile coordinates)
	void setCameraPos(const Point & pos);

	/// Get camera position
	const Point & getCameraPos() const { return cameraPos; }

	/// Move camera by delta
	void moveCamera(const Point & delta);

	/// Center camera on specific tile position
	void centerOn(const Point & tilePos);

	/// Convert screen coordinates to tile coordinates
	Point screenToTile(const Point & screenPos) const;

	/// Convert tile coordinates to screen coordinates
	Point tileToScreen(const Point & tilePos) const;

	/// Get visible tile range
	Rect getVisibleTiles() const;

	/// Set zoom level (tile size in pixels)
	void setZoom(int tileSize);

	/// Zoom in (increase tile size)
	void zoomIn();

	/// Zoom out (decrease tile size)
	void zoomOut();

	/// Get current tile size
	int getTileSize() const { return currentTileSize; }

private:
	/// Render terrain layer
	void renderTerrain(Canvas & canvas, const GameMap & map, const Rect & visibleTiles);

	/// Render objects layer
	void renderObjects(Canvas & canvas, const GameMap & map, const GameState & state, const Rect & visibleTiles);

	/// Render heroes layer
	void renderHeroes(Canvas & canvas, const GameState & state, const Rect & visibleTiles);

	/// Get terrain tile image
	const Image * getTerrainTile(TerrainType type) const;
};
