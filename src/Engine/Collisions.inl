#define _COLLIDE_FUNC_IMPL(_TA, _TB, _F0, _F1)\
	inline int32_t _TA##Vs##_TB(const _TA& a, const Vqs& xa, const _TB& b, const Vqs& xb, ContactInfo* r)\
	{\
		return CCDGJKInternal(&a, xa, (_F0), &b, xb, (_F1), r);\
	}

inline glm::vec4 Plane4(const glm::vec3& p, const glm::vec3& n)
{
	return glm::vec4(n, -glm::dot(n, p));
}

//-----------------------------------------------------------------------------
// Line/Segment
//-----------------------------------------------------------------------------

inline glm::vec3 LineClosestPoint(const Line& line, const glm::vec3& point)
{
	const glm::vec3 ab = line.b - line.a;
	const glm::vec3 pa = point - line.a;
	float t = glm::dot(pa, ab) / glm::dot(ab, ab);
	t = glm::clamp(t, 0.0f, 1.0f);
	glm::vec3 pt = line.a + (ab * t);
	return pt;
}

inline float LineDistance2Point(const Line& line, const glm::vec3& point)
{
	const glm::vec3 ab = (line.a - line.b);
	const glm::vec3 ap = (line.a - point);
	const glm::vec3 bp = (line.b - point);
	// handle cases p proj outside ab
	const float e = glm::dot(ap, ab); if (e <= 0) return glm::dot(ap, ap);
	const float f = glm::dot(ab, ab); if (e >= f) return glm::dot(bp, bp);
	return glm::dot(ap, ap) - (e * e) / f;
}

//-----------------------------------------------------------------------------
// Ray
//-----------------------------------------------------------------------------

inline float RayTestPlane(const Ray& ray, const glm::vec4& plane)
{
	const glm::vec3 p(plane.x, plane.y, plane.z);
	const float n = -(glm::dot(p, ray.p) + plane.w);
	if (fabs(n) < 0.0001f) return 0.0f;
	return n / (glm::dot(p, ray.d));
}

inline float RayTestTriangle(const Ray& ray, const Triangle& tr)
{
	/* calculate triangle normal */
	const glm::vec3 d10 = tr.p1 - tr.p0;
	const glm::vec3 d20 = tr.p2 - tr.p0;
	const glm::vec3 d21 = tr.p2 - tr.p1;
	const glm::vec3 d02 = tr.p0 - tr.p2;
	const glm::vec3 n = glm::cross(d10, d20);

	/* check for plane intersection */
	glm::vec4 p = Plane4(tr.p0, n);
	const float t = RayTestPlane(ray, p);
	if (t <= 0.0f) return t;

	/* intersection point */
	glm::vec3 in = ray.d * t;
	in = in + ray.p;

	/* check if point inside triangle in plane */
	const glm::vec3 di0 = in - tr.p0;
	const glm::vec3 di1 = in - tr.p1;
	const glm::vec3 di2 = in - tr.p2;

	const glm::vec3 in0 = glm::cross(d10, di0);
	const glm::vec3 in1 = glm::cross(d21, di1);
	const glm::vec3 in2 = glm::cross(d02, di2);

	if (glm::dot(in0, n) < 0.0f) return -1.0f;
	if (glm::dot(in1, n) < 0.0f) return -1.0f;
	if (glm::dot(in2, n) < 0.0f) return -1.0f;
	return t;
}

inline bool RayTestSphere(float* t0, float* t1, const Ray& r, const Sphere& s)
{
	const glm::vec3 a = s.c - r.p;
	const float tc = glm::dot(r.d, a);
	if (tc < 0.0f) return false;

	const float r2 = s.r * s.r;
	const float d2 = glm::dot(a, a) - tc * tc;
	if (d2 > r2) return false;
	const float td = sqrtf(r2 - d2);

	*t0 = tc - td;
	*t1 = tc + td;
	return true;
}

inline bool RayTestAABB(float* t0, float* t1, const Ray& r, const AABB& a)
{
	const float t0x = (a.min.x - r.p.x) / r.d.x;
	const float t0y = (a.min.y - r.p.y) / r.d.y;
	const float t0z = (a.min.z - r.p.z) / r.d.z;
	const float t1x = (a.max.x - r.p.x) / r.d.x;
	const float t1y = (a.max.y - r.p.y) / r.d.y;
	const float t1z = (a.max.z - r.p.z) / r.d.z;

	const float tminx = std::min(t0x, t1x);
	const float tminy = std::min(t0y, t1y);
	const float tminz = std::min(t0z, t1z);
	const float tmaxx = std::max(t0x, t1x);
	const float tmaxy = std::max(t0y, t1y);
	const float tmaxz = std::max(t0z, t1z);
	if (tminx > tmaxy || tminy > tmaxx)
		return false;

	*t0 = std::max(tminx, tminy);
	*t1 = std::min(tmaxy, tmaxx);
	if (*t0 > tmaxz || tminz > *t1)
		return false;

	*t0 = std::max(*t0, tminz);
	*t1 = std::min(*t1, tmaxz);
	return true;
}

inline Hit RayHitPlane(const Ray& r, const Plane& p)
{
	const glm::vec4 pf = Plane4(p.p, p.n);
	const float t = RayTestPlane(r, pf);
	if (t <= 0.0f) return Hit();
	Hit o;
	o.hit = 1;
	o.p = (r.p + (r.d * t));
	o.t0 = o.t1 = t;
	o.n = p.n * (-1.0f);
	return o;
}

inline Hit RayHitTriangle(const Ray& r, const Triangle& tr)
{
	const float t = RayTestTriangle(r, tr);
	if (t <= 0.0f) return Hit();

	Hit o;
	o.hit = 1;
	o.t0 = o.t1 = t;
	o.p = (r.p + (r.d * t));
	o.n = glm::normalize(glm::cross((tr.p1 - tr.p0), (tr.p2 - tr.p0)));
	return o;
}

inline Hit RayHitSphere(const Ray& r, const Sphere& s)
{
	Hit o;
	if (!RayTestSphere(&o.t0, &o.t1, r, s))
		return Hit();

	o.hit = 1;
	o.p = r.p + r.d * std::min(o.t0, o.t1);
	o.n = glm::normalize((o.p - s.c));
	return o;
}

inline Hit RayHitAABB(const Ray& r, const AABB& a)
{
	Hit o;

	glm::vec3 pnt, ext, c;
	float d, min;
	if (!RayTestAABB(&o.t0, &o.t1, r, a))
		return Hit();

	o.p = r.p + r.d * std::min(o.t0, o.t1);
	ext = a.max - a.min;
	c = a.min + ext * 0.5f;
	pnt = o.p - c;

	min = fabs(ext.x - fabs(pnt.x));
	o.n = glm::vec3(1, 0, 0) * glm::sign(pnt.x);
	d = fabs(ext.y - fabs(pnt.y));
	if (d < min) 
	{
		min = d;
		o.n = glm::vec3(0, 1, 0) * glm::sign(pnt.y);
	}
	d = fabs(ext.z - fabs(pnt.z));
	if (d < min)
		o.n = glm::vec3(0, 0, 1) * glm::sign(pnt.z);
	return o;
}

//-----------------------------------------------------------------------------
// Plane
//-----------------------------------------------------------------------------

//// Modified from: https://graphics.stanford.edu/~mdfisher/Code/Engine/Plane.cpp.html
//inline Plane PlaneFromPtNormal(const glm::vec3& pt, const glm::vec3& n)
//{
//
//	glm::vec3 nn = glm::normalize(n);
//	Plane p = { };
//	p.a = nn.x; p.b = nn.y; p.c = nn.z;
//	p.d = -glm::dot(pt, nn);
//	return p;
//}
//
//inline Plane PlaneFromPts(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c)
//{
//	glm::vec3 n = glm::normalize(glm::cross((b - a), (c - a)));
//	return PlaneFromPtNormal(a, n);
//}
//
//inline glm::vec3 PlaneNormal(const Plane& p)
//{
//	return glm::normalize(glm::vec3{ p.a, p.b, p.c });
//}
//
//inline glm::vec3 PlaneClosestPoint(const Plane& p, const glm::vec3& pt)
//{
//	return (pt - (PlaneNormal(p) * PlaneSignedDistance(p, pt)));
//}
//
//inline float PlaneSignedDistance(const Plane& p, const glm::vec3& pt)
//{
//	return (p.a * pt.x + p.b * pt.y + p.c * pt.z + p.d);
//}
//
//inline float PlaneUnsignedDistance(const Plane& p, const glm::vec3& pt)
//{
//	return fabsf(PlaneSignedDistance(p, pt));
//}
//
//inline Plane PlaneNormalized(const Plane& p)
//{
//	float d = sqrtf(p.a * p.a + p.b * p.b + p.c * p.c);
//	Plane pn = {};
//	pn.a = p.a / d; pn.b = p.b / d; pn.c = p.c / d; pn.d = p.d / d;
//	return pn;
//}

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