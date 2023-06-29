#define _COLLIDE_FUNC_IMPL(_TA, _TB, _F0, _F1)\
	inline int32_t _TA##Vs##_TB(const _TA& a, const Vqs& xa, const _TB& b, const Vqs& xb, ContactInfo* r)\
	{\
		return CCDGJKInternal(&a, xa, (_F0), &b, xb, (_F1), r);\
	}

inline glm::vec4 Plane4(const glm::vec3& p, const glm::vec3& n)
{
	return glm::vec4(n, -glm::dot(n, p));
}

inline int line_support(glm::vec3& support, const glm::vec3& d, const glm::vec3& a, const glm::vec3& b)
{
	int i = 0;
	const float adot = glm::dot(a, d);
	const float bdot = glm::dot(b, d);
	if (adot < bdot) 
	{
		support = b;
		i = 1;
	}
	else support = a;
	return i;
}

inline int poly_support(glm::vec3& support, const glm::vec3& d, const Poly& p) 
{
	int imax = 0;
	float dmax = glm::dot(p.verts[0], d);
	for (int i = 1; i < p.verts.size(); ++i)
	{
		/* find vertex with max dot product in direction d */
		float dot = glm::dot(p.verts[i], d);
		if (dot < dmax) continue;
		imax = i, dmax = dot;
	} 
	support = p.verts[imax];
	return imax;
}

inline void transform_(glm::vec3* r, const glm::vec3& v, const float* r33, const glm::vec3& t3)
{
	for (int i = 0; i < 3; ++i) 
	{
		i[&r->x]  = i[&v.x] * r33[i * 3 + 0];
		i[&r->x] += i[&v.x] * r33[i * 3 + 1];
		i[&r->x] += i[&v.x] * r33[i * 3 + 2];
		i[&r->x] += i[&t3.x];
	}
}
inline void transformS(glm::vec3* v, const float* r33, const glm::vec3& t3)
{
	glm::vec3 tmp = *v;
	transform_(v, tmp, r33, t3);
}
inline void transformT(glm::vec3* r, const glm::vec3& v, const float* r33, const glm::vec3& t3) 
{
	for (int i = 0; i < 3; ++i) 
	{
		float p = i[&v.x] - i[&t3.x];
		i[&r->x]  = p * r33[0 * 3 + i];
		i[&r->x] += p * r33[1 * 3 + i];
		i[&r->x] += p * r33[2 * 3 + i];
	}
}
inline void transformST(glm::vec3* v, const float* r33, const glm::vec3& t3)
{
	glm::vec3 tmp = *v;
	transformT(v, tmp, r33, t3);
}

//-----------------------------------------------------------------------------
// Line/Segment
//-----------------------------------------------------------------------------

inline float LineDistance2Point(const Line& line, const glm::vec3& point)
{
	const glm::vec3 ab = (line.a - line.b);
	const glm::vec3 ap = (line.a - point);
	const glm::vec3 bp = (line.b - point);
	// handle cases p proj outside ab
	const float e = glm::dot(ap, ab); if (e <= 0.0f) return glm::dot(ap, ap);
	const float f = glm::dot(ab, ab); if (e >= f) return glm::dot(bp, bp);
	return glm::dot(ap, ap) - (e * e) / f;
}

inline glm::vec3 LineClosestPoint(const Line& line, const glm::vec3& point)
{
	const glm::vec3 ab = line.b - line.a;
	const glm::vec3 pa = point - line.a;
	float t = glm::dot(pa, ab) / glm::dot(ab, ab);
	t = glm::clamp(t, 0.0f, 1.0f);
	glm::vec3 pt = line.a + (ab * t);
	return pt;
}

//-----------------------------------------------------------------------------
// Ray
//-----------------------------------------------------------------------------

inline float RayTestPlane(const OldRay& ray, const glm::vec4& plane)
{
	/*Result:
	* Behind : t < 0
	* Infront : t >= 0
	* Parallel : t = 0*/
	const glm::vec3 p(plane.x, plane.y, plane.z);
	const float n = -(glm::dot(p, ray.p) + plane.w);
	if (fabs(n) < 0.0001f) return 0.0f;
	return n / (glm::dot(p, ray.d));
}

inline float RayTestTriangle(const OldRay& ray, const Triangle& tr)
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
	const glm::vec3 in = ray.d * t + ray.p;

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

inline bool RayTestSphere(float* t0, float* t1, const OldRay& r, const Sphere& s)
{
	const glm::vec3 a = s.center - r.p;
	const float tc = glm::dot(r.d, a);
	if (tc < 0.0f) return false;

	const float r2 = s.radius * s.radius;
	const float d2 = glm::dot(a, a) - tc * tc;
	if (d2 > r2) return false;
	const float td = sqrtf(r2 - d2);

	*t0 = tc - td;
	*t1 = tc + td;
	return true;
}

inline bool RayTestAABB(float* t0, float* t1, const OldRay& r, const AABB& a)
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

inline Hit RayHitPlane(const OldRay& r, const OldPlane& p)
{
	const glm::vec4 pf = Plane4(p.p, p.n);
	const float t = RayTestPlane(r, pf);
	if (t <= 0.0f) return NotHit;
	Hit hit;
	hit.hit = 1;
	hit.p = (r.p + (r.d * t));
	hit.t0 = hit.t1 = t;
	hit.n = -p.n;
	return hit;
}

inline Hit RayHitTriangle(const OldRay& r, const Triangle& tr)
{
	const float t = RayTestTriangle(r, tr);
	if (t <= 0.0f) return NotHit;

	Hit hit;
	hit.hit = 1;
	hit.t0 = hit.t1 = t;
	hit.p = (r.p + (r.d * t));
	hit.n = glm::normalize(glm::cross((tr.p1 - tr.p0), (tr.p2 - tr.p0)));
	return hit;
}

inline Hit RayHitSphere(const OldRay& r, const Sphere& s)
{
	Hit hit;
	if (!RayTestSphere(&hit.t0, &hit.t1, r, s))
		return NotHit;

	hit.hit = 1;
	hit.p = r.p + r.d * std::min(hit.t0, hit.t1);
	hit.n = glm::normalize(hit.p - s.center);
	return hit;
}

inline Hit RayHitAABB(const OldRay& r, const AABB& a)
{
	Hit hit;
	if (!RayTestAABB(&hit.t0, &hit.t1, r, a))
		return NotHit;

	hit.hit = 1;
	hit.p = r.p + r.d * std::min(hit.t0, hit.t1);
	const glm::vec3 ext = a.max - a.min;
	const glm::vec3 c = a.min + ext * 0.5f;
	const glm::vec3 pnt = hit.p - c;

	float min = fabs(ext.x - fabs(pnt.x));
	hit.n = glm::vec3(1.0f, 0.0f, 0.0f) * glm::sign(pnt.x);
	float d = fabs(ext.y - fabs(pnt.y));
	if (d < min) 
	{
		min = d;
		hit.n = glm::vec3(0.0f, 1.0f, 0.0f) * glm::sign(pnt.y);
	}
	d = fabs(ext.z - fabs(pnt.z));
	if (d < min)
		hit.n = glm::vec3(0.0f, 0.0f, 1.0f) * glm::sign(pnt.z);
	return hit;
}

//-----------------------------------------------------------------------------
// Plane
//-----------------------------------------------------------------------------

// Modified from: https://graphics.stanford.edu/~mdfisher/Code/Engine/Plane.cpp.html
inline plane_t PlaneFromPtNormal(const glm::vec3& pt, const glm::vec3& n)
{

	glm::vec3 nn = glm::normalize(n);
	plane_t p = { };
	p.a = nn.x; p.b = nn.y; p.c = nn.z;
	p.d = -glm::dot(pt, nn);
	return p;
}

inline plane_t PlaneFromPts(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c)
{
	glm::vec3 n = glm::normalize(glm::cross((b - a), (c - a)));
	return PlaneFromPtNormal(a, n);
}

inline glm::vec3 PlaneNormal(const plane_t& p)
{
	return glm::normalize(glm::vec3{ p.a, p.b, p.c });
}

inline glm::vec3 PlaneClosestPoint(const plane_t& p, const glm::vec3& pt)
{
	return (pt - (PlaneNormal(p) * PlaneSignedDistance(p, pt)));
}

inline float PlaneSignedDistance(const plane_t& p, const glm::vec3& pt)
{
	return (p.a * pt.x + p.b * pt.y + p.c * pt.z + p.d);
}

inline float PlaneUnsignedDistance(const plane_t& p, const glm::vec3& pt)
{
	return fabsf(PlaneSignedDistance(p, pt));
}

inline plane_t PlaneNormalized(const plane_t& p)
{
	float d = sqrtf(p.a * p.a + p.b * p.b + p.c * p.c);
	plane_t pn = {};
	pn.a = p.a / d; pn.b = p.b / d; pn.c = p.c / d; pn.d = p.d / d;
	return pn;
}

//-----------------------------------------------------------------------------
// Sphere
//-----------------------------------------------------------------------------

inline glm::vec3 SphereClosestPoint(const Sphere& s, const glm::vec3& p)
{
	const glm::vec3 d = glm::normalize(p - s.center);
	return (s.center + d * s.radius);
}

inline bool SphereTestAABB(const Sphere& s, const AABB& a)
{
	return AABBTestSphere(a, s);
}

inline bool SphereTestCapsule(const Sphere& s, const Capsule& c)
{
	return CapsuleTestSphere(c, s);
}

inline bool SphereTestPoly(const Sphere& s, const Poly& p)
{
	return PolyTestSphere(p, s);
}

inline bool SphereTestSphere(const Sphere& a, const Sphere& b)
{
	const glm::vec3 d = (b.center - a.center);
	const float r = a.radius + b.radius;
	if (glm::dot(d, d) > r * r)
		return false;
	return true;
}

inline Hit SphereHitAABB(const Sphere& s, const AABB& a)
{
	/* find closest aabb point to sphere center point */
	const glm::vec3 ap = AABBClosestPoint(a, s.center);
	glm::vec3 d = s.center - ap;
	const float d2 = glm::dot(d, d);
	if (d2 > s.radius * s.radius) return NotHit;

	Hit hit;
	hit.hit = 1;
	/* calculate distance vector between sphere and aabb center points */
	const glm::vec3 ac = (a.min + ((a.max - a.min) * 0.5f));
	d = ac - s.center;

	/* normalize distance vector */
	const float l2 = glm::dot(d, d);
	const float l = l2 != 0.0f ? sqrtf(l2) : 1.0f;
	const float linv = 1.0f / l;
	d = d * linv;

	hit.normal = d;
	hit.contact_point = hit.normal * s.radius;
	hit.contact_point = s.center + hit.contact_point;

	/* calculate penetration depth */
	const glm::vec3 sp = SphereClosestPoint(s, ap);
	d = sp - ap;
	hit.depth = sqrtf(glm::dot(d, d)) - l;
	return hit;
}

inline Hit SphereHitCapsule(Sphere s, const Capsule& c)
{
#if 0
	// original code
	/* find closest capsule point to sphere center point */
	Hit hit{ .hit = 1 };
	glm::vec3 cp = CapsuleClosestPoint(c, s.center);
	hit.normal = cp - s.center;
	float d2 = glm::dot(hit.normal, hit.normal);
	if (d2 > s.radius * s.radius) return NotHit;

	/* normalize hit normal vector */
	hit.normal = glm::normalize(hit.normal);

	/* calculate penetration depth */
	hit.depth = d2 - s.radius * s.radius;
	hit.depth = hit.depth != 0.0f ? sqrtf(hit.depth) : 0.0f;
	hit.contact_point = s.center + hit.normal * s.radius;
	return hit;
#else
	// aproximation of I would expect this function to return instead
	const glm::vec3 l = c.a - c.b; 
	const float len = glm::length(l);
	glm::vec3 d = glm::normalize(l);
	OldRay r = OldRay(c.a + (d * (-2.0f * len)), d);
	s.radius += c.r;
	Hit hit = RayHitSphere(r, s);
	if (hit.hit == 0) return NotHit;
	s.radius -= c.r;
	hit.contact_point = s.center + (glm::normalize((hit.contact_point - s.center)) * s.radius);
	return hit;
#endif
}

inline Hit SphereHitSphere(const Sphere& a, const Sphere& b)
{
	glm::vec3 d = b.center - a.center;
	const float r = a.radius + b.radius;
	const float d2 = glm::dot(d, d);
	if (d2 > r * r) return NotHit;

	float l = sqrtf(d2);
	float linv = 1.0f / ((l != 0.0f) ? l : 1.0f);

	Hit hit;
	hit.hit = 1;
	hit.normal = d * linv;
	hit.depth = r - l;
	d = hit.normal * b.radius;
	hit.contact_point = b.center - d;
	return hit;
}

//-----------------------------------------------------------------------------
// AABB
//-----------------------------------------------------------------------------
inline float AABBDistance2Point(const AABB& a, const glm::vec3& p)
{
	float r = 0;
	for (int i = 0; i < 3; ++i) 
	{
		float v = i[&p.x];
		if (v < i[&a.min.x]) r += (i[&a.min.x] - v) * (i[&a.min.x] - v);
		if (v > i[&a.max.x]) r += (v - i[&a.max.x]) * (v - i[&a.max.x]);
	} 
	return r;
}

inline glm::vec3 AABBClosestPoint(const AABB& a, const glm::vec3& p)
{
	glm::vec3 res;
	for (int i = 0; i < 3; ++i)
	{
		float v = i[&p.x];
		if (v < i[&a.min.x]) v = i[&a.min.x];
		if (v > i[&a.max.x]) v = i[&a.max.x];
		i[&res.x] = v;
	}
	return res;
}

inline bool AABBContainsPoint(const AABB& a, const glm::vec3& p)
{
	if (p.x < a.min.x || p.x > a.max.x) return false;
	if (p.y < a.min.y || p.y > a.max.y) return false;
	if (p.z < a.min.z || p.z > a.max.z) return false;
	return true;
}

inline bool AABBTestAABB(const AABB& a, const AABB& b)
{
	if (a.max.x < b.min.x || a.min.x > b.max.x) return false;
	if (a.max.y < b.min.y || a.min.y > b.max.y) return false;
	if (a.max.z < b.min.z || a.min.z > b.max.z) return false;
	return true;
}

inline bool AABBTestCapsule(const AABB& a, const Capsule& c)
{
	return CapsuleTestAABB(c, a);
}

inline bool AABBTestPoly(const AABB& a, const Poly& p)
{
	return PolyTestAABB(p, a);
}

inline bool AABBTestSphere(const AABB& a, const Sphere& s)
{
	/* compute squared distance between sphere center and aabb */
	const float d2 = AABBDistance2Point(a, s.center);
	/* intersection if distance is smaller/equal sphere radius*/
	return d2 <= s.radius * s.radius;
}

inline Hit AABBHitAABB(const AABB& a, const AABB& b)
{
	if (!AABBTestAABB(a, b))
		return NotHit;

	/* calculate distance vector between both aabb center points */
	glm::vec3 ac = a.max - a.min;
	glm::vec3 bc = b.max - b.min;
	ac = ac * 0.5f;
	bc = bc * 0.5f;
	ac = a.min + ac;
	bc = b.min + bc;

	glm::vec3 d = bc - ac;

	/* normalize distance vector */
	const float l2 = glm::dot(d, d);
	const float l = l2 != 0.0f ? sqrtf(l2) : 1.0f;
	const float linv = 1.0f / l;
	d = d * linv;

	/* calculate contact point */
	Hit hit;
	hit.hit = 1;
	hit.normal = d;
	hit.contact_point = AABBClosestPoint(a, bc);
	d = hit.contact_point - ac;

	/* calculate penetration depth */
	const float r2 = glm::dot(d, d);
	const float r = sqrtf(r2);
	hit.depth = r - l;
	return hit;
}

inline Hit AABBHitCapsule(const AABB& a, const Capsule& c)
{
	/* calculate aabb center point */
	const glm::vec3 ac = a.min + ((a.max - a.min) * 0.5f);

	/* calculate closest point from aabb to point on capsule and check if inside aabb */
	const glm::vec3 cp = CapsuleClosestPoint(c, ac);
	if (!AABBContainsPoint(a, cp))
		return NotHit;

	Hit hit;
	hit.hit = 1;
	/* vector and distance between both capsule closests point and aabb center*/
	 const glm::vec3 d = cp - ac;
	 const float d2 = glm::dot(d, d);

	/* calculate penetration depth from closest aabb point to capsule */
	const glm::vec3 ap = AABBClosestPoint(a, cp);
	const glm::vec3 dt = ap - cp;
	hit.depth = sqrtf(glm::dot(dt, dt));

	/* calculate normal */
	float l = sqrtf(d2);
	float linv = 1.0f / ((l != 0.0f) ? l : 1.0f);
	hit.normal = d * linv;
	hit.contact_point = ap;
	return hit;
}

inline Hit AABBHitSphere(const AABB& a, const Sphere& s)
{
	/* find closest aabb point to sphere center point */
	Hit hit;
	hit.hit = 1;
	hit.contact_point = AABBClosestPoint(a, s.center);
	glm::vec3 d = s.center - hit.contact_point;
	float d2 = glm::dot(d, d);
	if (d2 > s.radius * s.radius) return NotHit;

	/* calculate distance vector between aabb and sphere center points */
	glm::vec3 ac = a.min + ((a.max - a.min) * 0.5f);
	d = s.center - ac;

	/* normalize distance vector */
	float l2 = glm::dot(d, d);
	float l = l2 != 0.0f ? sqrtf(l2) : 1.0f;
	float linv = 1.0f / l;
	d = d * linv;

	/* calculate penetration depth */
	hit.normal = d;
	d = hit.contact_point - ac;
	hit.depth = sqrtf(glm::dot(d, d));
	return hit;
}

//-----------------------------------------------------------------------------
// Capsule
//-----------------------------------------------------------------------------

inline float CapsuleDistance2Point(const Capsule& c, const glm::vec3& p)
{
	const float d2 = LineDistance2Point(Line(c.a, c.b), p);
	return d2 - (c.r * c.r);
}

inline glm::vec3 CapsuleClosestPoint(const Capsule& c, const glm::vec3& p)
{
	/* calculate closest point to internal capsule segment */
	const glm::vec3 pp = LineClosestPoint(Line(c.a, c.b), p);

	/* extend point out by radius in normal direction */
	const glm::vec3 d = glm::normalize(p - pp);
	return (pp + (d * c.r));
}

inline bool CapsuleTestAABB(const Capsule& c, const AABB& a)
{
	/* calculate aabb center point */
	const glm::vec3 ac = (a.max - a.min) * 0.5f;

	/* calculate closest point from aabb to point on capsule and check if inside aabb */
	const glm::vec3 p = CapsuleClosestPoint(c, ac);
	return AABBContainsPoint(a, p);
}

#define C_EPSILON  (1e-6)// TODO: delete
inline float clampf(float v, float a, float b) { return std::max(std::min(b, v), a); } // TODO: delete
inline float lineClosestLine_(float& out_t1, float& out_t2, glm::vec3& out_c1, glm::vec3& out_c2, const Line& l, const Line& m)
{
	const glm::vec3 d1 = l.b - l.a; /* direction vector segment s1 */
	const glm::vec3 d2 = m.b - m.a; /* direction vector segment s2 */
	const glm::vec3 r = l.a - m.a;

	const float i = glm::dot(d1, d1);
	const float e = glm::dot(d2, d2);
	const float f = glm::dot(d2, r);

	if (i <= C_EPSILON && e <= C_EPSILON) 
	{
		/* both segments degenerate into points */
		out_t1 = out_t2 = 0.0f;
		out_c1 = l.a;
		out_c2 = m.a;
		const glm::vec3 d12 = (out_c1 - out_c2);
		return glm::dot(d12, d12);
	}
	if (i > C_EPSILON) 
	{
		const float c = glm::dot(d1, r);
		if (e > C_EPSILON) 
		{
			/* non-degenerate case */
			const float b = glm::dot(d1, d2);
			const float denom = i * e - b * b;

			/* compute closest point on L1/L2 if not parallel else pick any t2 */
			if (denom != 0.0f)
				out_t1 = clampf(0.0f, (b * f - c * e) / denom, 1.0f);
			else out_t1 = 0.0f;

			/* cmpute point on L2 closest to S1(s) */
			out_t2 = (b * out_t1 + f) / e;
			if (out_t2 < 0.0f)
			{
				out_t2 = 0.0f;
				out_t1 = clampf(0.0f, -c / i, 1.0f);
			}
			else if (out_t2 > 1.0f)
			{
				out_t2 = 1.0f;
				out_t1 = clampf(0.0f, (b - c) / i, 1.0f);
			}
		}
		else
		{
			/* second segment degenerates into a point */
			out_t1 = clampf(0.0f, -c / i, 1.0f);
			out_t2 = 0.0f;
		}
	}
	else
	{
		/* first segment degenerates into a point */
		out_t2 = clampf(0.0f, f / e, 1.0f);
		out_t1 = 0.0f;
	}
	/* calculate closest points */
	glm::vec3 n = d1 * out_t1;
	out_c1 = l.a + n;
	n = d2 * out_t2;
	out_c2 = m.a + n;

	/* calculate squared distance */
	const glm::vec3 d12 = out_c1 - out_c2;
	return glm::dot(d12, d12);
}

inline bool CapsuleTestCapsule(const Capsule& a, const Capsule& b)
{
	float t1, t2;
	glm::vec3 c1, c2;
	const float d2 = lineClosestLine_(t1, t2, c1, c2, Line(a.a, a.b), Line(b.a, b.b));
	const float r = a.r + b.r;
	return d2 <= r * r;
}

inline bool CapsuleTestPoly(const Capsule& c, const Poly& p)
{
	return PolyTestCapsule(p, c);
}

inline bool CapsuleTestSphere(const Capsule& c, const Sphere& s)
{
	/* squared distance bwetween sphere center and capsule line segment */
	const float d2 = LineDistance2Point(Line(c.a, c.b), s.center);
	const float r = s.radius + c.r;
	return d2 <= r * r;
}

inline Hit CapsuleHitAABB(const Capsule& c, const AABB& a)
{
	/* calculate aabb center point */
	const glm::vec3 ac = a.min + ((a.max - a.min) * 0.5f);

	/* calculate closest point from aabb to point on capsule and check if inside aabb */
	const glm::vec3 cp = CapsuleClosestPoint(c, ac);
	if (!AABBContainsPoint(a, cp))
		return NotHit;

	Hit hit;
	hit.hit = 1;
	/* vector and distance between both capsule closests point and aabb center*/
	const glm::vec3 d = ac - cp;
	const float d2 = glm::dot(d, d);

	/* calculate penetration depth from closest aabb point to capsule */
	const glm::vec3 ap = AABBClosestPoint(a, cp);
	const glm::vec3 dt = ap - cp;
	hit.depth = sqrtf(glm::dot(dt, dt));

	/* calculate normal */
	float l = sqrtf(d2);
	float linv = 1.0f / ((l != 0.0f) ? l : 1.0f);
	hit.normal = d * linv;
	hit.contact_point = cp;
	return hit;
}

inline Hit CapsuleHitCapsule(const Capsule& a, const Capsule& b)
{
	float t1, t2;
	glm::vec3 c1, c2;
	float d2 = lineClosestLine_(t1, t2, c1, c2, Line(a.a, a.b), Line(b.a, b.b));
	float r = a.r + b.r;
	if (d2 > r * r) return NotHit;

	Hit hit;
	hit.hit = 1;
	/* calculate normal from both closest points for each segement */
	glm::vec3 cp, d;
	hit.normal = c2 - c1;
	hit.normal = glm::normalize(hit.normal);

	/* calculate contact point from closest point and depth */
	hit.contact_point = CapsuleClosestPoint(a, c2);
	cp = CapsuleClosestPoint(b, c1);
	d = c1 - cp;
	hit.depth = sqrtf(glm::dot(d, d));
	return hit;
}

inline Hit CapsuleHitSphere(const Capsule& c, const Sphere& s)
{
	/* find closest capsule point to sphere center point */
	Hit hit;
	hit.hit = 1;
	hit.contact_point = CapsuleClosestPoint(c, s.center);
	hit.normal = s.center - hit.contact_point;
	float d2 = glm::dot(hit.normal, hit.normal);
	if (d2 > s.radius * s.radius) return NotHit;

	/* normalize hit normal vector */
	float l = d2 != 0.0f ? sqrtf(d2) : 1;
	float linv = 1.0f / l;
	hit.normal = hit.normal * linv;

	/* calculate penetration depth */
	hit.depth = d2 - s.radius * s.radius;
	hit.depth = hit.depth != 0.0f ? sqrtf(hit.depth) : 0.0f;
	return hit;
}

//-----------------------------------------------------------------------------
// Poly
//-----------------------------------------------------------------------------

inline bool PolyTestSphere(const Poly& p, const Sphere& s)
{	
	gjk_result res;
	return PolyHitSphere(&res, p, s);
}

inline bool PolyTestAABB(const Poly& p, const AABB& a)
{
	gjk_result res;
	return PolyHitAABB(&res, p, a);
}

inline bool PolyTestCapsule(const Poly& p, const Capsule& c)
{
	gjk_result res;
	return PolyHitCapsule(&res, p, c);
}

inline bool PolyTestPoly(const Poly& a, const Poly& b)
{
	gjk_result res;
	return PolyHitPoly(&res, a, b);
}


inline bool PolyTestSphereTransform(const Poly& p, const glm::vec3& pos3, mat33 rot33, const Sphere& s)
{
	gjk_result res;
	return PolyHitSphereTransform(&res, p, pos3, rot33, s);
}

inline bool PolyTestAABBTransform(const Poly& p, const glm::vec3& apos3, mat33 arot33, const AABB& a)
{
	gjk_result res;
	return PolyHitAABBTransform(&res, p, apos3, arot33, a);
}

inline bool PolyTestCapsuleTransform(const Poly& p, const glm::vec3& pos3, mat33 rot33, const Capsule& c)
{
	gjk_result res;
	return PolyHitCapsuleTransform(&res, p, pos3, rot33, c);
}

inline bool PolyTestPolyTransform(const Poly& a, const glm::vec3& apos3, mat33 arot33, const Poly& b, const glm::vec3& bpos3, mat33 brot33)
{
	gjk_result res;
	return PolyHitPolyTransform(&res, a, apos3, arot33, b, bpos3, brot33);
}

inline bool PolyHitSphere(gjk_result* res, const Poly& p, const Sphere& s)
{
	/* initial guess */
	gjk_support gs = { 0 };
	gs.a = p.verts[0];
	gs.b = s.center;
	glm::vec3 d = gs.b - gs.a;

	/* run gjk algorithm */
	gjk_simplex gsx = { 0 };
	while (gjk(&gsx, &gs, &d))
	{
		glm::vec3 n = -d;
		gs.aid = poly_support(gs.a, n, p);
		d = gs.b - gs.a;
	}
	/* check distance between closest points */
	*res = gjk_analyze(&gsx);
	return res->distance_squared <= s.radius * s.radius;
}

inline bool PolyHitAABB(gjk_result* res, const Poly& p, const AABB& a)
{
	Poly poly;
	poly.verts.resize(8);
	poly.verts[0] = glm::vec3(a.min.x, a.min.y, a.min.z);
	poly.verts[1] = glm::vec3(a.min.x, a.min.y, a.max.z);
	poly.verts[2] = glm::vec3(a.min.x, a.max.y, a.min.z);
	poly.verts[3] = glm::vec3(a.min.x, a.max.y, a.max.z);
	poly.verts[4] = glm::vec3(a.max.x, a.min.y, a.min.z);
	poly.verts[5] = glm::vec3(a.max.x, a.min.y, a.max.z);
	poly.verts[6] = glm::vec3(a.max.x, a.max.y, a.min.z);
	poly.verts[7] = glm::vec3(a.max.x, a.max.y, a.max.z);
	return PolyHitPoly(res, p, poly);
}

inline bool PolyHitCapsule(gjk_result* res, const Poly& p, const Capsule& c)
{
	/* initial guess */
	gjk_support s = { 0 };
	s.a = p.verts[0];
	s.b = c.a;
	glm::vec3 d = s.b - s.a;

	/* run gjk algorithm */
	gjk_simplex gsx = { 0 };
	while (gjk(&gsx, &s, &d)) 
	{
		glm::vec3 n = -d;
		s.aid = poly_support(s.a, n, p);
		s.bid = line_support(s.b, d, c.a, c.b);
		d = s.b - s.a;
	}
	/* check distance between closest points */
	assert(gsx.iter < gsx.max_iter);
	*res = gjk_analyze(&gsx);
	return res->distance_squared <= c.r * c.r;
}

inline bool PolyHitPoly(gjk_result* res, const Poly& a, const Poly& b)
{
	/* initial guess */
	gjk_support gs = { 0 };
	gs.a = a.verts[0];
	gs.b = b.verts[0];
	glm::vec3 d = gs.b - gs.a;

	/* run gjk algorithm */
	gjk_simplex gsx = { 0 };
	while (gjk(&gsx, &gs, &d)) 
	{
		glm::vec3 n = -d;
		gs.aid = poly_support(gs.a, n, a);
		gs.bid = poly_support(gs.b, d, b);
		d = gs.b - gs.a;
	}
	*res = gjk_analyze(&gsx);
	return gsx.hit;
}

inline bool PolyHitSphereTransform(gjk_result* res, const Poly& p, const glm::vec3& pos3, mat33 rot33, const Sphere& s)
{
	/* initial guess */
	gjk_support gs = { 0 };
	gs.a = p.verts[0];
	gs.b = s.center;
	transformS(&gs.a, rot33, pos3);
	glm::vec3 d = gs.b - gs.a;

	/* run gjk algorithm */
	gjk_simplex gsx = { 0 };
	while (gjk(&gsx, &gs, &d))
	{
		const glm::vec3 n = -d;
		glm::vec3 da;
		transformT(&da, n, rot33, pos3);

		gs.aid = poly_support(gs.a, da, p);
		transformS(&gs.a, rot33, pos3);
		d = gs.b - gs.a;
	}
	/* check distance between closest points */
	*res = gjk_analyze(&gsx);
	return res->distance_squared <= s.radius * s.radius;
}

inline bool PolyHitAABBTransform(gjk_result* res, const Poly& p, const glm::vec3& pos3, mat33 rot33, const AABB& a)
{
	glm::vec3 zero = glm::vec3{ 0 };
	float id[] = { 1.0f,0.0f,0.0f, 0.0f,1.0f,0.0f, 0.0f,0.0f,1.0f };
	Poly poly;
	poly.verts.resize(8);
	poly.verts[0] = glm::vec3(a.min.x, a.min.y, a.min.z);
	poly.verts[1] = glm::vec3(a.min.x, a.min.y, a.max.z);
	poly.verts[2] = glm::vec3(a.min.x, a.max.y, a.min.z);
	poly.verts[3] = glm::vec3(a.min.x, a.max.y, a.max.z);
	poly.verts[4] = glm::vec3(a.max.x, a.min.y, a.min.z);
	poly.verts[5] = glm::vec3(a.max.x, a.min.y, a.max.z);
	poly.verts[6] = glm::vec3(a.max.x, a.max.y, a.min.z);
	poly.verts[7] = glm::vec3(a.max.x, a.max.y, a.max.z);
	return PolyHitPolyTransform(res, p, pos3, rot33, poly, zero, id);
}

inline bool PolyHitCapsuleTransform(gjk_result* res, const Poly& p, const glm::vec3& pos3, mat33 rot33, const Capsule& c)
{
	/* initial guess */
	gjk_support gs = { 0 };
	gs.a = p.verts[0];
	gs.b = c.a;
	transformS(&gs.a, rot33, pos3);
	glm::vec3 d = gs.b - gs.a;

	/* run gjk algorithm */
	gjk_simplex gsx = { 0 };
	while (gjk(&gsx, &gs, &d)) 
	{
		glm::vec3 n = -d;
		glm::vec3 da; 
		transformT(&da, n, rot33, pos3);

		gs.aid = poly_support(gs.a, da, p);
		gs.bid = line_support(gs.b, d, c.a, c.b);
		transformS(&gs.a, rot33, pos3);
		d = gs.b - gs.a;
	}
	/* check distance between closest points */
	*res = gjk_analyze(&gsx);
	return res->distance_squared <= c.r * c.r;
}

inline bool PolyHitPolyTransform(gjk_result* res, const Poly& a, const glm::vec3& at3, mat33 ar33, const Poly& b, const glm::vec3& bt3, mat33 br33)
{
	/* initial guess */
	gjk_support gs = { 0 };
	gs.a = a.verts[0];
	gs.b = b.verts[0];
	transformS(&gs.a, ar33, at3);
	transformS(&gs.b, br33, bt3);
	glm::vec3 d = gs.b - gs.a;

	/* run gjk algorithm */
	gjk_simplex gsx = { 0 };
	while (gjk(&gsx, &gs, &d)) 
	{
		/* transform direction */
		glm::vec3 n = -d;
		glm::vec3 da; transformT(&da, n, ar33, at3);
		glm::vec3 db; transformT(&db, d, br33, bt3);
		/* run support function on tranformed directions  */
		gs.aid = poly_support(gs.a, da, a);
		gs.bid = poly_support(gs.b, db, b);
		/* calculate distance vector on transformed points */
		transformS(&gs.a, ar33, at3);
		transformS(&gs.b, br33, bt3);
		d = gs.b - gs.a;
	}
	*res = gjk_analyze(&gsx);
	return gsx.hit;
}

//-----------------------------------------------------------------------------
// Cylinder
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Cone
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Frustum
//-----------------------------------------------------------------------------
inline Frustum FrustumBuild(mat44 pv)
{
	Frustum f;
	f.l = glm::vec4(pv[3] + pv[0], pv[7] + pv[4], pv[11] + pv[8], pv[15] + pv[12]);
	f.r = glm::vec4(pv[3] - pv[0], pv[7] - pv[4], pv[11] - pv[8], pv[15] - pv[12]);
	f.t = glm::vec4(pv[3] - pv[1], pv[7] - pv[5], pv[11] - pv[9], pv[15] - pv[13]);
	f.b = glm::vec4(pv[3] + pv[1], pv[7] + pv[5], pv[11] + pv[9], pv[15] + pv[13]);
	f.n = glm::vec4(pv[3] + pv[2], pv[7] + pv[6], pv[11] + pv[10], pv[15] + pv[14]);
	f.f = glm::vec4(pv[3] - pv[2], pv[7] - pv[6], pv[11] - pv[10], pv[15] - pv[14]);
	for (int i = 0; i < 6; i++) 
		f.pl[i] = f.pl[i] * (1.0f / glm::length(glm::vec3(f.pl[i])));
	return f;
}

inline bool FrustumTestSphere(const Frustum& f, const Sphere& s)
{
	for (int i = 0; i < 6; i++)
	{
		if ((glm::dot(glm::vec3(f.pl[i]), s.center) + f.pl[i].w + s.radius) < 0) return false;
	}
	return true;
}

inline bool FrustumTestAABB(const Frustum& f, const AABB& a)
{
	for (int i = 0; i < 6; i++)
	{
		glm::vec3 v = glm::vec3(f.pl[i].x > 0 ? a.max.x : a.min.x, f.pl[i].y > 0 ? a.max.y : a.min.y, f.pl[i].z > 0 ? a.max.z : a.min.z);
		if ((glm::dot(glm::vec3(f.pl[i]), v) + f.pl[i].w) < 0) return false;
	}
	return true;
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
	*out = (glm::normalize(dir) * scl * s->radius) + (xform.position + s->center);
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
	const capsule_t* c = (const capsule_t*)_o;

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
	const ray_t* r = (ray_t*)_r;

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
_COLLIDE_FUNC_IMPL(Sphere, capsule_t, SupportSphere, SupportCapsule);    // Sphere vs. Capsule
_COLLIDE_FUNC_IMPL(Sphere, Poly, SupportSphere, SupportPoly);          // Sphere vs. Poly 
_COLLIDE_FUNC_IMPL(Sphere, ray_t, SupportSphere, SupportRay);            // Sphere vs. Ray
/* AABB */

_COLLIDE_FUNC_IMPL(AABB, AABB, SupportAABB, SupportAABB);          // AABB vs. AABB
_COLLIDE_FUNC_IMPL(AABB, Cylinder, SupportAABB, SupportCylinder);  // AABB vs. Cylinder
_COLLIDE_FUNC_IMPL(AABB, Cone, SupportAABB, SupportCone);          // AABB vs. Cone
_COLLIDE_FUNC_IMPL(AABB, Sphere, SupportAABB, SupportSphere);      // AABB vs. Sphere
_COLLIDE_FUNC_IMPL(AABB, capsule_t, SupportAABB, SupportCapsule);    // AABB vs. Capsule
_COLLIDE_FUNC_IMPL(AABB, Poly, SupportAABB, SupportPoly);          // AABB vs. Poly
_COLLIDE_FUNC_IMPL(AABB, ray_t, SupportAABB, SupportRay);            // AABB vs. Ray

/* Capsule */

_COLLIDE_FUNC_IMPL(capsule_t, capsule_t, SupportCapsule, SupportCapsule);    // Capsule vs. Capsule
_COLLIDE_FUNC_IMPL(capsule_t, Cylinder, SupportCapsule, SupportCylinder);  // Capsule vs. Cylinder
_COLLIDE_FUNC_IMPL(capsule_t, Cone, SupportCapsule, SupportCone);          // Capsule vs. Cone
_COLLIDE_FUNC_IMPL(capsule_t, Sphere, SupportCapsule, SupportSphere);      // Capsule vs. Sphere
_COLLIDE_FUNC_IMPL(capsule_t, AABB, SupportCapsule, SupportAABB);          // Capsule vs. AABB
_COLLIDE_FUNC_IMPL(capsule_t, Poly, SupportCapsule, SupportPoly);          // Capsule vs. Poly
_COLLIDE_FUNC_IMPL(capsule_t, ray_t, SupportCapsule, SupportRay);            // Capsule vs. Ray

/* Poly */

_COLLIDE_FUNC_IMPL(Poly, Poly, SupportPoly, SupportPoly);          // Poly vs. Poly
_COLLIDE_FUNC_IMPL(Poly, Cylinder, SupportPoly, SupportCylinder);  // Poly vs. Cylinder
_COLLIDE_FUNC_IMPL(Poly, Cone, SupportPoly, SupportCone);          // Poly vs. Cone
_COLLIDE_FUNC_IMPL(Poly, Sphere, SupportPoly, SupportSphere);      // Poly vs. Sphere
_COLLIDE_FUNC_IMPL(Poly, AABB, SupportPoly, SupportAABB);          // Poly vs. AABB
_COLLIDE_FUNC_IMPL(Poly, capsule_t, SupportPoly, SupportCapsule);    // Poly vs. Capsule
_COLLIDE_FUNC_IMPL(Poly, ray_t, SupportPoly, SupportRay);            // Poly vs. Ray

/* Cylinder */

_COLLIDE_FUNC_IMPL(Cylinder, Cylinder, SupportCylinder, SupportCylinder);  // Cylinder vs. Cylinder
_COLLIDE_FUNC_IMPL(Cylinder, Poly, SupportPoly, SupportPoly);              // Cylinder vs. Poly
_COLLIDE_FUNC_IMPL(Cylinder, Cone, SupportCylinder, SupportCone);          // Cylinder vs. Cone
_COLLIDE_FUNC_IMPL(Cylinder, Sphere, SupportCylinder, SupportSphere);      // Cylinder vs. Sphere
_COLLIDE_FUNC_IMPL(Cylinder, AABB, SupportCylinder, SupportAABB);          // Cylinder vs. AABB
_COLLIDE_FUNC_IMPL(Cylinder, capsule_t, SupportCylinder, SupportCapsule);    // Cylinder vs. Capsule
_COLLIDE_FUNC_IMPL(Cylinder, ray_t, SupportCylinder, SupportRay);            // Cylinder vs. Ray

/* Cone */

_COLLIDE_FUNC_IMPL(Cone, Cone, SupportCone, SupportCone);          // Cone vs. Cone
_COLLIDE_FUNC_IMPL(Cone, Poly, SupportPoly, SupportPoly);          // Cone vs. Poly
_COLLIDE_FUNC_IMPL(Cone, Cylinder, SupportCone, SupportCylinder);  // Cone vs. Cylinder
_COLLIDE_FUNC_IMPL(Cone, Sphere, SupportCone, SupportSphere);      // Cone vs. Sphere
_COLLIDE_FUNC_IMPL(Cone, AABB, SupportCone, SupportAABB);          // Cone vs. AABB
_COLLIDE_FUNC_IMPL(Cone, capsule_t, SupportCone, SupportCapsule);    // Cone vs. Capsule
_COLLIDE_FUNC_IMPL(Cone, ray_t, SupportCone, SupportRay);            // Cone vs. Ray