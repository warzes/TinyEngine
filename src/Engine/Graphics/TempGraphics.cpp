#include "stdafx.h"
#include "GraphicsResource.h"
#include "GraphicsSystem.h"
#include "Core/IO/FileSystem.h"
#include "RenderAPI/RenderSystem.h"
//-----------------------------------------------------------------------------
const std::vector<VertexAttribute> NewMeshVertex::GetAttribs()
{
	return 
	{
		{.location = 0, .size = 3, .normalized = GL_FALSE, .stride = sizeof(NewMeshVertex), .offset = (void*)offsetof(NewMeshVertex, positions) },
		{.location = 1, .size = 3, .normalized = GL_FALSE, .stride = sizeof(NewMeshVertex), .offset = (void*)offsetof(NewMeshVertex, normals) },
		{.location = 2, .size = 4, .normalized = GL_FALSE, .stride = sizeof(NewMeshVertex), .offset = (void*)offsetof(NewMeshVertex, tangents) },
		{.location = 3, .size = 4, .normalized = GL_FALSE, .stride = sizeof(NewMeshVertex), .offset = (void*)offsetof(NewMeshVertex, colors) },
		{.location = 4, .size = 2, .normalized = GL_FALSE, .stride = sizeof(NewMeshVertex), .offset = (void*)offsetof(NewMeshVertex, texCoords) },
		{.location = 5, .size = 2, .normalized = GL_FALSE, .stride = sizeof(NewMeshVertex), .offset = (void*)offsetof(NewMeshVertex, texCoords2) },
	};
}
//-----------------------------------------------------------------------------
NewModel LoadOBJ(const std::string& fileName);
NewModel LoadIQM(const std::string& fileName);
ModelAnimation* LoadModelAnimationsIQM(const std::string& fileName, unsigned int& animCount);
NewModel LoadGLTF(const std::string& fileName);
ModelAnimation* LoadModelAnimationsGLTF(const std::string& fileName, unsigned int& animCount);
NewModel LoadM3D(const std::string& filename);
ModelAnimation* LoadModelAnimationsM3D(const std::string& fileName, unsigned int& animCount);
//-----------------------------------------------------------------------------
NewMaterial LoadMaterialDefault();
glm::vec3 Vector3RotateByQuaternion(const glm::vec3& v, const glm::quat& q);
//-----------------------------------------------------------------------------
// Upload vertex data into a VAO and VBO
void UploadMesh(NewMesh& mesh, bool dynamic)
{
	if (mesh.geometry != nullptr)
	{
		// Check if mesh has already been loaded in GPU
		LogWarning("VAO: [ID " + std::to_string(mesh.geometry->vao->Id()) + "] Trying to re-load an already loaded mesh");
		return;
	}

	// TODO: оптимизировать без копирования буфера
	mesh.vert.resize(mesh.vertexCount);

	float* vertices = (mesh.animVertices != nullptr) ? mesh.animVertices : mesh.vertices;
	float* normals = (mesh.animNormals != nullptr) ? mesh.animNormals : mesh.normals;

	for (size_t i = 0, v = 0, tc = 0, n = 0, c = 0, t = 0, tc2 = 0; i < mesh.vert.size(); i++)
	{
		mesh.vert[i].positions = { vertices[v + 0], vertices[v + 1], vertices[v + 2] };
		if (mesh.texcoords != nullptr)
			mesh.vert[i].texCoords = { mesh.texcoords[tc + 0], mesh.texcoords[tc + 1] };
		else
			mesh.vert[i].texCoords = { 0.0f, 0.0f };

		if (normals != nullptr)
			mesh.vert[i].normals = { normals[n + 0], normals[n + 1], normals[n + 2] };
		else
			mesh.vert[i].normals = { 1.0f, 1.0f, 1.0f };

		if (mesh.colors != nullptr)
			mesh.vert[i].colors = { mesh.colors[c + 0] / 255.0f, mesh.colors[c + 1] / 255.0f, mesh.colors[c + 2] / 255.0f, mesh.colors[c + 3] / 255.0f };
		else
			mesh.vert[i].colors = { 1.0f, 1.0f, 1.0f, 1.0f };

		if (mesh.tangents != nullptr)
			mesh.vert[i].tangents = { mesh.tangents[t + 0], mesh.tangents[t + 1], mesh.tangents[t + 2], mesh.tangents[t + 3] };
		else
			mesh.vert[i].tangents = { 0.0f, 0.0f, 0.0f, 0.0f };

		if (mesh.texcoords2 != nullptr)
			mesh.vert[i].texCoords2 = { mesh.texcoords2[tc2 + 0], mesh.texcoords2[tc2 + 1] };
		else
			mesh.vert[i].texCoords2 = { 0.0f, 0.0f };

		v += 3;
		tc += 2;
		n += 3;
		c += 4;
		t += 4;
		tc2 += 2;
	}

	mesh.geometry = GetRenderSystem().CreateGeometryBuffer(dynamic ? BufferUsage::DynamicDraw : BufferUsage::StaticDraw, mesh.vert.size(), sizeof(NewMeshVertex), mesh.vert.data(), mesh.indices.size(), IndexFormat::UInt32, mesh.indices.data(), NewMeshVertex::GetAttribs());
}
//-----------------------------------------------------------------------------
NewModel LoadModel(const std::string& fileName)
{
	NewModel model;

	if (FileSystem::IsFileExtension(fileName, ".obj")) model = LoadOBJ(fileName);
	if (FileSystem::IsFileExtension(fileName, ".iqm")) model = LoadIQM(fileName);
	if (FileSystem::IsFileExtension(fileName, ".gltf") || FileSystem::IsFileExtension(fileName, ".glb")) model = LoadGLTF(fileName);
	if (FileSystem::IsFileExtension(fileName, ".m3d")) model = LoadM3D(fileName);

	if (model.meshes.size() > 0)
	{
		// Upload vertex data to GPU (static meshes)
		for (size_t i = 0; i < model.meshes.size(); i++)
			UploadMesh(model.meshes[i], false);
	}
	else LogWarning("MESH: [" + fileName + "] Failed to load model mesh(es) data");

	if (model.materialCount == 0)
	{
		LogWarning("MATERIAL: [" + fileName + "] Failed to load model material data, default to white material");

		// TODO: убрать сырой массив в пользу вектора
		model.materialCount = 1;
		model.materials = (NewMaterial*)calloc(model.materialCount, sizeof(Material));
		model.materials[0] = LoadMaterialDefault();

		if (model.meshMaterial == NULL) model.meshMaterial = (int*)calloc(model.meshes.size(), sizeof(int));
	}

	return model;
}
//-----------------------------------------------------------------------------
ModelAnimation* LoadModelAnimations(const std::string& fileName, unsigned int& animCount)
{
	ModelAnimation* animations = nullptr;

	if (FileSystem::IsFileExtension(fileName, ".iqm")) animations = LoadModelAnimationsIQM(fileName, animCount);
	if (FileSystem::IsFileExtension(fileName, ".gltf") || FileSystem::IsFileExtension(fileName, ".glb"))
		animations = LoadModelAnimationsGLTF(fileName, animCount);

	return animations;
}
//-----------------------------------------------------------------------------
// Update model animated vertex data (positions and normals) for a given frame
// NOTE: Updated data is uploaded to GPU
void UpdateModelAnimation(const NewModel& model, const ModelAnimation& anim, int frame)
{
	auto& render = GetRenderSystem();

	if ((anim.frameCount > 0) && (anim.bones != nullptr) && (anim.framePoses != nullptr))
	{
		if (frame >= anim.frameCount) frame = frame % anim.frameCount;

		for (int m = 0; m < model.meshes.size(); m++)
		{
			const NewMesh& mesh = model.meshes[m];

			if (mesh.boneIds == nullptr || mesh.boneWeights == nullptr)
			{
				LogWarning("MODEL: UpdateModelAnimation(): Mesh " + std::to_string(m) + " has no connection to bones");
				continue;
			}

			bool updated = false; // Flag to check when anim vertex information is updated
			glm::vec3 animVertex = glm::vec3{ 0 };
			glm::vec3 animNormal = glm::vec3{ 0 };

			glm::vec3 inTranslation = glm::vec3{ 0 };
			glm::quat inRotation = glm::quat(1, 0, 0, 0);

			glm::vec3 outTranslation = glm::vec3{ 0 };
			glm::quat outRotation = glm::quat(1, 0, 0, 0);
			glm::vec3 outScale = glm::vec3{ 0 };

			int boneId = 0;
			int boneCounter = 0;
			float boneWeight = 0.0;

			const int vValues = mesh.vertexCount * 3;
			for (int vCounter = 0; vCounter < vValues; vCounter += 3)
			{
				mesh.animVertices[vCounter] = 0;
				mesh.animVertices[vCounter + 1] = 0;
				mesh.animVertices[vCounter + 2] = 0;
				if (mesh.animNormals != nullptr)
				{
					mesh.animNormals[vCounter] = 0;
					mesh.animNormals[vCounter + 1] = 0;
					mesh.animNormals[vCounter + 2] = 0;
				}

				// Iterates over 4 bones per vertex
				for (int j = 0; j < 4; j++, boneCounter++)
				{
					boneWeight = mesh.boneWeights[boneCounter];

					// Early stop when no transformation will be applied
					if (boneWeight == 0.0f) continue;

					boneId = mesh.boneIds[boneCounter];
					inTranslation = model.bindPose[boneId].translation;
					inRotation = model.bindPose[boneId].rotation;
					outTranslation = anim.framePoses[frame][boneId].translation;
					outRotation = anim.framePoses[frame][boneId].rotation;
					outScale = anim.framePoses[frame][boneId].scale;

					// Vertices processing
					// NOTE: We use meshes.vertices (default vertex position) to calculate meshes.animVertices (animated vertex position)
					animVertex = { mesh.vertices[vCounter + 0], mesh.vertices[vCounter + 1], mesh.vertices[vCounter + 2] };
					animVertex = animVertex - inTranslation;
					animVertex = animVertex * outScale;
					animVertex = Vector3RotateByQuaternion(animVertex, (outRotation * glm::inverse(inRotation)));
					animVertex = animVertex + outTranslation;
					mesh.animVertices[vCounter + 0] += animVertex.x * boneWeight;
					mesh.animVertices[vCounter + 1] += animVertex.y * boneWeight;
					mesh.animVertices[vCounter + 2] += animVertex.z * boneWeight;
					updated = true;

					// Normals processing
					// NOTE: We use meshes.baseNormals (default normal) to calculate meshes.normals (animated normals)
					if (mesh.normals != nullptr)
					{
						animNormal = { mesh.normals[vCounter], mesh.normals[vCounter + 1], mesh.normals[vCounter + 2] };
						animNormal = Vector3RotateByQuaternion(animNormal, (outRotation * glm::inverse(inRotation)));
						mesh.animNormals[vCounter + 0] += animNormal.x * boneWeight;
						mesh.animNormals[vCounter + 1] += animNormal.y * boneWeight;
						mesh.animNormals[vCounter + 2] += animNormal.z * boneWeight;
					}
				}
			}

			// Upload new vertex data to GPU for model drawing
			// NOTE: Only update data when values changed
			if (updated)
			{
				auto vb = mesh.geometry->GetVBO();

				render.Bind(vb);
				NewMeshVertex* data = (NewMeshVertex*)render.MapBuffer(vb);
				for (size_t i = 0, v = 0, n = 0; i < mesh.vert.size(); i++)
				{
					data[i].positions = { mesh.animVertices[v + 0], mesh.animVertices[v + 1], mesh.animVertices[v + 2] };
					data[i].normals = { mesh.animNormals[n + 0], mesh.animNormals[n + 1], mesh.animNormals[n + 2] };
					v += 3;
					n += 3;
				}
				render.UnmapBuffer(vb);
			}
		}
	}
}
//-----------------------------------------------------------------------------