#pragma once

/*
В этом примере создается прямоугольник на который накладывается текстура. Также вместо VB, IB и VAO используется специальный объект GeometryBuffer
Из системы рендера используются следующие объекты:
	ShaderProgramRef - указатель на шейдерную программу
	Uniform - параметр который передается в шейдер
	Texture2DRef - текстура
	GeometryBufferRef - специальный объект который объединяет в себе VB, IB и VAO
*/

class _005BasicTexture final : public IApp
{
	bool Create() final;
	void Destroy() final;

	void Render() final;
	void Update(float deltaTime) final;

private:
	ShaderProgramRef m_shader;
	Uniform m_uniformProjectionMatrix;
	glm::mat4 m_perspective;
	GeometryBufferRef m_geom;
	Texture2DRef m_texture;
};