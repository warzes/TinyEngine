#pragma once

#include "Core/Geometry/GeometryShapes.h"
#include "Core/Math/Color.h"

// debugdraw framework
// Credits: Based on work by @glampert https://github.com/glampert/debug-draw (PD)
namespace DebugDraw
{
	void DrawPoint(const glm::vec3& from, unsigned rgb);
	void DrawLine(const glm::vec3& from, const glm::vec3& to, unsigned rgb);
	void DrawLineDashed(glm::vec3 from, glm::vec3 to, unsigned rgb);

	void DrawAxis(float units);
	void DrawGround(float scale);
	void DrawGrid(float scale);

	void DrawTriangle(const Triangle& tri, unsigned rgb);
	void DrawTriangle(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, unsigned rgb);

	void DrawArrow(const glm::vec3& begin, const glm::vec3& end, unsigned rgb);
	void DrawBounds(const glm::vec3 points[8], unsigned rgb); // TODO: передавать points по ссылке, сейчас оно копируется
	void DrawBox(const glm::vec3& center, const glm::vec3& extents, unsigned rgb);
	void DrawCube(const glm::vec3& center, float radius, unsigned rgb);
	void DrawCube(const glm::vec3& center, const glm::vec3& radius, const glm::mat3& M, unsigned rgb);
	void DrawPlane(const glm::vec3& p, const glm::vec3& n, float scale, unsigned rgb);
	void DrawSphere(const glm::vec3& pos, float radius, unsigned rgb);
	void DrawCapsule(const glm::vec3& from, const glm::vec3& to, float radius, unsigned rgb);

	void DrawDiamond(const glm::vec3& from, const glm::vec3& to, float size, unsigned rgb);
	void DrawPyramid(const glm::vec3& center, float height, int segments, unsigned rgb);
	void DrawPrism(const glm::vec3& center, float radius, float height, const glm::vec3& normal, int segments, unsigned rgb);
	void DrawSquare(const glm::vec3& pos, float radius, unsigned rgb);
	void DrawCylinder(const glm::vec3& center, float height, int segments, unsigned rgb);
	void DrawPentagon(const glm::vec3& pos, float radius, unsigned rgb);
	void DrawHexagon(const glm::vec3& pos, float radius, unsigned rgb);
	void DrawCone(const glm::vec3& center, const glm::vec3& top, float radius, unsigned rgb);
	void DrawCircle(const glm::vec3& pos, const glm::vec3& n, float radius, unsigned rgb);
	void DrawRing(const glm::vec3& pos, const glm::vec3& n, float radius, unsigned rgb);
	void DrawAABB(const glm::vec3& minbb, const glm::vec3& maxbb, unsigned rgb);

	void DrawPosition(const glm::vec3& pos, float radius);
	void DrawPositionDir(const glm::vec3& pos, const glm::vec3& dir, float radius);
	void DrawNormal(const glm::vec3& pos, const glm::vec3& n);
	void DrawBone(const glm::vec3& center, const glm::vec3& end, unsigned rgb);
	void DrawBoid(const glm::vec3& pos, glm::vec3 dir);

	void DrawFrustum(const glm::mat4& projview, unsigned rgb);

	void Flush(const glm::mat4& ViewProj);

	bool Init();
	void Close();

	void DrawDemo();
} // DebugDraw