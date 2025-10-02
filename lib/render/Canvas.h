/*
 * Canvas.h - Simplified canvas for Realms of Eldoria
 * Based on VCMI engine Canvas class
 *
 * Original VCMI Authors: listed in file AUTHORS in VCMI repository
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */
#pragma once

#include "../geometry/Rect.h"
#include "../geometry/Color.h"
#include <SDL2/SDL.h>
#include <memory>

class Image;

/// Simplified canvas class for rendering operations
class Canvas
{
private:
	SDL_Surface * surface;
	bool ownsSurface;  // true if we created the surface and should delete it
	Rect renderArea;

public:
	/// Create canvas with new surface of specified size
	explicit Canvas(const Point & size);

	/// Create canvas from existing surface (doesn't take ownership)
	static Canvas createFromSurface(SDL_Surface * surf);

	/// Create sub-canvas with clipping rect
	Canvas(const Canvas & other, const Rect & clipRect);

	~Canvas();

	// Disable copy/move for now (can implement later if needed)
	Canvas(const Canvas &) = delete;
	Canvas & operator=(const Canvas &) = delete;

	/// Get underlying SDL surface (for compatibility)
	SDL_Surface * getSurface() { return surface; }
	const SDL_Surface * getSurface() const { return surface; }

	/// Get canvas dimensions
	Point dimensions() const;

	/// Draw another canvas onto this one
	void draw(const Canvas & other, const Point & pos);

	/// Draw image onto canvas
	void draw(const Image & image, const Point & pos);

	/// Draw section of image onto canvas
	void draw(const Image & image, const Point & pos, const Rect & srcRect);

	/// Draw image scaled to target size
	void drawScaled(const Image & image, const Point & pos, const Point & targetSize);

	/// Draw filled rectangle
	void drawRect(const Rect & rect, const ColorRGBA & color);

	/// Draw rectangle border
	void drawBorder(const Rect & rect, const ColorRGBA & color, int width = 1);

	/// Draw line
	void drawLine(const Point & from, const Point & to, const ColorRGBA & color);

	/// Draw single pixel
	void drawPoint(const Point & pos, const ColorRGBA & color);

	/// Fill entire canvas with color
	void fill(const ColorRGBA & color);

	/// Clear canvas (fill with transparent black)
	void clear();

private:
	/// Constructor for sub-canvas
	Canvas(SDL_Surface * surf, bool owns, const Rect & area);

	/// Convert ColorRGBA to SDL pixel format
	uint32_t mapColor(const ColorRGBA & color) const;
};
