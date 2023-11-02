#include "stdafx.h"
#include "GraphicsResource.h"
#include "GraphicsSystem.h"
#include "Core/IO/FileSystem.h"

#include <cgltf.h>

// Load data from file into a buffer
unsigned char* LoadFileData(const char* fileName, int* dataSize)
{
	unsigned char* data = NULL;
	*dataSize = 0;

	if (fileName != NULL)
	{
		FILE* file = nullptr;
		errno_t err = fopen_s(&file, fileName, "rb");

		if (file != nullptr && err == 0)
		{
			// WARNING: On binary streams SEEK_END could not be found,
			// using fseek() and ftell() could not work in some (rare) cases
			fseek(file, 0, SEEK_END);
			int size = ftell(file);     // WARNING: ftell() returns 'long int', maximum size returned is INT_MAX (2147483647 bytes)
			fseek(file, 0, SEEK_SET);

			if (size > 0)
			{
				data = (unsigned char*)malloc(size * sizeof(unsigned char));

				if (data != NULL)
				{
					// NOTE: fread() returns number of read elements instead of bytes, so we read [1 byte, size elements]
					size_t count = fread(data, sizeof(unsigned char), size, file);

					// WARNING: fread() returns a size_t value, usually 'unsigned int' (32bit compilation) and 'unsigned long long' (64bit compilation)
					// dataSize is unified along raylib as a 'int' type, so, for file-sizes > INT_MAX (2147483647 bytes) we have a limitation
					if (count > 2147483647)
					{
						LogWarning("FILEIO: [" + std::string(fileName) + "] File is bigger than 2147483647 bytes, avoid using LoadFileData()");

						free(data);
						data = NULL;
					}
					else
					{
						*dataSize = (int)count;

						if ((*dataSize) != size)
							LogWarning("FILEIO: [" + std::string(fileName) + "] File partially loaded (" + std::to_string(*dataSize) + " bytes out of %" + std::to_string(*dataSize) + ")");
						else LogPrint("FILEIO: [" + std::string(fileName) + "] File loaded successfully");
					}
				}
				else LogWarning("FILEIO: [" + std::string(fileName) + "] Failed to allocated memory for file reading");
			}
			else LogWarning("FILEIO: [" + std::string(fileName) + "] Failed to read file");

			fclose(file);
		}
		else LogWarning("FILEIO: [" + std::string(fileName) + "] Failed to open file");
	}
	else LogWarning("FILEIO: File name provided is not valid");

	return data;
}

// Load glTF file into model struct, .gltf and .glb supported
NewModel LoadGLTF(const char* fileName)
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

	NewModel model = { };

	// glTF file loading
	int dataSize = 0;
	unsigned char* fileData = LoadFileData(fileName, &dataSize);
	if (fileData == NULL) return model;

	// glTF data loading
	cgltf_options options = {};
	cgltf_data* data = NULL;
	cgltf_result result = cgltf_parse(&options, fileData, dataSize, &data);


	if (result == cgltf_result_success)
	{
		if (data->file_type == cgltf_file_type_glb) LogPrint("MODEL: [" + std::string(fileName) +  "] Model basic data (glb) loaded successfully");
		else if (data->file_type == cgltf_file_type_gltf) LogPrint("MODEL: [" + std::string(fileName) + "] Model basic data (glTF) loaded successfully");
		else LogWarning("MODEL: [" + std::string(fileName) + "] Model format not recognized");

		LogPrint("    > Meshes count: " + std::to_string(data->meshes_count));
		LogPrint("    > Materials count: " + std::to_string(data->materials_count) + " (+1 default)");
		LogPrint("    > Buffers count: " + std::to_string(data->buffers_count));
		LogPrint("    > Images count: " + std::to_string(data->images_count));
		LogPrint("    > Textures count: " + std::to_string(data->textures_count));

		// Force reading data buffers (fills buffer_view->buffer->data)
		// NOTE: If an uri is defined to base64 data or external path, it's automatically loaded
		result = cgltf_load_buffers(&options, data, fileName);
		if (result != cgltf_result_success) LogPrint("MODEL: [" + std::string(fileName) + "] Failed to load mesh/material buffers");

		int primitivesCount = 0;
		// NOTE: We will load every primitive in the glTF as a separate raylib mesh
		for (unsigned int i = 0; i < data->meshes_count; i++) primitivesCount += (int)data->meshes[i].primitives_count;

		// Load our model data: meshes and materials
		model.meshCount = primitivesCount;
		model.meshes = (NewMesh*)calloc(model.meshCount, sizeof(NewMesh));

		// NOTE: We keep an extra slot for default material, in case some mesh requires it
		model.materialCount = (int)data->materials_count + 1;
		model.materials = (NewMaterial*)calloc(model.materialCount, sizeof(NewMaterial));
		model.materials[0] = LoadMaterialDefault();     // Load default material (index: 0)

		// Load mesh-material indices, by default all meshes are mapped to material index: 0
		model.meshMaterial = (int*)calloc(model.meshCount, sizeof(int));

		// Load materials data
		//----------------------------------------------------------------------------------------------------
		for (unsigned int i = 0, j = 1; i < data->materials_count; i++, j++)
		{
			model.materials[j] = LoadMaterialDefault();
			const char* texPath = GetDirectoryPath(fileName);

			// Check glTF material flow: PBR metallic/roughness flow
			// NOTE: Alternatively, materials can follow PBR specular/glossiness flow
			if (data->materials[i].has_pbr_metallic_roughness)
			{
				// Load base color texture (albedo)
				if (data->materials[i].pbr_metallic_roughness.base_color_texture.texture)
				{
					Image imAlbedo = LoadImageFromCgltfImage(data->materials[i].pbr_metallic_roughness.base_color_texture.texture->image, texPath);
					if (imAlbedo.data != NULL)
					{
						model.materials[j].maps[MATERIAL_MAP_ALBEDO].texture = LoadTextureFromImage(imAlbedo);
						UnloadImage(imAlbedo);
					}
				}
				// Load base color factor (tint)
				model.materials[j].maps[MATERIAL_MAP_ALBEDO].color.r = (unsigned char)(data->materials[i].pbr_metallic_roughness.base_color_factor[0] * 255);
				model.materials[j].maps[MATERIAL_MAP_ALBEDO].color.g = (unsigned char)(data->materials[i].pbr_metallic_roughness.base_color_factor[1] * 255);
				model.materials[j].maps[MATERIAL_MAP_ALBEDO].color.b = (unsigned char)(data->materials[i].pbr_metallic_roughness.base_color_factor[2] * 255);
				model.materials[j].maps[MATERIAL_MAP_ALBEDO].color.a = (unsigned char)(data->materials[i].pbr_metallic_roughness.base_color_factor[3] * 255);

				// Load metallic/roughness texture
				if (data->materials[i].pbr_metallic_roughness.metallic_roughness_texture.texture)
				{
					Image imMetallicRoughness = LoadImageFromCgltfImage(data->materials[i].pbr_metallic_roughness.metallic_roughness_texture.texture->image, texPath);
					if (imMetallicRoughness.data != NULL)
					{
						model.materials[j].maps[MATERIAL_MAP_ROUGHNESS].texture = LoadTextureFromImage(imMetallicRoughness);
						UnloadImage(imMetallicRoughness);
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
					Image imNormal = LoadImageFromCgltfImage(data->materials[i].normal_texture.texture->image, texPath);
					if (imNormal.data != NULL)
					{
						model.materials[j].maps[MATERIAL_MAP_NORMAL].texture = LoadTextureFromImage(imNormal);
						UnloadImage(imNormal);
					}
				}

				// Load ambient occlusion texture
				if (data->materials[i].occlusion_texture.texture)
				{
					Image imOcclusion = LoadImageFromCgltfImage(data->materials[i].occlusion_texture.texture->image, texPath);
					if (imOcclusion.data != NULL)
					{
						model.materials[j].maps[MATERIAL_MAP_OCCLUSION].texture = LoadTextureFromImage(imOcclusion);
						UnloadImage(imOcclusion);
					}
				}

				// Load emissive texture
				if (data->materials[i].emissive_texture.texture)
				{
					Image imEmissive = LoadImageFromCgltfImage(data->materials[i].emissive_texture.texture->image, texPath);
					if (imEmissive.data != NULL)
					{
						model.materials[j].maps[MATERIAL_MAP_EMISSION].texture = LoadTextureFromImage(imEmissive);
						UnloadImage(imEmissive);
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
							// Init raylib mesh vertices to copy glTF attribute data
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
							// Init raylib mesh normals to copy glTF attribute data
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
							// Init raylib mesh tangent to copy glTF attribute data
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
							// Init raylib mesh texcoords to copy glTF attribute data
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
							// Init raylib mesh color to copy glTF attribute data
							model.meshes[meshIndex].colors = (unsigned char*)malloc(attribute->count * 4 * sizeof(unsigned char));

							// Load 4 components of unsigned char data type into mesh.colors
							LOAD_ATTRIBUTE(attribute, 4, unsigned char, model.meshes[meshIndex].colors)
						}
						else if ((attribute->component_type == cgltf_component_type_r_16u) && (attribute->type == cgltf_type_vec4))
						{
							// Init raylib mesh color to copy glTF attribute data
							model.meshes[meshIndex].colors = (unsigned char*)malloc(attribute->count * 4 * sizeof(unsigned char));

							// Load data into a temp buffer to be converted to raylib data type
							unsigned short* temp = (unsigned short*)malloc(attribute->count * 4 * sizeof(unsigned short));
							LOAD_ATTRIBUTE(attribute, 4, unsigned short, temp);

							// Convert data to raylib color data type (4 bytes)
							for (unsigned int c = 0; c < attribute->count * 4; c++) model.meshes[meshIndex].colors[c] = (unsigned char)(((float)temp[c] / 65535.0f) * 255.0f);

							free(temp);
						}
						else if ((attribute->component_type == cgltf_component_type_r_32f) && (attribute->type == cgltf_type_vec4))
						{
							// Init raylib mesh color to copy glTF attribute data
							model.meshes[meshIndex].colors = (unsigned char*)malloc(attribute->count * 4 * sizeof(unsigned char));

							// Load data into a temp buffer to be converted to raylib data type
							float* temp = (float*)malloc(attribute->count * 4 * sizeof(float));
							LOAD_ATTRIBUTE(attribute, 4, float, temp);

							// Convert data to raylib color data type (4 bytes), we expect the color data normalized
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
						model.meshes[meshIndex].indices = (unsigned short*)malloc(attribute->count * sizeof(unsigned short));

						// Load unsigned short data type into mesh.indices
						LOAD_ATTRIBUTE(attribute, 1, unsigned short, model.meshes[meshIndex].indices)
					}
					else if (attribute->component_type == cgltf_component_type_r_32u)
					{
						// Init mesh indices to copy glTF attribute data
						model.meshes[meshIndex].indices = (unsigned short*)malloc(attribute->count * sizeof(unsigned short));

						// Load data into a temp buffer to be converted to data type
						unsigned int* temp = (unsigned int*)malloc(attribute->count * sizeof(unsigned int));
						LOAD_ATTRIBUTE(attribute, 1, unsigned int, temp);

						// Convert data to indices data type (unsigned short)
						for (unsigned int d = 0; d < attribute->count; d++) model.meshes[meshIndex].indices[d] = (unsigned short)temp[d];

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
					// NOTE: JOINTS_1 + WEIGHT_1 will be used for +4 joints influencing a vertex -> Not supported by raylib

					if (data->meshes[i].primitives[p].attributes[j].type == cgltf_attribute_type_joints)        // JOINTS_n (vec4: 4 bones max per vertex / u8, u16)
					{
						cgltf_accessor* attribute = data->meshes[i].primitives[p].attributes[j].data;

						if ((attribute->component_type == cgltf_component_type_r_8u) && (attribute->type == cgltf_type_vec4))
						{
							// Init raylib mesh bone ids to copy glTF attribute data
							model.meshes[meshIndex].boneIds = (unsigned char*)calloc(model.meshes[meshIndex].vertexCount * 4, sizeof(unsigned char));

							// Load 4 components of unsigned char data type into mesh.boneIds
							// for cgltf_attribute_type_joints we have:
							//   - data.meshes[0] (256 vertices)
							//   - 256 values, provided as cgltf_type_vec4 of bytes (4 byte per joint, stride 4)
							LOAD_ATTRIBUTE(attribute, 4, unsigned char, model.meshes[meshIndex].boneIds)
						}
						else LogWarning("MODEL: [" + std::string(fileName) + "] Joint attribute data format not supported, use vec4 u8");
					}
					else if (data->meshes[i].primitives[p].attributes[j].type == cgltf_attribute_type_weights)  // WEIGHTS_n (vec4 / u8, u16, f32)
					{
						cgltf_accessor* attribute = data->meshes[i].primitives[p].attributes[j].data;

						if ((attribute->component_type == cgltf_component_type_r_32f) && (attribute->type == cgltf_type_vec4))
						{
							// Init raylib mesh bone weight to copy glTF attribute data
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
	UnloadFileData(fileData);

	return model;
}