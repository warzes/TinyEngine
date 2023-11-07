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

//=============================================================================
// ANIMATION MESH
//=============================================================================

class NewMeshVertex final
{
public:
	glm::vec3 positions;
	glm::vec3 normals;
	glm::vec4 tangents;
	glm::vec4 colors;
	glm::vec2 texCoords;
	glm::vec2 texCoords2;
};

// TODO: возможно переименовать в baseMesh и убрать staticmesh
class NewMesh final
{
public:
	int vertexCount;        // Number of vertices stored in arrays
	int triangleCount;      // Number of triangles stored (indexed or not)

	// Vertex attributes data
	float* vertices;        // Vertex position (XYZ - 3 components per vertex) (shader-location = 0)
	float* texcoords;       // Vertex texture coordinates (UV - 2 components per vertex) (shader-location = 1)
	float* texcoords2;      // Vertex texture second coordinates (UV - 2 components per vertex) (shader-location = 5)
	float* normals;         // Vertex normals (XYZ - 3 components per vertex) (shader-location = 2)
	float* tangents;        // Vertex tangents (XYZW - 4 components per vertex) (shader-location = 4)
	unsigned char* colors;      // Vertex colors (RGBA - 4 components per vertex) (shader-location = 3)
	unsigned short* indices;    // Vertex indices (in case vertex data comes indexed)

	//int triangleCount;      // Number of triangles stored (indexed or not)
	std::vector<NewMeshVertex> vert;
	std::vector<uint32_t> index;

	GeometryBufferRef geometry;

	// Animation vertex data
	float* animVertices;    // Animated vertex positions (after bones transformations)
	float* animNormals;     // Animated normals (after bones transformations)
	unsigned char* boneIds; // Vertex bone ids, max 255 bone ids, up to 4 bones influence by vertex (skinning)
	float* boneWeights;     // Vertex bone weight, up to 4 bones influence by vertex (skinning)
};

// TODO: убрать заменив старым классом
struct TempTransform 
{
	glm::vec3 translation;
	glm::quat rotation;
	glm::vec3 scale;
};

// Bone, skeletal animation bone
struct NewBoneInfo 
{
	char name[32];          // Bone name
	int parent;             // Bone parent
};

struct MaterialMap 
{
	Texture2DRef texture;                // Material map texture
	glm::vec4 color = glm::vec4{ 1.0f }; // Material map color
	float value;                         // Material map value
};

// TODO: возможно этот вариант материала лучше - он позволяет добавлять новые виды
struct NewMaterial 
{
	ShaderProgramRef shader; // Material shader
	MaterialMap* maps;       // Material maps array (MAX_MATERIAL_MAPS)
	float params[4];         // Material generic parameters (if required)
};

class NewModel final
{
public:
	glm::mat4 transform;    // Local transform matrix

	int meshCount;          // Number of meshes
	int materialCount;      // Number of materials
	NewMesh* meshes;           // Meshes array
	NewMaterial* materials;    // Materials array
	int* meshMaterial;      // Mesh material number

	// Animation data
	int boneCount;          // Number of bones
	NewBoneInfo* bones;        // Bones information (skeleton)
	TempTransform* bindPose;    // Bones base transformation (pose)
};

struct ModelAnimation {
	int boneCount;          // Number of bones
	int frameCount;         // Number of animation frames
	NewBoneInfo* bones;        // Bones information (skeleton)
	TempTransform** framePoses; // Poses array by frame
	char name[32];          // Animation name
};

// Material map index
enum MaterialMapIndex {
	MATERIAL_MAP_ALBEDO = 0,        // Albedo material (same as: MATERIAL_MAP_DIFFUSE)
	MATERIAL_MAP_METALNESS,         // Metalness material (same as: MATERIAL_MAP_SPECULAR)
	MATERIAL_MAP_NORMAL,            // Normal material
	MATERIAL_MAP_ROUGHNESS,         // Roughness material
	MATERIAL_MAP_OCCLUSION,         // Ambient occlusion material
	MATERIAL_MAP_EMISSION,          // Emission material
	MATERIAL_MAP_HEIGHT,            // Heightmap material
	MATERIAL_MAP_CUBEMAP,           // Cubemap material (NOTE: Uses GL_TEXTURE_CUBE_MAP)
	MATERIAL_MAP_IRRADIANCE,        // Irradiance material (NOTE: Uses GL_TEXTURE_CUBE_MAP)
	MATERIAL_MAP_PREFILTER,         // Prefilter material (NOTE: Uses GL_TEXTURE_CUBE_MAP)
	MATERIAL_MAP_BRDF               // Brdf material
};
#define MATERIAL_MAP_DIFFUSE      MATERIAL_MAP_ALBEDO
#define MATERIAL_MAP_SPECULAR     MATERIAL_MAP_METALNESS

#define MAX_MATERIAL_MAPS              12       // Maximum number of shader maps supported

// TEMP Func
NewModel LoadModel(const char* fileName);
ModelAnimation* LoadModelAnimations(const char* fileName, unsigned int* animCount);// Load model animations from file
void UpdateModelAnimation(NewModel model, ModelAnimation anim, int frame); // Update model animation pose