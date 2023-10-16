#pragma once

#include "GraphicsResource.h"

struct TrianglesInfo
{
	std::vector<glm::vec3> vertices;
	std::vector<unsigned> indexes;
};

class GraphicsSystem final
{
	friend class EngineDevice;
public:
	GraphicsSystem() = default;

	bool Create();
	void Destroy();

	RenderTargetRef CreateRenderTarget(uint16_t width, uint16_t height);

	StaticModelRef CreateModel(const char* fileName, const char* pathMaterialFiles = "./");
	StaticModelRef CreateModel(std::vector<StaticMesh>&& meshes);

	void ResizeRenderTarget(RenderTargetRef rt, uint16_t width, uint16_t height);

	void BindRenderTarget(RenderTargetRef rt);
	void BindRenderTargetAsTexture(RenderTargetRef rt, unsigned textureSlot);

	void Draw(StaticMesh& subMesh);
	void Draw(StaticModelRef model);
	std::vector<glm::vec3> GetVertexInMesh(const StaticMesh& mesh) const;
	std::vector<glm::vec3> GetVertexInModel(StaticModelRef model) const;

	TrianglesInfo GetTrianglesInMesh(const StaticMesh& mesh) const;
	TrianglesInfo GetTrianglesInModel(StaticModelRef model) const;

private:
	GraphicsSystem(GraphicsSystem&&) = delete;
	GraphicsSystem(const GraphicsSystem&) = delete;
	GraphicsSystem& operator=(GraphicsSystem&&) = delete;
	GraphicsSystem& operator=(const GraphicsSystem&) = delete;

	StaticModelRef createMeshBuffer(std::vector<StaticMesh>&& meshes);
	StaticModelRef loadObjFile(const char* fileName, const char* pathMaterialFiles = "./");
	void computeSubMeshesAABB(std::vector<StaticMesh>& meshes);
};

GraphicsSystem& GetGraphicsSystem();