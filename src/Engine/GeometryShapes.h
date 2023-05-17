#pragma once

struct Line
{
	glm::vec3 a, b;
};

struct Sphere
{
	glm::vec3 c;
	float r;
};

struct Plane
{
	glm::vec3 p;
	glm::vec3 n;
};

struct Capsule
{
	glm::vec3 a;
	glm::vec3 b;
	float r;
};

struct Ray
{
	glm::vec3 p;
	glm::vec3 d;
};

struct Triangle
{
	glm::vec3 p0;
	glm::vec3 p1;
	glm::vec3 p2;
};

struct Poly
{
	std::vector<glm::vec3> verts;
};

struct Frustum
{
	struct { glm::vec4 l, r, t, b, n, f; };
	glm::vec4 pl[6];
	float v[24];
};