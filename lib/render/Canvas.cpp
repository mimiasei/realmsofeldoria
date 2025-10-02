/*
 * Canvas.cpp - Simplified canvas for Realms of Eldoria
 * Based on VCMI engine Canvas class
 *
 * Original VCMI Authors: listed in file AUTHORS in VCMI repository
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */
#include "Canvas.h"
#include "Image.h"
#include <algorithm>
#include <stdexcept>
#include <string>

Canvas::Canvas(const Point & size)
	: surface(nullptr)
	, ownsSurface(true)
	, renderArea(0, 0, size.x, size.y)
{
	surface = SDL_CreateRGBSurface(0, size.x, size.y, 32,
		0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

	if (!surface)
	{
		throw std::runtime_error(std::string("Failed to create SDL surface: ") + SDL_GetError());
	}
}

Canvas Canvas::createFromSurface(SDL_Surface * surf)
{
	return Canvas(surf, false, Rect(0, 0, surf->w, surf->h));
}

Canvas::Canvas(const Canvas & other, const Rect & clipRect)
	: surface(other.surface)
	, ownsSurface(false)
	, renderArea(clipRect)
{
	// Clamp renderArea to actual surface bounds
	renderArea = renderArea.intersect(Rect(0, 0, surface->w, surface->h));
}

Canvas::Canvas(SDL_Surface * surf, bool owns, const Rect & area)
	: surface(surf)
	, ownsSurface(owns)
	, renderArea(area)
{
}

Canvas::~Canvas()
{
	if (ownsSurface && surface)
	{
		SDL_FreeSurface(surface);
	}
}

Point Canvas::dimensions() const
{
	return Point(renderArea.w, renderArea.h);
}

uint32_t Canvas::mapColor(const ColorRGBA & color) const
{
	return SDL_MapRGBA(surface->format, color.r, color.g, color.b, color.a);
}

void Canvas::draw(const Canvas & other, const Point & pos)
{
	SDL_Rect srcRect = { other.renderArea.x, other.renderArea.y,
	                      other.renderArea.w, other.renderArea.h };
	SDL_Rect dstRect = { renderArea.x + pos.x, renderArea.y + pos.y,
	                      other.renderArea.w, other.renderArea.h };

	SDL_BlitSurface(other.surface, &srcRect, surface, &dstRect);
}

void Canvas::draw(const Image & image, const Point & pos)
{
	SDL_Rect dstRect = { renderArea.x + pos.x, renderArea.y + pos.y, 0, 0 };
	SDL_BlitSurface(const_cast<SDL_Surface*>(image.getSurface()), nullptr, surface, &dstRect);
}

void Canvas::draw(const Image & image, const Point & pos, const Rect & srcRect)
{
	SDL_Rect src = { srcRect.x, srcRect.y, srcRect.w, srcRect.h };
	SDL_Rect dst = { renderArea.x + pos.x, renderArea.y + pos.y, srcRect.w, srcRect.h };
	SDL_BlitSurface(const_cast<SDL_Surface*>(image.getSurface()), &src, surface, &dst);
}

void Canvas::drawScaled(const Image & image, const Point & pos, const Point & targetSize)
{
	// Use SDL_BlitScaled for efficient scaling
	SDL_Rect dst = { renderArea.x + pos.x, renderArea.y + pos.y, targetSize.x, targetSize.y };
	SDL_BlitScaled(const_cast<SDL_Surface*>(image.getSurface()), nullptr, surface, &dst);
}

void Canvas::drawRect(const Rect & rect, const ColorRGBA & color)
{
	SDL_Rect sdlRect = {
		renderArea.x + rect.x,
		renderArea.y + rect.y,
		rect.w,
		rect.h
	};

	SDL_FillRect(surface, &sdlRect, mapColor(color));
}

void Canvas::drawBorder(const Rect & rect, const ColorRGBA & color, int width)
{
	// Top
	drawRect(Rect(rect.x, rect.y, rect.w, width), color);
	// Bottom
	drawRect(Rect(rect.x, rect.y + rect.h - width, rect.w, width), color);
	// Left
	drawRect(Rect(rect.x, rect.y, width, rect.h), color);
	// Right
	drawRect(Rect(rect.x + rect.w - width, rect.y, width, rect.h), color);
}

void Canvas::drawLine(const Point & from, const Point & to, const ColorRGBA & color)
{
	// Bresenham's line algorithm
	Point p1 = from + Point(renderArea.x, renderArea.y);
	Point p2 = to + Point(renderArea.x, renderArea.y);

	int dx = std::abs(p2.x - p1.x);
	int dy = std::abs(p2.y - p1.y);
	int sx = p1.x < p2.x ? 1 : -1;
	int sy = p1.y < p2.y ? 1 : -1;
	int err = dx - dy;

	while (true)
	{
		drawPoint(Point(p1.x - renderArea.x, p1.y - renderArea.y), color);

		if (p1.x == p2.x && p1.y == p2.y)
			break;

		int e2 = 2 * err;
		if (e2 > -dy)
		{
			err -= dy;
			p1.x += sx;
		}
		if (e2 < dx)
		{
			err += dx;
			p1.y += sy;
		}
	}
}

void Canvas::drawPoint(const Point & pos, const ColorRGBA & color)
{
	int x = renderArea.x + pos.x;
	int y = renderArea.y + pos.y;

	if (x < 0 || x >= surface->w || y < 0 || y >= surface->h)
		return;

	uint32_t * pixels = (uint32_t*)surface->pixels;
	pixels[y * surface->w + x] = mapColor(color);
}

void Canvas::fill(const ColorRGBA & color)
{
	SDL_Rect area = { renderArea.x, renderArea.y, renderArea.w, renderArea.h };
	SDL_FillRect(surface, &area, mapColor(color));
}

void Canvas::clear()
{
	fill(ColorRGBA(0, 0, 0, 0));
}
