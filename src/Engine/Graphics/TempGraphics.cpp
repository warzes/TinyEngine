#include "stdafx.h"
#include "GraphicsResource.h"
#include "GraphicsSystem.h"
#include "Core/IO/FileSystem.h"

NewModel LoadGLTF(const char* fileName);

NewModel LoadModel(const char* fileName)
{
	NewModel model = {};

	if (FileSystem::IsFileExtension(fileName, ".gltf") || FileSystem::IsFileExtension(fileName, ".glb")) model = LoadGLTF(fileName);


	return model;
}