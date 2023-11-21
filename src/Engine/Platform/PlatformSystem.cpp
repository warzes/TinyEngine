#include "stdafx.h"
#include "PlatformSystem.h"
#include "WindowSystem.h"
//-----------------------------------------------------------------------------
void Platform::SetClipboardText(const char* text)
{
	glfwSetClipboardString(GetGLFWWindow(), text);
}
//-----------------------------------------------------------------------------
const char* Platform::GetClipboardText()
{
	// TODO: ��� �� ������ ������? ��� ��������� � GLFW
	return glfwGetClipboardString(GetGLFWWindow());
}
//-----------------------------------------------------------------------------