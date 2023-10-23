#pragma once

// Three-dimensional bounding box with integer values.
class IntBox
{
public:
	bool operator==(const IntBox& rhs) const { return left == rhs.left && top == rhs.top && right == rhs.right && bottom == rhs.bottom && near == rhs.near && far == rhs.far; }
	bool operator!=(const IntBox& rhs) const { return !(*this == rhs); }

	// Left coordinate.
	int left;
	// Top coordinate.
	int top;
	// Near coordinate.
	int near;
	// Right coordinate.
	int right;
	// Bottom coordinate.
	int bottom;
	// Far coordinate.
	int far;
};

namespace StringUtils
{
	bool FromString(IntBox& out, const char* string);
	std::string ToString(const IntBox& in);
}