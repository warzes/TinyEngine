#pragma once

// GJK distance algorithm. original code by @vurtun and @randygaul, public domain.
// [src] https://gist.github.com/vurtun/29727217c269a2fbf4c0ed9a1d11cb40

/*
Gilbert–Johnson–Keerthi (GJK) 3D distance algorithm
The Gilbert–Johnson–Keerthi (GJK) distance algorithm is a method of determining
the minimum distance between two convex sets. The algorithm's stability, speed
which operates in near-constant time, and small storage footprint make it
popular for realtime collision detection.

Unlike many other distance algorithms, it has no requirments on geometry data
to be stored in any specific format, but instead relies solely on a support
function to iteratively generate closer simplices to the correct answer using
the Minkowski sum (CSO) of two convex shapes.

GJK algorithms are used incrementally. In this mode, the final simplex from a
previous solution is used as the initial guess in the next iteration. If the
positions in the new frame are close to those in the old frame, the algorithm
will converge in one or two iterations.
*/

//-----------------------------------------------------------------------------
// GJK
//-----------------------------------------------------------------------------

#define GJK_MAX_ITERATIONS 20

struct gjk_support
{
	int aid, bid;
	glm::vec3 a;
	glm::vec3 b;
};
struct gjk_vertex
{
	glm::vec3 a;
	glm::vec3 b;
	glm::vec3 p;
	int aid, bid;
};
struct gjk_simplex
{
	int max_iter, iter;
	int hit, cnt;
	gjk_vertex v[4];
	float bc[4], D;
};
struct gjk_result
{
	int hit;
	glm::vec3 p0;
	glm::vec3 p1;
	float distance_squared;
	int iterations;
};

int gjk(gjk_simplex* s, const gjk_support* sup, glm::vec3* dv);
gjk_result gjk_analyze(const gjk_simplex* s);
gjk_result gjk_quad(float a_radius, float b_radius);

//-----------------------------------------------------------------------------
// GJK impl
//-----------------------------------------------------------------------------

#define GJK_FLT_MAX FLT_MAX // 3.40282347E+38F
#define GJK_EPSILON FLT_EPSILON // 1.19209290E-07F

inline float gjk_inv_sqrt(float n) 
{
	union { unsigned u; float f; } conv; conv.f = n;
	conv.u = 0x5f375A84 - (conv.u >> 1);
	conv.f = conv.f * (1.5f - (n * 0.5f * conv.f * conv.f));
	return conv.f;
}

inline int gjk(gjk_simplex* s, const gjk_support* sup, glm::vec3* dv)
{
	assert(s);
	assert(dv);
	assert(sup);
	if (!s || !sup || !dv) return 0;
	if (s->max_iter > 0 && s->iter >= s->max_iter)
		return 0;

	/* I.) Initialize */
	if (s->cnt == 0) 
	{
		s->D = GJK_FLT_MAX;
		s->max_iter = !s->max_iter ? GJK_MAX_ITERATIONS : s->max_iter;
	}
	/* II.) Check for duplications */
	for (int i = 0; i < s->cnt; ++i) 
	{
		if (sup->aid != s->v[i].aid) continue;
		if (sup->bid != s->v[i].bid) continue;
		return 0;
	}
	/* III.) Add vertex into simplex */
	gjk_vertex* vert = &s->v[s->cnt];
	vert->a = sup->a;
	vert->b = sup->b;
	vert->p = *dv;
	vert->aid = sup->aid;
	vert->bid = sup->bid;
	s->bc[s->cnt++] = 1.0f;

	/* IV.) Find closest simplex point */
	switch (s->cnt) 
	{
	case 1: break;
	case 2:
	{
		/* -------------------- Line ----------------------- */
		glm::vec3 a = s->v[0].p;
		glm::vec3 b = s->v[1].p;

		/* compute barycentric coordinates */
		glm::vec3 ab = a - b;
		glm::vec3 ba = b - a;

		float u = glm::dot(b, ba);
		float v = glm::dot(a, ab);
		if (v <= 0.0f)
		{
			/* region A */
			s->bc[0] = 1.0f;
			s->cnt = 1;
			break;
		}
		if (u <= 0.0f) 
		{
			/* region B */
			s->v[0] = s->v[1];
			s->bc[0] = 1.0f;
			s->cnt = 1;
			break;
		}
		/* region AB */
		s->bc[0] = u;
		s->bc[1] = v;
		s->cnt = 2;
	} break;
	case 3: 
	{
		/* -------------------- Triangle ----------------------- */
		glm::vec3 a = s->v[0].p;
		glm::vec3 b = s->v[1].p;
		glm::vec3 c = s->v[2].p;

		glm::vec3 ab = a - b;
		glm::vec3 ba = b - a;
		glm::vec3 bc = b - c;
		glm::vec3 cb = c - b;
		glm::vec3 ca = c - a;
		glm::vec3 ac = a - c;

		/* compute barycentric coordinates */
		float u_ab = glm::dot(b, ba);
		float v_ab = glm::dot(a, ab);

		float u_bc = glm::dot(c, cb);
		float v_bc = glm::dot(b, bc);

		float u_ca = glm::dot(a, ac);
		float v_ca = glm::dot(c, ca);

		if (v_ab <= 0.0f && u_ca <= 0.0f) 
		{
			/* region A */
			s->bc[0] = 1.0f;
			s->cnt = 1;
			break;
		}
		if (u_ab <= 0.0f && v_bc <= 0.0f) 
		{
			/* region B */
			s->v[0] = s->v[1];
			s->bc[0] = 1.0f;
			s->cnt = 1;
			break;
		}
		if (u_bc <= 0.0f && v_ca <= 0.0f) 
		{
			/* region C */
			s->v[0] = s->v[2];
			s->bc[0] = 1.0f;
			s->cnt = 1;
			break;
		}
		/* calculate fractional area */
		glm::vec3 n = glm::cross(ba, ca);
		glm::vec3 n1 = glm::cross(b, c);
		glm::vec3 n2 = glm::cross(c, a);
		glm::vec3 n3 = glm::cross(a, b);

		float u_abc = glm::dot(n1, n);
		float v_abc = glm::dot(n2, n);
		float w_abc = glm::dot(n3, n);

		if (u_ab > 0.0f && v_ab > 0.0f && w_abc <= 0.0f)
		{
			/* region AB */
			s->bc[0] = u_ab;
			s->bc[1] = v_ab;
			s->cnt = 2;
			break;
		}
		if (u_bc > 0.0f && v_bc > 0.0f && u_abc <= 0.0f) 
		{
			/* region BC */
			s->v[0] = s->v[1];
			s->v[1] = s->v[2];
			s->bc[0] = u_bc;
			s->bc[1] = v_bc;
			s->cnt = 2;
			break;
		}
		if (u_ca > 0.0f && v_ca > 0.0f && v_abc <= 0.0f) 
		{
			/* region CA */
			s->v[1] = s->v[0];
			s->v[0] = s->v[2];
			s->bc[0] = u_ca;
			s->bc[1] = v_ca;
			s->cnt = 2;
			break;
		}
		/* region ABC */
		assert(u_abc > 0.0f && v_abc > 0.0f && w_abc > 0.0f);
		s->bc[0] = u_abc;
		s->bc[1] = v_abc;
		s->bc[2] = w_abc;
		s->cnt = 3;
	} break;
	case 4:
	{
		/* -------------------- Tetrahedron ----------------------- */
		glm::vec3 a = s->v[0].p;
		glm::vec3 b = s->v[1].p;
		glm::vec3 c = s->v[2].p;
		glm::vec3 d = s->v[3].p;

		glm::vec3 ab = a - b;
		glm::vec3 ba = b - a;
		glm::vec3 bc = b - c;
		glm::vec3 cb = c - b;
		glm::vec3 ca = c - a;
		glm::vec3 ac = a - c;
		glm::vec3 db = d - b;
		glm::vec3 bd = b - d;
		glm::vec3 dc = d - c;
		glm::vec3 cd = c - d;
		glm::vec3 da = d - a;
		glm::vec3 ad = a - d;

		/* compute barycentric coordinates */
		float u_ab = glm::dot(b, ba);
		float v_ab = glm::dot(a, ab);

		float u_bc = glm::dot(c, cb);
		float v_bc = glm::dot(b, bc);

		float u_ca = glm::dot(a, ac);
		float v_ca = glm::dot(c, ca);

		float u_bd = glm::dot(d, db);
		float v_bd = glm::dot(b, bd);

		float u_dc = glm::dot(c, cd);
		float v_dc = glm::dot(d, dc);

		float u_ad = glm::dot(d, da);
		float v_ad = glm::dot(a, ad);

		/* check verticies for closest point */
		if (v_ab <= 0.0f && u_ca <= 0.0f && v_ad <= 0.0f) 
		{
			/* region A */
			s->bc[0] = 1.0f;
			s->cnt = 1;
			break;
		}
		if (u_ab <= 0.0f && v_bc <= 0.0f && v_bd <= 0.0f)
		{
			/* region B */
			s->v[0] = s->v[1];
			s->bc[0] = 1.0f;
			s->cnt = 1;
			break;
		}
		if (u_bc <= 0.0f && v_ca <= 0.0f && u_dc <= 0.0f) 
		{
			/* region C */
			s->v[0] = s->v[2];
			s->bc[0] = 1.0f;
			s->cnt = 1;
			break;
		}
		if (u_bd <= 0.0f && v_dc <= 0.0f && u_ad <= 0.0f)
		{
			/* region D */
			s->v[0] = s->v[3];
			s->bc[0] = 1.0f;
			s->cnt = 1;
			break;
		}
		/* calculate fractional area */
		glm::vec3 n = glm::cross(da, ba);
		glm::vec3 n1 = glm::cross(d, b);
		glm::vec3 n2 = glm::cross(b, a);
		glm::vec3 n3 = glm::cross(a, d);

		float u_adb = glm::dot(n1, n);
		float v_adb = glm::dot(n2, n);
		float w_adb = glm::dot(n3, n);

		n = glm::cross(ca, da);
		n1 = glm::cross(c, d);
		n2 = glm::cross(d, a);
		n3 = glm::cross(a, c);

		float u_acd = glm::dot(n1, n);
		float v_acd = glm::dot(n2, n);
		float w_acd = glm::dot(n3, n);

		n = glm::cross(bc, dc);
		n1 = glm::cross(b, d);
		n2 = glm::cross(d, c);
		n3 = glm::cross(c, b);

		float u_cbd = glm::dot(n1, n);
		float v_cbd = glm::dot(n2, n);
		float w_cbd = glm::dot(n3, n);

		n = glm::cross(ba, ca);
		n1 = glm::cross(b, c);
		n2 = glm::cross(c, a);
		n3 = glm::cross(a, b);

		float u_abc = glm::dot(n1, n);
		float v_abc = glm::dot(n2, n);
		float w_abc = glm::dot(n3, n);

		/* check edges for closest point */
		if (w_abc <= 0.0f && v_adb <= 0.0f && u_ab > 0.0f && v_ab > 0.0f)
		{
			/* region AB */
			s->bc[0] = u_ab;
			s->bc[1] = v_ab;
			s->cnt = 2;
			break;
		}
		if (u_abc <= 0.0f && w_cbd <= 0.0f && u_bc > 0.0f && v_bc > 0.0f)
		{
			/* region BC */
			s->v[0] = s->v[1];
			s->v[1] = s->v[2];
			s->bc[0] = u_bc;
			s->bc[1] = v_bc;
			s->cnt = 2;
			break;
		}
		if (v_abc <= 0.0f && w_acd <= 0.0f && u_ca > 0.0f && v_ca > 0.0f)
		{
			/* region CA */
			s->v[1] = s->v[0];
			s->v[0] = s->v[2];
			s->bc[0] = u_ca;
			s->bc[1] = v_ca;
			s->cnt = 2;
			break;
		}
		if (v_cbd <= 0.0f && u_acd <= 0.0f && u_dc > 0.0f && v_dc > 0.0f)
		{
			/* region DC */
			s->v[0] = s->v[3];
			s->v[1] = s->v[2];
			s->bc[0] = u_dc;
			s->bc[1] = v_dc;
			s->cnt = 2;
			break;
		}
		if (v_acd <= 0.0f && w_adb <= 0.0f && u_ad > 0.0f && v_ad > 0.0f)
		{
			/* region AD */
			s->v[1] = s->v[3];
			s->bc[0] = u_ad;
			s->bc[1] = v_ad;
			s->cnt = 2;
			break;
		}
		if (u_cbd <= 0.0f && u_adb <= 0.0f && u_bd > 0.0f && v_bd > 0.0f)
		{
			/* region BD */
			s->v[0] = s->v[1];
			s->v[1] = s->v[3];
			s->bc[0] = u_bd;
			s->bc[1] = v_bd;
			s->cnt = 2;
			break;
		}
		/* calculate fractional volume (volume can be negative!) */
		float denom = glm::dot(glm::cross(cb, ab), db);        // box3(cb, ab, db)
		float volume = (denom == 0) ? 1.0f : 1.0f / denom;
		float u_abcd = glm::dot(glm::cross(c, d), b) * volume; // box3(c, d, b)
		float v_abcd = glm::dot(glm::cross(c, a), d) * volume; // box3(c, a, d)
		float w_abcd = glm::dot(glm::cross(d, a), b) * volume; // box3(d, a, b)
		float x_abcd = glm::dot(glm::cross(b, a), c) * volume; // box3(b, a, c)

		/* check faces for closest point */
		if (x_abcd <= 0.0f && u_abc > 0.0f && v_abc > 0.0f && w_abc > 0.0f) 
		{
			/* region ABC */
			s->bc[0] = u_abc;
			s->bc[1] = v_abc;
			s->bc[2] = w_abc;
			s->cnt = 3;
			break;
		}
		if (u_abcd <= 0.0f && u_cbd > 0.0f && v_cbd > 0.0f && w_cbd > 0.0f) 
		{
			/* region CBD */
			s->v[0] = s->v[2];
			s->v[2] = s->v[3];
			s->bc[0] = u_cbd;
			s->bc[1] = v_cbd;
			s->bc[2] = w_cbd;
			s->cnt = 3;
			break;
		}
		if (v_abcd <= 0.0f && u_acd > 0.0f && v_acd > 0.0f && w_acd > 0.0f) 
		{
			/* region ACD */
			s->v[1] = s->v[2];
			s->v[2] = s->v[3];
			s->bc[0] = u_acd;
			s->bc[1] = v_acd;
			s->bc[2] = w_acd;
			s->cnt = 3;
			break;
		}
		if (w_abcd <= 0.0f && u_adb > 0.0f && v_adb > 0.0f && w_adb > 0.0f) 
		{
			/* region ADB */
			s->v[2] = s->v[1];
			s->v[1] = s->v[3];
			s->bc[0] = u_adb;
			s->bc[1] = v_adb;
			s->bc[2] = w_adb;
			s->cnt = 3;
			break;
		}
		/* region ABCD */
		// assert(u_abcd > 0.0f && v_abcd > 0.0f && w_abcd > 0.0f && x_abcd > 0.0f); // tcc+linux asserts in here: both u_abcd and v_abcd are negative
		s->bc[0] = u_abcd;
		s->bc[1] = v_abcd;
		s->bc[2] = w_abcd;
		s->bc[3] = x_abcd;
		s->cnt = 4;
	} break;
	}

	/* V.) Check if origin is enclosed by tetrahedron */
	if (s->cnt == 4)
	{
		s->hit = 1;
		return 0;
	}
	/* VI.) Ensure closing in on origin to prevent multi-step cycling */
	glm::vec3 pnt; float denom = 0;
	for (int i = 0; i < s->cnt; ++i)
		denom += s->bc[i];
	denom = 1.0f / denom;

	switch (s->cnt) 
	{
	case 1: pnt = s->v[0].p; break;
	case 2:
	{
		/* --------- Line -------- */
		glm::vec3 a = s->v[0].p * denom * s->bc[0];
		glm::vec3 b = s->v[1].p * denom * s->bc[1];
		pnt = a + b;
	}
	break;
	case 3: 
	{
		/* ------- Triangle ------ */
		glm::vec3 a = s->v[0].p * denom * s->bc[0];
		glm::vec3 b = s->v[1].p * denom * s->bc[1];
		glm::vec3 c = s->v[2].p * denom * s->bc[2];

		pnt = a + b;
		pnt = pnt + c;
	} 
	break;
	case 4:
	{
		/* ----- Tetrahedron ----- */
		glm::vec3 a = s->v[0].p * denom * s->bc[0];
		glm::vec3 b = s->v[1].p * denom * s->bc[1];
		glm::vec3 c = s->v[2].p * denom * s->bc[2];
		glm::vec3 d = s->v[3].p * denom * s->bc[3];

		pnt = a + b;
		pnt = pnt + c;
		pnt = pnt + d;
	} break;
	}

	float d2 = glm::dot(pnt, pnt);
	if (d2 >= s->D) return 0;
	s->D = d2;

	/* VII.) New search direction */
	switch (s->cnt) 
	{
	default: assert(0); break;
	case 1:
	{
		/* --------- Point -------- */
		*dv = -(s->v[0].p);
	} break;
	case 2: 
	{
		/* ------ Line segment ---- */
		glm::vec3 ba = s->v[1].p - s->v[0].p;
		glm::vec3 b0 = -(s->v[1].p);
		glm::vec3 t;  t = glm::cross(ba, b0);
		*dv = glm::cross(t, ba);
	} break;
	case 3: 
	{
		/* ------- Triangle ------- */
		glm::vec3 ab = s->v[1].p - s->v[0].p;
		glm::vec3 ac = s->v[2].p - s->v[0].p;
		glm::vec3 n;  n = glm::cross(ab, ac);
		if (glm::dot(n, s->v[0].p) <= 0.0f)
			*dv = n;
		else *dv = -n;
	}
	}
	if (glm::dot(*dv, *dv) < GJK_EPSILON * GJK_EPSILON)
		return 0;
	return 1;
}

inline gjk_result gjk_analyze(const gjk_simplex* s) 
{
	gjk_result r = { 0 }, * res = &r;

	res->iterations = s->iter;
	res->hit = s->hit;

	/* calculate normalization denominator */
	float denom = 0;
	for (int i = 0; i < s->cnt; ++i)
		denom += s->bc[i];
	denom = 1.0f / denom;

	/* compute closest points */
	switch (s->cnt) {
	default: assert(0); break;
	case 1: {
		/* Point */
		res->p0 = s->v[0].a;
		res->p1 = s->v[0].b;
	} break;
	case 2:
	{
		/* Line */
		float as = denom * s->bc[0];
		float bs = denom * s->bc[1];

		glm::vec3 a = s->v[0].a * as;
		glm::vec3 b = s->v[1].a * bs;
		glm::vec3 c = s->v[0].b * as;
		glm::vec3 d = s->v[1].b * bs;

		res->p0 = a + b;
		res->p1 = c + d;
	} break;
	case 3:
	{
		/* Triangle */
		float as = denom * s->bc[0];
		float bs = denom * s->bc[1];
		float cs = denom * s->bc[2];

		glm::vec3 a = s->v[0].a * as;
		glm::vec3 b = s->v[1].a * bs;
		glm::vec3 c = s->v[2].a * cs;

		glm::vec3 d = s->v[0].b * as;
		glm::vec3 e = s->v[1].b * bs;
		glm::vec3 f = s->v[2].b * cs;

		res->p0 = a + b;
		res->p0 = res->p0 + c;

		res->p1 = d + e;
		res->p1 = res->p1 + f;
	} break;
	case 4:
	{
		/* Tetrahedron */
		glm::vec3 a = s->v[0].a * denom * s->bc[0];
		glm::vec3 b = s->v[1].a * denom * s->bc[1];
		glm::vec3 c = s->v[2].a * denom * s->bc[2];
		glm::vec3 d = s->v[3].a * denom * s->bc[3];

		res->p0 = a + b;
		res->p0 = res->p0 + c;
		res->p0 = res->p0 + d;
		res->p1 = res->p0;
	} break;
	}

	if (!res->hit)
	{
		/* compute distance */
		glm::vec3 d = res->p1 - res->p0;
		res->distance_squared = glm::dot(d, d);
	}
	else res->distance_squared = 0;
	return r;
}

inline gjk_result gjk_quad(float a_radius, float b_radius) 
{
	gjk_result r = { 0 }, * res = &r;
	float radius = a_radius + b_radius;
	float radius_squared = radius * radius;
	if (res->distance_squared > GJK_EPSILON &&
		res->distance_squared > radius_squared) 
	{
		res->distance_squared -= radius_squared;

		/* calculate normal */
		glm::vec3 n = res->p1 - res->p0;
		float l2 = glm::dot(n, n);
		if (l2 != 0.0f)
		{
			float il = gjk_inv_sqrt(l2);
			n = n * il;
		}
		glm::vec3 da = n * a_radius;
		glm::vec3 db = n * b_radius;

		/* calculate new collision points */
		res->p0 = res->p0 + da;
		res->p1 = res->p1 - db;
	}
	else 
	{
		glm::vec3 p = res->p0 + res->p1;
		res->p0 = p * 0.5f;
		res->p1 = res->p0;
		res->distance_squared = 0;
		res->hit = 1;
	}
	return r;
}