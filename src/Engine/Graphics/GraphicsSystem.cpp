#include "stdafx.h"
#include "GraphicsSystem.h"
#include "RenderAPI/RenderSystem.h"
#include "DebugDraw.h"
//-----------------------------------------------------------------------------
GraphicsSystem gGraphicsSystem;
//-----------------------------------------------------------------------------
bool GraphicsSystem::Create()
{
	DebugDraw::Init();

	LogPrint("GraphicsSystem Create");

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
void GraphicsSystem::ResizeRenderTarget(RenderTargetRef rt, uint16_t width, uint16_t height)
{
	if (rt->IsValid() && rt->width == width && rt->height == height)
		return;

	rt.reset();
	rt = CreateRenderTarget(width, height);
}
//-----------------------------------------------------------------------------
void GraphicsSystem::BindRenderTarget(RenderTargetRef rt)
{
	auto& render = GetRenderSystem();

	if (rt == nullptr)
		render.MainScreen();
	else
	{
		assert(rt->IsValid());
		render.Bind(rt->fb);
	}
}
//-----------------------------------------------------------------------------
void GraphicsSystem::BindRenderTargetAsTexture(RenderTargetRef rt, unsigned textureSlot)
{
	assert(rt->IsValid());
	GetRenderSystem().Bind(rt->fb->colorTextures[0], textureSlot);
}
//-----------------------------------------------------------------------------