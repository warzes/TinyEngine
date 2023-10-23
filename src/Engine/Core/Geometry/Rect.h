#pragma once

// Two-dimensional bounding rectangle.
class Rect
{
public:
	glm::vec2 min;
	glm::vec2 max;
};

// Two-dimensional bounding rectangle with integer values.
class IntRect
{
public:
	bool operator==(const IntRect& rhs) const { return left == rhs.left && top == rhs.top && right == rhs.right && bottom == rhs.bottom; }
	bool operator!=(const IntRect& rhs) const { return !(*this == rhs); }

	// Left coordinate.
	int left;
	// Top coordinate.
	int top;
	// Right coordinate.
	int right;
	// Bottom coordinate.
	int bottom;
};

namespace StringUtils
{
	bool FromString(Rect& out, const char* string);
	bool FromString(IntRect& out, const char* string);
	std::string ToString(const Rect& in);
	std::string ToString(const IntRect& in);
}