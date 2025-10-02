/*
 * Point.h, part of VCMI engine
 * Adapted for Realms of Eldoria
 *
 * Original Authors: listed in file AUTHORS in VCMI repository
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */
#pragma once

#include <limits>
#include <cmath>

// A point with x/y coordinate, used mostly for graphic rendering
class Point
{
public:
	int x, y;

	//constructors
	constexpr Point() : x(0), y(0)
	{
	}

	constexpr Point(int X, int Y)
		: x(X)
		, y(Y)
	{
	}

	constexpr static Point makeInvalid()
	{
		return Point(std::numeric_limits<int>::min(), std::numeric_limits<int>::min());
	}

	template<typename T>
	constexpr Point operator+(const T &b) const
	{
		return Point(x+b.x,y+b.y);
	}

	template<typename T>
	constexpr Point operator/(const T &div) const
	{
		return Point(x/div, y/div);
	}

	template<typename T>
	constexpr Point operator*(const T &mul) const
	{
		return Point(x*mul, y*mul);
	}

	constexpr Point operator/(const Point &b) const
	{
		return Point(x/b.x,y/b.y);
	}

	constexpr Point operator*(const Point &b) const
	{
		return Point(x*b.x,y*b.y);
	}

	template<typename T>
	constexpr Point& operator+=(const T &b)
	{
		x += b.x;
		y += b.y;
		return *this;
	}

	constexpr Point operator-() const
	{
		return Point(-x, -y);
	}

	template<typename T>
	constexpr Point operator-(const T &b) const
	{
		return Point(x - b.x, y - b.y);
	}

	template<typename T>
	constexpr Point& operator-=(const T &b)
	{
		x -= b.x;
		y -= b.y;
		return *this;
	}

	template<typename T> constexpr Point& operator=(const T &t)
	{
		x = t.x;
		y = t.y;
		return *this;
	}
	template<typename T> constexpr bool operator==(const T &t) const
	{
		return x == t.x  &&  y == t.y;
	}
	template<typename T> constexpr bool operator!=(const T &t) const
	{
		return !(*this == t);
	}

	constexpr bool isValid() const
	{
		return x > std::numeric_limits<int>::min() && y > std::numeric_limits<int>::min();
	}

	constexpr int lengthSquared() const
	{
		return x * x + y * y;
	}

	int length() const
	{
		return std::sqrt(lengthSquared());
	}

	double angle() const
	{
		return std::atan2(y, x); // rad
	}
};
