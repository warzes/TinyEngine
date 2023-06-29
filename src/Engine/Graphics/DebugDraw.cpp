#include "stdafx.h"
#include "DebugDraw.h"
#include "RenderAPI/RenderSystem.h"
//-----------------------------------------------------------------------------
namespace {
	ShaderProgramRef shaderProgram;
	Uniform uniformProjectionMatrix;
	Uniform uniformColor;
	GPUBufferRef vb;
	VertexArrayRef vao;
	std::map<unsigned, std::vector<glm::vec3>> Points;
	std::map<unsigned, std::vector<glm::vec3>> Lines;
}
//-----------------------------------------------------------------------------
// TODO: можно оптимизировать, если хранить цвет в вершине, тогда не нужно использовать мап, можно использовать массив который только растет (а сбрасывается только счетчик). но займет больше памяти. хотя и н сильно
//-----------------------------------------------------------------------------
void drawGround(float scale)
{ // 10x10
	// outer
	for (float i = -scale, c = 0.0f; c <= 20.0f; c += 20.0f, i += c * (scale / 10.0f))
	{
		DebugDraw::DrawLine({ -scale, 0.0f, i }, { scale, 0.0f, i }, WHITE); // horiz
		DebugDraw::DrawLine({ i, 0.0f, -scale }, { i, 0.0f, +scale }, WHITE); // vert
	}
	// inner
	for (float i = -scale + scale / 10.0f, c = 1.0f; c < 20.0f; ++c, i += (scale / 10.0f))
	{
		DebugDraw::DrawLine({ -scale, 0.0f, i }, { +scale, 0.0f, i }, GRAY); // horiz
		DebugDraw::DrawLine({ i, 0.0f, -scale }, { i, 0.0f, +scale }, GRAY); // vert
	}
}
//-----------------------------------------------------------------------------
void DrawConeLowres(const glm::vec3& center, const glm::vec3& top, float radius, unsigned rgb)
{
	const glm::vec3 diff3 = top - center;
	DebugDraw::DrawPrism(center, radius ? radius : 1.0f, glm::length(diff3), glm::normalize(diff3), 3, rgb);
}
//-----------------------------------------------------------------------------
void DrawCircleWithOrientation(const glm::vec3& center, glm::vec3 dir, float radius, unsigned rgb)
{
	// we'll skip 3 segments out of 32. 1.5 per half circle.
	int segments = 32, skip = 3, drawn_segments = segments - skip;

	//  dir = norm3(dir);
	glm::vec3 right = glm::cross(dir, { 0.0f, 1.0f, 0.0f });
	glm::vec3 up = glm::cross(dir, right);
	right = glm::cross(dir, up);

	glm::vec3 point, lastPoint;
	dir = dir * radius;
	right = right * radius;
	//lastPoint = (center + dir);

	{
		const float radians = (glm::pi<float>() * 2.0f) * (0.0f + (float)skip / 2.0f) / (float)segments;
		glm::vec3 vs = right * sinf(radians);
		glm::vec3 vc = dir * cosf(radians);
		lastPoint = center + vs;
		lastPoint = lastPoint + vc;
	}

	DebugDraw::DrawLine(lastPoint, (center + (dir * radius * 1.25f)), rgb);

	glm::vec3 vs, vc;
	for (int i = 0; i <= drawn_segments; ++i)
	{
		const float radians = (glm::pi<float>() * 2.0f) * ((float)i + (float)skip / 2.0f) / (float)segments;

		vs = (right * sinf(radians));
		vc = (dir * cosf(radians));

		point = (center + vs);
		point = (point + vc);

		DebugDraw::DrawLine(lastPoint, point, rgb);
		lastPoint = point;
	}

	DebugDraw::DrawLine(lastPoint, (center + (dir * radius * 1.25f)), rgb);
}
//-----------------------------------------------------------------------------

inline void orthoVec(glm::vec3* left, glm::vec3* up, glm::vec3 v)
{
	* left = (v.z * v.z) < (v.x * v.x) ? glm::vec3(v.y, -v.x, 0.0f) : glm::vec3(0.0f, -v.z, v.y);
	*up = glm::cross(*left, v);
}
//-----------------------------------------------------------------------------
void DebugDraw::DrawPoint(const glm::vec3& from, unsigned rgb)
{
	Points[rgb].push_back(from);
}
//-----------------------------------------------------------------------------
void DebugDraw::DrawLine(const glm::vec3& from, const glm::vec3& to, unsigned rgb)
{
	Lines[rgb].push_back(from);
	Lines[rgb].push_back(to);
}
//-----------------------------------------------------------------------------
void DebugDraw::DrawLineDashed(glm::vec3 from, glm::vec3 to, unsigned rgb)
{
	const glm::vec3 dist = (to - from);
	const glm::vec3 unit = glm::normalize(dist);
	for (float len = 0.0f, mag = glm::length(dist) / 2.0f; len < mag; ++len)
	{
		to = (from + unit);
		DrawLine(from, to, rgb);
		from = (to + unit);
	}
}
//-----------------------------------------------------------------------------
void DebugDraw::DrawAxis(float units)
{
	DrawLine({ 0.0f, 0.0f, 0.0f }, { units, 0.0f, 0.0f }, RED);
	DrawLineDashed({ 0.0f, 0.0f, 0.0f }, { -units, 0.0f, 0.0f }, RED);

	DrawLine({ 0.0f, 0.0f, 0.0f }, { 0.0f, units, 0.0f }, GREEN);
	DrawLineDashed({ 0.0f, 0.0f, 0.0f }, { 0.0f, -units, 0.0f }, GREEN);

	DrawLine({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, units }, BLUE);
	DrawLineDashed({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, -units }, BLUE);
}
//-----------------------------------------------------------------------------
void DebugDraw::DrawGround(float scale)
{
	if (scale)
	{
		drawGround(scale);
	}
	else
	{
		drawGround(100.0f);
		drawGround(10.0f);
		drawGround(1.0f);
		drawGround(0.1f);
		drawGround(0.01f);
	}
}
//-----------------------------------------------------------------------------
void DebugDraw::DrawGrid(float scale)
{
	DrawGround(scale);
	DrawAxis(scale ? scale : 100.0f);
}
//-----------------------------------------------------------------------------
void DebugDraw::DrawTriangle(const Triangle& tri, unsigned rgb)
{
	DrawTriangle(tri.p0, tri.p1, tri.p2, rgb);
}
//-----------------------------------------------------------------------------
void DebugDraw::DrawTriangle(const glm::vec3& pa, const glm::vec3& pb, const glm::vec3& pc, unsigned rgb)
{
	DrawLine(pa, pb, rgb);
	DrawLine(pa, pc, rgb);
	DrawLine(pb, pc, rgb);
}
//-----------------------------------------------------------------------------
void DebugDraw::DrawArrow(const glm::vec3& begin, const glm::vec3& end, unsigned rgb)
{
	const glm::vec3 diff = (end - begin);
	const float len = glm::length(diff);
	const float stickLen = len * 2.0f / 3.0f;

	DrawLine(begin, end, rgb);
	DrawConeLowres((begin + (glm::normalize(diff) * stickLen)), end, len / 6.0f, rgb);
}
//-----------------------------------------------------------------------------
void DebugDraw::DrawBounds(const glm::vec3 points[8], unsigned rgb)
{
	for (unsigned i = 0; i < 4; ++i)
	{
		DrawLine(points[i], points[(i + 1) & 3], rgb);
		DrawLine(points[i], points[4 + i], rgb);
		DrawLine(points[4 + i], points[4 + ((i + 1) & 3)], rgb);
	}
}
//-----------------------------------------------------------------------------
void DebugDraw::DrawBox(const glm::vec3& c, const glm::vec3& extents, unsigned rgb)
{
	glm::vec3 points[8], whd = (extents * 0.5f);
#define DD_BOX_V(v, op1, op2, op3) (v).x = c.x op1 whd.x; (v).y = c.y op2 whd.y; (v).z = c.z op3 whd.z
	DD_BOX_V(points[0], -, +, +);
	DD_BOX_V(points[1], -, +, -);
	DD_BOX_V(points[2], +, +, -);
	DD_BOX_V(points[3], +, +, +);
	DD_BOX_V(points[4], -, -, +);
	DD_BOX_V(points[5], -, -, -);
	DD_BOX_V(points[6], +, -, -);
	DD_BOX_V(points[7], +, -, +);
#undef DD_BOX_V
	DrawBounds(points, rgb);
}
//-----------------------------------------------------------------------------
void DebugDraw::DrawCube(const glm::vec3& center, float radius, unsigned rgb)
{
	// draw_prism(center, 1, -1, vec3(0,1,0), 4);
	const float half = radius * 0.5f;
	const glm::vec3 l = { center.x - half, center.y + half, center.z - half }; // left-top-far
	const glm::vec3 r = { center.x + half, center.y - half, center.z + half }; // right-bottom-near

	DrawLine(l, { r.x, l.y, l.z }, rgb);
	DrawLine({ r.x, l.y, l.z }, { r.x, l.y, r.z }, rgb);
	DrawLine({ r.x, l.y, r.z }, { l.x, l.y, r.z }, rgb);
	DrawLine({ l.x, l.y, r.z }, l, rgb);
	DrawLine(l, { l.x, r.y, l.z }, rgb);

	DrawLine(r, { l.x, r.y, r.z }, rgb);
	DrawLine({ l.x, r.y, r.z }, { l.x, r.y, l.z }, rgb);
	DrawLine({ l.x, r.y, l.z }, { r.x, r.y, l.z }, rgb);
	DrawLine({ r.x, r.y, l.z }, r, rgb);
	DrawLine(r, { r.x, l.y, r.z }, rgb);

	DrawLine({ l.x, l.y, r.z }, { l.x, r.y, r.z }, rgb);
	DrawLine({ r.x, l.y, l.z }, { r.x, r.y, l.z }, rgb);
}
//-----------------------------------------------------------------------------
glm::vec3 transform33(const glm::mat3& mat, const glm::vec3& p) // TODO: ==> glm
{
	const float* m = glm::value_ptr(mat); // TODO: проверить
	float x = (m[0] * p.x) + (m[4] * p.y) + (m[8] * p.z);
	float y = (m[1] * p.x) + (m[5] * p.y) + (m[9] * p.z);
	float z = (m[2] * p.x) + (m[6] * p.y) + (m[10] * p.z);
	return glm::vec3(x, y, z);
}
//-----------------------------------------------------------------------------
void DebugDraw::DrawCube(const glm::vec3& center, const glm::vec3& radius, const glm::mat3& M, unsigned rgb)
{
	const glm::vec3 half = radius * 0.5f;
	const glm::vec3 l = { -half.x, +half.y, -half.z }; // left-top-far
	const glm::vec3 r = { +half.x, -half.y, +half.z }; // right-bottom-near

	glm::vec3 points[8] = 
	{
		{l.x, r.y, r.z},
		{l.x, r.y, l.z},
		{r.x, r.y, l.z},
		{r.x, r.y, r.z},
		{l.x, l.y, r.z},
		{l.x, l.y, l.z},
		{r.x, l.y, l.z},
		{r.x, l.y, r.z},
	};

	for (unsigned i = 0; i < 8; ++i)
		points[i] = (center + transform33(M, points[i]));

	DrawBounds(points, rgb);
}
//-----------------------------------------------------------------------------
void DebugDraw::DrawPlane(const glm::vec3& p, const glm::vec3& n, float scale, unsigned rgb)
{
	// if n is too similar to up vector, use right. else use up vector
	glm::vec3 v1 = glm::cross(n, glm::dot(n, { 0.0f, 1.0f, 0.0f }) > 0.8f ? glm::vec3(1.0f, 0.0f, 0.0f) : glm::vec3(0.0f, 1.0f, 0.0f));
	glm::vec3 v2 = glm::cross(n, v1);

	// draw axis
	DrawLine(p, (p + n), rgb);
	DrawLine(p, (p + v1), rgb);
	DrawLine(p, (p + v2), rgb);

	// get plane coords
	v1 = (v1 * scale);
	v2 = (v2 * scale);
	const glm::vec3 p1 = ((p + v1) + v2);
	const glm::vec3 p2 = ((p - v1) + v2);
	const glm::vec3 p3 = ((p - v1) - v2);
	const glm::vec3 p4 = ((p + v1) - v2);

	// draw plane
	DrawLine(p1, p2, rgb);
	DrawLine(p2, p3, rgb);
	DrawLine(p3, p4, rgb);
	DrawLine(p4, p1, rgb);
}
//-----------------------------------------------------------------------------
void DebugDraw::DrawSphere(const glm::vec3& center, float radius, unsigned rgb)
{
#if 0
	float lod = 6.0f, yp = -radius, rp = 0.0f, y, r, x, z;
#else // new
	float lod = 8.0f, yp = -radius, rp = 0.0f, y, r, x, z;
#endif
	for (int j = 1; j <= (int)(lod / 2.0f); ++j, yp = y, rp = r)
	{
		y = (float)j * 2.0f / (lod / 2.0f) - 1;
		r = cosf(y * 3.14159f / 2.0f) * radius;
		y = sinf(y * 3.14159f / 2.0f) * radius;

		float xp = 1.0f, zp = 0.0f;
		for (int i = 1; i <= (int)lod; ++i, xp = x, zp = z)
		{
			x = 3.14159f * 2.0f * (float)i / lod;
			z = sinf(x);
			x = cosf(x);

			glm::vec3 a1 = (center + glm::vec3(xp * rp, yp, zp * rp));
			glm::vec3 b1 = (center + glm::vec3(xp * r, y, zp * r));
			glm::vec3 c1 = (center + glm::vec3(x * r, y, z * r));

			DrawLine(a1, b1, rgb);
			DrawLine(b1, c1, rgb);
			DrawLine(c1, a1, rgb);

			glm::vec3 a2 = (center + glm::vec3(xp * rp, yp, zp * rp));
			glm::vec3 b2 = (center + glm::vec3(x * r, y, z * r));
			glm::vec3 c2 = (center + glm::vec3(x * rp, yp, z * rp));

			DrawLine(a2, b2, rgb);
			DrawLine(b2, c2, rgb);
			DrawLine(c2, a2, rgb);
		}
	}
}
//-----------------------------------------------------------------------------
void DebugDraw::DrawCapsule(const glm::vec3& from, const glm::vec3& to, float r, unsigned rgb)
{
	/* calculate axis */
	glm::vec3 up, right, forward;
	forward = (to - from);
	forward = glm::normalize(forward);
	orthoVec(&right, &up, forward);

	/* calculate first two cone verts (buttom + top) */
	glm::vec3 lastf, lastt;
	lastf = (up * r);
	lastt = (to + lastf);
	lastf = (from + lastf);

	/* step along circle outline and draw lines */
	const int step_size = 20;
	for (int i = step_size; i <= 360; i += step_size)
	{
		/* calculate current rotation */
		glm::vec3 ax = (right * sinf(glm::radians((float)i)));
		glm::vec3 ay = (up * cosf(glm::radians((float)i)));

		/* calculate current vertices on cone */
		glm::vec3 tmp = (ax + ay);
		glm::vec3 pf = (tmp * r);
		glm::vec3 pt = (tmp * r);

		pf = (pf + from);
		pt = (pt + to);

		/* draw cone vertices */
		DrawLine(lastf, pf, rgb);
		DrawLine(lastt, pt, rgb);
		DrawLine(pf, pt, rgb);

		lastf = pf;
		lastt = pt;

		/* calculate first top sphere vert */
		glm::vec3 prevt = (tmp * r);
		glm::vec3 prevf = (prevt + from);
		prevt = (prevt + to);

		/* sphere (two half spheres )*/
		for (int j = 1; j < 180 / step_size; j++)
		{
			/* angles */
			float ta = (float)j * step_size;
			float fa = 360.0f - ((float)j * step_size);

			/* top half-sphere */
			ax = (forward * sinf(glm::radians(ta)));
			ay = (tmp * cosf(glm::radians(ta)));

			glm::vec3 t = (ax + ay);
			pf = (t * r);
			pf = (pf + to);
			DrawLine(pf, prevt, rgb);
			prevt = pf;

			/* bottom half-sphere */
			ax = (forward * sinf(glm::radians(fa)));
			ay = (tmp * cosf(glm::radians(fa)));

			t = (ax + ay);
			pf = (t * r);
			pf = (pf + from);
			DrawLine(pf, prevf, rgb);
			prevf = pf;
		}
	}
}
//-----------------------------------------------------------------------------
void DebugDraw::DrawDiamond(const glm::vec3& from, const glm::vec3& to, float size, unsigned rgb)
{

	class poly // TODO: удалить?
	{
	public:
		std::vector<glm::vec3> verts;
		int cnt = 0;

		static poly Pyramid(const glm::vec3& from, const glm::vec3& to, float size)
		{
			/* calculate axis */
			glm::vec3 up, right, forward = glm::normalize(to - from);
			orthoVec(&right, &up, forward);

			/* calculate extend */
			glm::vec3 xext = (right * size);
			glm::vec3 yext = (up * size);
			glm::vec3 nxext = (right * -size);
			glm::vec3 nyext = (up * -size);

			/* calculate base vertices */
			poly p;
			p.verts = {
				((from + xext) + yext), /*a*/
				((from + xext) + nyext), /*b*/
				((from + nxext) + nyext), /*c*/
				((from + nxext) + yext), /*d*/
				to, /*r*/
				{} // empty
			};
			p.cnt = 5; /*+1 for diamond case*/ // array_resize(p.verts, 5+1); p.cnt = 5;

			return p;
		}

		static poly Diamond(const glm::vec3& from, const glm::vec3& to, float size)
		{
			glm::vec3 mid = (from + ((to - from) * 0.5f));
			poly p = Pyramid(mid, to, size);
			p.verts[5] = from; p.cnt = 6;
			return p;
		}
	};


	poly p = poly::Diamond(from, to, size);
	glm::vec3* dmd = p.verts.data();

	glm::vec3* a = dmd + 0;
	glm::vec3* b = dmd + 1;
	glm::vec3* c = dmd + 2;
	glm::vec3* d = dmd + 3;
	glm::vec3* t = dmd + 4;
	glm::vec3* f = dmd + 5;

	/* draw vertices */
	DrawLine(*a, *b, rgb);
	DrawLine(*b, *c, rgb);
	DrawLine(*c, *d, rgb);
	DrawLine(*d, *a, rgb);

	/* draw roof */
	DrawLine(*a, *t, rgb);
	DrawLine(*b, *t, rgb);
	DrawLine(*c, *t, rgb);
	DrawLine(*d, *t, rgb);

	/* draw floor */
	DrawLine(*a, *f, rgb);
	DrawLine(*b, *f, rgb);
	DrawLine(*c, *f, rgb);
	DrawLine(*d, *f, rgb);
}
//-----------------------------------------------------------------------------
void DebugDraw::DrawPyramid(const glm::vec3& center, float height, int segments, unsigned rgb)
{
	DrawPrism(center, 1, height, glm::vec3(0, 1, 0), segments, rgb);
}
//-----------------------------------------------------------------------------
void DebugDraw::DrawPrism(const glm::vec3& center, float radius, float height, const glm::vec3& normal, int segments, unsigned rgb)
{
	glm::vec3 left = glm::vec3{ 0 }, up = glm::vec3{ 0 };
	orthoVec(&left, &up, normal);

	glm::vec3 point, lastPoint;
	up = (up * radius);
	left = (left * radius);
	lastPoint = (center + up);
	glm::vec3 pivot = (center + (normal * height));

	for (int i = 1; i <= segments; ++i)
	{
		const float radians = (glm::pi<float>() * 2) * (float)(i / segments);

		glm::vec3 vs = (left * sinf(radians));
		glm::vec3 vc = (up * cosf(radians));

		point = (center + vs);
		point = (point + vc);

		DrawLine(lastPoint, point, rgb);
		if (height > 0)
			DrawLine(point, pivot, rgb);
		else if (height < 0)
		{
			DrawLine(point, (point + (normal * -height)), rgb);
		}
		lastPoint = point;
	}

	if (height < 0)
		DrawPrism((center + (normal * -height)), radius, 0, normal, segments, rgb);
}
//-----------------------------------------------------------------------------
void DebugDraw::DrawSquare(const glm::vec3& pos, float radius, unsigned rgb)
{
	DrawPrism(pos, radius, 0, glm::vec3(0, 1, 0), 4, rgb);
}
//-----------------------------------------------------------------------------
void DebugDraw::DrawCylinder(const glm::vec3& center, float height, int segments, unsigned rgb)
{
	DrawPrism(center, 1, -height, glm::vec3(0, 1, 0), segments, rgb);
}
//-----------------------------------------------------------------------------
void DebugDraw::DrawPentagon(const glm::vec3& pos, float radius, unsigned rgb)
{
	DrawPrism(pos, radius, 0, glm::vec3(0, 1, 0), 5, rgb);
}
//-----------------------------------------------------------------------------
void DebugDraw::DrawHexagon(const glm::vec3& pos, float radius, unsigned rgb)
{
	DrawPrism(pos, radius, 0, glm::vec3(0, 1, 0), 6, rgb);
}
//-----------------------------------------------------------------------------
void DebugDraw::DrawCone(const glm::vec3& center, const glm::vec3& top, float radius, unsigned rgb)
{
	glm::vec3 diff3 = (top - center);
	DrawPrism(center, radius ? radius : 1, glm::length(diff3), glm::normalize(diff3), 24, rgb);
}
//-----------------------------------------------------------------------------
void DebugDraw::DrawCircle(const glm::vec3& pos, const glm::vec3& n, float radius, unsigned rgb)
{
	DrawPrism(pos, radius, 0, n, 32, rgb);
}
//-----------------------------------------------------------------------------
void DebugDraw::DrawRing(const glm::vec3& pos, const glm::vec3& n, float radius, unsigned rgb)
{
	DrawCircle(pos, n, radius, rgb);
	DrawCircle(pos, n, radius * 0.9f, rgb);
}
//-----------------------------------------------------------------------------
void DebugDraw::DrawAABB(const glm::vec3& minbb, const glm::vec3& maxbb, unsigned rgb)
{
	glm::vec3 points[8], bb[2] = { minbb, maxbb };
	for (int i = 0; i < 8; ++i)
	{
		points[i].x = bb[(i ^ (i >> 1)) & 1].x;
		points[i].y = bb[     (i >> 1)  & 1].y;
		points[i].z = bb[     (i >> 2)  & 1].z;
	}
	DrawBounds/*_corners*/(points, rgb);
}
//-----------------------------------------------------------------------------
void DebugDraw::DrawPosition(const glm::vec3& pos, float radius)
{
	DrawPositionDir(pos, glm::vec3(0, 0, 0), radius);
}
//-----------------------------------------------------------------------------
void DebugDraw::DrawPositionDir(const glm::vec3& position, const glm::vec3& direction, float radius)
{
	// idea from http://www.cs.caltech.edu/~keenan/m3drv.pdf and flotilla game UI

	glm::vec3 ground = glm::vec3(position.x, 0, position.z);
	unsigned clr = position.y < 0 ? PINK/*ORANGE*/ : CYAN;

	DrawPoint(ground, clr);
	DrawPoint(position, clr);
	(position.y < 0 ? DrawLineDashed(ground, position, clr) : DrawLine(ground, position, clr));

	glm::vec3 n = glm::normalize(direction), up = glm::vec3(0, 1, 0);
	for (int i = 0; i < 10 && i <= (int)fabs(position.y); ++i)
	{
		if (i < 2 && glm::length(direction))
			DrawCircleWithOrientation(ground, n, radius, clr);
		else
			DrawCircle(ground, up, radius, clr);
		radius *= 0.9f;
	}
}
//-----------------------------------------------------------------------------
void DebugDraw::DrawNormal(const glm::vec3& pos, const glm::vec3& n)
{
	DrawLine(pos, (pos + glm::normalize(n)), YELLOW);
}
//-----------------------------------------------------------------------------
void DebugDraw::DrawBone(const glm::vec3& center, const glm::vec3& end, unsigned rgb)
{
	glm::vec3 diff3 = (end - center);
	float len = glm::length(diff3), len10 = len / 10;
	DrawPrism(center, len10, 0, glm::vec3(1, 0, 0), 24, rgb);
	DrawPrism(center, len10, 0, glm::vec3(0, 1, 0), 24, rgb);
	DrawPrism(center, len10, 0, glm::vec3(0, 0, 1), 24, rgb);
	DrawLine(end, (center + glm::vec3(0, +len10, 0)), rgb);
	DrawLine(end, (center + glm::vec3(0, -len10, 0)), rgb);
}
//-----------------------------------------------------------------------------
void DebugDraw::DrawBoid(const glm::vec3& position, glm::vec3 dir)
{
	dir = glm::normalize(dir);

	// if n is too similar to up vector, use right. else use up vector
	glm::vec3 v1 = glm::cross(dir, glm::dot(dir, glm::vec3(0, 1, 0)) > 0.8f ? glm::vec3(1, 0, 0) : glm::vec3(0, 1, 0));
	glm::vec3 v2 = glm::cross(dir, v1);
	v1 = glm::cross(dir, v2);

	uint32_t clr = position.y < 0 ? ORANGE : CYAN;

	glm::vec3 front = (position + (dir * 1.0f));
	glm::vec3 back = (position + (dir * -0.25f));
	glm::vec3 right = (back + (v1 * 0.5f));
	glm::vec3 left = (back + (v1 * -0.5f));
	DrawLine(front, left, clr);
	DrawLine(left, position, clr);
	DrawLine(position, right, clr);
	DrawLine(right, front, clr);
}
//-----------------------------------------------------------------------------
void DebugDraw::DrawFrustum(const glm::mat4& projview, unsigned rgb)
{
	glm::mat4 clipmatrix = glm::inverse(projview);// clip matrix

	// Start with the standard clip volume, then bring it back to world space.
	const glm::vec3 planes[8] = {
		{-1,-1,-1}, {+1,-1,-1}, {+1,+1,-1}, {-1,+1,-1}, // near plane
		{-1,-1,+1}, {+1,-1,+1}, {+1,+1,+1}, {-1,+1,+1}, // far plane
	};

	glm::vec3 points[8];
	float wCoords[8];

	// Transform the planes by the inverse clip matrix:
	for (int i = 0; i < 8; ++i)
	{
		// wCoords[i] = matTransformPointXYZW2(&points[i], planes[i], clipmatrix);
		glm::vec3* out = &points[i], in = planes[i]; 
		const float* m = glm::value_ptr(clipmatrix);
		out->x = (m[0] * in.x) + (m[4] * in.y) + (m[8] * in.z) + m[12]; // in.w (vec4) assumed to be 1
		out->y = (m[1] * in.x) + (m[5] * in.y) + (m[9] * in.z) + m[13];
		out->z = (m[2] * in.x) + (m[6] * in.y) + (m[10] * in.z) + m[14];
		wCoords[i] = (m[3] * in.x) + (m[7] * in.y) + (m[11] * in.z) + m[15]; // rw

		// bail if any W ended up as zero.
		const float epsilon = 1e-9f;
		if (glm::abs(wCoords[i]) < epsilon) 
		{
			return;
		}
	}

	// Divide by the W component of each:
	for (int i = 0; i < 8; ++i) 
	{
		points[i].x /= wCoords[i];
		points[i].y /= wCoords[i];
		points[i].z /= wCoords[i];
	}

	// Connect the dots:
	DrawBounds(points, rgb);
}
//-----------------------------------------------------------------------------
void DebugDraw::Flush(const glm::mat4& ViewProj)
{
	if (Points.empty() && Lines.empty()) return;

	auto& renderSystem = GetRenderSystem();

	renderSystem.Bind(shaderProgram);
	renderSystem.SetUniform(uniformProjectionMatrix, ViewProj);
	
	//glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LEQUAL);
	//glEnable(GL_PROGRAM_POINT_SIZE); // for GL_POINTS
	//glEnable(GL_LINE_SMOOTH); // for GL_LINES (thin)

	// Draw Points
	{
		//glPointSize(6);
		for (auto& it : Points)
		{
			renderSystem.SetUniform(uniformColor, RGBToVec(it.first));
			const size_t count = it.second.size();

			renderSystem.UpdateBuffer(vb, 0, (unsigned)count, (unsigned)sizeof(glm::vec3), it.second.data());
			renderSystem.Draw(vao, PrimitiveTopology::Points);
		}
		//glPointSize(1);
	}

	// Draw Lines
	{
		for (auto& it : Lines)
		{
			renderSystem.SetUniform(uniformColor, RGBToVec(it.first));
			const size_t count = it.second.size();
			renderSystem.UpdateBuffer(vb, 0, (unsigned)count, (unsigned)sizeof(glm::vec3), it.second.data());
			renderSystem.Draw(vao, PrimitiveTopology::Lines);
		}
	}

	//glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LESS);
	//glDisable(GL_LINE_SMOOTH);
	//glDisable(GL_PROGRAM_POINT_SIZE);

	Points.clear(); // TODO: надо по другому - без реальной очистки памяти
	Lines.clear(); // TODO: надо по другому - без реальной очистки памяти
}
//-----------------------------------------------------------------------------
bool DebugDraw::Init()
{
	const char* vertexSource = R"(
#version 330 core
layout(location = 0) in vec3 aPosition;
uniform mat4 uMVP;
uniform vec3 uColor;
out vec3 fColor;
void main()
{
	gl_Position =  uMVP * vec4(aPosition, 1);
	fColor = uColor;
}
)";

	const char* fragmentSource = R"(
#version 330 core
in vec3 fColor;
out vec4 fragcolor;
void main()
{
	fragcolor = vec4(fColor, 1.0);
}
)";
	auto& renderSystem = GetRenderSystem();
	shaderProgram = renderSystem.CreateShaderProgram({ vertexSource }, { fragmentSource });
	uniformProjectionMatrix = renderSystem.GetUniform(shaderProgram, "uMVP");
	uniformColor = renderSystem.GetUniform(shaderProgram, "uColor");

	vb = renderSystem.CreateVertexBuffer(BufferUsage::DynamicDraw, 1, (unsigned)sizeof(glm::vec3), nullptr);
	vao = renderSystem.CreateVertexArray(vb, nullptr, shaderProgram);

	return true;
}
//-----------------------------------------------------------------------------
void DebugDraw::Close()
{
	shaderProgram.reset();
	vb.reset();
	vao.reset();
	Points.clear();
	Lines.clear();
}
//-----------------------------------------------------------------------------
void DebugDraw::DrawDemo()
{
	// freeze current frame for (frustum) camera forensics
	//static mat44 projview_copy;
	//do_once{
	//	multiply44x2(projview_copy, camera_get_active()->proj, camera_get_active()->view);
	//}
	//ddraw_frustum(projview_copy);

	//DrawGrid();

	glm::vec3 origin = { 0,0,0 };
	DrawArrow(origin, glm::vec3(-1, 1, 1), ORANGE);

	for (int i = -5; i <= 5; ++i) 
	{
		DrawPyramid(glm::vec3(i * 2, 0, 3), 0, i + 5 + 2, YELLOW);
		DrawPyramid(glm::vec3(i * 2, 0, 6), -2, i + 5 + 2, YELLOW);
		DrawPyramid(glm::vec3(i * 2, 0, 9), +2, i + 5 + 2, YELLOW);
	}

#if 1 // @fixme: add positions to these
	// ddraw_triangle(origin, 1);
	DrawSquare(origin, 1, YELLOW);
	DrawPentagon(origin, 1, YELLOW);
	DrawHexagon(origin, 1, YELLOW);
	DrawCube(origin, 1, YELLOW);
	DrawPyramid(origin, 2, 3, YELLOW);
	DrawPyramid(origin, 2, 16, YELLOW);
	DrawCone(origin, (origin + glm::vec3(0, 1, 0)), 0.5f, YELLOW);
	DrawArrow(origin, glm::vec3(0, 1, 0), YELLOW);
	DrawBone(glm::vec3(0, 0, 0), glm::vec3(3, 3, 3), YELLOW);
	DrawAABB(glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), YELLOW);
#endif

	DrawPlane(glm::vec3(0, 10, 0), glm::vec3(0, 1, 0), 10, YELLOW);
	//DrawBoid(vec3(15,0,15), vec3(-15,0,-15) );
	DrawPosition(glm::vec3(10, 10, 10), 1);
	DrawPosition(glm::vec3(-10, -10, 10), 1);

	DrawPoint(glm::vec3(-2, 0, -2), YELLOW);
	DrawSphere(glm::vec3(-3, 0, -3), 1, PURPLE);
}
//-----------------------------------------------------------------------------