#pragma once

struct Line
{
	glm::vec3 a, b;
};

struct Plane
{
	glm::vec3 p;
	glm::vec3 n;
};

struct plane_t
{
	union {
		glm::vec3 p;
		struct { float a, b, c; };
	};
	float d;
};

class Capsule
{
public:
	glm::vec3 a;
	glm::vec3 b;
	float r;
};

struct capsule_t
{
	glm::vec3 base;
	float r, height;
};

class Ray
{
public:
	glm::vec3 p;
	glm::vec3 d;
};

struct ray_t
{
	glm::vec3 p;
	glm::vec3 d;
	float len;
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

struct Cylinder
{
	float r;
	glm::vec3 base;
	float height;
};

struct Cone
{
	float r;
	glm::vec3 base;
	float height;
};