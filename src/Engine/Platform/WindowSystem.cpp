#include "stdafx.h"
#include "WindowSystem.h"
#include "Core/Logging/Log.h"
//-----------------------------------------------------------------------------
WindowSystem gWindowSystem;
//-----------------------------------------------------------------------------
extern void ExitRequest();
//-----------------------------------------------------------------------------
void GLFWErrorCallback(int error, const char *description) noexcept
{
	LogError(std::string("GLFW error ") + std::to_string(error) + ": " + description);
}
//-----------------------------------------------------------------------------
void GLFWFramebufferSizeCallback(GLFWwindow* window, int width, int height) noexcept
{
	const auto app = static_cast<WindowSystem*>(glfwGetWindowUserPointer(window));
	if( app == nullptr ) return;
	app->m_windowWidth = width;
	app->m_windowHeight = height;
}
//-----------------------------------------------------------------------------
#if PLATFORM_EMSCRIPTEN
static EM_BOOL EmscriptenFullscreenChangeCallback(int /*eventType*/, const EmscriptenFullscreenChangeEvent* /*emscEvent*/, void* /*userData*/)
{
	return 1;
}
#endif
//-----------------------------------------------------------------------------
#if PLATFORM_EMSCRIPTEN
EM_BOOL EmscriptenBlurCallback(int /*eventType*/, const EmscriptenFocusEvent* /*emscEvent*/, void* /*userData*/)
{
	gWindowSystem.m_focused = false;
	LogPrint("window unfocused");
	return true;
}
#endif
//-----------------------------------------------------------------------------
#if PLATFORM_EMSCRIPTEN
EM_BOOL EmscriptenfocusCallback(int /*eventType*/, const EmscriptenFocusEvent* /*emscEvent*/, void* /*userData*/)
{
	gWindowSystem.m_focused = true;
	LogPrint("window focused");
	return true;
}
#endif
//-----------------------------------------------------------------------------
bool WindowSystem::Create(const WindowCreateInfo& createInfo)
{
	m_requestedVSync = createInfo.vsyncEnabled;
#if PLATFORM_EMSCRIPTEN
	m_requestedVSync = true; // TODO: ???
#endif

	glfwSetErrorCallback(GLFWErrorCallback);

	if( glfwInit() != GLFW_TRUE )
	{
		LogFatal("GLFW: Failed to initialize GLFW.");
		return false;
	}
	LogPrint("GLFW "
		+ std::to_string(GLFW_VERSION_MAJOR) + "."
		+ std::to_string(GLFW_VERSION_MINOR) + "."
		+ std::to_string(GLFW_VERSION_REVISION) + " initialized."
	);

	glfwDefaultWindowHints();

	// OpenGL Config
#if PLATFORM_EMSCRIPTEN
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
	glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_NATIVE_CONTEXT_API);
#else
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_FALSE);
#	if defined(_DEBUG)
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#	else
	glfwWindowHint(GLFW_CONTEXT_NO_ERROR, GLFW_TRUE);
#	endif
#endif

	// Window Config
	glfwWindowHint(GLFW_RESIZABLE, createInfo.resizable ? GL_TRUE : GL_FALSE);
	glfwWindowHint(GLFW_MAXIMIZED, createInfo.maximized ? GLFW_TRUE : GLFW_FALSE);
	//glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_TRUE);

	m_window = glfwCreateWindow(createInfo.width, createInfo.height, createInfo.title.c_str(), nullptr, nullptr);
	if( !m_window )
	{
		LogFatal("GLFW: Failed to initialize WindowSystem");
		return false;
	}
	glfwSetWindowUserPointer(m_window, this);

	glfwSetFramebufferSizeCallback(m_window, GLFWFramebufferSizeCallback);

	glfwMakeContextCurrent(m_window);

#if PLATFORM_DESKTOP
	if( !gladLoadGL((GLADloadfunc)glfwGetProcAddress) )
	{
		LogFatal("GLAD: Cannot load OpenGL extensions.");
		return false;
	}
#endif

	glfwSwapInterval(m_requestedVSync ? 1 : 0);

	glfwGetFramebufferSize(m_window, &m_windowWidth, &m_windowHeight);

#if PLATFORM_EMSCRIPTEN
	emscripten_set_blur_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, 0, true, EmscriptenBlurCallback); // TODO: glfw events?
	emscripten_set_focus_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, 0, true, EmscriptenfocusCallback);
	emscripten_set_fullscreenchange_callback("#canvas", NULL, 1, EmscriptenFullscreenChangeCallback);
#endif

	LogPrint("WindowSystem Create");

	return true;
}
//-----------------------------------------------------------------------------
void WindowSystem::Destroy()
{
	glfwDestroyWindow(m_window);
	glfwTerminate();
	LogPrint("WindowSystem Destroy");
}
//-----------------------------------------------------------------------------
void WindowSystem::Update()
{
	if (glfwWindowShouldClose(m_window) == GLFW_TRUE) ExitRequest();
}
//-----------------------------------------------------------------------------
void WindowSystem::Present()
{
	glfwSwapBuffers(m_window);
}
//-----------------------------------------------------------------------------
[[nodiscard]] void* WindowSystem::GetHandle() const noexcept
{
	return (void*)m_window;
}
//-----------------------------------------------------------------------------
[[nodiscard]] int WindowSystem::GetWidth() const noexcept
{
	return m_windowWidth;
}
//-----------------------------------------------------------------------------
[[nodiscard]] int WindowSystem::GetHeight() const noexcept
{
	return m_windowHeight;
}
//-----------------------------------------------------------------------------
[[nodiscard]] glm::vec2 WindowSystem::GetPosition() const
{
	int x = 0;
	int y = 0;
#if PLATFORM_DESKTOP
	glfwGetWindowPos(m_window, &x, &y);
#endif
	return { (float)x, (float)y };
}
//-----------------------------------------------------------------------------
void WindowSystem::SetTitle(const char* title)
{
#if PLATFORM_DESKTOP
	glfwSetWindowTitle(m_window, title);
#elif PLATFORM_EMSCRIPTEN
	emscripten_set_window_title((title != 0) ? title : " ");
#endif
}
//-----------------------------------------------------------------------------
void WindowSystem::SetPosition(int x, int y)
{
#if PLATFORM_DESKTOP
	glfwSetWindowPos(m_window, x, y);
#endif
}
//-----------------------------------------------------------------------------
void WindowSystem::SetSize(int width, int height)
{
	glfwSetWindowSize(m_window, width, height);
}
//-----------------------------------------------------------------------------
bool WindowSystem::IsFocused() const
{
	return m_focused;
}
//-----------------------------------------------------------------------------
WindowSystem& GetWindowSystem()
{
	return gWindowSystem;
}
//-----------------------------------------------------------------------------
[[nodiscard]] void* GetWindowHandle() noexcept
{
	return gWindowSystem.GetHandle();
}
//-----------------------------------------------------------------------------
[[nodiscard]] bool IsWindowFullscreen() noexcept
{
	return false; // TODO: доделать
}
//-----------------------------------------------------------------------------
[[nodiscard]] int GetWindowWidth() noexcept
{
	return gWindowSystem.GetWidth();
}
//-----------------------------------------------------------------------------
[[nodiscard]] int GetWindowHeight() noexcept
{
	return gWindowSystem.GetHeight();
}
//-----------------------------------------------------------------------------
[[nodiscard]] float GetWindowSizeAspect() noexcept
{
	assert(gWindowSystem.GetHeight() > 0);
	return static_cast<float>(gWindowSystem.GetWidth()) / static_cast<float>(gWindowSystem.GetHeight());
}
//-----------------------------------------------------------------------------