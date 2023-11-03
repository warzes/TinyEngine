#include "stdafx.h"
#include "GraphicsResource.h"
#include "GraphicsSystem.h"
#include "Core/IO/FileSystem.h"
#include "RenderAPI/RenderSystem.h"

NewModel LoadGLTF(const char* fileName);
NewMaterial LoadMaterialDefault();

ModelAnimation* LoadModelAnimationsGLTF(const char* fileName, unsigned int* animCount);

// Upload vertex data into a VAO (if supported) and VBO
void UploadMesh(NewMesh* mesh, bool dynamic, ShaderProgramRef program)
{
	if (GetRenderSystem().IsValid(mesh->geometry))
	{
		// Check if mesh has already been loaded in GPU
		LogWarning("VAO: [ID " + std::to_string(mesh->geometry->vao->Id()) + "] Trying to re-load an already loaded mesh");
		return;
	}

	// TODO: оптимизировать без копирования буфера
	mesh->vert.resize(mesh->vertexCount);

	float* vertices = (mesh->animVertices != NULL) ? mesh->animVertices : mesh->vertices;
	float* normals = (mesh->animNormals != NULL) ? mesh->animNormals : mesh->normals;

	for (size_t i = 0, v = 0, tc = 0, n = 0, c = 0, t = 0, tc2 = 0; i < mesh->vert.size(); i++)
	{
		mesh->vert[i].positions.x = vertices[v + 0];
		mesh->vert[i].positions.y = vertices[v + 1];
		mesh->vert[i].positions.z = vertices[v + 2];

		if (mesh->texcoords2 != NULL)
		{
			mesh->vert[i].texCoords.x = mesh->texcoords[tc + 0];
			mesh->vert[i].texCoords.y = mesh->texcoords[tc + 1];
		}
		else
			mesh->vert[i].texCoords = { 0.0f, 0.0f };

		if (normals != NULL)
		{
			mesh->vert[i].normals.x = normals[n + 0];
			mesh->vert[i].normals.y = normals[n + 1];
			mesh->vert[i].normals.z = normals[n + 2];
		}
		else
			mesh->vert[i].normals = { 1.0f, 1.0f, 1.0f };

		if (mesh->colors != NULL)
		{
			mesh->vert[i].colors.x = mesh->colors[c + 0] / 255.0f;
			mesh->vert[i].colors.y = mesh->colors[c + 1] / 255.0f;
			mesh->vert[i].colors.z = mesh->colors[c + 2] / 255.0f;
			mesh->vert[i].colors.w = mesh->colors[c + 3] / 255.0f;
		}
		else
			mesh->vert[i].colors = { 1.0f, 1.0f, 1.0f, 1.0f };

		if (mesh->tangents != NULL)
		{
			mesh->vert[i].tangents.x = mesh->tangents[t + 0];
			mesh->vert[i].tangents.y = mesh->tangents[t + 1];
			mesh->vert[i].tangents.z = mesh->tangents[t + 2];
			mesh->vert[i].tangents.w = mesh->tangents[t + 3];
		}
		else
			mesh->vert[i].tangents = { 0.0f, 0.0f, 0.0f, 0.0f };

		if (mesh->texcoords2 != NULL)
		{
			mesh->vert[i].texCoords2.x = mesh->texcoords2[tc2 + 0];
			mesh->vert[i].texCoords2.y = mesh->texcoords2[tc2 + 1];
		}
		else
			mesh->vert[i].texCoords2 = { 0.0f, 0.0f };

		v += 3;
		tc += 2;
		n += 3;
		c += 4;
		t += 4;
		tc2 += 2;
	}

	if (mesh->indices != NULL)
	{
		mesh->index.resize(mesh->triangleCount * 3);
		for (size_t i = 0; i < mesh->index.size(); i++)
		{
			mesh->index[i] = mesh->indices[i];
		}
	}

	mesh->geometry = GetRenderSystem().CreateGeometryBuffer(dynamic ? BufferUsage::DynamicDraw : BufferUsage::StaticDraw, mesh->vert.size(), sizeof(NewMeshVertex), mesh->vert.data(), mesh->index.size(), IndexFormat::UInt32, mesh->index.data(), program);
}

NewModel LoadModel(const char* fileName, ShaderProgramRef program)
{
	NewModel model = {};

	if (FileSystem::IsFileExtension(fileName, ".gltf") || FileSystem::IsFileExtension(fileName, ".glb")) model = LoadGLTF(fileName);

	// TODO: другие форматы

	// Make sure model transform is set to identity matrix!
	model.transform = glm::mat4(1.0f);

	if ((model.meshCount != 0) && (model.meshes != NULL))
	{
		// Upload vertex data to GPU (static meshes)
		for (int i = 0; i < model.meshCount; i++) UploadMesh(&model.meshes[i], false, program);
	}
	else LogWarning("MESH: [" + std::string(fileName) + "] Failed to load model mesh(es) data");

	if (model.materialCount == 0)
	{
		LogWarning("MATERIAL: [" + std::string(fileName) + "] Failed to load model material data, default to white material");

		model.materialCount = 1;
		model.materials = (NewMaterial*)calloc(model.materialCount, sizeof(Material));
		model.materials[0] = LoadMaterialDefault();

		if (model.meshMaterial == NULL) model.meshMaterial = (int*)calloc(model.meshCount, sizeof(int));
	}

	return model;
}

ModelAnimation* LoadModelAnimations(const char* fileName, unsigned int* animCount)
{
	ModelAnimation* animations = NULL;

	//if (FileSystem::IsFileExtension(fileName, ".gltf;.glb")) 
	if (FileSystem::IsFileExtension(fileName, ".gltf") || FileSystem::IsFileExtension(fileName, ".glb"))
		animations = LoadModelAnimationsGLTF(fileName, animCount);

	return animations;
}

// Invert provided quaternion
glm::quat QuaternionInvert(glm::quat q)
{
	glm::quat result = q;

	float lengthSq = q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;

	if (lengthSq != 0.0f)
	{
		float invLength = 1.0f / lengthSq;

		result.x *= -invLength;
		result.y *= -invLength;
		result.z *= -invLength;
		result.w *= invLength;
	}

	return result;
}

glm::vec3 Vector3RotateByQuaternion(const glm::vec3& v, const glm::quat& q);

// Update model animated vertex data (positions and normals) for a given frame
// NOTE: Updated data is uploaded to GPU
void UpdateModelAnimation(NewModel model, ModelAnimation anim, int frame)
{
	if((anim.frameCount > 0) && (anim.bones != NULL) && (anim.framePoses != NULL))
	{
		if (frame >= anim.frameCount) frame = frame % anim.frameCount;

		for (int m = 0; m < model.meshCount; m++)
		{
			NewMesh mesh = model.meshes[m];

			if (mesh.boneIds == NULL || mesh.boneWeights == NULL)
			{
				LogWarning("MODEL: UpdateModelAnimation(): Mesh " + std::to_string(m) + " has no connection to bones");
				continue;
			}

			bool updated = false;           // Flag to check when anim vertex information is updated
			glm::vec3 animVertex = glm::vec3{ 0 };
			glm::vec3 animNormal = glm::vec3{ 0 };

			glm::vec3 inTranslation = glm::vec3{ 0 };
			glm::quat inRotation = glm::quat(1, 0, 0, 0);
			// Vector3 inScale = { 0 };

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

				if (mesh.animNormals != NULL)
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
					//int boneIdParent = model.bones[boneId].parent;
					inTranslation = model.bindPose[boneId].translation;
					inRotation = model.bindPose[boneId].rotation;
					//inScale = model.bindPose[boneId].scale;
					outTranslation = anim.framePoses[frame][boneId].translation;
					outRotation = anim.framePoses[frame][boneId].rotation;
					outScale = anim.framePoses[frame][boneId].scale;

					// Vertices processing
					// NOTE: We use meshes.vertices (default vertex position) to calculate meshes.animVertices (animated vertex position)
					animVertex = { mesh.vertices[vCounter], mesh.vertices[vCounter + 1], mesh.vertices[vCounter + 2] };
					animVertex = animVertex - inTranslation;
					animVertex = animVertex * outScale;
					animVertex = Vector3RotateByQuaternion(animVertex, (outRotation * QuaternionInvert(inRotation)));
					animVertex = animVertex + outTranslation;
					//animVertex = Vector3Transform(animVertex, model.transform);
					mesh.animVertices[vCounter] += animVertex.x * boneWeight;
					mesh.animVertices[vCounter + 1] += animVertex.y * boneWeight;
					mesh.animVertices[vCounter + 2] += animVertex.z * boneWeight;
					updated = true;

					// Normals processing
					// NOTE: We use meshes.baseNormals (default normal) to calculate meshes.normals (animated normals)
					if (mesh.normals != NULL)
					{
						animNormal = { mesh.normals[vCounter], mesh.normals[vCounter + 1], mesh.normals[vCounter + 2] };
						animNormal = Vector3RotateByQuaternion(animNormal, (outRotation * QuaternionInvert(inRotation)));
						mesh.animNormals[vCounter] += animNormal.x * boneWeight;
						mesh.animNormals[vCounter + 1] += animNormal.y * boneWeight;
						mesh.animNormals[vCounter + 2] += animNormal.z * boneWeight;
					}
				}
			}

			// Upload new vertex data to GPU for model drawing
			// NOTE: Only update data when values changed
			if (updated)
			{
				//rlUpdateVertexBuffer(mesh.vboId[0], mesh.animVertices, mesh.vertexCount * 3 * sizeof(float), 0); // Update vertex position
				//rlUpdateVertexBuffer(mesh.vboId[2], mesh.animNormals, mesh.vertexCount * 3 * sizeof(float), 0);  // Update vertex normals


				auto& render = GetRenderSystem();
				render.Bind(mesh.geometry->GetVBO());
				NewMeshVertex* data = (NewMeshVertex*)render.MapBuffer(mesh.geometry->GetVBO());
				for (size_t i = 0, v = 0, n = 0; i < mesh.vert.size(); i++)
				{
					data[i].positions.x = mesh.animVertices[v + 0];
					data[i].positions.y = mesh.animVertices[v + 1];
					data[i].positions.z = mesh.animVertices[v + 2];

					v += 3;
				}
				render.UnmapBuffer(mesh.geometry->GetVBO());
			}
		}
	}
}