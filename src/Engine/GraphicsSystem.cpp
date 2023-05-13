#include "stdafx.h"
#include "GraphicsSystem.h"
#include "DebugDraw.h"
//-----------------------------------------------------------------------------
GraphicsSystem gGraphicsSystem;
//-----------------------------------------------------------------------------
void GraphicsSystem::Create()
{
	DebugDraw::Init();
}
//-----------------------------------------------------------------------------
void GraphicsSystem::Destroy()
{
	DebugDraw::Close();
}
//-----------------------------------------------------------------------------
GraphicsSystem& GetGraphicsSystem()
{
	return gGraphicsSystem;
}
//-----------------------------------------------------------------------------