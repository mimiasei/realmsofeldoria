/*
 * Image.cpp - Simplified image system for Realms of Eldoria
 * Based on VCMI SDLImage
 *
 * Original VCMI Authors: listed in file AUTHORS in VCMI repository
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */
#include "Image.h"
#include "Canvas.h"
#include <stdexcept>
#include <cstring>

Image::Image(const std::string & filename)
	: surface(nullptr)
	, ownsSurface(true)
	, margins(0, 0)
	, fullSize(0, 0)
{
	surface = SDL_LoadBMP(filename.c_str());
	if (!surface)
	{
		throw std::runtime_error("Failed to load image " + filename + ": " + SDL_GetError());
	}

	fullSize = Point(surface->w, surface->h);
}

Image::Image(SDL_Surface * surf, bool own)
	: surface(surf)
	, ownsSurface(own)
	, margins(0, 0)
	, fullSize(surf ? Point(surf->w, surf->h) : Point(0, 0))
{
	if (!surface)
	{
		throw std::runtime_error("Cannot create Image from null surface");
	}
}

Image::Image(const Point & size, const ColorRGBA & color)
	: surface(nullptr)
	, ownsSurface(true)
	, margins(0, 0)
	, fullSize(size)
{
	surface = SDL_CreateRGBSurface(0, size.x, size.y, 32,
		0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

	if (!surface)
	{
		throw std::runtime_error("Failed to create surface");
	}

	// Fill with color
	SDL_FillRect(surface, nullptr,
		SDL_MapRGBA(surface->format, color.r, color.g, color.b, color.a));
}

Image::~Image()
{
	if (ownsSurface && surface)
	{
		SDL_FreeSurface(surface);
	}
}

Image::Image(Image && other) noexcept
	: surface(other.surface)
	, ownsSurface(other.ownsSurface)
	, margins(other.margins)
	, fullSize(other.fullSize)
{
	other.surface = nullptr;
	other.ownsSurface = false;
}

Image & Image::operator=(Image && other) noexcept
{
	if (this != &other)
	{
		if (ownsSurface && surface)
		{
			SDL_FreeSurface(surface);
		}

		surface = other.surface;
		ownsSurface = other.ownsSurface;
		margins = other.margins;
		fullSize = other.fullSize;

		other.surface = nullptr;
		other.ownsSurface = false;
	}
	return *this;
}

void Image::draw(Canvas & canvas, const Point & pos) const
{
	SDL_Rect dstRect = { pos.x, pos.y, surface->w, surface->h };
	SDL_BlitSurface(surface, nullptr, canvas.getSurface(), &dstRect);
}

void Image::draw(Canvas & canvas, const Point & pos, const Rect & srcRect) const
{
	SDL_Rect src = { srcRect.x, srcRect.y, srcRect.w, srcRect.h };
	SDL_Rect dst = { pos.x, pos.y, srcRect.w, srcRect.h };
	SDL_BlitSurface(surface, &src, canvas.getSurface(), &dst);
}

bool Image::isTransparent(const Point & coords) const
{
	if (coords.x < 0 || coords.y < 0 || coords.x >= surface->w || coords.y >= surface->h)
		return true;

	SDL_LockSurface(surface);

	uint32_t * pixels = (uint32_t*)surface->pixels;
	uint32_t pixel = pixels[coords.y * surface->w + coords.x];

	SDL_UnlockSurface(surface);

	uint8_t r, g, b, a;
	SDL_GetRGBA(pixel, surface->format, &r, &g, &b, &a);

	return a == 0;
}

std::unique_ptr<Image> Image::clone() const
{
	SDL_Surface * newSurf = SDL_ConvertSurface(surface, surface->format, 0);
	if (!newSurf)
	{
		throw std::runtime_error("Failed to clone surface");
	}

	return std::make_unique<Image>(newSurf, true);
}

std::unique_ptr<Image> Image::horizontalFlip() const
{
	SDL_Surface * flipped = SDL_CreateRGBSurface(0, surface->w, surface->h,
		surface->format->BitsPerPixel,
		surface->format->Rmask, surface->format->Gmask,
		surface->format->Bmask, surface->format->Amask);

	if (!flipped)
	{
		throw std::runtime_error("Failed to create flipped surface");
	}

	SDL_LockSurface(surface);
	SDL_LockSurface(flipped);

	int bpp = surface->format->BytesPerPixel;
	for (int y = 0; y < surface->h; y++)
	{
		for (int x = 0; x < surface->w; x++)
		{
			uint8_t * srcPixel = (uint8_t*)surface->pixels + y * surface->pitch + x * bpp;
			uint8_t * dstPixel = (uint8_t*)flipped->pixels + y * flipped->pitch + (surface->w - 1 - x) * bpp;
			std::memcpy(dstPixel, srcPixel, bpp);
		}
	}

	SDL_UnlockSurface(flipped);
	SDL_UnlockSurface(surface);

	return std::make_unique<Image>(flipped, true);
}

std::unique_ptr<Image> Image::verticalFlip() const
{
	SDL_Surface * flipped = SDL_CreateRGBSurface(0, surface->w, surface->h,
		surface->format->BitsPerPixel,
		surface->format->Rmask, surface->format->Gmask,
		surface->format->Bmask, surface->format->Amask);

	if (!flipped)
	{
		throw std::runtime_error("Failed to create flipped surface");
	}

	SDL_LockSurface(surface);
	SDL_LockSurface(flipped);

	for (int y = 0; y < surface->h; y++)
	{
		uint8_t * srcRow = (uint8_t*)surface->pixels + y * surface->pitch;
		uint8_t * dstRow = (uint8_t*)flipped->pixels + (surface->h - 1 - y) * flipped->pitch;
		std::memcpy(dstRow, srcRow, surface->pitch);
	}

	SDL_UnlockSurface(flipped);
	SDL_UnlockSurface(surface);

	return std::make_unique<Image>(flipped, true);
}
