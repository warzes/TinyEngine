#pragma once

// Three-dimensional bounding box with integer values.
class IntBox
{
public:
	/// Left coordinate.
	int left;
	/// Top coordinate.
	int top;
	/// Near coordinate.
	int near;
	/// Right coordinate.
	int right;
	/// Bottom coordinate.
	int bottom;
	/// Far coordinate.
	int far;
};