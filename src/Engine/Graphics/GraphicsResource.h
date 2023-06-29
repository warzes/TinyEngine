#pragma once

#include "RenderAPI/RenderResource.h"
#include "Core/Geometry/AABB.h"

class Material final
{
public:
	Material() = default;
	Material(Material&&) = default;
	Material(const Material&) = delete;
	~Material() = default;
	Material& operator=(Material&&) = default;
	Material& operator=(const Material&) = delete;

	inline bool HasDiffuseMap() const { return diffuseTexture != nullptr; }
	inline bool HasSpecularMap() const { return specularTexture != nullptr; }
	inline bool HasNormalMap() const { return normalTexture != nullptr; }
	inline bool HasEmissiveMap() const { return emissiveTexture != nullptr; }

	Texture2DRef diffuseTexture = nullptr;
	Texture2DRef specularTexture = nullptr;
	Texture2DRef normalTexture = nullptr;
	Texture2DRef emissiveTexture = nullptr;

	glm::vec3 ambientColor = glm::vec3{ 1.0f };
	glm::vec3 diffuseColor = glm::vec3{ 1.0f };
	glm::vec3 specularColor = glm::vec3{ 1.0f };
	float shininess = 1.0f;
};

class VertexMesh final
{
public:
	bool operator==(const VertexMesh& v) const { return position == v.position && normal == v.normal && color == v.color && texCoord == v.texCoord; }

	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 color;
	glm::vec2 texCoord;
};

class Mesh final
{
public:
	Mesh() = default;
	Mesh(Mesh&&) = default;
	Mesh(const Mesh&) = delete;
	~Mesh() = default;
	Mesh& operator=(Mesh&&) = default;
	Mesh& operator=(const Mesh&) = delete;

	glm::vec3 GetMeshPos() const { return globalAABB.GetCenter(); }
	AABB GetLocalAABB() const 
	{ 
		AABB temp;
		temp.min = globalAABB.min - GetMeshPos();
		temp.max = globalAABB.max - GetMeshPos();
		return temp;
	}

	std::vector<VertexMesh> vertices;
	std::vector<uint32_t> indices;
	Material material;
	std::string meshName;

	GeometryBufferRef geometry;

	// global bouncing box
	AABB globalAABB;
};

class Model final
{
public:
	Model() = default;
	Model(Model&&) = default;
	Model(const Model&) = delete;
	~Model() = default;
	Model& operator=(Model&&) = default;
	Model& operator=(const Model&) = delete;

	std::vector<Mesh> subMeshes;
	// bouncing box
	AABB aabb;
};
using ModelRef = std::shared_ptr<Model>;