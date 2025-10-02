/*
 * Image.h - Simplified image system for Realms of Eldoria
 * Based on VCMI SDLImage
 *
 * Original VCMI Authors: listed in file AUTHORS in VCMI repository
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */
#pragma once

#include "../geometry/Point.h"
#include "../geometry/Rect.h"
#include "../geometry/Color.h"
#include <SDL2/SDL.h>
#include <string>
#include <memory>

class Canvas;

/// Simplified image class for loading and drawing sprites
class Image
{
private:
	SDL_Surface * surface;
	bool ownsSurface;
	Point margins;      // Left/top margins for trimmed images
	Point fullSize;     // Full size including margins

public:
	/// Load image from BMP file
	explicit Image(const std::string & filename);

	/// Create from existing surface (takes ownership if own=true)
	explicit Image(SDL_Surface * surf, bool own = true);

	/// Create filled image of specified size
	Image(const Point & size, const ColorRGBA & color);

	~Image();

	// Disable copy, allow move
	Image(const Image &) = delete;
	Image & operator=(const Image &) = delete;
	Image(Image && other) noexcept;
	Image & operator=(Image && other) noexcept;

	/// Get image dimensions
	Point dimensions() const { return Point(surface->w, surface->h); }

	/// Get full size including margins
	Point fullDimensions() const { return fullSize; }

	/// Draw onto canvas at position
	void draw(Canvas & canvas, const Point & pos) const;

	/// Draw section of image onto canvas
	void draw(Canvas & canvas, const Point & pos, const Rect & srcRect) const;

	/// Get underlying SDL surface
	SDL_Surface * getSurface() { return surface; }
	const SDL_Surface * getSurface() const { return surface; }

	/// Check if pixel at coords is transparent
	bool isTransparent(const Point & coords) const;

	/// Create a copy of this image
	std::unique_ptr<Image> clone() const;

	/// Flip horizontally
	std::unique_ptr<Image> horizontalFlip() const;

	/// Flip vertically
	std::unique_ptr<Image> verticalFlip() const;
};
