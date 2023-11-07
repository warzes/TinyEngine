#include "stdafx.h"
#include "GraphicsResource.h"

NewMaterial LoadMaterialDefault()
{
	NewMaterial material = { 0 };
	material.maps = (MaterialMap*)calloc(MAX_MATERIAL_MAPS, sizeof(MaterialMap));

	// Using rlgl default shader

	// Using rlgl default texture (1x1 pixel, UNCOMPRESSED_R8G8B8A8, 1 mipmap)
	//material.maps[MATERIAL_MAP_DIFFUSE].texture = (Texture2D){ rlGetTextureIdDefault(), 1, 1, 1, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8 };
	//material.maps[MATERIAL_MAP_NORMAL].texture;         // NOTE: By default, not set
	//material.maps[MATERIAL_MAP_SPECULAR].texture;       // NOTE: By default, not set

	material.maps[MATERIAL_MAP_DIFFUSE].color = glm::vec4(1.0);    // Diffuse color
	material.maps[MATERIAL_MAP_SPECULAR].color = glm::vec4(1.0);   // Specular color

	return material;
}

// String pointer reverse break: returns right-most occurrence of charset in s
const char* strprbrk(const char* s, const char* charset)
{
	const char* latestMatch = NULL;
	for (; s = strpbrk(s, charset), s != NULL; latestMatch = s++) {}
	return latestMatch;
}

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


#define MAX_FILEPATH_LENGTH          4096       // Maximum length for filepaths (Linux PATH_MAX default value)
// Get directory for a given filePath
const char* GetDirectoryPath(const char* filePath)
{
	/*
	// NOTE: Directory separator is different in Windows and other platforms,
	// fortunately, Windows also support the '/' separator, that's the one should be used
	#if defined(_WIN32)
		char separator = '\\';
	#else
		char separator = '/';
	#endif
	*/
	const char* lastSlash = NULL;
	static char dirPath[MAX_FILEPATH_LENGTH] = { 0 };
	memset(dirPath, 0, MAX_FILEPATH_LENGTH);

	// In case provided path does not contain a root drive letter (C:\, D:\) nor leading path separator (\, /),
	// we add the current directory path to dirPath
	if (filePath[1] != ':' && filePath[0] != '\\' && filePath[0] != '/')
	{
		// For security, we set starting path to current directory,
		// obtained path will be concatenated to this
		dirPath[0] = '.';
		dirPath[1] = '/';
	}

	lastSlash = strprbrk(filePath, "\\/");
	if (lastSlash)
	{
		if (lastSlash == filePath)
		{
			// The last and only slash is the leading one: path is in a root directory
			dirPath[0] = filePath[0];
			dirPath[1] = '\0';
		}
		else
		{
			// NOTE: Be careful, strncpy() is not safe, it does not care about '\0'
			char* dirPathPtr = dirPath;
			if ((filePath[1] != ':') && (filePath[0] != '\\') && (filePath[0] != '/')) dirPathPtr += 2;     // Skip drive letter, "C:"
			memcpy(dirPathPtr, filePath, strlen(filePath) - (strlen(lastSlash) - 1));
			dirPath[strlen(filePath) - strlen(lastSlash) + (((filePath[1] != ':') && (filePath[0] != '\\') && (filePath[0] != '/')) ? 2 : 0)] = '\0';  // Add '\0' manually
		}
	}

	return dirPath;
}




// Calculate linear interpolation between two vectors
tempVec3 Vector3Lerp(glm::vec3 v1, glm::vec3 v2, float amount)
{
	tempVec3 result = { 0 };
	result.x = v1.x + amount * (v2.x - v1.x);
	result.y = v1.y + amount * (v2.y - v1.y);
	result.z = v1.z + amount * (v2.z - v1.z);
	return result;
}

// Calculate slerp-optimized interpolation between two quaternions
tempVec4 QuaternionNlerp(glm::vec4 q1, glm::vec4 q2, float amount)
{
	tempVec4 result = { 0 };

	// QuaternionLerp(q1, q2, amount)
	result.x = q1.x + amount * (q2.x - q1.x);
	result.y = q1.y + amount * (q2.y - q1.y);
	result.z = q1.z + amount * (q2.z - q1.z);
	result.w = q1.w + amount * (q2.w - q1.w);

	// QuaternionNormalize(q);
	tempVec4 q = result;
	float length = sqrtf(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
	if (length == 0.0f) length = 1.0f;
	float ilength = 1.0f / length;

	result.x = q.x * ilength;
	result.y = q.y * ilength;
	result.z = q.z * ilength;
	result.w = q.w * ilength;

	return result;
}

// Calculates spherical linear interpolation between two quaternions
tempVec4 QuaternionSlerp(glm::vec4 q1, glm::vec4 q2, float amount)
{
	tempVec4 result = { 0 };

#if !defined(EPSILON)
#define EPSILON 0.000001f
#endif

	float cosHalfTheta = q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w;

	if (cosHalfTheta < 0)
	{
		q2.x = -q2.x; q2.y = -q2.y; q2.z = -q2.z; q2.w = -q2.w;
		cosHalfTheta = -cosHalfTheta;
	}

	if (fabsf(cosHalfTheta) >= 1.0f)
	{
		result.x = q1.x;
		result.y = q1.y;
		result.z = q1.z;
		result.w = q1.w;
	}
	else if (cosHalfTheta > 0.95f) result = QuaternionNlerp(q1, q2, amount);
	else
	{
		float halfTheta = acosf(cosHalfTheta);
		float sinHalfTheta = sqrtf(1.0f - cosHalfTheta * cosHalfTheta);

		if (fabsf(sinHalfTheta) < EPSILON)
		{
			result.x = (q1.x * 0.5f + q2.x * 0.5f);
			result.y = (q1.y * 0.5f + q2.y * 0.5f);
			result.z = (q1.z * 0.5f + q2.z * 0.5f);
			result.w = (q1.w * 0.5f + q2.w * 0.5f);
		}
		else
		{
			float ratioA = sinf((1 - amount) * halfTheta) / sinHalfTheta;
			float ratioB = sinf(amount * halfTheta) / sinHalfTheta;

			result.x = (q1.x * ratioA + q2.x * ratioB);
			result.y = (q1.y * ratioA + q2.y * ratioB);
			result.z = (q1.z * ratioA + q2.z * ratioB);
			result.w = (q1.w * ratioA + q2.w * ratioB);
		}
	}

	return result;
}