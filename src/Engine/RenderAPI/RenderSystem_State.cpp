#include "stdafx.h"
#include "RenderSystem.h"
#include "OpenGLTranslateToGL.h"
//-----------------------------------------------------------------------------
void RenderSystem::Bind(RasterizerState state)
{
	if (m_cache.CurrentRasterizerState.polygonMode != state.polygonMode)
	{
		m_cache.CurrentRasterizerState.polygonMode = state.polygonMode;
		glPolygonMode(GL_FRONT_AND_BACK, TranslateToGL(state.polygonMode));
	}
	if (m_cache.CurrentRasterizerState.depthClampEnabled != state.depthClampEnabled)
	{
		m_cache.CurrentRasterizerState.depthClampEnabled = state.depthClampEnabled;
		if (state.depthClampEnabled) glEnable(GL_DEPTH_CLAMP);
		else glDisable(GL_DEPTH_CLAMP);
	}
	if (m_cache.CurrentRasterizerState.multiSampleEnabled != state.multiSampleEnabled)
	{
		m_cache.CurrentRasterizerState.multiSampleEnabled = state.multiSampleEnabled;
		if (state.multiSampleEnabled) glEnable(GL_MULTISAMPLE);
		else glDisable(GL_MULTISAMPLE);
	}
	if (m_cache.CurrentRasterizerState.antiAliasedLineEnabled != state.antiAliasedLineEnabled)
	{
		m_cache.CurrentRasterizerState.antiAliasedLineEnabled = state.antiAliasedLineEnabled;
		if (state.antiAliasedLineEnabled) glEnable(GL_LINE_SMOOTH);
		else glDisable(GL_LINE_SMOOTH);
	}
	if (m_cache.CurrentRasterizerState.face != state.face)
	{
		m_cache.CurrentRasterizerState.face = state.face;
		glFrontFace(TranslateToGL(state.face));
	}
	if (m_cache.CurrentRasterizerState.lineWidth != state.lineWidth)
	{
		m_cache.CurrentRasterizerState.lineWidth = state.lineWidth;
		glLineWidth(state.lineWidth);
	}
	if (m_cache.CurrentRasterizerState.discardEnabled != state.discardEnabled)
	{
		m_cache.CurrentRasterizerState.discardEnabled = state.discardEnabled;
		if (state.discardEnabled) glEnable(GL_RASTERIZER_DISCARD);
		else glDisable(GL_RASTERIZER_DISCARD);
	}
	if (m_cache.CurrentRasterizerState.scissorTestEnabled != state.scissorTestEnabled)
	{
		m_cache.CurrentRasterizerState.scissorTestEnabled = state.scissorTestEnabled;
		if (state.scissorTestEnabled) glEnable(GL_SCISSOR_TEST);
		else glDisable(GL_SCISSOR_TEST);
	}
	if (m_cache.CurrentRasterizerState.cullMode != state.cullMode)
	{
		m_cache.CurrentRasterizerState.cullMode = state.cullMode;
		if (state.cullMode != RasterizerCullMode::FrontAndBack)
		{
			glEnable(GL_CULL_FACE);
			glCullFace(TranslateToGL(state.cullMode));
		}
		else
			glDisable(GL_CULL_FACE);
	}
	if (m_cache.CurrentRasterizerState.depthBias.constantFactor != state.depthBias.constantFactor ||
		m_cache.CurrentRasterizerState.depthBias.slopeFactor != state.depthBias.slopeFactor || 
		m_cache.CurrentRasterizerState.depthBias.clamp != state.depthBias.clamp)
	{
		m_cache.CurrentRasterizerState.depthBias = state.depthBias;
		
		const bool polygonOffsetEnabled = IsPolygonOffsetEnabled(state.depthBias);
		if (polygonOffsetEnabled)
		{
			if (state.polygonMode == RasterizerFillMode::Solid) glEnable(GL_POLYGON_OFFSET_FILL);
			else if (state.polygonMode == RasterizerFillMode::Wireframe) glEnable(GL_POLYGON_OFFSET_LINE);
			else if (state.polygonMode == RasterizerFillMode::Point) glEnable(GL_POLYGON_OFFSET_POINT);

			if (OpenGLExtensions::version >= OPENGL46)
				glPolygonOffsetClamp(state.depthBias.slopeFactor, state.depthBias.constantFactor, state.depthBias.clamp);
			else
				glPolygonOffset(state.depthBias.slopeFactor, state.depthBias.constantFactor);
		}
		else
		{
			if (state.polygonMode == RasterizerFillMode::Solid) glDisable(GL_POLYGON_OFFSET_FILL);
			else if (state.polygonMode == RasterizerFillMode::Wireframe) glDisable(GL_POLYGON_OFFSET_LINE);
			else if (state.polygonMode == RasterizerFillMode::Point) glDisable(GL_POLYGON_OFFSET_POINT);
		}
	}
}
//-----------------------------------------------------------------------------