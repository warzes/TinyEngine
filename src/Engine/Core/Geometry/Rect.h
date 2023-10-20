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
	/// Left coordinate.
	int left;
	/// Top coordinate.
	int top;
	/// Right coordinate.
	int right;
	/// Bottom coordinate.
	int bottom;
};