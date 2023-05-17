#define _COLLIDE_FUNC_IMPL(_TA, _TB, _F0, _F1)\
	inline int32_t _TA##Vs##_TB(const _TA& a, const Vqs& xa, const _TB& b, const Vqs& xb, ContactInfo* r)\
	{\
		return CCDGJKInternal(&a, xa, (_F0), &b, xb, (_F1), r);\
	}

//-----------------------------------------------------------------------------
// Line/Segment
//-----------------------------------------------------------------------------

inline Line LineClosestLine(const Line& l, const glm::vec3& p)
{
	glm::vec3 cp = LineClosestPoint(l, p);
	return Line(p, cp);
}

inline glm::vec3 LineClosestPoint(const Line& l, const glm::vec3& p)
{
	const glm::vec3 ab = l.b - l.a;
	const glm::vec3 pa = p - l.a;
	float t = glm::dot(pa, ab) / glm::dot(ab, ab);
	t = glm::clamp(t, 0.0f, 1.0f);
	glm::vec3 pt = l.a + (ab * t);
	return pt;
}

inline glm::vec3 LineDirection(const Line& l)
{
	return glm::normalize(l.b - l.a);
}

//-----------------------------------------------------------------------------
// Ray
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Plane
//-----------------------------------------------------------------------------

// Modified from: https://graphics.stanford.edu/~mdfisher/Code/Engine/Plane.cpp.html
inline Plane PlaneFromPtNormal(const glm::vec3& pt, const glm::vec3& n)
{

	glm::vec3 nn = glm::normalize(n);
	Plane p = { };
	p.a = nn.x; p.b = nn.y; p.c = nn.z;
	p.d = -glm::dot(pt, nn);
	return p;
}

inline Plane PlaneFromPts(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c)
{
	glm::vec3 n = glm::normalize(glm::cross((b - a), (c - a)));
	return PlaneFromPtNormal(a, n);
}

inline glm::vec3 PlaneNormal(const Plane& p)
{
	return glm::normalize(glm::vec3{ p.a, p.b, p.c });
}

inline glm::vec3 PlaneClosestPoint(const Plane& p, const glm::vec3& pt)
{
	return (pt - (PlaneNormal(p) * PlaneSignedDistance(p, pt)));
}

inline float PlaneSignedDistance(const Plane& p, const glm::vec3& pt)
{
	return (p.a * pt.x + p.b * pt.y + p.c * pt.z + p.d);
}

inline float PlaneUnsignedDistance(const Plane& p, const glm::vec3& pt)
{
	return fabsf(PlaneSignedDistance(p, pt));
}

inline Plane PlaneNormalized(const Plane& p)
{
	float d = sqrtf(p.a * p.a + p.b * p.b + p.c * p.c);
	Plane pn = {};
	pn.a = p.a / d; pn.b = p.b / d; pn.c = p.c / d; pn.d = p.d / d;
	return pn;
}

//-----------------------------------------------------------------------------
// Sphere
//-----------------------------------------------------------------------------

constexpr bool t1 = (bool)0;
constexpr bool t2 = (bool)1;

inline bool SphereTestSphere(const Sphere& a, const Sphere& b)
{
	const glm::vec3 d = (b.c - a.c);
	const float r = a.r + b.r;
	if (glm::dot(d, d) > r * r)
		return false;
	return true;
}

inline Hit SphereHitSphere(const Sphere& a, const Sphere& b)
{
	glm::vec3 d = b.c - a.c;
	float r = a.r + b.r;
	float d2 = glm::dot(d, d);
	if (d2 > r * r) return Hit();

	float l = sqrtf(d2);
	float linv = 1.0f / ((l != 0.0f) ? l : 1.0f);

	Hit m;
	m.hit = 1;
	m.normal = d * linv;
	m.depth = r - l;
	d = m.normal * b.r;
	m.contact_point = b.c - d;
	return m;
}

//-----------------------------------------------------------------------------
// Support Functions
//-----------------------------------------------------------------------------

// Inverse := Conjugate / Dot;
inline glm::quat _quat_inverse(const glm::quat& q) // TODO: delete
{
	return (glm::conjugate(q) * (1.0f / glm::dot(q, q)));
}

inline glm::vec3 _quat_rotate(const glm::quat& q, const glm::vec3& v) // TODO: delete
{
	// nVidia SDK implementation
	glm::vec3 qvec = glm::vec3(q.x, q.y, q.z);
	glm::vec3 uv = glm::cross(qvec, v);
	glm::vec3 uuv = glm::cross(qvec, uv);
	uv = uv * (2.f * q.w);
	uuv = uuv * 2.f;
	return (v + (uv + uuv));
}

inline float _vec3_signY(const glm::vec3& v)
{
	return (v.y < 0.f ? -1.f : v.y > 0.f ? 1.f : 0.f);
}

#define _XAXIS glm::vec3(1.f, 0.f, 0.f)
#define _YAXIS glm::vec3(0.f, 1.f, 0.f)
#define _ZAXIS glm::vec3(0.f, 0.f, 1.f)

inline void SupportPoly(const void* _o, const Vqs& xform, const glm::vec3& dir, glm::vec3* out)
{
	const Poly* p = (Poly*)_o;

	// Bring direction vector into rotation space
	glm::quat qinv = _quat_inverse(xform.rotation);
	glm::vec3 d = _quat_rotate(qinv, dir);

	// Iterate over all points, find dot farthest in direction of d
	double max_dot, dot = 0.0;
	max_dot = (double)-FLT_MAX;
	for (uint32_t i = 0; i < p->verts.size(); i++)
	{
		dot = (double)glm::dot(d, p->verts[i]);
		if (dot > max_dot) 
		{
			*out = p->verts[i];
			max_dot = dot;
		}
	}

	// Transform support point by rotation and translation of object
	*out = _quat_rotate(xform.rotation, *out);
	*out = xform.scale * (*out);
	*out = xform.position + (*out);
}

inline void SupportSphere(const void* _o, const Vqs& xform, const glm::vec3& dir, glm::vec3* out)
{
	// Support function is made according to Gino van den Bergen's paper
	//  A Fast and Robust CCD Implementation for Collision Detection of Convex Objects
	const Sphere* s = (Sphere*)_o;
	float scl = glm::max(xform.scale.x, glm::max(xform.scale.z, xform.scale.y));
	*out = (glm::normalize(dir) * scl * s->r) + (xform.position + s->c);
}

inline void SupportAABB(const void* _o, const Vqs& xform, const glm::vec3& dir, glm::vec3* out)
{
	const AABB* a = (AABB*)_o;

	// Bring direction vector into rotation space
	glm::quat qinv = _quat_inverse(xform.rotation);
	glm::vec3 d = _quat_rotate(qinv, dir);

	// Compute half coordinates and sign for aabb (scale by transform)
	const float hx = (a->max.x - a->min.x) * 0.5f * xform.scale.x;
	const float hy = (a->max.y - a->min.y) * 0.5f * xform.scale.y;
	const float hz = (a->max.z - a->min.z) * 0.5f * xform.scale.z;
	glm::vec3 s = glm::sign(d); // TODO: ???

	// Compure support for aabb
	*out = glm::vec3(s.x * hx, s.y * hy, s.z * hz);

	// Transform support point by rotation and translation of object
	*out = _quat_rotate(xform.rotation, *out);
	*out = (xform.position + *out);
}

inline void SupportCylinder(const void* _o, const Vqs& xform, const glm::vec3& dir, glm::vec3* out)
{
	// Support function is made according to Gino van den Bergen's paper
	//  A Fast and Robust CCD Implementation for Collision Detection of Convex Objects

	const Cylinder* c = (const Cylinder*)_o;

	// Bring direction vector into rotation space
	glm::quat qinv = _quat_inverse(xform.rotation);
	glm::vec3 d = _quat_rotate(qinv, dir);

	// Compute support point (cylinder is standing on y axis, half height at origin)
	double zdist = sqrt(d.x * d.x + d.z * d.z);
	double hh = (double)c->height * 0.5 * xform.scale.y;
	if (zdist == 0.0)
	{
		*out = glm::vec3(0.0, _vec3_signY(d) * hh, 0.0);
	}
	else
	{
		double r = (double)c->r / zdist;
		*out = glm::vec3(r * d.x * xform.scale.x, _vec3_signY(d) * hh, r * d.z * xform.scale.z);
	}

	// Transform support point into world space
	*out = _quat_rotate(xform.rotation, *out);
	*out = ((xform.position + c->base) + *out);
}

inline void SupportCone(const void* _o, const Vqs& xform, const glm::vec3& dir, glm::vec3* out)
{
	const Cone* c = (const Cone*)_o;

	// Bring direction vector into rotation space
	glm::quat qinv = _quat_inverse(xform.rotation);
	glm::vec3 d = _quat_rotate(qinv, dir);

	// Compute support point (cone is standing on y axis, half height at origin)
	double sin_angle = c->r / sqrt((double)c->r * (double)c->r + (double)c->height * (double)c->height);
	double hh = (double)c->height * 0.5 * xform.scale.y;
	double len = glm::length(d);

	if (d.y > len * sin_angle)
	{
		*out = glm::vec3(0.0f, (float)hh, 0.0f);
	}
	else
	{
		double s = sqrt(d.x * d.x + d.z * d.z);
		if (s > (double)GJK_EPSILON)
		{
			double _d = (double)c->r / s;
			*out = glm::vec3(d.x * _d * xform.scale.x, (float)-hh, d.z * _d * xform.scale.z);
		}
		else
		{
			*out = glm::vec3(0.0, (float)-hh, 0.0);
		}
	}

	// Transform support point into world space
	*out = _quat_rotate(xform.rotation, *out);
	*out = ((xform.position + c->base) + *out);
}

inline void SupportCapsule(const void* _o, const Vqs& xform, const glm::vec3& dir, glm::vec3* out)
{
	const Capsule* c = (const Capsule*)_o;

	// Bring direction vector into rotation space
	glm::quat qinv = _quat_inverse(xform.rotation);
	glm::vec3 d = _quat_rotate(qinv, dir);

	// Compute support point (cone is standing on y axis, half height at origin)
	const float s = glm::max(xform.scale.x, xform.scale.z);
	*out = glm::normalize(d) * (c->r * s);
	double hh = (double)c->height * 0.5 * xform.scale.y;

	if (glm::dot(d, _YAXIS) > 0.0) 
	{
		out->y += hh;
	}
	else
	{
		out->y -= hh;
	}

	// Transform support point into world space
	*out = _quat_rotate(xform.rotation, *out);
	*out = ((xform.position + c->base) + *out);
}

inline void SupportRay(const void* _r, const Vqs& xform, const glm::vec3& dir, glm::vec3* out)
{
	const Ray* r = (Ray*)_r;

	// Bring direction vector into rotation space
	glm::quat qinv = _quat_inverse(xform.rotation);
	glm::vec3 d = _quat_rotate(qinv, dir);

	glm::vec3 rs = r->p;
	glm::vec3 re = (r->p + (r->d * r->len));

	if (glm::dot(rs, d) > glm::dot(re, d))
	{
		*out = rs;
	}
	else {
		*out = re;
	}

	// Transform support point by rotation and translation of object
	*out = _quat_rotate(xform.rotation, *out);
	*out = (xform.scale * *out);
	*out = (xform.position + *out);
}

//-----------------------------------------------------------------------------
// Test impl
//-----------------------------------------------------------------------------

/* Sphere */

_COLLIDE_FUNC_IMPL(Sphere, Sphere, SupportSphere, SupportSphere);      // Sphere vs. Sphere 
_COLLIDE_FUNC_IMPL(Sphere, Cylinder, SupportSphere, SupportCylinder);  // Sphere vs. Cylinder
_COLLIDE_FUNC_IMPL(Sphere, Cone, SupportSphere, SupportCone);          // Sphere vs. Cone
_COLLIDE_FUNC_IMPL(Sphere, AABB, SupportSphere, SupportAABB);          // Sphere vs. AABB
_COLLIDE_FUNC_IMPL(Sphere, Capsule, SupportSphere, SupportCapsule);    // Sphere vs. Capsule
_COLLIDE_FUNC_IMPL(Sphere, Poly, SupportSphere, SupportPoly);          // Sphere vs. Poly 
_COLLIDE_FUNC_IMPL(Sphere, Ray, SupportSphere, SupportRay);            // Sphere vs. Ray
/* AABB */

_COLLIDE_FUNC_IMPL(AABB, AABB, SupportAABB, SupportAABB);          // AABB vs. AABB
_COLLIDE_FUNC_IMPL(AABB, Cylinder, SupportAABB, SupportCylinder);  // AABB vs. Cylinder
_COLLIDE_FUNC_IMPL(AABB, Cone, SupportAABB, SupportCone);          // AABB vs. Cone
_COLLIDE_FUNC_IMPL(AABB, Sphere, SupportAABB, SupportSphere);      // AABB vs. Sphere
_COLLIDE_FUNC_IMPL(AABB, Capsule, SupportAABB, SupportCapsule);    // AABB vs. Capsule
_COLLIDE_FUNC_IMPL(AABB, Poly, SupportAABB, SupportPoly);          // AABB vs. Poly
_COLLIDE_FUNC_IMPL(AABB, Ray, SupportAABB, SupportRay);            // AABB vs. Ray

/* Capsule */

_COLLIDE_FUNC_IMPL(Capsule, Capsule, SupportCapsule, SupportCapsule);    // Capsule vs. Capsule
_COLLIDE_FUNC_IMPL(Capsule, Cylinder, SupportCapsule, SupportCylinder);  // Capsule vs. Cylinder
_COLLIDE_FUNC_IMPL(Capsule, Cone, SupportCapsule, SupportCone);          // Capsule vs. Cone
_COLLIDE_FUNC_IMPL(Capsule, Sphere, SupportCapsule, SupportSphere);      // Capsule vs. Sphere
_COLLIDE_FUNC_IMPL(Capsule, AABB, SupportCapsule, SupportAABB);          // Capsule vs. AABB
_COLLIDE_FUNC_IMPL(Capsule, Poly, SupportCapsule, SupportPoly);          // Capsule vs. Poly
_COLLIDE_FUNC_IMPL(Capsule, Ray, SupportCapsule, SupportRay);            // Capsule vs. Ray

/* Poly */

_COLLIDE_FUNC_IMPL(Poly, Poly, SupportPoly, SupportPoly);          // Poly vs. Poly
_COLLIDE_FUNC_IMPL(Poly, Cylinder, SupportPoly, SupportCylinder);  // Poly vs. Cylinder
_COLLIDE_FUNC_IMPL(Poly, Cone, SupportPoly, SupportCone);          // Poly vs. Cone
_COLLIDE_FUNC_IMPL(Poly, Sphere, SupportPoly, SupportSphere);      // Poly vs. Sphere
_COLLIDE_FUNC_IMPL(Poly, AABB, SupportPoly, SupportAABB);          // Poly vs. AABB
_COLLIDE_FUNC_IMPL(Poly, Capsule, SupportPoly, SupportCapsule);    // Poly vs. Capsule
_COLLIDE_FUNC_IMPL(Poly, Ray, SupportPoly, SupportRay);            // Poly vs. Ray

/* Cylinder */

_COLLIDE_FUNC_IMPL(Cylinder, Cylinder, SupportCylinder, SupportCylinder);  // Cylinder vs. Cylinder
_COLLIDE_FUNC_IMPL(Cylinder, Poly, SupportPoly, SupportPoly);              // Cylinder vs. Poly
_COLLIDE_FUNC_IMPL(Cylinder, Cone, SupportCylinder, SupportCone);          // Cylinder vs. Cone
_COLLIDE_FUNC_IMPL(Cylinder, Sphere, SupportCylinder, SupportSphere);      // Cylinder vs. Sphere
_COLLIDE_FUNC_IMPL(Cylinder, AABB, SupportCylinder, SupportAABB);          // Cylinder vs. AABB
_COLLIDE_FUNC_IMPL(Cylinder, Capsule, SupportCylinder, SupportCapsule);    // Cylinder vs. Capsule
_COLLIDE_FUNC_IMPL(Cylinder, Ray, SupportCylinder, SupportRay);            // Cylinder vs. Ray

/* Cone */

_COLLIDE_FUNC_IMPL(Cone, Cone, SupportCone, SupportCone);          // Cone vs. Cone
_COLLIDE_FUNC_IMPL(Cone, Poly, SupportPoly, SupportPoly);          // Cone vs. Poly
_COLLIDE_FUNC_IMPL(Cone, Cylinder, SupportCone, SupportCylinder);  // Cone vs. Cylinder
_COLLIDE_FUNC_IMPL(Cone, Sphere, SupportCone, SupportSphere);      // Cone vs. Sphere
_COLLIDE_FUNC_IMPL(Cone, AABB, SupportCone, SupportAABB);          // Cone vs. AABB
_COLLIDE_FUNC_IMPL(Cone, Capsule, SupportCone, SupportCapsule);    // Cone vs. Capsule
_COLLIDE_FUNC_IMPL(Cone, Ray, SupportCone, SupportRay);            // Cone vs. Ray