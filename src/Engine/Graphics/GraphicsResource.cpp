﻿#include "stdafx.h"
#include "GraphicsResource.h"
#include "GraphicsSystem.h"
#include "RenderAPI/RenderSystem.h"
//-----------------------------------------------------------------------------
namespace std
{
	template <>
	struct hash<StaticMeshVertex>
	{
		size_t operator()(const StaticMeshVertex& vertex) const
		{
			return ((hash<glm::vec3>()(vertex.positions) ^ (hash<glm::vec2>()(vertex.texCoords) << 1)) >> 1);
		}
	};
} // namespace std
//-----------------------------------------------------------------------------
RenderTargetRef GraphicsSystem::CreateRenderTarget(uint16_t width, uint16_t height)
{
	auto& renderSystem = GetRenderSystem();

	RenderTargetRef rt = std::make_shared<RenderTarget>();

	Texture2DCreateInfo createInfo = {
		.format = TexelsFormat::RGBA_U8,
		.width = width,
		.height = height,
	};
	Texture2DInfo info = {
		.minFilter = TextureMinFilter::Linear,
		.magFilter = TextureMagFilter::Linear,
		.mipmap = false
	};
	rt->colorTexture = renderSystem.CreateTexture2D(createInfo, info);
	rt->rbo = renderSystem.CreateRenderbuffer({ width, height }, ImageFormat::D24S8);
	rt->fb = renderSystem.CreateFramebuffer(rt->colorTexture, rt->rbo);
	rt->width = width;
	rt->height = height;

	return std::move(rt);
}
//-----------------------------------------------------------------------------
StaticModelRef GraphicsSystem::CreateModel(const char* fileName, const char* pathMaterialFiles)
{
	// TODO: переделать сделав нормальное получение расшиерия
	if (std::string(fileName).find(".obj") != std::string::npos)
		return loadObjFile(fileName, pathMaterialFiles);

	return {};
}
//-----------------------------------------------------------------------------
StaticModelRef GraphicsSystem::CreateModel(std::vector<StaticMesh>&& meshes)
{
	return createMeshBuffer(std::move(meshes));
}
//-----------------------------------------------------------------------------
void GraphicsSystem::Draw(StaticMesh& subMesh)
{
	auto& renderSystem = GetRenderSystem();
	if(renderSystem.IsValid(subMesh.geometry) )
	{
		renderSystem.Bind(subMesh.material.diffuseTexture, 0);
		renderSystem.Draw(subMesh.geometry->vao, PrimitiveTopology::Triangles);
	}
}
//-----------------------------------------------------------------------------
void GraphicsSystem::Draw(StaticModelRef model)
{
	for (size_t i = 0; i < model->subMeshes.size(); i++)
	{
		Draw(model->subMeshes[i]);
	}
}
//-----------------------------------------------------------------------------
std::vector<glm::vec3> GraphicsSystem::GetVertexInMesh(const StaticMesh& mesh) const
{
	std::vector<glm::vec3> v;
	v.reserve(mesh.indices.size());
	// востановление треугольников по индексному буферу
	for (size_t i = 0; i < mesh.indices.size(); i++)
		v.push_back(mesh.vertices[mesh.indices[i]].positions);
	return v;
}
//-----------------------------------------------------------------------------
std::vector<glm::vec3> GraphicsSystem::GetVertexInModel(StaticModelRef model) const
{
	std::vector<glm::vec3> v;
	for (size_t i = 0; i < model->subMeshes.size(); i++)
	{
		auto subV = GetVertexInMesh(model->subMeshes[i]);
		// https://www.techiedelight.com/ru/concatenate-two-vectors-cpp/
		std::move(subV.begin(), subV.end(), std::back_inserter(v));
	}
	return v;
}
//-----------------------------------------------------------------------------
TrianglesInfo GraphicsSystem::GetTrianglesInMesh(const StaticMesh& mesh) const
{
	TrianglesInfo info;
	info.vertices.resize(mesh.vertices.size());
	for (size_t i = 0; i < mesh.vertices.size(); i++)
		info.vertices[i] = mesh.vertices[i].positions;
	info.indexes = mesh.indices;
	return info;
}
//-----------------------------------------------------------------------------
TrianglesInfo GraphicsSystem::GetTrianglesInModel(StaticModelRef model) const
{
	TrianglesInfo info;
	unsigned prevIndex = 0;
	for (size_t i = 0; i < model->subMeshes.size(); i++)
	{
		TrianglesInfo infoSubMesh = GetTrianglesInMesh(model->subMeshes[i]);

		info.vertices.insert(info.vertices.end(), infoSubMesh.vertices.begin(), infoSubMesh.vertices.end());

		for (size_t j = 0; j < infoSubMesh.indexes.size(); j++)
			info.indexes.push_back(infoSubMesh.indexes[j] + prevIndex);

		prevIndex += infoSubMesh.indexes.size();
	}

	return info;
}
//-----------------------------------------------------------------------------
StaticModelRef GraphicsSystem::createMeshBuffer(std::vector<StaticMesh>&& meshes)
{
	const std::vector<VertexAttribute> formatVertex =
	{
		{.location = 0, .size = 3, .normalized = false, .stride = sizeof(StaticMeshVertex), .offset = (void*)offsetof(StaticMeshVertex, positions)},
		{.location = 1, .size = 3, .normalized = false, .stride = sizeof(StaticMeshVertex), .offset = (void*)offsetof(StaticMeshVertex, normals)},
		{.location = 2, .size = 3, .normalized = false, .stride = sizeof(StaticMeshVertex), .offset = (void*)offsetof(StaticMeshVertex, colors)},
		{.location = 3, .size = 2, .normalized = false, .stride = sizeof(StaticMeshVertex), .offset = (void*)offsetof(StaticMeshVertex, texCoords)}
	};

	StaticModelRef model(new StaticModel());
	model->subMeshes = std::move(meshes);
	model->aabb.min = model->subMeshes[0].globalAABB.min;
	model->aabb.max = model->subMeshes[0].globalAABB.max;

	for (size_t i = 0; i < model->subMeshes.size(); i++)
	{
		model->subMeshes[i].geometry = GetRenderSystem().CreateGeometryBuffer(
			BufferUsage::StaticDraw,
			(unsigned)model->subMeshes[i].vertices.size(), sizeof(model->subMeshes[i].vertices[0]), model->subMeshes[i].vertices.data(),
			(unsigned)model->subMeshes[i].indices.size(), IndexFormat::UInt32, model->subMeshes[i].indices.data(), 
			formatVertex);

		// compute AABB
		{
			model->aabb.Merge(model->subMeshes[i].globalAABB);
		}
	}
	return model;
}
//-----------------------------------------------------------------------------
StaticModelRef GraphicsSystem::loadObjFile(const char* fileName, const char* pathMaterialFiles)
{
	tinyobj::ObjReaderConfig readerConfig;
	readerConfig.mtl_search_path = pathMaterialFiles; // Path to material files

	tinyobj::ObjReader reader;
	if (!reader.ParseFromFile(fileName, readerConfig))
	{
		if (!reader.Error().empty())
			LogError("TinyObjReader: " + reader.Error());
		return {};
	}
	if (!reader.Warning().empty())
		LogWarning("TinyObjReader: " + reader.Warning());

	auto& attributes = reader.GetAttrib();
	auto& shapes = reader.GetShapes();
	auto& materials = reader.GetMaterials();
	const bool isFindMaterials = !materials.empty();

	std::vector<StaticMesh> meshes(shapes.size());
	std::vector<int> materialIds(shapes.size());

	// Loop over shapes
	for (size_t shapeId = 0; shapeId < shapes.size(); shapeId++)
	{
		meshes[shapeId].meshName = shapes[shapeId].name;

		std::unordered_map<StaticMeshVertex, uint32_t> uniqueVertices;

		// Loop over faces(polygon)
		size_t index_offset = 0;
		for (size_t faceId = 0; faceId < shapes[shapeId].mesh.num_face_vertices.size(); faceId++)
		{
			const size_t fv = static_cast<size_t>(shapes[shapeId].mesh.num_face_vertices[faceId]);

			// per-face material
			materialIds[shapeId] = shapes[shapeId].mesh.material_ids[faceId];
			if ( materialIds[shapeId] < 0) materialIds[shapeId] = 0;

			// Loop over vertices in the face.
			for (size_t v = 0; v < fv; v++)
			{
				// access to vertex
				const tinyobj::index_t idx = shapes[shapeId].mesh.indices[index_offset + v];

				// vertex position
				const tinyobj::real_t vx = attributes.vertices[3 * size_t(idx.vertex_index) + 0];
				const tinyobj::real_t vy = attributes.vertices[3 * size_t(idx.vertex_index) + 1];
				const tinyobj::real_t vz = attributes.vertices[3 * size_t(idx.vertex_index) + 2];

				// Check if `normal_index` is zero or positive. negative = no normal data
				tinyobj::real_t nx = 0.0f;
				tinyobj::real_t ny = 0.0f;
				tinyobj::real_t nz = 0.0f;
				if (idx.normal_index >= 0)
				{
					nx = attributes.normals[3 * size_t(idx.normal_index) + 0];
					ny = attributes.normals[3 * size_t(idx.normal_index) + 1];
					nz = attributes.normals[3 * size_t(idx.normal_index) + 2];
				}

				// Check if `texcoord_index` is zero or positive. negative = no texcoord data
				tinyobj::real_t tx = 0;
				tinyobj::real_t ty = 0;
				if (idx.texcoord_index >= 0)
				{
					tx = attributes.texcoords[2 * size_t(idx.texcoord_index) + 0];
					ty = attributes.texcoords[2 * size_t(idx.texcoord_index) + 1];
				}

				// vertex colors
				const tinyobj::real_t r = attributes.colors[3 * size_t(idx.vertex_index) + 0];
				const tinyobj::real_t g = attributes.colors[3 * size_t(idx.vertex_index) + 1];
				const tinyobj::real_t b = attributes.colors[3 * size_t(idx.vertex_index) + 2];

				StaticMeshVertex vertex = {
					.positions = { vx, vy, vz },
					.normals = { nx, ny, nz },
					.colors = { r, g, b },
					.texCoords = { tx,ty }
				};

				if (uniqueVertices.count(vertex) == 0)
				{
					uniqueVertices[vertex] = static_cast<uint32_t>(meshes[shapeId].vertices.size());
					meshes[shapeId].vertices.emplace_back(vertex);
				}

				meshes[shapeId].indices.emplace_back(uniqueVertices[vertex]);
			}
			index_offset += fv;
		}
	}

	// load materials
	if (isFindMaterials)
	{
		for (size_t i = 0; i < shapes.size(); i++)
		{
			const size_t matId = static_cast<size_t>(materialIds[i]);


			if (materials[matId].diffuse_texname.empty()) continue;

			std::string diffuseMap = pathMaterialFiles + materials[matId].diffuse_texname;
			meshes[i].material.diffuseTexture = GetRenderSystem().CreateTexture2D(diffuseMap.c_str(), true);
		}
	}

	// compute AABB
	computeSubMeshesAABB(meshes);

	return createMeshBuffer(std::move(meshes));
}
//-----------------------------------------------------------------------------
void GraphicsSystem::computeSubMeshesAABB(std::vector<StaticMesh>& meshes)
{
	for( size_t i = 0; i < meshes.size(); i++ )
	{
		for( size_t j = 0; j < meshes[i].vertices.size(); j++ )
		{
			meshes[i].globalAABB.Merge(meshes[i].vertices[j].positions);
		}
	}
}
//-----------------------------------------------------------------------------