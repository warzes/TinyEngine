#include "stdafx.h"
#include "GraphicsResource.h"

void BuildPoseFromParentJoints(NewBoneInfo* bones, int boneCount, TempTransform* transforms); // LoadGLTF.cpp

NewModel LoadIQM(const std::string& fileName)
{
	return NewModel();
}

ModelAnimation* LoadModelAnimationsIQM(const std::string& fileName, unsigned int& animCount)
{
	return nullptr;
}