#include "stdafx.h"
#include "GraphicsSystem.h"
#include "DebugDraw.h"
//-----------------------------------------------------------------------------
GraphicsSystem gGraphicsSystem;
//-----------------------------------------------------------------------------
bool GraphicsSystem::Create()
{
	DebugDraw::Init();

	return true;
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