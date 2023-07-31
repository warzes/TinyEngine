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

class WindowSystem final
{
	friend class InputSystem;
	friend void GLFWFramebufferSizeCallback(GLFWwindow*, int, int) noexcept;
public:
	WindowSystem() = default;

	bool Create(const WindowCreateInfo& createInfo);
	void Destroy();

	void Update();
	void Present();

	[[nodiscard]] void* GetHandle() const noexcept;

	[[nodiscard]] int GetWidth() const noexcept;
	[[nodiscard]] int GetHeight() const noexcept;

	[[nodiscard]] glm::vec2 GetPosition() const;

	void SetTitle(const char* title);
	void SetPosition(int x, int y);
	void SetSize(int width, int height);

private:
	WindowSystem(WindowSystem&&) = delete;
	WindowSystem(const WindowSystem&) = delete;
	WindowSystem& operator=(WindowSystem&&) = delete;
	WindowSystem& operator=(const WindowSystem&) = delete;

	GLFWwindow* m_window = nullptr;
	bool m_requestedVSync = false;
	int m_windowWidth = 0;
	int m_windowHeight = 0;
};

WindowSystem& GetWindowSystem();
[[nodiscard]] void* GetWindowHandle() noexcept;
[[nodiscard]] bool IsWindowFullscreen() noexcept;
[[nodiscard]] int GetWindowWidth() noexcept;
[[nodiscard]] int GetWindowHeight() noexcept;
[[nodiscard]] float GetWindowSizeAspect() noexcept;