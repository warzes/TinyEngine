#include "stdafx.h"
#include "Input.h"
#include "Window.h"
//-----------------------------------------------------------------------------
Input gInput;
//-----------------------------------------------------------------------------
void GLFWKeyCallback(GLFWwindow* /*window*/, int key, int /*scancode*/, int action, int /*mods*/) noexcept
{
	if (key < 0) return;

	if (action == GLFW_RELEASE) gInput.m_keyboard.currentKeyState[key] = 0;
	else gInput.m_keyboard.currentKeyState[key] = 1;

	if ((gInput.m_keyboard.keyPressedQueueCount < MAX_KEY_PRESSED_QUEUE) && (action == GLFW_PRESS))
	{
		gInput.m_keyboard.keyPressedQueue[gInput.m_keyboard.keyPressedQueueCount] = key;
		gInput.m_keyboard.keyPressedQueueCount++;
	}
}
//-----------------------------------------------------------------------------
void GLFWCharCallback(GLFWwindow* /*window*/, unsigned int key) noexcept
{
	if (gInput.m_keyboard.charPressedQueueCount < MAX_KEY_PRESSED_QUEUE)
	{
		gInput.m_keyboard.charPressedQueue[gInput.m_keyboard.charPressedQueueCount] = (int)key;
		gInput.m_keyboard.charPressedQueueCount++;
	}
}
//-----------------------------------------------------------------------------
void GLFWMouseButtonCallback(GLFWwindow* /*window*/, int button, int action, int /*mods*/) noexcept
{
	gInput.m_mouse.currentButtonState[button] = (char)action;
}
//-----------------------------------------------------------------------------
void GLFWMouseCursorPosCallback(GLFWwindow* /*window*/, double x, double y) noexcept
{
	gInput.m_mouse.currentPosition.x = (float)x;
	gInput.m_mouse.currentPosition.y = (float)y;
}
//-----------------------------------------------------------------------------
void GLFWMouseScrollCallback(GLFWwindow* /*window*/, double xoffset, double yoffset) noexcept
{
	gInput.m_mouse.currentWheelMove = { (float)xoffset, (float)yoffset };
}
//-----------------------------------------------------------------------------
void GLFWCursorEnterCallback(GLFWwindow* /*window*/, int enter) noexcept
{
	gInput.m_mouse.cursorOnScreen = (enter == GLFW_TRUE);
}
//-----------------------------------------------------------------------------
void Input::Create()
{
	Window& wnd = GetWindow();

	glfwSetKeyCallback(wnd.m_window, GLFWKeyCallback);
	glfwSetCharCallback(wnd.m_window, GLFWCharCallback);
	glfwSetMouseButtonCallback(wnd.m_window, GLFWMouseButtonCallback);
	glfwSetCursorPosCallback(wnd.m_window, GLFWMouseCursorPosCallback);
	glfwSetScrollCallback(wnd.m_window, GLFWMouseScrollCallback);
	glfwSetCursorEnterCallback(wnd.m_window, GLFWCursorEnterCallback);

	m_mouse.currentPosition.x = (float)wnd.m_windowWidth / 2.0f;
	m_mouse.currentPosition.y = (float)wnd.m_windowHeight / 2.0f;
}
//-----------------------------------------------------------------------------
void Input::Update()
{
	m_keyboard.keyPressedQueueCount = 0;
	m_keyboard.charPressedQueueCount = 0;

	for (int i = 0; i < MAX_KEYBOARD_KEYS; i++) m_keyboard.previousKeyState[i] = m_keyboard.currentKeyState[i];
	for (int i = 0; i < MAX_MOUSE_BUTTONS; i++) m_mouse.previousButtonState[i] = m_mouse.currentButtonState[i];

	m_mouse.previousWheelMove = m_mouse.currentWheelMove;
	m_mouse.currentWheelMove = { 0.0f, 0.0f };
	m_mouse.previousPosition = m_mouse.currentPosition;

	glfwPollEvents();
}
//-----------------------------------------------------------------------------
bool Input::IsKeyPressed(int key) const
{
	if ((m_keyboard.previousKeyState[key] == 0) && (m_keyboard.currentKeyState[key] == 1)) return true;
	return false;
}
//-----------------------------------------------------------------------------
bool Input::IsKeyDown(int key) const
{
	if (m_keyboard.currentKeyState[key] == 1) return true;
	else return false;
}
//-----------------------------------------------------------------------------
bool Input::IsKeyReleased(int key) const
{
	if ((m_keyboard.previousKeyState[key] == 1) && (m_keyboard.currentKeyState[key] == 0)) return true;
	return false;
}
//-----------------------------------------------------------------------------
bool Input::IsKeyUp(int key) const
{
	if (m_keyboard.currentKeyState[key] == 0) return true;
	else return false;
}
//-----------------------------------------------------------------------------
int Input::GetKeyPressed()
{
	int value = 0;

	if (m_keyboard.keyPressedQueueCount > 0)
	{
		value = m_keyboard.keyPressedQueue[0];

		for (int i = 0; i < (m_keyboard.keyPressedQueueCount - 1); i++)
			m_keyboard.keyPressedQueue[i] = m_keyboard.keyPressedQueue[i + 1];

		m_keyboard.keyPressedQueue[m_keyboard.keyPressedQueueCount] = 0;
		m_keyboard.keyPressedQueueCount--;
	}

	return value;
}
//-----------------------------------------------------------------------------
int Input::GetCharPressed()
{
	int value = 0;

	if (m_keyboard.charPressedQueueCount > 0)
	{
		value = m_keyboard.charPressedQueue[0];

		for (int i = 0; i < (m_keyboard.charPressedQueueCount - 1); i++)
			m_keyboard.charPressedQueue[i] = m_keyboard.charPressedQueue[i + 1];

		m_keyboard.charPressedQueue[m_keyboard.charPressedQueueCount] = 0;
		m_keyboard.charPressedQueueCount--;
	}

	return value;
}
//-----------------------------------------------------------------------------
bool Input::IsMouseButtonPressed(int button) const
{
	if ((m_mouse.currentButtonState[button] == 1) && (m_mouse.previousButtonState[button] == 0)) return true;
	return false;
}
//-----------------------------------------------------------------------------
bool Input::IsMouseButtonDown(int button) const
{
	if (m_mouse.currentButtonState[button] == 1) return true;
	return false;
}
//-----------------------------------------------------------------------------
bool Input::IsMouseButtonReleased(int button) const
{
	if ((m_mouse.currentButtonState[button] == 0) && (m_mouse.previousButtonState[button] == 1)) return true;
	return false;
}
//-----------------------------------------------------------------------------
bool Input::IsMouseButtonUp(int button) const
{
	return !IsMouseButtonDown(button);
}
//-----------------------------------------------------------------------------
glm::vec2 Input::GetMousePosition() const
{
	return
	{
		m_mouse.currentPosition.x,
		m_mouse.currentPosition.y
	};
}
//-----------------------------------------------------------------------------
glm::vec2 Input::GetMouseDeltaPosition() const
{
	return
	{
		m_mouse.currentPosition.x - m_mouse.previousPosition.x,
		m_mouse.currentPosition.y - m_mouse.previousPosition.y
	};
}
//-----------------------------------------------------------------------------
void Input::SetMousePosition(int x, int y)
{
	Window& wnd = GetWindow();
	m_mouse.currentPosition = { (float)x, (float)y };
	m_mouse.previousPosition = m_mouse.currentPosition;
	glfwSetCursorPos(wnd.m_window, m_mouse.currentPosition.x, m_mouse.currentPosition.y);
}
//-----------------------------------------------------------------------------
float Input::GetMouseWheelMove() const
{
	float result = 0.0f;
	if (fabsf(m_mouse.currentWheelMove.x) > fabsf(m_mouse.currentWheelMove.y)) result = (float)m_mouse.currentWheelMove.x;
	else result = (float)m_mouse.currentWheelMove.y;
	return result;
}
//-----------------------------------------------------------------------------
glm::vec2 Input::GetMouseWheelMoveV() const
{
	return m_mouse.currentWheelMove;
}
//-----------------------------------------------------------------------------
void Input::SetMouseLock(bool lock)
{
	if (m_mouse.cursorHidden == lock) return;

	Window& wnd = GetWindow();

#if defined(__EMSCRIPTEN__)
	if (lock) emscripten_request_pointerlock("#canvas", 1);
	else emscripten_exit_pointerlock();
#else
	glfwSetInputMode(wnd.m_window, GLFW_CURSOR, lock ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
#endif
	SetMousePosition(GetWindowWidth() / 2, GetWindowHeight() / 2);

	m_mouse.cursorHidden = lock;
}
//-----------------------------------------------------------------------------
bool Input::IsCursorOnScreen() const
{
	return m_mouse.cursorOnScreen;
}
//-----------------------------------------------------------------------------
Input& GetInput()
{
	return gInput;
}
//-----------------------------------------------------------------------------