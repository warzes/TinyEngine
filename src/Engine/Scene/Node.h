#pragma once

#include "Core/Object/Serializable.h"

class Node;
class Octree;
class Scene;
class ObjectResolver;

static const unsigned char NF_ENABLED = 0x1;
static const unsigned char NF_TEMPORARY = 0x2;
static const unsigned char NF_SPATIAL = 0x4;
static const unsigned char NF_SPATIAL_PARENT = 0x8;
static const unsigned char NF_WORLD_TRANSFORM_DIRTY = 0x10;

static const unsigned char LAYER_DEFAULT = 0x0;
static const unsigned LAYERMASK_ALL = 0xffffffff;

// Less time-critical implementation part to speed up linear processing of renderable nodes.
struct NodeImpl
{
	Scene* scene;
	unsigned id;
	std::string name;
};

// Base class for scene nodes.
class Node : public Serializable
{
public:

protected:

private:

};