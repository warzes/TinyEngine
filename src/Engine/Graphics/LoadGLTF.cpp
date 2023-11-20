#include "stdafx.h"
#include "GraphicsResource.h"
#include "GraphicsSystem.h"
#include "Core/IO/FileSystem.h"
#include "Core/IO/Image.h"
#include "RenderAPI/RenderSystem.h"
#include <cgltf.h>
//-----------------------------------------------------------------------------
NewMaterial LoadMaterialDefault();
const char* strprbrk(const char* s, const char* charset);
unsigned char* LoadFileData(const char* fileName, int* dataSize);
const char* GetDirectoryPath(const char* filePath);
tempVec3 Vector3Lerp(glm::vec3 v1, glm::vec3 v2, float amount);
tempVec4 QuaternionNlerp(glm::vec4 q1, glm::vec4 q2, float amount);
tempVec4 QuaternionSlerp(glm::vec4 q1, glm::vec4 q2, float amount);
//-----------------------------------------------------------------------------
// Build pose from parent joints
// NOTE: Required for animations loading (required by IQM and GLTF)
void BuildPoseFromParentJoints(NewBoneInfo* bones, int boneCount, TempTransform* transforms)
{
	for (int i = 0; i < boneCount; i++)
	{
		if (bones[i].parent >= 0)
		{
			if (bones[i].parent > i)
			{
				LogWarning("Assumes bones are toplogically sorted, but bone " + std::to_string(i) + " has parent " + std::to_string(bones[i].parent) + ". Skipping.");
				continue;
			}
			transforms[i].rotation = transforms[bones[i].parent].rotation * transforms[i].rotation;
			transforms[i].translation = transforms[bones[i].parent].rotation * transforms[i].translation; // Vector3RotateByQuaternion
			transforms[i].translation = transforms[i].translation + transforms[bones[i].parent].translation;
			transforms[i].scale = transforms[i].scale * transforms[bones[i].parent].scale;
		}
	}
}
//-----------------------------------------------------------------------------
// Load bone info from GLTF skin data
NewBoneInfo* LoadBoneInfoGLTF(cgltf_skin skin, int* boneCount)
{
	*boneCount = (int)skin.joints_count;
	NewBoneInfo* bones = (NewBoneInfo*)malloc(skin.joints_count * sizeof(NewBoneInfo));

	for (unsigned int i = 0; i < skin.joints_count; i++)
	{
		cgltf_node node = *skin.joints[i];
		strncpy_s(bones[i].name, node.name, sizeof(bones[i].name));

		// Find parent bone index
		unsigned int parentIndex = -1;

		for (unsigned int j = 0; j < skin.joints_count; j++)
		{
			if (skin.joints[j] == node.parent)
			{
				parentIndex = j;
				break;
			}
		}

		bones[i].parent = parentIndex;
	}

	return bones;
}
//-----------------------------------------------------------------------------
// Load image from different glTF provided methods (uri, path, buffer_view)
ImageRef LoadImageFromCgltfImage(cgltf_image* cgltfImage, const char* texPath)
{
	ImageRef image{ new Image() };

	if (cgltfImage->uri != NULL)     // Check if image data is provided as an uri (base64 or path)
	{
		if ((strlen(cgltfImage->uri) > 5) &&
			(cgltfImage->uri[0] == 'd') &&
			(cgltfImage->uri[1] == 'a') &&
			(cgltfImage->uri[2] == 't') &&
			(cgltfImage->uri[3] == 'a') &&
			(cgltfImage->uri[4] == ':'))     // Check if image is provided as base64 text data
		{
			// Data URI Format: data:<mediatype>;base64,<data>

			// Find the comma
			int i = 0;
			while ((cgltfImage->uri[i] != ',') && (cgltfImage->uri[i] != 0)) i++;

			if (cgltfImage->uri[i] == 0) 
				LogWarning("IMAGE: glTF data URI is not a valid image");
			else
			{
				int base64Size = (int)strlen(cgltfImage->uri + i + 1);
				int outSize = 3 * (base64Size / 4);         // TODO: Consider padding (-numberOfPaddingCharacters)
				void* data = NULL;

				cgltf_options options = { };
				cgltf_result result = cgltf_load_buffer_base64(&options, outSize, cgltfImage->uri + i + 1, &data);

				if (result == cgltf_result_success)
				{

					image->LoadFromMemory((uint8_t*)data, outSize);
					free(data);
				}
			}
		}
		else     // Check if image is provided as image path
		{
			image->LoadFromFile(std::string(texPath) + "/" + std::string(cgltfImage->uri));
		}
	}
	else if (cgltfImage->buffer_view->buffer->data != NULL)    // Check if image is provided as data buffer
	{
		unsigned char* data = (unsigned char*)malloc(cgltfImage->buffer_view->size);
		int offset = (int)cgltfImage->buffer_view->offset;
		int stride = (int)cgltfImage->buffer_view->stride ? (int)cgltfImage->buffer_view->stride : 1;

		// Copy buffer data to memory for loading
		for (unsigned int i = 0; i < cgltfImage->buffer_view->size; i++)
		{
			data[i] = ((unsigned char*)cgltfImage->buffer_view->buffer->data)[offset];
			offset += stride;
		}

		// Check mime_type for image: (cgltfImage->mime_type == "image/png")
		// NOTE: Detected that some models define mime_type as "image\\/png"
		if ((strcmp(cgltfImage->mime_type, "image\\/png") == 0) ||
			(strcmp(cgltfImage->mime_type, "image/png") == 0)) image->LoadFromMemory(data, (int)cgltfImage->buffer_view->size);
		else if ((strcmp(cgltfImage->mime_type, "image\\/jpeg") == 0) ||
			(strcmp(cgltfImage->mime_type, "image/jpeg") == 0)) image->LoadFromMemory(data, (int)cgltfImage->buffer_view->size);
		else LogWarning("MODEL: glTF image data MIME type not recognized" + std::string(texPath) + "/" + std::string(cgltfImage->uri));

		free(data);
	}

	return image;
}
//-----------------------------------------------------------------------------
// Load glTF file into model struct, .gltf and .glb supported
NewModel LoadGLTF(const std::string& fileName)
{
	/*********************************************************************************************

	FEATURES:
	- Supports .gltf and .glb files
	- Supports embedded (base64) or external textures
	- Supports PBR metallic/roughness flow, loads material textures, values and colors
	PBR specular/glossiness flow and extended texture flows not supported
	- Supports multiple meshes per model (every primitives is loaded as a separate mesh)
	- Supports basic animations

	RESTRICTIONS:
	- Only triangle meshes supported
	- Vertex attribute types and formats supported:
	> Vertices (position): vec3: float
	> Normals: vec3: float
	> Texcoords: vec2: float
	> Colors: vec4: u8, u16, f32 (normalized)
	> Indices: u16, u32 (truncated to u16)
	- Node hierarchies or transforms not supported

	***********************************************************************************************/

	// Macro to simplify attributes loading code
#define LOAD_ATTRIBUTE(accesor, numComp, dataType, dstPtr) \
	{ \
		int n = 0; \
		dataType *buffer = (dataType *)accesor->buffer_view->buffer->data + accesor->buffer_view->offset/sizeof(dataType) + accesor->offset/sizeof(dataType); \
		for (unsigned int k = 0; k < accesor->count; k++) \
		{\
			for (int l = 0; l < numComp; l++) \
			{\
				dstPtr[numComp*k + l] = buffer[n + l];\
			}\
			n += (int)(accesor->stride/sizeof(dataType));\
		}\
	}

	auto& render = GetRenderSystem();

	NewModel model = { };

	// glTF file loading
	int dataSize = 0;
	unsigned char* fileData = LoadFileData(fileName.c_str(), &dataSize);
	if (fileData == NULL) return model;

	// glTF data loading
	cgltf_options options = {};
	cgltf_data* data = NULL;
	cgltf_result result = cgltf_parse(&options, fileData, dataSize, &data);


	if (result == cgltf_result_success)
	{
		if (data->file_type == cgltf_file_type_glb) LogPrint("MODEL: [" + std::string(fileName) + "] Model basic data (glb) loaded successfully");
		else if (data->file_type == cgltf_file_type_gltf) LogPrint("MODEL: [" + std::string(fileName) + "] Model basic data (glTF) loaded successfully");
		else LogWarning("MODEL: [" + std::string(fileName) + "] Model format not recognized");

		LogPrint("    > Meshes count: " + std::to_string(data->meshes_count));
		LogPrint("    > Materials count: " + std::to_string(data->materials_count) + " (+1 default)");
		LogPrint("    > Buffers count: " + std::to_string(data->buffers_count));
		LogPrint("    > Images count: " + std::to_string(data->images_count));
		LogPrint("    > Textures count: " + std::to_string(data->textures_count));

		// Force reading data buffers (fills buffer_view->buffer->data)
		// NOTE: If an uri is defined to base64 data or external path, it's automatically loaded
		result = cgltf_load_buffers(&options, data, fileName.c_str());
		if (result != cgltf_result_success) LogPrint("MODEL: [" + std::string(fileName) + "] Failed to load mesh/material buffers");

		int primitivesCount = 0;
		// NOTE: We will load every primitive in the glTF as a separate mesh
		for (unsigned int i = 0; i < data->meshes_count; i++) primitivesCount += (int)data->meshes[i].primitives_count;

		// Load our model data: meshes and materials
		model.meshes.resize(primitivesCount);

		// NOTE: We keep an extra slot for default material, in case some mesh requires it
		model.materialCount = (int)data->materials_count + 1;
		model.materials = (NewMaterial*)calloc(model.materialCount, sizeof(NewMaterial));
		model.materials[0] = LoadMaterialDefault();     // Load default material (index: 0)

		// Load mesh-material indices, by default all meshes are mapped to material index: 0
		model.meshMaterial = (int*)calloc(model.meshes.size(), sizeof(int));

		// Load materials data
		//----------------------------------------------------------------------------------------------------
		for (unsigned int i = 0, j = 1; i < data->materials_count; i++, j++)
		{
			model.materials[j] = LoadMaterialDefault();
			const char* texPath = GetDirectoryPath(fileName.c_str());

			// Check glTF material flow: PBR metallic/roughness flow
			// NOTE: Alternatively, materials can follow PBR specular/glossiness flow
			if (data->materials[i].has_pbr_metallic_roughness)
			{
				// Load base color texture (albedo)
				if (data->materials[i].pbr_metallic_roughness.base_color_texture.texture)
				{
					ImageRef imAlbedo = LoadImageFromCgltfImage(data->materials[i].pbr_metallic_roughness.base_color_texture.texture->image, texPath);
					if (imAlbedo->IsValid())
					{
						model.materials[j].maps[MATERIAL_MAP_ALBEDO].texture = render.CreateTexture2D(imAlbedo); // TODO: не забыть про textureInfo
					}
				}
				// Load base color factor (tint)
				model.materials[j].maps[MATERIAL_MAP_ALBEDO].color.x = (unsigned char)(data->materials[i].pbr_metallic_roughness.base_color_factor[0] * 255);
				model.materials[j].maps[MATERIAL_MAP_ALBEDO].color.y = (unsigned char)(data->materials[i].pbr_metallic_roughness.base_color_factor[1] * 255);
				model.materials[j].maps[MATERIAL_MAP_ALBEDO].color.z = (unsigned char)(data->materials[i].pbr_metallic_roughness.base_color_factor[2] * 255);
				model.materials[j].maps[MATERIAL_MAP_ALBEDO].color.w
					= (unsigned char)(data->materials[i].pbr_metallic_roughness.base_color_factor[3] * 255);

				// Load metallic/roughness texture
				if (data->materials[i].pbr_metallic_roughness.metallic_roughness_texture.texture)
				{
					ImageRef imMetallicRoughness = LoadImageFromCgltfImage(data->materials[i].pbr_metallic_roughness.metallic_roughness_texture.texture->image, texPath);
					if (imMetallicRoughness->IsValid())
					{
						model.materials[j].maps[MATERIAL_MAP_ROUGHNESS].texture = render.CreateTexture2D(imMetallicRoughness); // TODO: не забыть про textureInfo
					}

					// Load metallic/roughness material properties
					float roughness = data->materials[i].pbr_metallic_roughness.roughness_factor;
					model.materials[j].maps[MATERIAL_MAP_ROUGHNESS].value = roughness;

					float metallic = data->materials[i].pbr_metallic_roughness.metallic_factor;
					model.materials[j].maps[MATERIAL_MAP_METALNESS].value = metallic;
				}

				// Load normal texture
				if (data->materials[i].normal_texture.texture)
				{
					ImageRef imNormal = LoadImageFromCgltfImage(data->materials[i].normal_texture.texture->image, texPath);
					if (imNormal->IsValid())
					{
						model.materials[j].maps[MATERIAL_MAP_NORMAL].texture = render.CreateTexture2D(imNormal); // TODO: не забыть про textureInfo
					}
				}

				// Load ambient occlusion texture
				if (data->materials[i].occlusion_texture.texture)
				{
					ImageRef imOcclusion = LoadImageFromCgltfImage(data->materials[i].occlusion_texture.texture->image, texPath);
					if (imOcclusion->IsValid())
					{
						model.materials[j].maps[MATERIAL_MAP_OCCLUSION].texture = render.CreateTexture2D(imOcclusion); // TODO: не забыть про textureInfo
					}
				}

				// Load emissive texture
				if (data->materials[i].emissive_texture.texture)
				{
					ImageRef imEmissive = LoadImageFromCgltfImage(data->materials[i].emissive_texture.texture->image, texPath);
					if (imEmissive->IsValid())
					{
						model.materials[j].maps[MATERIAL_MAP_EMISSION].texture = render.CreateTexture2D(imEmissive); // TODO: не забыть про textureInfo
					}

					// Load emissive color factor
					model.materials[j].maps[MATERIAL_MAP_EMISSION].color.x = (unsigned char)(data->materials[i].emissive_factor[0] * 255);
					model.materials[j].maps[MATERIAL_MAP_EMISSION].color.y = (unsigned char)(data->materials[i].emissive_factor[1] * 255);
					model.materials[j].maps[MATERIAL_MAP_EMISSION].color.z = (unsigned char)(data->materials[i].emissive_factor[2] * 255);
					model.materials[j].maps[MATERIAL_MAP_EMISSION].color.w = 255;
				}
			}

			// Other possible materials not supported by pipeline:
			// has_clearcoat, has_transmission, has_volume, has_ior, has specular, has_sheen
		}

		// Load meshes data
		//----------------------------------------------------------------------------------------------------
		for (unsigned int i = 0, meshIndex = 0; i < data->meshes_count; i++)
		{
			// NOTE: meshIndex accumulates primitives

			for (unsigned int p = 0; p < data->meshes[i].primitives_count; p++)
			{
				// NOTE: We only support primitives defined by triangles
				// Other alternatives: points, lines, line_strip, triangle_strip
				if (data->meshes[i].primitives[p].type != cgltf_primitive_type_triangles) continue;

				// NOTE: Attributes data could be provided in several data formats (8, 8u, 16u, 32...),
				// Only some formats for each attribute type are supported, read info at the top of this function!

				for (unsigned int j = 0; j < data->meshes[i].primitives[p].attributes_count; j++)
				{
					// Check the different attributes for every primitive
					if (data->meshes[i].primitives[p].attributes[j].type == cgltf_attribute_type_position)      // POSITION
					{
						cgltf_accessor* attribute = data->meshes[i].primitives[p].attributes[j].data;

						// WARNING: SPECS: POSITION accessor MUST have its min and max properties defined.

						if ((attribute->component_type == cgltf_component_type_r_32f) && (attribute->type == cgltf_type_vec3))
						{
							// Init mesh vertices to copy glTF attribute data
							model.meshes[meshIndex].vertexCount = (int)attribute->count;
							model.meshes[meshIndex].vertices = (float*)malloc(attribute->count * 3 * sizeof(float));

							// Load 3 components of float data type into mesh.vertices
							LOAD_ATTRIBUTE(attribute, 3, float, model.meshes[meshIndex].vertices)
						}
						else LogWarning("MODEL: [" + std::string(fileName) + "] Vertices attribute data format not supported, use vec3 float");
					}
					else if (data->meshes[i].primitives[p].attributes[j].type == cgltf_attribute_type_normal)   // NORMAL
					{
						cgltf_accessor* attribute = data->meshes[i].primitives[p].attributes[j].data;

						if ((attribute->component_type == cgltf_component_type_r_32f) && (attribute->type == cgltf_type_vec3))
						{
							// Init mesh normals to copy glTF attribute data
							model.meshes[meshIndex].normals = (float*)malloc(attribute->count * 3 * sizeof(float));

							// Load 3 components of float data type into mesh.normals
							LOAD_ATTRIBUTE(attribute, 3, float, model.meshes[meshIndex].normals)
						}
						else LogWarning("MODEL: [" + std::string(fileName) + "] Normal attribute data format not supported, use vec3 float");
					}
					else if (data->meshes[i].primitives[p].attributes[j].type == cgltf_attribute_type_tangent)   // TANGENT
					{
						cgltf_accessor* attribute = data->meshes[i].primitives[p].attributes[j].data;

						if ((attribute->component_type == cgltf_component_type_r_32f) && (attribute->type == cgltf_type_vec4))
						{
							// Init mesh tangent to copy glTF attribute data
							model.meshes[meshIndex].tangents = (float*)malloc(attribute->count * 4 * sizeof(float));

							// Load 4 components of float data type into mesh.tangents
							LOAD_ATTRIBUTE(attribute, 4, float, model.meshes[meshIndex].tangents)
						}
						else LogWarning("MODEL: [" + std::string(fileName) + "] Tangent attribute data format not supported, use vec4 float");
					}
					else if (data->meshes[i].primitives[p].attributes[j].type == cgltf_attribute_type_texcoord) // TEXCOORD_0
					{
						// TODO: Support additional texture coordinates: TEXCOORD_1 -> mesh.texcoords2

						cgltf_accessor* attribute = data->meshes[i].primitives[p].attributes[j].data;

						if ((attribute->component_type == cgltf_component_type_r_32f) && (attribute->type == cgltf_type_vec2))
						{
							// Init mesh texcoords to copy glTF attribute data
							model.meshes[meshIndex].texcoords = (float*)malloc(attribute->count * 2 * sizeof(float));

							// Load 3 components of float data type into mesh.texcoords
							LOAD_ATTRIBUTE(attribute, 2, float, model.meshes[meshIndex].texcoords)
						}
						else LogWarning("MODEL: [" + std::string(fileName) + "] Texcoords attribute data format not supported, use vec2 float");
					}
					else if (data->meshes[i].primitives[p].attributes[j].type == cgltf_attribute_type_color)    // COLOR_0
					{
						cgltf_accessor* attribute = data->meshes[i].primitives[p].attributes[j].data;

						// WARNING: SPECS: All components of each COLOR_n accessor element MUST be clamped to [0.0, 1.0] range.

						if ((attribute->component_type == cgltf_component_type_r_8u) && (attribute->type == cgltf_type_vec4))
						{
							// Init mesh color to copy glTF attribute data
							model.meshes[meshIndex].colors = (unsigned char*)malloc(attribute->count * 4 * sizeof(unsigned char));

							// Load 4 components of unsigned char data type into mesh.colors
							LOAD_ATTRIBUTE(attribute, 4, unsigned char, model.meshes[meshIndex].colors)
						}
						else if ((attribute->component_type == cgltf_component_type_r_16u) && (attribute->type == cgltf_type_vec4))
						{
							// Init mesh color to copy glTF attribute data
							model.meshes[meshIndex].colors = (unsigned char*)malloc(attribute->count * 4 * sizeof(unsigned char));

							// Load data into a temp buffer to be converted to data type
							unsigned short* temp = (unsigned short*)malloc(attribute->count * 4 * sizeof(unsigned short));
							LOAD_ATTRIBUTE(attribute, 4, unsigned short, temp);

							// Convert data to color data type (4 bytes)
							for (unsigned int c = 0; c < attribute->count * 4; c++) model.meshes[meshIndex].colors[c] = (unsigned char)(((float)temp[c] / 65535.0f) * 255.0f);

							free(temp);
						}
						else if ((attribute->component_type == cgltf_component_type_r_32f) && (attribute->type == cgltf_type_vec4))
						{
							// Init mesh color to copy glTF attribute data
							model.meshes[meshIndex].colors = (unsigned char*)malloc(attribute->count * 4 * sizeof(unsigned char));

							// Load data into a temp buffer to be converted to data type
							float* temp = (float*)malloc(attribute->count * 4 * sizeof(float));
							LOAD_ATTRIBUTE(attribute, 4, float, temp);

							// Convert data to color data type (4 bytes), we expect the color data normalized
							for (unsigned int c = 0; c < attribute->count * 4; c++) model.meshes[meshIndex].colors[c] = (unsigned char)(temp[c] * 255.0f);

							free(temp);
						}
						else LogWarning("MODEL: [" + std::string(fileName) + "] Color attribute data format not supported");
					}

					// NOTE: Attributes related to animations are processed separately
				}

				// Load primitive indices data (if provided)
				if (data->meshes[i].primitives[p].indices != NULL)
				{
					cgltf_accessor* attribute = data->meshes[i].primitives[p].indices;

					model.meshes[meshIndex].triangleCount = (int)attribute->count / 3;

					if (attribute->component_type == cgltf_component_type_r_16u)
					{
						// Init mesh indices to copy glTF attribute data
						model.meshes[meshIndex].indices.resize(attribute->count);

						// Load unsigned short data type into mesh.indices
						LOAD_ATTRIBUTE(attribute, 1, unsigned short, model.meshes[meshIndex].indices.data())
					}
					else if (attribute->component_type == cgltf_component_type_r_32u)
					{
						// Init mesh indices to copy glTF attribute data
						model.meshes[meshIndex].indices.resize(attribute->count);

						// Load data into a temp buffer to be converted to data type
						unsigned int* temp = (unsigned int*)malloc(attribute->count * sizeof(unsigned int));
						LOAD_ATTRIBUTE(attribute, 1, unsigned int, temp);

						// Convert data to indices data type (unsigned short)
						for (unsigned int d = 0; d < attribute->count; d++) model.meshes[meshIndex].indices[d] = (unsigned)temp[d];

						LogWarning("MODEL: [" + std::string(fileName) + "] Indices data converted from u32 to u16, possible loss of data");

						free(temp);
					}
					else LogWarning("MODEL: [" + std::string(fileName) + "] Indices data format not supported, use u16");
				}
				else model.meshes[meshIndex].triangleCount = model.meshes[meshIndex].vertexCount / 3;    // Unindexed mesh

				// Assign to the primitive mesh the corresponding material index
				// NOTE: If no material defined, mesh uses the already assigned default material (index: 0)
				for (unsigned int m = 0; m < data->materials_count; m++)
				{
					// The primitive actually keeps the pointer to the corresponding material,
					// instead assigns to the mesh the by its index, as loaded in model.materials array
					// To get the index, we check if material pointers match, and we assign the corresponding index,
					// skipping index 0, the default material
					if (&data->materials[m] == data->meshes[i].primitives[p].material)
					{
						model.meshMaterial[meshIndex] = m + 1;
						break;
					}
				}

				meshIndex++;       // Move to next mesh
			}
		}

		// Load glTF meshes animation data
		// REF: https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#skins
		// REF: https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html#skinned-mesh-attributes
		//
		// LIMITATIONS:
		//  - Only supports 1 armature per file, and skips loading it if there are multiple armatures
		//  - Only supports linear interpolation (default method in Blender when checked "Always Sample Animations" when exporting a GLTF file)
		//  - Only supports translation/rotation/scale animation channel.path, weights not considered (i.e. morph targets)
		//----------------------------------------------------------------------------------------------------
		if (data->skins_count == 1)
		{
			cgltf_skin skin = data->skins[0];
			model.bones = LoadBoneInfoGLTF(skin, &model.boneCount);
			model.bindPose = (TempTransform*)malloc(model.boneCount * sizeof(TempTransform));

			for (int i = 0; i < model.boneCount; i++)
			{
				cgltf_node node = *skin.joints[i];
				model.bindPose[i].translation.x = node.translation[0];
				model.bindPose[i].translation.y = node.translation[1];
				model.bindPose[i].translation.z = node.translation[2];

				model.bindPose[i].rotation.x = node.rotation[0];
				model.bindPose[i].rotation.y = node.rotation[1];
				model.bindPose[i].rotation.z = node.rotation[2];
				model.bindPose[i].rotation.w = node.rotation[3];

				model.bindPose[i].scale.x = node.scale[0];
				model.bindPose[i].scale.y = node.scale[1];
				model.bindPose[i].scale.z = node.scale[2];
			}

			BuildPoseFromParentJoints(model.bones, model.boneCount, model.bindPose);
		}
		else if (data->skins_count > 1)
		{
			LogError("MODEL: [" + std::string(fileName) + "] can only load one skin (armature) per model, but gltf skins_count == " + std::to_string(data->skins_count));
		}

		for (unsigned int i = 0, meshIndex = 0; i < data->meshes_count; i++)
		{
			for (unsigned int p = 0; p < data->meshes[i].primitives_count; p++)
			{
				// NOTE: We only support primitives defined by triangles
				if (data->meshes[i].primitives[p].type != cgltf_primitive_type_triangles) continue;

				for (unsigned int j = 0; j < data->meshes[i].primitives[p].attributes_count; j++)
				{
					// NOTE: JOINTS_1 + WEIGHT_1 will be used for +4 joints influencing a vertex -> Not supported

					if (data->meshes[i].primitives[p].attributes[j].type == cgltf_attribute_type_joints)        // JOINTS_n (vec4: 4 bones max per vertex / u8, u16)
					{
						cgltf_accessor* attribute = data->meshes[i].primitives[p].attributes[j].data;

						if ((attribute->component_type == cgltf_component_type_r_8u) && (attribute->type == cgltf_type_vec4))
						{
							// Handle 8-bit unsigned byte, vec4 format
							model.meshes[meshIndex].boneIds = (unsigned char*)calloc(model.meshes[meshIndex].vertexCount * 4, sizeof(unsigned char));

							LOAD_ATTRIBUTE(attribute, 4, unsigned char, model.meshes[meshIndex].boneIds)
						}
						else if ((attribute->component_type == cgltf_component_type_r_16u) && (attribute->type == cgltf_type_vec2))
						{
							// Handle 16-bit unsigned short, vec2 format
							model.meshes[meshIndex].boneIds = (unsigned char*)calloc(model.meshes[meshIndex].vertexCount * 2, sizeof(unsigned short));
							LOAD_ATTRIBUTE(attribute, 2, unsigned short, model.meshes[meshIndex].boneIds)
						}
						else if ((attribute->component_type == cgltf_component_type_r_32u) && (attribute->type == cgltf_type_vec4))
						{
							// Handle 32-bit unsigned int, vec4 format
							model.meshes[meshIndex].boneIds = (unsigned char*)calloc(model.meshes[meshIndex].vertexCount * 4, sizeof(unsigned int));
							LOAD_ATTRIBUTE(attribute, 4, unsigned int, model.meshes[meshIndex].boneIds)
						}
						else if ((attribute->component_type == cgltf_component_type_r_32f) && (attribute->type == cgltf_type_vec2))
						{
							// Handle 32-bit float, vec2 format
							model.meshes[meshIndex].boneIds = (unsigned char*)calloc(model.meshes[meshIndex].vertexCount * 2, sizeof(float));
							LOAD_ATTRIBUTE(attribute, 2, float, model.meshes[meshIndex].boneIds)
						}
						else LogWarning("MODEL: [" + std::string(fileName) + "] Joint attribute data format not supported");
					}
					else if (data->meshes[i].primitives[p].attributes[j].type == cgltf_attribute_type_weights)  // WEIGHTS_n (vec4 / u8, u16, f32)
					{
						cgltf_accessor* attribute = data->meshes[i].primitives[p].attributes[j].data;

						if ((attribute->component_type == cgltf_component_type_r_32f) && (attribute->type == cgltf_type_vec4))
						{
							// Init mesh bone weight to copy glTF attribute data
							model.meshes[meshIndex].boneWeights = (float*)calloc(model.meshes[meshIndex].vertexCount * 4, sizeof(float));

							// Load 4 components of float data type into mesh.boneWeights
							// for cgltf_attribute_type_weights we have:
							//   - data.meshes[0] (256 vertices)
							//   - 256 values, provided as cgltf_type_vec4 of float (4 byte per joint, stride 16)
							LOAD_ATTRIBUTE(attribute, 4, float, model.meshes[meshIndex].boneWeights)
						}
						else LogWarning("MODEL: [" + std::string(fileName) + "] Joint weight attribute data format not supported, use vec4 float");
					}
				}

				// Animated vertex data
				model.meshes[meshIndex].animVertices = (float*)calloc(model.meshes[meshIndex].vertexCount * 3, sizeof(float));
				memcpy(model.meshes[meshIndex].animVertices, model.meshes[meshIndex].vertices, model.meshes[meshIndex].vertexCount * 3 * sizeof(float));
				model.meshes[meshIndex].animNormals = (float*)calloc(model.meshes[meshIndex].vertexCount * 3, sizeof(float));
				if (model.meshes[meshIndex].normals != NULL) {
					memcpy(model.meshes[meshIndex].animNormals, model.meshes[meshIndex].normals, model.meshes[meshIndex].vertexCount * 3 * sizeof(float));
				}

				meshIndex++;       // Move to next mesh
			}

		}

		// Free all cgltf loaded data
		cgltf_free(data);
	}
	else LogWarning("MODEL: [" + std::string(fileName) + "] Failed to load glTF data");

	// WARNING: cgltf requires the file pointer available while reading data
	free(fileData);

	return model;
}
//-----------------------------------------------------------------------------
// Get interpolated pose for bone sampler at a specific time. Returns true on success.
bool GetPoseAtTimeGLTF(cgltf_accessor* input, cgltf_accessor* output, float time, void* data)
{
	// Input and output should have the same count
	float tstart = 0.0f;
	float tend = 0.0f;
	int keyframe = 0;       // Defaults to first pose

	for (int i = 0; i < (int)input->count - 1; i++)
	{
		cgltf_bool r1 = cgltf_accessor_read_float(input, i, &tstart, 1);
		if (!r1) return false;

		cgltf_bool r2 = cgltf_accessor_read_float(input, i + 1, &tend, 1);
		if (!r2) return false;

		if ((tstart <= time) && (time < tend))
		{
			keyframe = i;
			break;
		}
	}

	float t = (time - tstart) / (tend - tstart);
	t = (t < 0.0f) ? 0.0f : t;
	t = (t > 1.0f) ? 1.0f : t;

	if (output->component_type != cgltf_component_type_r_32f) return false;

	if (output->type == cgltf_type_vec3)
	{
		float tmp[3] = { 0.0f };
		cgltf_accessor_read_float(output, keyframe, tmp, 3);
		glm::vec3 v1 = { tmp[0], tmp[1], tmp[2] };
		cgltf_accessor_read_float(output, keyframe + 1, tmp, 3);
		glm::vec3 v2 = { tmp[0], tmp[1], tmp[2] };

		tempVec3* r = (tempVec3*)data;
		*r = Vector3Lerp(v1, v2, t);
	}
	else if (output->type == cgltf_type_vec4)
	{
		float tmp[4] = { 0.0f };
		cgltf_accessor_read_float(output, keyframe, tmp, 4);
		glm::vec4 v1 = { tmp[0], tmp[1], tmp[2], tmp[3] };
		cgltf_accessor_read_float(output, keyframe + 1, tmp, 4);
		glm::vec4 v2 = { tmp[0], tmp[1], tmp[2], tmp[3] };
		tempVec4* r = (tempVec4*)data;

		// Only v4 is for rotations, so we know it's a quaternion
		*r = QuaternionSlerp(v1, v2, t);
	}

	return true;
}
//-----------------------------------------------------------------------------
#define GLTF_ANIMDELAY 17    // Animation frames delay, (~1000 ms/60 FPS = 16.666666* ms)
ModelAnimation* LoadModelAnimationsGLTF(const std::string& fileName, unsigned int& animCount)
{
	// glTF file loading
	int dataSize = 0;
	unsigned char* fileData = LoadFileData(fileName.c_str(), &dataSize);

	ModelAnimation* animations = NULL;

	// glTF data loading
	cgltf_options options = { };
	cgltf_data* data = NULL;
	cgltf_result result = cgltf_parse(&options, fileData, dataSize, &data);

	if (result != cgltf_result_success)
	{
		LogWarning("MODEL: [" + std::string(fileName) + "] Failed to load glTF data");
		animCount = 0;
		return NULL;
	}

	result = cgltf_load_buffers(&options, data, fileName.c_str());
	if (result != cgltf_result_success) LogPrint("MODEL: [" + std::string(fileName) + "] Failed to load animation buffers");

	if (result == cgltf_result_success)
	{
		if (data->skins_count == 1)
		{
			cgltf_skin skin = data->skins[0];
			animCount = (unsigned)data->animations_count;
			animations = (ModelAnimation*)malloc(data->animations_count * sizeof(ModelAnimation));

			for (unsigned int i = 0; i < data->animations_count; i++)
			{
				animations[i].bones = LoadBoneInfoGLTF(skin, &animations[i].boneCount);

				cgltf_animation animData = data->animations[i];

				struct Channels {
					cgltf_animation_channel* translate;
					cgltf_animation_channel* rotate;
					cgltf_animation_channel* scale;
				};

				Channels* boneChannels = (Channels*)calloc(animations[i].boneCount, sizeof(Channels));
				float animDuration = 0.0f;

				for (unsigned int j = 0; j < animData.channels_count; j++)
				{
					cgltf_animation_channel channel = animData.channels[j];
					int boneIndex = -1;

					for (unsigned int k = 0; k < skin.joints_count; k++)
					{
						if (animData.channels[j].target_node == skin.joints[k])
						{
							boneIndex = k;
							break;
						}
					}

					if (boneIndex == -1)
					{
						// Animation channel for a node not in the armature
						continue;
					}

					if (animData.channels[j].sampler->interpolation == cgltf_interpolation_type_linear)
					{
						if (channel.target_path == cgltf_animation_path_type_translation)
						{
							boneChannels[boneIndex].translate = &animData.channels[j];
						}
						else if (channel.target_path == cgltf_animation_path_type_rotation)
						{
							boneChannels[boneIndex].rotate = &animData.channels[j];
						}
						else if (channel.target_path == cgltf_animation_path_type_scale)
						{
							boneChannels[boneIndex].scale = &animData.channels[j];
						}
						else
						{
							LogWarning("MODEL: [" + std::string(fileName) + "] Unsupported target_path on channel " + std::to_string(j) + "'s sampler for animation " + std::to_string(i) + ". Skipping.");
						}
					}
					else LogWarning("MODEL: [" + std::string(fileName) + "] Only linear interpolation curves are supported for GLTF animation.");

					float t = 0.0f;
					cgltf_bool r = cgltf_accessor_read_float(channel.sampler->input, channel.sampler->input->count - 1, &t, 1);

					if (!r)
					{
						LogWarning("MODEL: [" + std::string(fileName) + "] Failed to load input time");
						continue;
					}

					animDuration = (t > animDuration) ? t : animDuration;
				}

				strncpy_s(animations[i].name, animData.name, sizeof(animations[i].name));
				animations[i].name[sizeof(animations[i].name) - 1] = '\0';

				animations[i].frameCount = (int)(animDuration * 1000.0f / GLTF_ANIMDELAY);
				animations[i].framePoses = (TempTransform**)malloc(animations[i].frameCount * sizeof(TempTransform*));

				for (int j = 0; j < animations[i].frameCount; j++)
				{
					animations[i].framePoses[j] = (TempTransform*)malloc(animations[i].boneCount * sizeof(TempTransform));
					float time = ((float)j * GLTF_ANIMDELAY) / 1000.0f;

					for (int k = 0; k < animations[i].boneCount; k++)
					{
						glm::vec3 translation = { 0, 0, 0 };
						//glm::quat rotation = { 1, 0, 0, 0 };
						glm::vec4 rotation = { 0, 0, 0, 1 }; // GetPoseAtTimeGLTF() работает с vec4 в которм w последний (в кватернионе он первый). потом пофиксить
						glm::vec3 scale = { 1, 1, 1 };

						if (boneChannels[k].translate)
						{
							if (!GetPoseAtTimeGLTF(boneChannels[k].translate->sampler->input, boneChannels[k].translate->sampler->output, time, &translation))
							{
								LogPrint("MODEL: [" + std::string(fileName) + "] Failed to load translate pose data for bone " + std::string(animations[i].bones[k].name));
							}
						}

						if (boneChannels[k].rotate)
						{
							if (!GetPoseAtTimeGLTF(boneChannels[k].rotate->sampler->input, boneChannels[k].rotate->sampler->output, time, &rotation))
							{
								LogPrint("MODEL: [" + std::string(fileName) + "] Failed to load rotate pose data for bone " + std::string(animations[i].bones[k].name));
							}
						}

						if (boneChannels[k].scale)
						{
							if (!GetPoseAtTimeGLTF(boneChannels[k].scale->sampler->input, boneChannels[k].scale->sampler->output, time, &scale))
							{
								LogPrint("MODEL: [" + std::string(fileName) + "] Failed to load scale pose data for bone " + std::string(animations[i].bones[k].name));
							}
						}

						animations[i].framePoses[j][k] = TempTransform{
							.translation = translation,
							.rotation = {rotation.w, rotation.x, rotation.y, rotation.z},
							.scale = scale
						};
					}

					BuildPoseFromParentJoints(animations[i].bones, animations[i].boneCount, animations[i].framePoses[j]);
				}

				LogPrint("MODEL: [" + std::string(fileName) + "] Loaded animation: " + std::string(animData.name) + " (" + std::to_string(animations[i].frameCount) + " frames, " + std::to_string(animDuration) + "s)");
				free(boneChannels);
			}
		}
		else LogError("MODEL: [" + std::string(fileName) + "] expected exactly one skin to load animation data from, but found " + std::to_string(data->skins_count));

		cgltf_free(data);
	}
	free(fileData);
	return animations;
}
//-----------------------------------------------------------------------------