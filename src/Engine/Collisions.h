#pragma once

#include "GeometryShapes.h"
#include "Sphere.h"
#include "AABB.h"
#include "Transform.h"
#include "Temp.h"
#include "GJK.h"

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

constexpr Hit NotHit{.hit = 0};

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

float LineDistance2Point(const Line& line, const glm::vec3& point);
glm::vec3 LineClosestPoint(const Line& line, const glm::vec3& point);

//-----------------------------------------------------------------------------
// Ray
//-----------------------------------------------------------------------------

float RayTestPlane(const OldRay& ray, const glm::vec4& p4);
float RayTestTriangle(const OldRay& ray, const Triangle& t);
bool RayTestSphere(float* t0, float* t1, const OldRay& r, const Sphere& s);
bool RayTestAABB(float* t0, float* t1, const OldRay& r, const AABB& a);

Hit RayHitPlane(const OldRay& r, const OldPlane& p);
Hit RayHitTriangle(const OldRay& r, const Triangle& t);
Hit RayHitSphere(const OldRay& r, const Sphere& s);
Hit RayHitAABB(const OldRay& r, const AABB& a);

//-----------------------------------------------------------------------------
// Plane
//-----------------------------------------------------------------------------

// libccd
plane_t PlaneFromPtNormal(const glm::vec3& pt, const glm::vec3& n);
plane_t PlaneFromPts(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);
glm::vec3 PlaneNormal(const plane_t& p);
glm::vec3 PlaneClosestPoint(const plane_t& p, const glm::vec3& pt);
float PlaneSignedDistance(const plane_t& p, const glm::vec3& pt);
float PlaneUnsignedDistance(const plane_t& p, const glm::vec3& pt);
plane_t PlaneNormalized(const plane_t& p);
int32_t PlaneVsSphere(const plane_t& a, const Vqs& formA, const Sphere& b, const Vqs& formB, ContactInfo* res);

//-----------------------------------------------------------------------------
// Sphere
//-----------------------------------------------------------------------------
// Alternative 
// (TODO: mod transform)
glm::vec3 SphereClosestPoint(const Sphere& s, const glm::vec3& p);
bool SphereTestAABB(const Sphere& s, const AABB& a);
bool SphereTestCapsule(const Sphere& s, const Capsule& c);
bool SphereTestPoly(const Sphere& s, const Poly& p);
bool SphereTestSphere(const Sphere& a, const Sphere& b);
Hit SphereHitAABB(const Sphere& s, const AABB& a);
Hit SphereHitCapsule(Sphere s, const Capsule& c);
Hit SphereHitSphere(const Sphere& a, const Sphere& b);

// libccd
int32_t SphereVsSphere(const Sphere& a, const Vqs& formA, const Sphere& b, const Vqs& formB, ContactInfo* res);
int32_t SphereVsAABB(const Sphere& a, const Vqs& formA, const AABB& b, const Vqs& formB, ContactInfo* res);
int32_t SphereVsPoly(const Sphere& a, const Vqs& formA, const Poly& b, const Vqs& formB, ContactInfo* res);
int32_t SphereVsCylinder(const Sphere& a, const Vqs& formA, const Cylinder& b, const Vqs& formB, ContactInfo* res);
int32_t SphereVsCone(const Sphere& a, const Vqs& formA, const Cone& b, const Vqs& formB, ContactInfo* res);
int32_t SphereVsCapsule(const Sphere& a, const Vqs& formA, const capsule_t& b, const Vqs& formB, ContactInfo* res);
int32_t SphereVsRay(const Sphere& a, const Vqs& formA, const ray_t& b, const Vqs& formB, ContactInfo* res);

//-----------------------------------------------------------------------------
// AABB
//-----------------------------------------------------------------------------
// Alternative 
// (TODO: mod transform)
float AABBDistance2Point(const AABB& a, const glm::vec3& p);
glm::vec3 AABBClosestPoint(const AABB& a, const glm::vec3& p);
bool AABBContainsPoint(const AABB& a, const glm::vec3& p);
bool AABBTestAABB(const AABB& a, const AABB& b);
bool AABBTestCapsule(const AABB& a, const Capsule& c);
bool AABBTestPoly(const AABB& a, const Poly& p);
bool AABBTestSphere(const AABB& a, const Sphere& s);
Hit AABBHitAABB(const AABB& a, const AABB& b);
Hit AABBHitCapsule(const AABB& a, const Capsule& c);
Hit AABBHitSphere(const AABB& a, const Sphere& s);

//-----------------------------------------------------------------------------
// Box
//-----------------------------------------------------------------------------
// libccd
int32_t AABBVsAABB(const AABB& a, const Vqs& formA, const AABB& b, const Vqs& formB, ContactInfo* res);
int32_t AABBVsSphere(const AABB& a, const Vqs& formA, const Sphere& b, const Vqs& formB, ContactInfo* res);
int32_t AABBVsPoly(const AABB& a, const Vqs& formA, const Poly& b, const Vqs& formB, ContactInfo* res);
int32_t AABBVsCylinder(const AABB& a, const Vqs& formA, const Cylinder& b, const Vqs& formB, ContactInfo* res);
int32_t AABBVsCone(const AABB& a, const Vqs& formA, const Cone& b, const Vqs& formB, ContactInfo* res);
int32_t AABBVsCapsule(const AABB& a, const Vqs& formA, const capsule_t& b, const Vqs& formB, ContactInfo* res);
int32_t AABBVsRay(const AABB& a, const Vqs& formA, const ray_t& b, const Vqs& formB, ContactInfo* res);

//-----------------------------------------------------------------------------
// Capsule
//-----------------------------------------------------------------------------

// Alternative 
// (TODO: mod transform)
float CapsuleDistance2Point(const Capsule& c, const glm::vec3& p);
glm::vec3 CapsuleClosestPoint(const Capsule& c, const glm::vec3& p);
bool CapsuleTestAABB(const Capsule& c, const AABB& a);
bool CapsuleTestCapsule(const Capsule& a, const Capsule& b);
bool CapsuleTestPoly(const Capsule& c, const Poly& p);
bool CapsuleTestSphere(const Capsule& c, const Sphere& s);
Hit CapsuleHitAABB(const Capsule& c, const AABB& a);
Hit CapsuleHitCapsule(const Capsule& a, const Capsule& b);
Hit CapsuleHitSphere(const Capsule& c, const Sphere& s);

// libccd
int32_t CapsuleVsAABB(const capsule_t& a, const Vqs& formA, const AABB& b, const Vqs& formB, ContactInfo* res);
int32_t CapsuleVsSphere(const capsule_t& a, const Vqs& formA, const Sphere& b, const Vqs& formB, ContactInfo* res);
int32_t CapsuleVsPoly(const capsule_t& a, const Vqs& formA, const Poly& b, const Vqs& formB, ContactInfo* res);
int32_t CapsuleVsCylinder(const capsule_t& a, const Vqs& formA, const Cylinder& b, const Vqs& formB, ContactInfo* res);
int32_t CapsuleVsCone(const capsule_t& a, const Vqs& formA, const Cone& b, const Vqs& formB, ContactInfo* res);
int32_t CapsuleVsCapsule(const capsule_t& a, const Vqs& formA, const capsule_t& b, const Vqs& formB, ContactInfo* res);
int32_t CapsuleVsRay(const capsule_t& a, const Vqs& formA, const ray_t& b, const Vqs& formB, ContactInfo* res);

//-----------------------------------------------------------------------------
// Poly
//-----------------------------------------------------------------------------

// Alternative 
/* poly: query */
bool PolyTestSphere(const Poly& p, const Sphere& s);
bool PolyTestAABB(const Poly& p, const AABB& a);
bool PolyTestCapsule(const Poly& p, const Capsule& c);
bool PolyTestPoly(const Poly& a, const Poly& b);

/* poly: query transformed */
bool PolyTestSphereTransform(const Poly& p, const glm::vec3& pos3, mat33 rot33, const Sphere& s);
bool PolyTestAABBTransform(const Poly& p, const glm::vec3& apos3, mat33 arot33, const AABB& a);
bool PolyTestCapsuleTransform(const Poly& p, const glm::vec3& pos3, mat33 rot33, const Capsule& c);
bool PolyTestPolyTransform(const Poly& a, const glm::vec3& apos3, mat33 arot33, const Poly& b, const glm::vec3& bpos3, mat33 brot33);

/* poly: gjk result */
bool PolyHitSphere(gjk_result* res, const Poly& p, const Sphere& s);
bool PolyHitAABB(gjk_result* res, const Poly& p, const AABB& a);
bool PolyHitCapsule(gjk_result* res, const Poly& p, const Capsule& c);
bool PolyHitPoly(gjk_result* res, const Poly& a, const Poly& b);

/* poly: gjk result transformed */
bool PolyHitSphereTransform(gjk_result* res, const Poly& p, const glm::vec3& pos3, mat33 rot33, const Sphere& s);
bool PolyHitAABBTransform(gjk_result* res, const Poly& p, const glm::vec3& pos3, mat33 rot33, const AABB& a);
bool PolyHitCapsuleTransform(gjk_result* res, const Poly& p, const glm::vec3& pos3, mat33 rot33, const Capsule& c);
bool PolyHitPolyTransform(gjk_result* res, const Poly& a, const glm::vec3& at3, mat33 ar33, const Poly& b, const glm::vec3& bt3, mat33 br33);

// libccd
int32_t PolyVsPoly(const Poly& a, const Vqs& formA, const Poly& b, const Vqs& formB, ContactInfo* res);
int32_t PolyVsSphere(const Poly& a, const Vqs& formA, const Sphere& b, const Vqs& formB, ContactInfo* res);
int32_t PolyVsAABB(const Poly& a, const Vqs& formA, const AABB& b, const Vqs& formB, ContactInfo* res);
int32_t PolyVsCylinder(const Poly& a, const Vqs& formA, const Cylinder& b, const Vqs& formB, ContactInfo* res);
int32_t PolyVsCone(const Poly& a, const Vqs& formA, const Cone& b, const Vqs& formB, ContactInfo* res);
int32_t PolyVsCapsule(const Poly& a, const Vqs& formA, const capsule_t& b, const Vqs& formB, ContactInfo* res);
int32_t PolyVsRay(const Poly& a, const Vqs& formA, const ray_t& b, const Vqs& formB, ContactInfo* res);

//-----------------------------------------------------------------------------
// Cylinder
//-----------------------------------------------------------------------------
int32_t CylinderVsCylinder(const Cylinder& a, const Vqs& formA, const Cylinder& b, const Vqs& formB, ContactInfo* res);
int32_t CylinderVsSphere(const Cylinder& a, const Vqs& formA, const Sphere& b, const Vqs& formB, ContactInfo* res);
int32_t CylinderVsAABB(const Cylinder& a, const Vqs& formA, const AABB& b, const Vqs& formB, ContactInfo* res);
int32_t CylinderVsPoly(const Cylinder& a, const Vqs& formA, const Poly& b, const Vqs& formB, ContactInfo* res);
int32_t CylinderVsCone(const Cylinder& a, const Vqs& formA, const Cone& b, const Vqs& formB, ContactInfo* res);
int32_t CylinderVsCapsule(const Cylinder& a, const Vqs& formA, const capsule_t& b, const Vqs& formB, ContactInfo* res);
int32_t CylinderVsRay(const Cylinder& a, const Vqs& formA, const ray_t& b, const Vqs& formB, ContactInfo* res);

//-----------------------------------------------------------------------------
// Cone
//-----------------------------------------------------------------------------
int32_t ConeVsCone(const Cone& a, const Vqs& formA, const Cone& b, const Vqs& formB, ContactInfo* res);
int32_t ConeVsSphere(const Cone& a, const Vqs& formA, const Sphere& b, const Vqs& formB, ContactInfo* res);
int32_t ConeVsAABB(const Cone& a, const Vqs& formA, const AABB& b, const Vqs& formB, ContactInfo* res);
int32_t ConeVsPoly(const Cone& a, const Vqs& formA, const Poly& b, const Vqs& formB, ContactInfo* res);
int32_t ConeVsCylinder(const Cone& a, const Vqs& formA, const Cylinder& b, const Vqs& formB, ContactInfo* res);
int32_t ConeVsCapsule(const Cone& a, const Vqs& formA, const capsule_t& b, const Vqs& formB, ContactInfo* res);
int32_t ConeVsRay(const Cone& a, const Vqs& formA, const ray_t& b, const Vqs& formB, ContactInfo* res);


//-----------------------------------------------------------------------------
// Frustum
//-----------------------------------------------------------------------------
Frustum FrustumBuild(mat44 projview);
bool FrustumTestSphere(const Frustum& f, const Sphere& s);
bool FrustumTestAABB(const Frustum& f, const AABB& a);

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