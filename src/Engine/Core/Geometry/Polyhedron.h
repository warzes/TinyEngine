#pragma once

// A convex volume built from polygon faces.
class Polyhedron
{
public:
	std::vector<std::vector<glm::vec3>> faces;
};