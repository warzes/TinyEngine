#pragma once

#include "RenderAPI/RenderResource.h"
#include "Core/Geometry/BoundingAABB.h"

class RenderTarget final
{
public:
	RenderTarget() = default;
	RenderTarget(RenderTarget&&) = default;
	RenderTarget(const RenderTarget&) = delete;
	~RenderTarget()
	{
		colorTexture.reset();
		rbo.reset();
		fb.reset();
	}
	RenderTarget& operator=(RenderTarget&&) = default;
	RenderTarget& operator=(const RenderTarget&) = delete;

	bool IsValid() const { return width > 0 && height >> 0 && fb > 0; }

	Texture2DRef colorTexture;
	RenderbufferRef rbo;
	FramebufferRef fb;
	unsigned width = 0;
	unsigned height = 0;

	// TODO: color
};
using RenderTargetRef = std::shared_ptr<RenderTarget>;

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

class StaticMeshVertex final
{
public:
	bool operator==(const StaticMeshVertex& v) const { return positions == v.positions && normals == v.normals && /*tangents == v.tangents &&*/ colors == v.colors && texCoords == v.texCoords; }

	glm::vec3 positions;
	glm::vec3 normals;
	//glm::vec4 tangents;
	glm::vec3 colors;
	glm::vec2 texCoords;
};

class StaticMesh final
{
public:
	StaticMesh() = default;
	StaticMesh(StaticMesh&&) = default;
	StaticMesh(const StaticMesh&) = delete;
	~StaticMesh() = default;
	StaticMesh& operator=(StaticMesh&&) = default;
	StaticMesh& operator=(const StaticMesh&) = delete;

	glm::vec3 GetMeshPos() const { return globalAABB.GetCenter(); }
	BoundingAABB GetLocalAABB() const
	{ 
		BoundingAABB temp;
		temp.min = globalAABB.min - GetMeshPos();
		temp.max = globalAABB.max - GetMeshPos();
		return temp;
	}

	std::vector<StaticMeshVertex> vertices;
	std::vector<uint32_t> indices;
	Material material;
	std::string meshName;

	GeometryBufferRef geometry;

	// global bouncing box
	BoundingAABB globalAABB;
};

class StaticModel final
{
public:
	StaticModel() = default;
	StaticModel(StaticModel&&) = default;
	StaticModel(const StaticModel&) = delete;
	~StaticModel() = default;
	StaticModel& operator=(StaticModel&&) = default;
	StaticModel& operator=(const StaticModel&) = delete;

	std::vector<StaticMesh> subMeshes;
	// bouncing box
	BoundingAABB aabb;
};
using StaticModelRef = std::shared_ptr<StaticModel>;