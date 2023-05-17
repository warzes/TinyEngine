#pragma once

#include "GeometryShapes.h"
#include "AABB.h"
#include "Transform.h"

struct Hit
{
	int32_t hit = 0;
	union {
		// General case
		float depth;
		// Rays only: Penetration (t0) and Extraction (t1) points along ray line
		struct { float t0, t1; };
		// GJK only
		struct { int32_t hits, iterations; glm::vec3 p0, p1; float distance2; };
	};
	union { glm::vec3 p; glm::vec3 contact_point; };
	union { glm::vec3 n; glm::vec3 normal; };
};

// Contact info
struct ContactInfo
{
	int32_t hit;
	glm::vec3 normal;
	float depth;
	glm::vec3 point;
};

//-----------------------------------------------------------------------------
// Line/Segment
//-----------------------------------------------------------------------------

glm::vec3 LineClosestPoint(const Line& line, const glm::vec3& point);
float LineDistance2Point(const Line& line, const glm::vec3& point);

//-----------------------------------------------------------------------------
// Ray
//-----------------------------------------------------------------------------

float RayTestPlane(const Ray& ray, const glm::vec4& p4);
float RayTestTriangle(const Ray& ray, const Triangle& t);
bool RayTestSphere(float* t0, float* t1, const Ray& r, const Sphere& s);
bool RayTestAABB(float* t0, float* t1, const Ray& r, const AABB& a);

Hit RayHitPlane(const Ray& r, const Plane& p);
Hit RayHitTriangle(const Ray& r, const Triangle& t);
Hit RayHitSphere(const Ray& r, const Sphere& s);
Hit RayHitAABB(const Ray& r, const AABB& a);

//-----------------------------------------------------------------------------
// Plane
//-----------------------------------------------------------------------------

Plane PlaneFromPtNormal(const glm::vec3& pt, const glm::vec3& n);
Plane PlaneFromPts(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);
glm::vec3 PlaneNormal(const Plane& p);
glm::vec3 PlaneClosestPoint(const Plane& p, const glm::vec3& pt);
float PlaneSignedDistance(const Plane& p, const glm::vec3& pt);
float PlaneUnsignedDistance(const Plane& p, const glm::vec3& pt);
Plane PlaneNormalized(const Plane& p);
int32_t PlaneVsSphere(const Plane& a, const Vqs& formA, const Sphere& b, const Vqs& formB, ContactInfo* res);

//-----------------------------------------------------------------------------
// Sphere
//-----------------------------------------------------------------------------
int32_t SphereVsSphere(const Sphere& a, const Vqs& formA, const Sphere& b, const Vqs& formB, ContactInfo* res);
int32_t SphereVsAABB(const Sphere& a, const Vqs& formA, const AABB& b, const Vqs& formB, ContactInfo* res);
int32_t SphereVsPoly(const Sphere& a, const Vqs& formA, const Poly& b, const Vqs& formB, ContactInfo* res);
int32_t SphereVsCylinder(const Sphere& a, const Vqs& formA, const Cylinder& b, const Vqs& formB, ContactInfo* res);
int32_t SphereVsCone(const Sphere& a, const Vqs& formA, const Cone& b, const Vqs& formB, ContactInfo* res);
int32_t SphereVsCapsule(const Sphere& a, const Vqs& formA, const Capsule& b, const Vqs& formB, ContactInfo* res);
int32_t SphereVsRay(const Sphere& a, const Vqs& formA, const Ray& b, const Vqs& formB, ContactInfo* res);

// Alternative (TODO: mod transform)
bool SphereTestSphere(const Sphere& a, const Sphere& b);
Hit SphereHitSphere(const Sphere& a, const Sphere& b);

//-----------------------------------------------------------------------------
// Box
//-----------------------------------------------------------------------------
int32_t AABBVsAABB(const AABB& a, const Vqs& formA, const AABB& b, const Vqs& formB, ContactInfo* res);
int32_t AABBVsSphere(const AABB& a, const Vqs& formA, const Sphere& b, const Vqs& formB, ContactInfo* res);
int32_t AABBVsPoly(const AABB& a, const Vqs& formA, const Poly& b, const Vqs& formB, ContactInfo* res);
int32_t AABBVsCylinder(const AABB& a, const Vqs& formA, const Cylinder& b, const Vqs& formB, ContactInfo* res);
int32_t AABBVsCone(const AABB& a, const Vqs& formA, const Cone& b, const Vqs& formB, ContactInfo* res);
int32_t AABBVsCapsule(const AABB& a, const Vqs& formA, const Capsule& b, const Vqs& formB, ContactInfo* res);
int32_t AABBVsRay(const AABB& a, const Vqs& formA, const Ray& b, const Vqs& formB, ContactInfo* res);

//-----------------------------------------------------------------------------
// Capsule
//-----------------------------------------------------------------------------
int32_t CapsuleVsAABB(const Capsule& a, const Vqs& formA, const AABB& b, const Vqs& formB, ContactInfo* res);
int32_t CapsuleVsSphere(const Capsule& a, const Vqs& formA, const Sphere& b, const Vqs& formB, ContactInfo* res);
int32_t CapsuleVsPoly(const Capsule& a, const Vqs& formA, const Poly& b, const Vqs& formB, ContactInfo* res);
int32_t CapsuleVsCylinder(const Capsule& a, const Vqs& formA, const Cylinder& b, const Vqs& formB, ContactInfo* res);
int32_t CapsuleVsCone(const Capsule& a, const Vqs& formA, const Cone& b, const Vqs& formB, ContactInfo* res);
int32_t CapsuleVsCapsule(const Capsule& a, const Vqs& formA, const Capsule& b, const Vqs& formB, ContactInfo* res);
int32_t CapsuleVsRay(const Capsule& a, const Vqs& formA, const Ray& b, const Vqs& formB, ContactInfo* res);

//-----------------------------------------------------------------------------
// Poly
//-----------------------------------------------------------------------------
int32_t PolyVsPoly(const Poly& a, const Vqs& formA, const Poly& b, const Vqs& formB, ContactInfo* res);
int32_t PolyVsSphere(const Poly& a, const Vqs& formA, const Sphere& b, const Vqs& formB, ContactInfo* res);
int32_t PolyVsAABB(const Poly& a, const Vqs& formA, const AABB& b, const Vqs& formB, ContactInfo* res);
int32_t PolyVsCylinder(const Poly& a, const Vqs& formA, const Cylinder& b, const Vqs& formB, ContactInfo* res);
int32_t PolyVsCone(const Poly& a, const Vqs& formA, const Cone& b, const Vqs& formB, ContactInfo* res);
int32_t PolyVsCapsule(const Poly& a, const Vqs& formA, const Capsule& b, const Vqs& formB, ContactInfo* res);
int32_t PolyVsRay(const Poly& a, const Vqs& formA, const Ray& b, const Vqs& formB, ContactInfo* res);

//-----------------------------------------------------------------------------
// Cylinder
//-----------------------------------------------------------------------------
int32_t CylinderVsCylinder(const Cylinder& a, const Vqs& formA, const Cylinder& b, const Vqs& formB, ContactInfo* res);
int32_t CylinderVsSphere(const Cylinder& a, const Vqs& formA, const Sphere& b, const Vqs& formB, ContactInfo* res);
int32_t CylinderVsAABB(const Cylinder& a, const Vqs& formA, const AABB& b, const Vqs& formB, ContactInfo* res);
int32_t CylinderVsPoly(const Cylinder& a, const Vqs& formA, const Poly& b, const Vqs& formB, ContactInfo* res);
int32_t CylinderVsCone(const Cylinder& a, const Vqs& formA, const Cone& b, const Vqs& formB, ContactInfo* res);
int32_t CylinderVsCapsule(const Cylinder& a, const Vqs& formA, const Capsule& b, const Vqs& formB, ContactInfo* res);
int32_t CylinderVsRay(const Cylinder& a, const Vqs& formA, const Ray& b, const Vqs& formB, ContactInfo* res);

//-----------------------------------------------------------------------------
// Cone
//-----------------------------------------------------------------------------
int32_t ConeVsCone(const Cone& a, const Vqs& formA, const Cone& b, const Vqs& formB, ContactInfo* res);
int32_t ConeVsSphere(const Cone& a, const Vqs& formA, const Sphere& b, const Vqs& formB, ContactInfo* res);
int32_t ConeVsAABB(const Cone& a, const Vqs& formA, const AABB& b, const Vqs& formB, ContactInfo* res);
int32_t ConeVsPoly(const Cone& a, const Vqs& formA, const Poly& b, const Vqs& formB, ContactInfo* res);
int32_t ConeVsCylinder(const Cone& a, const Vqs& formA, const Cylinder& b, const Vqs& formB, ContactInfo* res);
int32_t ConeVsCapsule(const Cone& a, const Vqs& formA, const Capsule& b, const Vqs& formB, ContactInfo* res);
int32_t ConeVsRay(const Cone& a, const Vqs& formA, const Ray& b, const Vqs& formB, ContactInfo* res);

//-----------------------------------------------------------------------------
// Support Functions
//-----------------------------------------------------------------------------

typedef void (*SupportFunc)(const void* collider, const Vqs& xform, const glm::vec3& dir, glm::vec3* out);

void SupportPoly(const void* p, const Vqs& xform, const glm::vec3& dir, glm::vec3* out);
void SupportSphere(const void* s, const Vqs& xform, const glm::vec3& dir, glm::vec3* out);
void SupportAABB(const void* a, const Vqs& xform, const glm::vec3& dir, glm::vec3* out);
void SupportCylinder(const void* c, const Vqs& xform, const glm::vec3& dir, glm::vec3* out);
void SupportCone(const void* c, const Vqs& xform, const glm::vec3& dir, glm::vec3* out);
void SupportCapsule(const void* c, const Vqs& xform, const glm::vec3& dir, glm::vec3* out);
void SupportRay(const void* r, const Vqs& xform, const glm::vec3& dir, glm::vec3* out);

//-----------------------------------------------------------------------------
// GJK
//-----------------------------------------------------------------------------

#define GJK_FLT_MAX FLT_MAX     // 3.40282347E+38F
#define GJK_EPSILON FLT_EPSILON // 1.19209290E-07F
#define GJK_MAX_ITERATIONS 64

#define EPA_TOLERANCE 0.0001
#define EPA_MAX_NUM_FACES 64
#define EPA_MAX_NUM_LOOSE_EDGES 32
#define EPA_MAX_NUM_ITERATIONS 64

enum GJKDimension
{
	GJK_DIMENSION_2D,
	GJK_DIMENSION_3D
};

struct GJKSupportPoint 
{
	glm::vec3 support_a;
	glm::vec3 support_b;
	glm::vec3 minkowski_hull_vert;
};

struct GJKSimplex
{
	union
	{
		GJKSupportPoint points[4];
		struct {
			GJKSupportPoint a;
			GJKSupportPoint b;
			GJKSupportPoint c;
			GJKSupportPoint d;
		};
	};
	uint32_t ct;
};

struct GJKPolytopeFace
{
	GJKSupportPoint points[3];
	glm::vec3 normal;
};

struct GJKEpaEdge
{
	glm::vec3 normal;
	uint32_t index;
	float distance;
	GJKSupportPoint a, b;
};

int32_t CCDGJKInternal(const void* c0, const Vqs& xform_a, SupportFunc f0, const void* c1, const Vqs& xform_b, SupportFunc f1, ContactInfo* res);

#include "Collisions.inl"