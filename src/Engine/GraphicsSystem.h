#pragma once

#include "GraphicsResource.h"

class GraphicsSystem final
{
	friend class EngineDevice;
public:
	GraphicsSystem() = default;

	void Create();
	void Destroy();

	ModelRef CreateModel(const char* fileName, const char* pathMaterialFiles = "./");
	ModelRef CreateModel(std::vector<Mesh>&& meshes);

	void Draw(Mesh& subMesh);
	void Draw(ModelRef model);
	std::vector<glm::vec3> GetVertexInMesh(const Mesh& mesh) const;
	std::vector<glm::vec3> GetVertexInModel(ModelRef model) const;

private:
	GraphicsSystem(GraphicsSystem&&) = delete;
	GraphicsSystem(const GraphicsSystem&) = delete;
	GraphicsSystem& operator=(GraphicsSystem&&) = delete;
	GraphicsSystem& operator=(const GraphicsSystem&) = delete;

	ModelRef createMeshBuffer(std::vector<Mesh>&& meshes);
	ModelRef loadObjFile(const char* fileName, const char* pathMaterialFiles = "./");
	void computeSubMeshesAABB(std::vector<Mesh>& meshes);
};

GraphicsSystem& GetGraphicsSystem();