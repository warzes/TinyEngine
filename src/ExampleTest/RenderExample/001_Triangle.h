#pragma once

/*
В этом примере создается примитивный треугольник.
Из системы рендера используются следующие объекты:
	ShaderProgramRef - указатель на шейдерную программу
	Uniform - параметр который передается в шейдер
	VertexBufferRef - буффер вершин
	VertexArrayRef - 
*/

class _001Triangle final : public IApp
{
	bool Create() final;
	void Destroy() final;

	void Render() final;
	void Update(float deltaTime) final;

private:
	ShaderProgramRef m_shader;
	Uniform m_uniformProjectionMatrix;
	glm::mat4 m_perspective;
	VertexBufferRef m_vb;
	VertexArrayRef m_vao;
};