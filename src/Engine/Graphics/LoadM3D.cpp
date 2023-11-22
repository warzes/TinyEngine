#include "stdafx.h"
#include "GraphicsResource.h"
#include "Core/IO/Image.h"
#include "RenderAPI/RenderSystem.h"
#include <m3d.h>

unsigned char* LoadFileData(const char* fileName, int* dataSize);
NewMaterial LoadMaterialDefault();

// Hook LoadFileData()/UnloadFileData() calls to M3D loaders
unsigned char* m3d_loaderhook(char* fn, unsigned int* len) { return LoadFileData((const char*)fn, (int*)len); }
void m3d_freehook(void* data) { free((unsigned char*)data); }

NewModel LoadM3D(const std::string& filename)
{
	auto& render = GetRenderSystem();

	NewModel model = { };

	m3d_t* m3d = NULL;
	m3dp_t* prop = NULL;
	int i, j, k, l, n, mi = -2, vcolor = 0;

	int dataSize = 0;
	unsigned char* fileData = LoadFileData(filename.c_str(), &dataSize);

	if (fileData != NULL)
	{
		m3d = m3d_load(fileData, m3d_loaderhook, m3d_freehook, NULL);

		if (!m3d || M3D_ERR_ISFATAL(m3d->errcode))
		{
			LogWarning("MODEL: [" + filename + "] Failed to load M3D data, error code " + std::to_string(m3d ? m3d->errcode : -2));
			if (m3d) m3d_free(m3d);
			free(fileData);
			return model;
		}
		else LogPrint("MODEL: [" + filename + "] M3D data loaded successfully: " + std::to_string(m3d->numface) + " faces/" + std::to_string(m3d->nummaterial) + " materials");

		// no face? this is probably just a material library
		if (!m3d->numface)
		{
			m3d_free(m3d);
			free(fileData);
			return model;
		}

		if (m3d->nummaterial > 0)
		{
			model.materialCount = m3d->nummaterial;
			model.meshes.resize(m3d->nummaterial);
			LogPrint("MODEL: model has " + std::to_string(model.materialCount) + " material meshes");
		}
		else
		{
			model.meshes.resize(1);
			model.materialCount = 0;
			LogPrint("MODEL: No materials, putting all meshes in a default material");
		}

		// We always need a default material, so we add +1
		model.materialCount++;

		// Faces must be in non-decreasing materialid order. Verify that quickly, sorting them otherwise.
		// WARNING: Sorting is not needed, valid M3D model files should already be sorted
		// Just keeping the sorting function for reference (Check PR #3363 #3385)
		/*
		for (i = 1; i < m3d->numface; i++)
		{
		if (m3d->face[i-1].materialid <= m3d->face[i].materialid) continue;

		// face[i-1] > face[i].  slide face[i] lower.
		m3df_t slider = m3d->face[i];
		j = i-1;

		do
		{   // face[j] > slider, face[j+1] is svailable vacant gap.
		m3d->face[j+1] = m3d->face[j];
		j = j-1;
		}
		while (j >= 0 && m3d->face[j].materialid > slider.materialid);

		m3d->face[j+1] = slider;
		}
		*/

		model.meshMaterial = (int*)calloc(model.meshes.size(), sizeof(int));
		model.materials = (NewMaterial*)calloc(model.materialCount + 1, sizeof(NewMaterial));

		// Map no material to index 0 with default shader, everything else materialid + 1
		model.materials[0] = LoadMaterialDefault();

		for (i = l = 0, k = -1; i < (int)m3d->numface; i++, l++)
		{
			// Materials are grouped together
			if (mi != m3d->face[i].materialid)
			{
				// there should be only one material switch per material kind, but be bulletproof for non-optimal model files
				if (k + 1 >= model.meshes.size())
				{
					model.meshes.emplace_back(NewMesh{});
					model.meshMaterial = (int*)realloc(model.meshMaterial, model.meshes.size() * sizeof(int));
				}

				k++;
				mi = m3d->face[i].materialid;

				// Only allocate colors VertexBuffer if there's a color vertex in the model for this material batch
				// if all colors are fully transparent black for all verteces of this materal, then we assume no vertex colors
				for (j = i, l = vcolor = 0; (j < (int)m3d->numface) && (mi == m3d->face[j].materialid); j++, l++)
				{
					if (!m3d->vertex[m3d->face[j].vertex[0]].color ||
						!m3d->vertex[m3d->face[j].vertex[1]].color ||
						!m3d->vertex[m3d->face[j].vertex[2]].color) vcolor = 1;
				}

				model.meshes[k].vertexCount = l * 3;
				model.meshes[k].triangleCount = l;
				model.meshes[k].vertices = (float*)calloc(model.meshes[k].vertexCount * 3, sizeof(float));
				model.meshes[k].texcoords = (float*)calloc(model.meshes[k].vertexCount * 2, sizeof(float));
				model.meshes[k].normals = (float*)calloc(model.meshes[k].vertexCount * 3, sizeof(float));

				// If no map is provided, or we have colors defined, we allocate storage for vertex colors
				// M3D specs only consider vertex colors if no material is provided, however uses both and mixes the colors
				if ((mi == M3D_UNDEF) || vcolor) model.meshes[k].colors = (unsigned char*)calloc(model.meshes[k].vertexCount * 4, sizeof(unsigned char));

				if (m3d->numbone && m3d->numskin)
				{
					model.meshes[k].boneIds = (unsigned char*)calloc(model.meshes[k].vertexCount * 4, sizeof(unsigned char));
					model.meshes[k].boneWeights = (float*)calloc(model.meshes[k].vertexCount * 4, sizeof(float));
					model.meshes[k].animVertices = (float*)calloc(model.meshes[k].vertexCount * 3, sizeof(float));
					model.meshes[k].animNormals = (float*)calloc(model.meshes[k].vertexCount * 3, sizeof(float));
				}

				model.meshMaterial[k] = mi + 1;
				l = 0;
			}

			// Process meshes per material, add triangles
			model.meshes[k].vertices[l * 9 + 0] = m3d->vertex[m3d->face[i].vertex[0]].x * m3d->scale;
			model.meshes[k].vertices[l * 9 + 1] = m3d->vertex[m3d->face[i].vertex[0]].y * m3d->scale;
			model.meshes[k].vertices[l * 9 + 2] = m3d->vertex[m3d->face[i].vertex[0]].z * m3d->scale;
			model.meshes[k].vertices[l * 9 + 3] = m3d->vertex[m3d->face[i].vertex[1]].x * m3d->scale;
			model.meshes[k].vertices[l * 9 + 4] = m3d->vertex[m3d->face[i].vertex[1]].y * m3d->scale;
			model.meshes[k].vertices[l * 9 + 5] = m3d->vertex[m3d->face[i].vertex[1]].z * m3d->scale;
			model.meshes[k].vertices[l * 9 + 6] = m3d->vertex[m3d->face[i].vertex[2]].x * m3d->scale;
			model.meshes[k].vertices[l * 9 + 7] = m3d->vertex[m3d->face[i].vertex[2]].y * m3d->scale;
			model.meshes[k].vertices[l * 9 + 8] = m3d->vertex[m3d->face[i].vertex[2]].z * m3d->scale;

			// Without vertex color (full transparency), we use the default color
			if (model.meshes[k].colors != NULL)
			{
				if (m3d->vertex[m3d->face[i].vertex[0]].color & 0xFF000000)
					memcpy(&model.meshes[k].colors[l * 12 + 0], &m3d->vertex[m3d->face[i].vertex[0]].color, 4);
				if (m3d->vertex[m3d->face[i].vertex[1]].color & 0xFF000000)
					memcpy(&model.meshes[k].colors[l * 12 + 4], &m3d->vertex[m3d->face[i].vertex[1]].color, 4);
				if (m3d->vertex[m3d->face[i].vertex[2]].color & 0xFF000000)
					memcpy(&model.meshes[k].colors[l * 12 + 8], &m3d->vertex[m3d->face[i].vertex[2]].color, 4);
			}

			if (m3d->face[i].texcoord[0] != M3D_UNDEF)
			{
				model.meshes[k].texcoords[l * 6 + 0] = m3d->tmap[m3d->face[i].texcoord[0]].u;
				model.meshes[k].texcoords[l * 6 + 1] = 1.0f - m3d->tmap[m3d->face[i].texcoord[0]].v;
				model.meshes[k].texcoords[l * 6 + 2] = m3d->tmap[m3d->face[i].texcoord[1]].u;
				model.meshes[k].texcoords[l * 6 + 3] = 1.0f - m3d->tmap[m3d->face[i].texcoord[1]].v;
				model.meshes[k].texcoords[l * 6 + 4] = m3d->tmap[m3d->face[i].texcoord[2]].u;
				model.meshes[k].texcoords[l * 6 + 5] = 1.0f - m3d->tmap[m3d->face[i].texcoord[2]].v;
			}

			if (m3d->face[i].normal[0] != M3D_UNDEF)
			{
				model.meshes[k].normals[l * 9 + 0] = m3d->vertex[m3d->face[i].normal[0]].x;
				model.meshes[k].normals[l * 9 + 1] = m3d->vertex[m3d->face[i].normal[0]].y;
				model.meshes[k].normals[l * 9 + 2] = m3d->vertex[m3d->face[i].normal[0]].z;
				model.meshes[k].normals[l * 9 + 3] = m3d->vertex[m3d->face[i].normal[1]].x;
				model.meshes[k].normals[l * 9 + 4] = m3d->vertex[m3d->face[i].normal[1]].y;
				model.meshes[k].normals[l * 9 + 5] = m3d->vertex[m3d->face[i].normal[1]].z;
				model.meshes[k].normals[l * 9 + 6] = m3d->vertex[m3d->face[i].normal[2]].x;
				model.meshes[k].normals[l * 9 + 7] = m3d->vertex[m3d->face[i].normal[2]].y;
				model.meshes[k].normals[l * 9 + 8] = m3d->vertex[m3d->face[i].normal[2]].z;
			}

			// Add skin (vertex / bone weight pairs)
			if (m3d->numbone && m3d->numskin)
			{
				for (n = 0; n < 3; n++)
				{
					int skinid = m3d->vertex[m3d->face[i].vertex[n]].skinid;

					// Check if there is a skin for this mesh, should be, just failsafe
					if ((skinid != M3D_UNDEF) && (skinid < (int)m3d->numskin))
					{
						for (j = 0; j < 4; j++)
						{
							model.meshes[k].boneIds[l * 12 + n * 4 + j] = m3d->skin[skinid].boneid[j];
							model.meshes[k].boneWeights[l * 12 + n * 4 + j] = m3d->skin[skinid].weight[j];
						}
					}
					else
					{
						// does not handle boneless meshes with skeletal animations, so
						// we put all vertices without a bone into a special "no bone" bone
						model.meshes[k].boneIds[l * 12 + n * 4] = m3d->numbone;
						model.meshes[k].boneWeights[l * 12 + n * 4] = 1.0f;
					}
				}
			}
		}

		// Load materials
		for (i = 0; i < (int)m3d->nummaterial; i++)
		{
			model.materials[i + 1] = LoadMaterialDefault();

			for (j = 0; j < m3d->material[i].numprop; j++)
			{
				prop = &m3d->material[i].prop[j];

				switch (prop->type)
				{
				case m3dp_Kd:
				{
					memcpy(&model.materials[i + 1].maps[MATERIAL_MAP_DIFFUSE].color, &prop->value.color, 4);
					model.materials[i + 1].maps[MATERIAL_MAP_DIFFUSE].value = 0.0f;
				} break;
				case m3dp_Ks:
				{
					memcpy(&model.materials[i + 1].maps[MATERIAL_MAP_SPECULAR].color, &prop->value.color, 4);
				} break;
				case m3dp_Ns:
				{
					model.materials[i + 1].maps[MATERIAL_MAP_SPECULAR].value = prop->value.fnum;
				} break;
				case m3dp_Ke:
				{
					memcpy(&model.materials[i + 1].maps[MATERIAL_MAP_EMISSION].color, &prop->value.color, 4);
					model.materials[i + 1].maps[MATERIAL_MAP_EMISSION].value = 0.0f;
				} break;
				case m3dp_Pm:
				{
					model.materials[i + 1].maps[MATERIAL_MAP_METALNESS].value = prop->value.fnum;
				} break;
				case m3dp_Pr:
				{
					model.materials[i + 1].maps[MATERIAL_MAP_ROUGHNESS].value = prop->value.fnum;
				} break;
				case m3dp_Ps:
				{
					model.materials[i + 1].maps[MATERIAL_MAP_NORMAL].color = glm::vec4(1.0f); // WHITE
					model.materials[i + 1].maps[MATERIAL_MAP_NORMAL].value = prop->value.fnum;
				} break;
				default:
				{
					if (prop->type >= 128)
					{
						ImageRef image{ new Image() };

						Image::PixelFormat format = (m3d->texture[prop->value.textureid].f == 4) ? Image::PixelFormat::RGBA_U8:
							((m3d->texture[prop->value.textureid].f == 3) ? Image::PixelFormat::RGB_U8 :
							((m3d->texture[prop->value.textureid].f == 2) ? Image::PixelFormat::RG_U8 : Image::PixelFormat::R_U8));

						image->Create(format, m3d->texture[prop->value.textureid].w, m3d->texture[prop->value.textureid].h, m3d->texture[prop->value.textureid].d);

						switch (prop->type)
						{
						case m3dp_map_Kd: model.materials[i + 1].maps[MATERIAL_MAP_DIFFUSE].texture = render.CreateTexture2D(image); break; // TODO: не забыть про TextureInfo
						case m3dp_map_Ks: model.materials[i + 1].maps[MATERIAL_MAP_SPECULAR].texture = render.CreateTexture2D(image); break; // TODO: не забыть про TextureInfo
						case m3dp_map_Ke: model.materials[i + 1].maps[MATERIAL_MAP_EMISSION].texture = render.CreateTexture2D(image); break; // TODO: не забыть про TextureInfo
						case m3dp_map_Km: model.materials[i + 1].maps[MATERIAL_MAP_NORMAL].texture = render.CreateTexture2D(image); break; // TODO: не забыть про TextureInfo
						case m3dp_map_Ka: model.materials[i + 1].maps[MATERIAL_MAP_OCCLUSION].texture = render.CreateTexture2D(image); break; // TODO: не забыть про TextureInfo
						case m3dp_map_Pm: model.materials[i + 1].maps[MATERIAL_MAP_ROUGHNESS].texture = render.CreateTexture2D(image); break; // TODO: не забыть про TextureInfo
						default: break;
						}
					}
				} break;
				}
			}
		}

		// Load bones
		if (m3d->numbone)
		{
			model.boneCount = m3d->numbone + 1;
			model.bones = (NewBoneInfo*)calloc(model.boneCount, sizeof(NewBoneInfo));
			model.bindPose = (TempTransform*)calloc(model.boneCount, sizeof(TempTransform));

			for (i = 0; i < (int)m3d->numbone; i++)
			{
				model.bones[i].parent = m3d->bone[i].parent;
				strncpy_s(model.bones[i].name, m3d->bone[i].name, sizeof(model.bones[i].name));
				model.bindPose[i].translation.x = m3d->vertex[m3d->bone[i].pos].x * m3d->scale;
				model.bindPose[i].translation.y = m3d->vertex[m3d->bone[i].pos].y * m3d->scale;
				model.bindPose[i].translation.z = m3d->vertex[m3d->bone[i].pos].z * m3d->scale;
				model.bindPose[i].rotation.x = m3d->vertex[m3d->bone[i].ori].x;
				model.bindPose[i].rotation.y = m3d->vertex[m3d->bone[i].ori].y;
				model.bindPose[i].rotation.z = m3d->vertex[m3d->bone[i].ori].z;
				model.bindPose[i].rotation.w = m3d->vertex[m3d->bone[i].ori].w;

				// TODO: If the orientation quaternion is not normalized, then that's encoding scaling
				model.bindPose[i].rotation = glm::normalize(model.bindPose[i].rotation);
				model.bindPose[i].scale.x = model.bindPose[i].scale.y = model.bindPose[i].scale.z = 1.0f;

				// Child bones are stored in parent bone relative space, convert that into model space
				if (model.bones[i].parent >= 0)
				{
					model.bindPose[i].rotation = model.bindPose[model.bones[i].parent].rotation * model.bindPose[i].rotation;
					model.bindPose[i].translation = model.bindPose[model.bones[i].parent].rotation * model.bindPose[i].translation; // Vector3RotateByQuaternion
					model.bindPose[i].translation = model.bindPose[i].translation + model.bindPose[model.bones[i].parent].translation;
					model.bindPose[i].scale = model.bindPose[i].scale * model.bindPose[model.bones[i].parent].scale;
				}
			}

			// Add a special "no bone" bone
			model.bones[i].parent = -1;
			strcpy_s(model.bones[i].name, "NO BONE");
			model.bindPose[i].translation.x = 0.0f;
			model.bindPose[i].translation.y = 0.0f;
			model.bindPose[i].translation.z = 0.0f;
			model.bindPose[i].rotation.x = 0.0f;
			model.bindPose[i].rotation.y = 0.0f;
			model.bindPose[i].rotation.z = 0.0f;
			model.bindPose[i].rotation.w = 1.0f;
			model.bindPose[i].scale.x = model.bindPose[i].scale.y = model.bindPose[i].scale.z = 1.0f;
		}

		// Load bone-pose default mesh into animation vertices. These will be updated when UpdateModelAnimation gets
		// called, but not before, however DrawMesh uses these if they exist (so not good if they are left empty).
		if (m3d->numbone && m3d->numskin)
		{
			for (i = 0; i < model.meshes.size(); i++)
			{
				memcpy(model.meshes[i].animVertices, model.meshes[i].vertices, model.meshes[i].vertexCount * 3 * sizeof(float));
				memcpy(model.meshes[i].animNormals, model.meshes[i].normals, model.meshes[i].vertexCount * 3 * sizeof(float));
			}
		}

		m3d_free(m3d);
		free(fileData);
	}

	return model;
}

#define M3D_ANIMDELAY 17    // Animation frames delay, (~1000 ms/60 FPS = 16.666666* ms)
ModelAnimation* LoadModelAnimationsM3D(const std::string& fileName, unsigned int& animCount)
{
	ModelAnimation* animations = NULL;

	m3d_t* m3d = NULL;
	int i = 0, j = 0;
	animCount = 0;

	int dataSize = 0;
	unsigned char* fileData = LoadFileData(fileName.c_str(), &dataSize);

	if (fileData != NULL)
	{
		m3d = m3d_load(fileData, m3d_loaderhook, m3d_freehook, NULL);

		if (!m3d || M3D_ERR_ISFATAL(m3d->errcode))
		{
			LogWarning("MODEL: [" + fileName + "] Failed to load M3D data, error code " + std::to_string(m3d ? m3d->errcode : -2));
			free(fileData);
			return NULL;
		}
		else LogPrint("MODEL: [" + fileName + "] M3D data loaded successfully: " + std::to_string(m3d->numaction) + " animations, " + std::to_string(m3d->numbone) + " bones, " + std::to_string(m3d->numskin) + " skins");

		// No animation or bone+skin?
		if (!m3d->numaction || !m3d->numbone || !m3d->numskin)
		{
			m3d_free(m3d);
			free(fileData);
			return NULL;
		}

		animations = (ModelAnimation*)malloc(m3d->numaction * sizeof(ModelAnimation));
		animCount = m3d->numaction;

		for (unsigned int a = 0; a < m3d->numaction; a++)
		{
			animations[a].frameCount = m3d->action[a].durationmsec / M3D_ANIMDELAY;
			animations[a].boneCount = m3d->numbone + 1;
			animations[a].bones = (NewBoneInfo*)malloc((m3d->numbone + 1) * sizeof(NewBoneInfo));
			animations[a].framePoses = (TempTransform**)malloc(animations[a].frameCount * sizeof(TempTransform*));
			// strncpy(animations[a].name, m3d->action[a].name, sizeof(animations[a].name));
			LogPrint("MODEL: [" + fileName + "] animation #" + std::to_string(a) + ": " + std::to_string(m3d->action[a].durationmsec) + " msec, " + std::to_string(animations[a].frameCount) + " frames");

			for (i = 0; i < (int)m3d->numbone; i++)
			{
				animations[a].bones[i].parent = m3d->bone[i].parent;
				strncpy_s(animations[a].bones[i].name, m3d->bone[i].name, sizeof(animations[a].bones[i].name));
			}

			// A special, never transformed "no bone" bone, used for boneless vertices
			animations[a].bones[i].parent = -1;
			strcpy_s(animations[a].bones[i].name, "NO BONE");

			// M3D stores frames at arbitrary intervals with sparse skeletons. We need full skeletons at
			// regular intervals, so let the M3D SDK do the heavy lifting and calculate interpolated bones
			for (i = 0; i < animations[a].frameCount; i++)
			{
				animations[a].framePoses[i] = (TempTransform*)malloc((m3d->numbone + 1) * sizeof(TempTransform));

				m3db_t* pose = m3d_pose(m3d, a, i * M3D_ANIMDELAY);

				if (pose != NULL)
				{
					for (j = 0; j < (int)m3d->numbone; j++)
					{
						animations[a].framePoses[i][j].translation.x = m3d->vertex[pose[j].pos].x * m3d->scale;
						animations[a].framePoses[i][j].translation.y = m3d->vertex[pose[j].pos].y * m3d->scale;
						animations[a].framePoses[i][j].translation.z = m3d->vertex[pose[j].pos].z * m3d->scale;
						animations[a].framePoses[i][j].rotation.x = m3d->vertex[pose[j].ori].x;
						animations[a].framePoses[i][j].rotation.y = m3d->vertex[pose[j].ori].y;
						animations[a].framePoses[i][j].rotation.z = m3d->vertex[pose[j].ori].z;
						animations[a].framePoses[i][j].rotation.w = m3d->vertex[pose[j].ori].w;
						animations[a].framePoses[i][j].rotation = glm::normalize(animations[a].framePoses[i][j].rotation);
						animations[a].framePoses[i][j].scale.x = animations[a].framePoses[i][j].scale.y = animations[a].framePoses[i][j].scale.z = 1.0f;

						// Child bones are stored in parent bone relative space, convert that into model space
						if (animations[a].bones[j].parent >= 0)
						{
							animations[a].framePoses[i][j].rotation = animations[a].framePoses[i][animations[a].bones[j].parent].rotation * animations[a].framePoses[i][j].rotation;
							animations[a].framePoses[i][j].translation = animations[a].framePoses[i][animations[a].bones[j].parent].rotation * animations[a].framePoses[i][j].translation; // Vector3RotateByQuaternion
							animations[a].framePoses[i][j].translation = animations[a].framePoses[i][j].translation + animations[a].framePoses[i][animations[a].bones[j].parent].translation;
							animations[a].framePoses[i][j].scale = animations[a].framePoses[i][j].scale * animations[a].framePoses[i][animations[a].bones[j].parent].scale;
						}
					}

					// Default transform for the "no bone" bone
					animations[a].framePoses[i][j].translation.x = 0.0f;
					animations[a].framePoses[i][j].translation.y = 0.0f;
					animations[a].framePoses[i][j].translation.z = 0.0f;
					animations[a].framePoses[i][j].rotation.x = 0.0f;
					animations[a].framePoses[i][j].rotation.y = 0.0f;
					animations[a].framePoses[i][j].rotation.z = 0.0f;
					animations[a].framePoses[i][j].rotation.w = 1.0f;
					animations[a].framePoses[i][j].scale.x = animations[a].framePoses[i][j].scale.y = animations[a].framePoses[i][j].scale.z = 1.0f;
					free(pose);
				}
			}
		}

		m3d_free(m3d);
		free(fileData);
	}

	return animations;
}