#pragma once

struct WindowCreateInfo final
{
	int width = 1024;
	int height = 768;
	std::string title = "Game";
	bool resizable = true;
	bool maximized = true;

	bool vsyncEnabled = false;
};

class Window final
{
	friend class EngineDevice;
	friend class Input;
	friend void GLFWFramebufferSizeCallback(GLFWwindow*, int, int) noexcept;
public:
	Window() = default;

	bool Create(const WindowCreateInfo& createInfo);
	void Destroy();

	void Update();
	void Present();

	void* GetHandle() const;

	int GetWidth() const;
	int GetHeight() const;

	glm::vec2 GetPosition() const;

	void SetTitle(const char* title);
	void SetPosition(int x, int y);
	void SetSize(int width, int height);



private:
	Window(Window&&) = delete;
	Window(const Window&) = delete;
	Window& operator=(Window&&) = delete;
	Window& operator=(const Window&) = delete;

	GLFWwindow* m_window = nullptr;
	bool m_requestedVSync = false;
	int m_windowWidth = 0;
	int m_windowHeight = 0;
};

Window& GetWindow();
void* GetWindowHandle();
bool IsWindowFullscreen();
int GetWindowWidth();
int GetWindowHeight();
float GetWindowSizeAspect();