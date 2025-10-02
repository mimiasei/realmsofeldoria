/*
 * Rect.cpp, part of VCMI engine
 * Adapted for Realms of Eldoria
 *
 * Original Authors: listed in file AUTHORS in VCMI repository
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */
#include "Rect.h"
#include <algorithm>
#include <cmath>

Rect Rect::createCentered(const Point & around, const Point & size)
{
	return Rect(around.x - size.x / 2, around.y - size.y / 2, size.x, size.y);
}

Rect Rect::createCentered(const Rect & target, const Point & size)
{
	return createCentered(target.center(), size);
}

Rect Rect::createAround(const Rect & r, int borderWidth)
{
	return Rect(r.x - borderWidth, r.y - borderWidth, r.w + 2 * borderWidth, r.h + 2 * borderWidth);
}

int Rect::distanceTo(const Point & target) const
{
	if (isInside(target))
		return 0;

	int dx = std::max({x - target.x, 0, target.x - (x + w)});
	int dy = std::max({y - target.y, 0, target.y - (y + h)});

	return std::sqrt(dx * dx + dy * dy);
}

bool Rect::intersectionTest(const Rect & other) const
{
	return x < other.x + other.w &&
	       x + w > other.x &&
	       y < other.y + other.h &&
	       y + h > other.y;
}

bool Rect::intersectionTest(const Point & line1, const Point & line2) const
{
	// Simple bounding box test for line intersection
	int minX = std::min(line1.x, line2.x);
	int maxX = std::max(line1.x, line2.x);
	int minY = std::min(line1.y, line2.y);
	int maxY = std::max(line1.y, line2.y);

	// Check if line bounding box intersects with rect
	return x < maxX && x + w > minX && y < maxY && y + h > minY;
}

Rect Rect::intersect(const Rect & other) const
{
	int x1 = std::max(x, other.x);
	int y1 = std::max(y, other.y);
	int x2 = std::min(x + w, other.x + other.w);
	int y2 = std::min(y + h, other.y + other.h);

	if (x2 > x1 && y2 > y1)
		return Rect(x1, y1, x2 - x1, y2 - y1);
	else
		return Rect(); // empty rect
}

Rect Rect::include(const Rect & other) const
{
	int x1 = std::min(x, other.x);
	int y1 = std::min(y, other.y);
	int x2 = std::max(x + w, other.x + other.w);
	int y2 = std::max(y + h, other.y + other.h);

	return Rect(x1, y1, x2 - x1, y2 - y1);
}
