namespace Intersect
{
	inline glm::vec3 Barycentric(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const glm::vec3& p)
	{
		const glm::vec3 v0 = b - a;
		const glm::vec3 v1 = c - a;
		const glm::vec3 v2 = p - a;

		const float d00 = glm::dot(v0, v0);
		const float d01 = glm::dot(v0, v1);
		const float d11 = glm::dot(v1, v1);
		const float d20 = glm::dot(v2, v0);
		const float d21 = glm::dot(v2, v1);

		const float denom = d00 * d11 - d01 * d01;
		const float denomRCP = 1.0f / denom;
		const float v = (d11 * d20 - d01 * d21) * denomRCP;
		const float w = (d00 * d21 - d01 * d20) * denomRCP;
		const float u = 1.0f - v - w;
		return glm::vec3(u, v, w);
	}

	inline bool TestPointTriangle(const glm::vec3& p, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c)
	{
		const glm::vec3 uvw = Barycentric(a, b, c, p);
		return { uvw.x >= 0.0f && uvw.y >= 0.0f && uvw.z >= 0.0f }; // TODO: проверить
	}

	inline bool TestPointTriangleCCW(const glm::vec3& p, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c)
	{
		// Translate the triangle so it lies in the origin p.
		const glm::vec3 pA = a - p;
		const glm::vec3 pB = b - p;
		const glm::vec3 pC = c - p;
		const float ab = glm::dot(pA, pB);
		const float ac = glm::dot(pA, pC);
		const float bc = glm::dot(pB, pC);
		const float cc = glm::dot(pC, pC);

		// Make sure plane normals for pab and pac point in the same direction.
		if (bc * ac - cc * ab < 0.0f)
			return false;

		const float bb = glm::dot(pB, pB);

		// Make sure plane normals for pab and pca point in the same direction.
		if (ab * bc - ac * bb < 0.0f)
			return false;

		// Otherwise p must be in the triangle.
		return true;
	}

	inline glm::vec3 ClosestPointTriangle(const glm::vec3& p, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c)
	{
		// Check if p in vertex region is outside a.
		const glm::vec3 ab = b - a;
		const glm::vec3 ac = c - a;
		const glm::vec3 ap = p - a;
		const float d1 = glm::dot(ab, ap);
		const float d2 = glm::dot(ac, ap);

		if (d1 <= 0.0f && d2 <= 0.0f)
			return a; // barycentric coordinates (1,0,0)

		// Check if p in vertex region is outside b.
		const glm::vec3 bp = p - b;
		const float d3 = glm::dot(ab, bp);
		const float d4 = glm::dot(ac, bp);
		if (d3 >= 0.0f && d4 <= d3)
			return b; // barycentric coordinates (0,1,0)

		// Check if p in edge region of ab, if so return projection of p onto ab.
		const float vc = d1 * d4 - d3 * d2;
		if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f)
		{
			const float v = d1 / (d1 - d3);
			return a + v * ab; // barycentric coordinates (1-v,v,0)
		}

		// Check if p in vertex region outside C.
		const glm::vec3 cp = p - c;
		const float d5 = glm::dot(ab, cp);
		const float d6 = glm::dot(ac, cp);
		if (d6 >= 0.0f && d5 <= d6)
			return c; // barycentric coordinates (0,0,1)

		// Check if P in edge region of ac, if so return projection of p onto ac.
		const float vb = d5 * d2 - d1 * d6;
		if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f)
		{
			const float w = d2 / (d2 - d6);
			return a + w * ac; // barycentric coordinates (1-w,0,w)
		}

		// Check if p in edge region of bc, if so return projection of p onto bc.
		const float va = d3 * d6 - d5 * d4;
		if (va <= 0.0f && d4 >= d3 && d5 >= d6)
		{
			const float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
			return b + w * (c - b);  // barycentric coordinates (0,1-w,w)
		}

		// p inside face region. Compute q trough its barycentric coordinates (u,v,w)
		const float denomRCP = 1.0f / (va + vb + vc);
		const float v = vb * denomRCP;
		const float w = vc * denomRCP;
		return a + ab * v + ac * w; // = u*a + v*b + w*c, u = va * denom = 1.0 - v - w
	}

	inline bool PointSphere(const glm::vec3& centerPosition, float radiusSquared, const glm::vec3& testPoint)
	{
		const glm::vec3 distance = testPoint - centerPosition;
		const float lengthSq = glm::dot(distance, distance);
		return lengthSq < radiusSquared;
	}

	inline bool PointCylinder(const glm::vec3& baseCenterPoint, const glm::vec3& axisVector, float axisLengthSquared, float radiusSquared, const glm::vec3& testPoint)
	{
		// If the cylinder shape has no volume then the point cannot be inside.
		if (axisLengthSquared <= 0.0f || radiusSquared <= 0.0f)
			return false;

		const glm::vec3 baseCenterPointToTestPoint = testPoint - baseCenterPoint;
		const float dotProduct = glm::dot(baseCenterPointToTestPoint, axisVector);

		// If the dot is < 0, the point is below the base cap of the cylinder, if it's > lengthSquared then it's beyond the other cap.
		if (dotProduct < 0.0f || dotProduct > axisLengthSquared)
			return false;
		else
		{
			const float lengthSq = glm::dot(baseCenterPointToTestPoint, baseCenterPointToTestPoint);
			const float distanceSquared = lengthSq - (dotProduct * dotProduct / axisLengthSquared);
			return distanceSquared <= radiusSquared;
		}
	}
}