#include "stdafx.h"
#include "GraphicsResource.h"
#include "GraphicsSystem.h"
#include "Core/IO/FileSystem.h"

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
	cgltf_options options = { 0 };
	cgltf_data* data = NULL;
	cgltf_result result = cgltf_parse(&options, fileData, dataSize, &data);


	return {};
}