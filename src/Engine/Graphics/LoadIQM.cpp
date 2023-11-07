#include "stdafx.h"
#include "GraphicsResource.h"

void BuildPoseFromParentJoints(NewBoneInfo* bones, int boneCount, TempTransform* transforms); // LoadGLTF.cpp
unsigned char* LoadFileData(const char* fileName, int* dataSize);
NewMaterial LoadMaterialDefault();

NewModel LoadIQM(const std::string& fileName)
{
#define IQM_MAGIC     "INTERQUAKEMODEL" // IQM file magic number
#define IQM_VERSION          2          // only IQM version 2 supported

#define BONE_NAME_LENGTH    32          // BoneInfo name string length
#define MESH_NAME_LENGTH    32          // Mesh name string length
#define MATERIAL_NAME_LENGTH 32         // Material name string length

	int dataSize = 0;
	unsigned char* fileData = LoadFileData(fileName.c_str(), &dataSize);
	unsigned char* fileDataPtr = fileData;

	// IQM file structs
	//-----------------------------------------------------------------------------------
	typedef struct IQMHeader {
		char magic[16];
		unsigned int version;
		unsigned int dataSize;
		unsigned int flags;
		unsigned int num_text, ofs_text;
		unsigned int num_meshes, ofs_meshes;
		unsigned int num_vertexarrays, num_vertexes, ofs_vertexarrays;
		unsigned int num_triangles, ofs_triangles, ofs_adjacency;
		unsigned int num_joints, ofs_joints;
		unsigned int num_poses, ofs_poses;
		unsigned int num_anims, ofs_anims;
		unsigned int num_frames, num_framechannels, ofs_frames, ofs_bounds;
		unsigned int num_comment, ofs_comment;
		unsigned int num_extensions, ofs_extensions;
	} IQMHeader;

	typedef struct IQMMesh {
		unsigned int name;
		unsigned int material;
		unsigned int first_vertex, num_vertexes;
		unsigned int first_triangle, num_triangles;
	} IQMMesh;

	typedef struct IQMTriangle {
		unsigned int vertex[3];
	} IQMTriangle;

	typedef struct IQMJoint {
		unsigned int name;
		int parent;
		float translate[3], rotate[4], scale[3];
	} IQMJoint;

	typedef struct IQMVertexArray {
		unsigned int type;
		unsigned int flags;
		unsigned int format;
		unsigned int size;
		unsigned int offset;
	} IQMVertexArray;

	// NOTE: Below IQM structures are not used but listed for reference
	/*
	typedef struct IQMAdjacency {
	unsigned int triangle[3];
	} IQMAdjacency;

	typedef struct IQMPose {
	int parent;
	unsigned int mask;
	float channeloffset[10];
	float channelscale[10];
	} IQMPose;

	typedef struct IQMAnim {
	unsigned int name;
	unsigned int first_frame, num_frames;
	float framerate;
	unsigned int flags;
	} IQMAnim;

	typedef struct IQMBounds {
	float bbmin[3], bbmax[3];
	float xyradius, radius;
	} IQMBounds;
	*/
	//-----------------------------------------------------------------------------------

	// IQM vertex data types
	enum {
		IQM_POSITION = 0,
		IQM_TEXCOORD = 1,
		IQM_NORMAL = 2,
		IQM_TANGENT = 3,       // NOTE: Tangents unused by default
		IQM_BLENDINDEXES = 4,
		IQM_BLENDWEIGHTS = 5,
		IQM_COLOR = 6,
		IQM_CUSTOM = 0x10     // NOTE: Custom vertex values unused by default
	};

	NewModel model = {};

	IQMMesh* imesh = NULL;
	IQMTriangle* tri = NULL;
	IQMVertexArray* va = NULL;
	IQMJoint* ijoint = NULL;

	float* vertex = NULL;
	float* normal = NULL;
	float* text = NULL;
	char* blendi = NULL;
	unsigned char* blendw = NULL;
	unsigned char* color = NULL;

	// In case file can not be read, return an empty model
	if (fileDataPtr == NULL) return model;

	// Read IQM header
	IQMHeader* iqmHeader = (IQMHeader*)fileDataPtr;

	if (memcmp(iqmHeader->magic, IQM_MAGIC, sizeof(IQM_MAGIC)) != 0)
	{
		LogWarning("MODEL: [" + fileName + "] IQM file is not a valid model");
		return model;
	}

	if (iqmHeader->version != IQM_VERSION)
	{
		LogWarning("MODEL: [" + fileName + "] IQM file version not supported (" + std::to_string(iqmHeader->version) + ")");
		return model;
	}

	//fileDataPtr += sizeof(IQMHeader);       // Move file data pointer

	// Meshes data processing
	imesh = (IQMMesh*)malloc(iqmHeader->num_meshes * sizeof(IQMMesh));
	//fseek(iqmFile, iqmHeader->ofs_meshes, SEEK_SET);
	//fread(imesh, sizeof(IQMMesh)*iqmHeader->num_meshes, 1, iqmFile);
	memcpy(imesh, fileDataPtr + iqmHeader->ofs_meshes, iqmHeader->num_meshes * sizeof(IQMMesh));

	model.meshes.resize(iqmHeader->num_meshes);

	model.materialCount = model.meshes.size();
	model.materials = (NewMaterial*)calloc(model.materialCount, sizeof(NewMaterial));
	model.meshMaterial = (int*)calloc(model.meshes.size(), sizeof(int));

	char name[MESH_NAME_LENGTH] = { 0 };
	char material[MATERIAL_NAME_LENGTH] = { 0 };

	for (int i = 0; i < model.meshes.size(); i++)
	{
		//fseek(iqmFile, iqmHeader->ofs_text + imesh[i].name, SEEK_SET);
		//fread(name, sizeof(char), MESH_NAME_LENGTH, iqmFile);
		memcpy(name, fileDataPtr + iqmHeader->ofs_text + imesh[i].name, MESH_NAME_LENGTH * sizeof(char));

		//fseek(iqmFile, iqmHeader->ofs_text + imesh[i].material, SEEK_SET);
		//fread(material, sizeof(char), MATERIAL_NAME_LENGTH, iqmFile);
		memcpy(material, fileDataPtr + iqmHeader->ofs_text + imesh[i].material, MATERIAL_NAME_LENGTH * sizeof(char));

		model.materials[i] = LoadMaterialDefault();

		LogPrint("MODEL: [" + fileName + "] mesh name (" + std::string(name) + "), material (" + std::string(material) + ")");

		model.meshes[i].vertexCount = imesh[i].num_vertexes;

		model.meshes[i].vertices = (float*)calloc(model.meshes[i].vertexCount * 3, sizeof(float));       // Default vertex positions
		model.meshes[i].normals = (float*)calloc(model.meshes[i].vertexCount * 3, sizeof(float));        // Default vertex normals
		model.meshes[i].texcoords = (float*)calloc(model.meshes[i].vertexCount * 2, sizeof(float));      // Default vertex texcoords

		model.meshes[i].boneIds = (unsigned char*)calloc(model.meshes[i].vertexCount * 4, sizeof(unsigned char));  // Up-to 4 bones supported!
		model.meshes[i].boneWeights = (float*)calloc(model.meshes[i].vertexCount * 4, sizeof(float));      // Up-to 4 bones supported!

		model.meshes[i].triangleCount = imesh[i].num_triangles;
		model.meshes[i].indices.resize(model.meshes[i].triangleCount * 3);

		// Animated vertex data, what we actually process for rendering
		// NOTE: Animated vertex should be re-uploaded to GPU (if not using GPU skinning)
		model.meshes[i].animVertices = (float*)calloc(model.meshes[i].vertexCount * 3, sizeof(float));
		model.meshes[i].animNormals = (float*)calloc(model.meshes[i].vertexCount * 3, sizeof(float));
	}

	// Triangles data processing
	tri = (IQMTriangle*)malloc(iqmHeader->num_triangles * sizeof(IQMTriangle));
	//fseek(iqmFile, iqmHeader->ofs_triangles, SEEK_SET);
	//fread(tri, sizeof(IQMTriangle), iqmHeader->num_triangles, iqmFile);
	memcpy(tri, fileDataPtr + iqmHeader->ofs_triangles, iqmHeader->num_triangles * sizeof(IQMTriangle));

	for (int m = 0; m < model.meshes.size(); m++)
	{
		int tcounter = 0;

		for (unsigned int i = imesh[m].first_triangle; i < (imesh[m].first_triangle + imesh[m].num_triangles); i++)
		{
			// IQM triangles indexes are stored in counter-clockwise, but raylib processes the index in linear order,
			// expecting they point to the counter-clockwise vertex triangle, so we need to reverse triangle indexes
			// NOTE: renders vertex data in counter-clockwise order (standard convention) by default
			model.meshes[m].indices[tcounter + 2] = tri[i].vertex[0] - imesh[m].first_vertex;
			model.meshes[m].indices[tcounter + 1] = tri[i].vertex[1] - imesh[m].first_vertex;
			model.meshes[m].indices[tcounter] = tri[i].vertex[2] - imesh[m].first_vertex;
			tcounter += 3;
		}
	}

	// Vertex arrays data processing
	va = (IQMVertexArray*)malloc(iqmHeader->num_vertexarrays * sizeof(IQMVertexArray));
	//fseek(iqmFile, iqmHeader->ofs_vertexarrays, SEEK_SET);
	//fread(va, sizeof(IQMVertexArray), iqmHeader->num_vertexarrays, iqmFile);
	memcpy(va, fileDataPtr + iqmHeader->ofs_vertexarrays, iqmHeader->num_vertexarrays * sizeof(IQMVertexArray));

	for (unsigned int i = 0; i < iqmHeader->num_vertexarrays; i++)
	{
		switch (va[i].type)
		{
		case IQM_POSITION:
		{
			vertex = (float*)malloc(iqmHeader->num_vertexes * 3 * sizeof(float));
			//fseek(iqmFile, va[i].offset, SEEK_SET);
			//fread(vertex, iqmHeader->num_vertexes*3*sizeof(float), 1, iqmFile);
			memcpy(vertex, fileDataPtr + va[i].offset, iqmHeader->num_vertexes * 3 * sizeof(float));

			for (unsigned int m = 0; m < iqmHeader->num_meshes; m++)
			{
				int vCounter = 0;
				for (unsigned int i = imesh[m].first_vertex * 3; i < (imesh[m].first_vertex + imesh[m].num_vertexes) * 3; i++)
				{
					model.meshes[m].vertices[vCounter] = vertex[i];
					model.meshes[m].animVertices[vCounter] = vertex[i];
					vCounter++;
				}
			}
		} break;
		case IQM_NORMAL:
		{
			normal = (float*)malloc(iqmHeader->num_vertexes * 3 * sizeof(float));
			//fseek(iqmFile, va[i].offset, SEEK_SET);
			//fread(normal, iqmHeader->num_vertexes*3*sizeof(float), 1, iqmFile);
			memcpy(normal, fileDataPtr + va[i].offset, iqmHeader->num_vertexes * 3 * sizeof(float));

			for (unsigned int m = 0; m < iqmHeader->num_meshes; m++)
			{
				int vCounter = 0;
				for (unsigned int i = imesh[m].first_vertex * 3; i < (imesh[m].first_vertex + imesh[m].num_vertexes) * 3; i++)
				{
					model.meshes[m].normals[vCounter] = normal[i];
					model.meshes[m].animNormals[vCounter] = normal[i];
					vCounter++;
				}
			}
		} break;
		case IQM_TEXCOORD:
		{
			text = (float*)malloc(iqmHeader->num_vertexes * 2 * sizeof(float));
			//fseek(iqmFile, va[i].offset, SEEK_SET);
			//fread(text, iqmHeader->num_vertexes*2*sizeof(float), 1, iqmFile);
			memcpy(text, fileDataPtr + va[i].offset, iqmHeader->num_vertexes * 2 * sizeof(float));

			for (unsigned int m = 0; m < iqmHeader->num_meshes; m++)
			{
				int vCounter = 0;
				for (unsigned int i = imesh[m].first_vertex * 2; i < (imesh[m].first_vertex + imesh[m].num_vertexes) * 2; i++)
				{
					model.meshes[m].texcoords[vCounter] = text[i];
					vCounter++;
				}
			}
		} break;
		case IQM_BLENDINDEXES:
		{
			blendi = (char*)malloc(iqmHeader->num_vertexes * 4 * sizeof(char));
			//fseek(iqmFile, va[i].offset, SEEK_SET);
			//fread(blendi, iqmHeader->num_vertexes*4*sizeof(char), 1, iqmFile);
			memcpy(blendi, fileDataPtr + va[i].offset, iqmHeader->num_vertexes * 4 * sizeof(char));

			for (unsigned int m = 0; m < iqmHeader->num_meshes; m++)
			{
				int boneCounter = 0;
				for (unsigned int i = imesh[m].first_vertex * 4; i < (imesh[m].first_vertex + imesh[m].num_vertexes) * 4; i++)
				{
					model.meshes[m].boneIds[boneCounter] = blendi[i];
					boneCounter++;
				}
			}
		} break;
		case IQM_BLENDWEIGHTS:
		{
			blendw = (unsigned char*)malloc(iqmHeader->num_vertexes * 4 * sizeof(unsigned char));
			//fseek(iqmFile, va[i].offset, SEEK_SET);
			//fread(blendw, iqmHeader->num_vertexes*4*sizeof(unsigned char), 1, iqmFile);
			memcpy(blendw, fileDataPtr + va[i].offset, iqmHeader->num_vertexes * 4 * sizeof(unsigned char));

			for (unsigned int m = 0; m < iqmHeader->num_meshes; m++)
			{
				int boneCounter = 0;
				for (unsigned int i = imesh[m].first_vertex * 4; i < (imesh[m].first_vertex + imesh[m].num_vertexes) * 4; i++)
				{
					model.meshes[m].boneWeights[boneCounter] = blendw[i] / 255.0f;
					boneCounter++;
				}
			}
		} break;
		case IQM_COLOR:
		{
			color = (unsigned char*)malloc(iqmHeader->num_vertexes * 4 * sizeof(unsigned char));
			//fseek(iqmFile, va[i].offset, SEEK_SET);
			//fread(blendw, iqmHeader->num_vertexes*4*sizeof(unsigned char), 1, iqmFile);
			memcpy(color, fileDataPtr + va[i].offset, iqmHeader->num_vertexes * 4 * sizeof(unsigned char));

			for (unsigned int m = 0; m < iqmHeader->num_meshes; m++)
			{
				model.meshes[m].colors = (unsigned char*)calloc(model.meshes[m].vertexCount * 4, sizeof(unsigned char));

				int vCounter = 0;
				for (unsigned int i = imesh[m].first_vertex * 4; i < (imesh[m].first_vertex + imesh[m].num_vertexes) * 4; i++)
				{
					model.meshes[m].colors[vCounter] = color[i];
					vCounter++;
				}
			}
		} break;
		}
	}

	// Bones (joints) data processing
	ijoint = (IQMJoint*)malloc(iqmHeader->num_joints * sizeof(IQMJoint));
	//fseek(iqmFile, iqmHeader->ofs_joints, SEEK_SET);
	//fread(ijoint, sizeof(IQMJoint), iqmHeader->num_joints, iqmFile);
	memcpy(ijoint, fileDataPtr + iqmHeader->ofs_joints, iqmHeader->num_joints * sizeof(IQMJoint));

	model.boneCount = iqmHeader->num_joints;
	model.bones = (NewBoneInfo*)malloc(iqmHeader->num_joints * sizeof(NewBoneInfo));
	model.bindPose = (TempTransform*)malloc(iqmHeader->num_joints * sizeof(TempTransform));

	for (unsigned int i = 0; i < iqmHeader->num_joints; i++)
	{
		// Bones
		model.bones[i].parent = ijoint[i].parent;
		//fseek(iqmFile, iqmHeader->ofs_text + ijoint[i].name, SEEK_SET);
		//fread(model.bones[i].name, sizeof(char), BONE_NAME_LENGTH, iqmFile);
		memcpy(model.bones[i].name, fileDataPtr + iqmHeader->ofs_text + ijoint[i].name, BONE_NAME_LENGTH * sizeof(char));

		// Bind pose (base pose)
		model.bindPose[i].translation.x = ijoint[i].translate[0];
		model.bindPose[i].translation.y = ijoint[i].translate[1];
		model.bindPose[i].translation.z = ijoint[i].translate[2];

		model.bindPose[i].rotation.x = ijoint[i].rotate[0];
		model.bindPose[i].rotation.y = ijoint[i].rotate[1];
		model.bindPose[i].rotation.z = ijoint[i].rotate[2];
		model.bindPose[i].rotation.w = ijoint[i].rotate[3];

		model.bindPose[i].scale.x = ijoint[i].scale[0];
		model.bindPose[i].scale.y = ijoint[i].scale[1];
		model.bindPose[i].scale.z = ijoint[i].scale[2];
	}

	BuildPoseFromParentJoints(model.bones, model.boneCount, model.bindPose);

	free(fileData);

	free(imesh);
	free(tri);
	free(va);
	free(vertex);
	free(normal);
	free(text);
	free(blendi);
	free(blendw);
	free(ijoint);
	free(color);

	return model;
}

ModelAnimation* LoadModelAnimationsIQM(const std::string& fileName, unsigned int& animCount)
{
#define IQM_MAGIC       "INTERQUAKEMODEL"   // IQM file magic number
#define IQM_VERSION     2                   // only IQM version 2 supported

	int dataSize = 0;
	unsigned char* fileData = LoadFileData(fileName.c_str(), &dataSize);
	unsigned char* fileDataPtr = fileData;

	typedef struct IQMHeader {
		char magic[16];
		unsigned int version;
		unsigned int dataSize;
		unsigned int flags;
		unsigned int num_text, ofs_text;
		unsigned int num_meshes, ofs_meshes;
		unsigned int num_vertexarrays, num_vertexes, ofs_vertexarrays;
		unsigned int num_triangles, ofs_triangles, ofs_adjacency;
		unsigned int num_joints, ofs_joints;
		unsigned int num_poses, ofs_poses;
		unsigned int num_anims, ofs_anims;
		unsigned int num_frames, num_framechannels, ofs_frames, ofs_bounds;
		unsigned int num_comment, ofs_comment;
		unsigned int num_extensions, ofs_extensions;
	} IQMHeader;

	typedef struct IQMJoint {
		unsigned int name;
		int parent;
		float translate[3], rotate[4], scale[3];
	} IQMJoint;

	typedef struct IQMPose {
		int parent;
		unsigned int mask;
		float channeloffset[10];
		float channelscale[10];
	} IQMPose;

	typedef struct IQMAnim {
		unsigned int name;
		unsigned int first_frame, num_frames;
		float framerate;
		unsigned int flags;
	} IQMAnim;

	// In case file can not be read, return an empty model
	if (fileDataPtr == NULL) return NULL;

	// Read IQM header
	IQMHeader* iqmHeader = (IQMHeader*)fileDataPtr;

	if (memcmp(iqmHeader->magic, IQM_MAGIC, sizeof(IQM_MAGIC)) != 0)
	{
		LogWarning("MODEL: [" + fileName + "] IQM file is not a valid model");
		return NULL;
	}

	if (iqmHeader->version != IQM_VERSION)
	{
		LogWarning("MODEL: [" + fileName + "] IQM file version not supported(" + std::to_string(iqmHeader->version) + ")");
		return NULL;
	}

	// Get bones data
	IQMPose* poses = (IQMPose*)malloc(iqmHeader->num_poses * sizeof(IQMPose));
	//fseek(iqmFile, iqmHeader->ofs_poses, SEEK_SET);
	//fread(poses, sizeof(IQMPose), iqmHeader->num_poses, iqmFile);
	memcpy(poses, fileDataPtr + iqmHeader->ofs_poses, iqmHeader->num_poses * sizeof(IQMPose));

	// Get animations data
	animCount = iqmHeader->num_anims;
	IQMAnim* anim = (IQMAnim*)malloc(iqmHeader->num_anims * sizeof(IQMAnim));
	//fseek(iqmFile, iqmHeader->ofs_anims, SEEK_SET);
	//fread(anim, sizeof(IQMAnim), iqmHeader->num_anims, iqmFile);
	memcpy(anim, fileDataPtr + iqmHeader->ofs_anims, iqmHeader->num_anims * sizeof(IQMAnim));

	ModelAnimation* animations = (ModelAnimation*)malloc(iqmHeader->num_anims * sizeof(ModelAnimation));

	// frameposes
	unsigned short* framedata = (unsigned short*)malloc(iqmHeader->num_frames * iqmHeader->num_framechannels * sizeof(unsigned short));
	//fseek(iqmFile, iqmHeader->ofs_frames, SEEK_SET);
	//fread(framedata, sizeof(unsigned short), iqmHeader->num_frames*iqmHeader->num_framechannels, iqmFile);
	memcpy(framedata, fileDataPtr + iqmHeader->ofs_frames, iqmHeader->num_frames * iqmHeader->num_framechannels * sizeof(unsigned short));

	// joints
	IQMJoint* joints = (IQMJoint*)malloc(iqmHeader->num_joints * sizeof(IQMJoint));
	memcpy(joints, fileDataPtr + iqmHeader->ofs_joints, iqmHeader->num_joints * sizeof(IQMJoint));

	for (unsigned int a = 0; a < iqmHeader->num_anims; a++)
	{
		animations[a].frameCount = anim[a].num_frames;
		animations[a].boneCount = iqmHeader->num_poses;
		animations[a].bones = (NewBoneInfo*)malloc(iqmHeader->num_poses * sizeof(NewBoneInfo));
		animations[a].framePoses = (TempTransform**)malloc(anim[a].num_frames * sizeof(TempTransform*));
		// animations[a].framerate = anim.framerate;     // TODO: Use animation framerate data?

		for (unsigned int j = 0; j < iqmHeader->num_poses; j++)
		{
			// If animations and skeleton are in the same file, copy bone names to anim
			if (iqmHeader->num_joints > 0)
				memcpy(animations[a].bones[j].name, fileDataPtr + iqmHeader->ofs_text + joints[j].name, BONE_NAME_LENGTH * sizeof(char));
			else
				strcpy_s(animations[a].bones[j].name, "ANIMJOINTNAME"); // default bone name otherwise
			animations[a].bones[j].parent = poses[j].parent;
		}

		for (unsigned int j = 0; j < anim[a].num_frames; j++) animations[a].framePoses[j] = (TempTransform*)malloc(iqmHeader->num_poses * sizeof(TempTransform));

		int dcounter = anim[a].first_frame * iqmHeader->num_framechannels;

		for (unsigned int frame = 0; frame < anim[a].num_frames; frame++)
		{
			for (unsigned int i = 0; i < iqmHeader->num_poses; i++)
			{
				animations[a].framePoses[frame][i].translation.x = poses[i].channeloffset[0];

				if (poses[i].mask & 0x01)
				{
					animations[a].framePoses[frame][i].translation.x += framedata[dcounter] * poses[i].channelscale[0];
					dcounter++;
				}

				animations[a].framePoses[frame][i].translation.y = poses[i].channeloffset[1];

				if (poses[i].mask & 0x02)
				{
					animations[a].framePoses[frame][i].translation.y += framedata[dcounter] * poses[i].channelscale[1];
					dcounter++;
				}

				animations[a].framePoses[frame][i].translation.z = poses[i].channeloffset[2];

				if (poses[i].mask & 0x04)
				{
					animations[a].framePoses[frame][i].translation.z += framedata[dcounter] * poses[i].channelscale[2];
					dcounter++;
				}

				animations[a].framePoses[frame][i].rotation.x = poses[i].channeloffset[3];

				if (poses[i].mask & 0x08)
				{
					animations[a].framePoses[frame][i].rotation.x += framedata[dcounter] * poses[i].channelscale[3];
					dcounter++;
				}

				animations[a].framePoses[frame][i].rotation.y = poses[i].channeloffset[4];

				if (poses[i].mask & 0x10)
				{
					animations[a].framePoses[frame][i].rotation.y += framedata[dcounter] * poses[i].channelscale[4];
					dcounter++;
				}

				animations[a].framePoses[frame][i].rotation.z = poses[i].channeloffset[5];

				if (poses[i].mask & 0x20)
				{
					animations[a].framePoses[frame][i].rotation.z += framedata[dcounter] * poses[i].channelscale[5];
					dcounter++;
				}

				animations[a].framePoses[frame][i].rotation.w = poses[i].channeloffset[6];

				if (poses[i].mask & 0x40)
				{
					animations[a].framePoses[frame][i].rotation.w += framedata[dcounter] * poses[i].channelscale[6];
					dcounter++;
				}

				animations[a].framePoses[frame][i].scale.x = poses[i].channeloffset[7];

				if (poses[i].mask & 0x80)
				{
					animations[a].framePoses[frame][i].scale.x += framedata[dcounter] * poses[i].channelscale[7];
					dcounter++;
				}

				animations[a].framePoses[frame][i].scale.y = poses[i].channeloffset[8];

				if (poses[i].mask & 0x100)
				{
					animations[a].framePoses[frame][i].scale.y += framedata[dcounter] * poses[i].channelscale[8];
					dcounter++;
				}

				animations[a].framePoses[frame][i].scale.z = poses[i].channeloffset[9];

				if (poses[i].mask & 0x200)
				{
					animations[a].framePoses[frame][i].scale.z += framedata[dcounter] * poses[i].channelscale[9];
					dcounter++;
				}

				animations[a].framePoses[frame][i].rotation = glm::normalize(animations[a].framePoses[frame][i].rotation);
			}
		}

		// Build frameposes
		for (unsigned int frame = 0; frame < anim[a].num_frames; frame++)
		{
			for (int i = 0; i < animations[a].boneCount; i++)
			{
				if (animations[a].bones[i].parent >= 0)
				{
					animations[a].framePoses[frame][i].rotation = animations[a].framePoses[frame][animations[a].bones[i].parent].rotation * animations[a].framePoses[frame][i].rotation;
					animations[a].framePoses[frame][i].translation = animations[a].framePoses[frame][animations[a].bones[i].parent].rotation * animations[a].framePoses[frame][i].translation; // Vector3RotateByQuaternion
					animations[a].framePoses[frame][i].translation = animations[a].framePoses[frame][i].translation + animations[a].framePoses[frame][animations[a].bones[i].parent].translation;
					animations[a].framePoses[frame][i].scale = animations[a].framePoses[frame][i].scale * animations[a].framePoses[frame][animations[a].bones[i].parent].scale;
				}
			}
		}
	}

	free(fileData);

	free(joints);
	free(framedata);
	free(poses);
	free(anim);

	return animations;
}