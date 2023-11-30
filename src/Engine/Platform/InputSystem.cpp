#include "stdafx.h"
#include "InputSystem.h"
#include "WindowSystem.h"
#include "Core/Logging/Log.h"
//-----------------------------------------------------------------------------
InputSystem gInputSystem;
//-----------------------------------------------------------------------------
void GLFWKeyCallback(GLFWwindow* /*window*/, int key, int /*scancode*/, int action, int mods) noexcept
{
	if (key < 0) return;

	//LogPrint("key " + std::to_string(key));

	if (action == GLFW_RELEASE)
	{
		if (gInputSystem.m_keyboard.keyState[key] > Input::BUTTON_RELEASE)
			gInputSystem.m_keyboard.keyState[key] = Input::BUTTON_RELEASE;
	}
	else
	{
		if (gInputSystem.m_keyboard.keyState[key] < Input::BUTTON_PRESS)
			gInputSystem.m_keyboard.keyState[key] = Input::BUTTON_PRESS;
	}

#if !PLATFORM_EMSCRIPTEN && 0 // TODO: fix
	// WARNING: Check if CAPS/NUM key modifiers are enabled and force down state for those keys
	if (((key == Input::KEY_CAPS_LOCK) && ((mods & GLFW_MOD_CAPS_LOCK) > 0)) ||
		((key == Input::KEY_NUM_LOCK) && ((mods & GLFW_MOD_NUM_LOCK) > 0))) gInputSystem.m_keyboard.currentKeyState[key] = 1;
#endif

	if ((gInputSystem.m_keyboard.keyPressedQueueCount < MAX_KEY_PRESSED_QUEUE) && (action == GLFW_PRESS))
	{
		gInputSystem.m_keyboard.keyPressedQueue[gInputSystem.m_keyboard.keyPressedQueueCount] = key;
		gInputSystem.m_keyboard.keyPressedQueueCount++;
	}
}
//-----------------------------------------------------------------------------
void GLFWCharCallback(GLFWwindow* /*window*/, unsigned int key) noexcept
{
	if (gInputSystem.m_keyboard.charPressedQueueCount < MAX_KEY_PRESSED_QUEUE)
	{
		gInputSystem.m_keyboard.charPressedQueue[gInputSystem.m_keyboard.charPressedQueueCount] = (int)key;
		gInputSystem.m_keyboard.charPressedQueueCount++;
	}
}
//-----------------------------------------------------------------------------
void GLFWMouseButtonCallback(GLFWwindow* /*window*/, int button, int action, int /*mods*/) noexcept
{
	gInputSystem.m_mouse.currentButtonState[button] = (char)action;
}
//-----------------------------------------------------------------------------
void GLFWMouseCursorPosCallback(GLFWwindow* /*window*/, double x, double y) noexcept
{
	gInputSystem.m_mouse.currentPosition.x = (float)x;
	gInputSystem.m_mouse.currentPosition.y = (float)y;
}
//-----------------------------------------------------------------------------
void GLFWMouseScrollCallback(GLFWwindow* /*window*/, double xoffset, double yoffset) noexcept
{
	gInputSystem.m_mouse.currentWheelMove = { (float)xoffset, (float)yoffset };
}
//-----------------------------------------------------------------------------
void GLFWCursorEnterCallback(GLFWwindow* /*window*/, int enter) noexcept
{
	gInputSystem.m_mouse.cursorOnScreen = (enter == GLFW_TRUE);
}
//-----------------------------------------------------------------------------
#if PLATFORM_EMSCRIPTEN
EM_BOOL EmscriptenMouseCallback(int /*eventType*/, const EmscriptenMouseEvent* /*mouseEvent*/, void* /*userData*/)
{
	return 1;
}
#endif
//-----------------------------------------------------------------------------
#if PLATFORM_EMSCRIPTEN
EM_BOOL EmscriptenPointerlockchangeCallback(int /*eventType*/, const EmscriptenPointerlockChangeEvent* emscEvent, void* /*userData*/)
{
	gInputSystem.m_mouse.cursorLocked = emscEvent->isActive;
	return EM_TRUE;
}
#endif
//-----------------------------------------------------------------------------
#if PLATFORM_EMSCRIPTEN
EM_BOOL EmscriptenPointerlockerrorCallback(int /*eventType*/, const void* /*reserved*/, void* /*userData*/)
{
	gInputSystem.m_mouse.cursorLocked = false;
	gInputSystem.m_mouse.cursorLockRequested = false;
	return EM_TRUE;
}
#endif
//-----------------------------------------------------------------------------
bool InputSystem::Create()
{
	WindowSystem& wnd = GetWindowSystem();

	glfwSetKeyCallback(wnd.m_window, GLFWKeyCallback);
	glfwSetCharCallback(wnd.m_window, GLFWCharCallback);
	glfwSetMouseButtonCallback(wnd.m_window, GLFWMouseButtonCallback);
	glfwSetCursorPosCallback(wnd.m_window, GLFWMouseCursorPosCallback);
	glfwSetScrollCallback(wnd.m_window, GLFWMouseScrollCallback);
	glfwSetCursorEnterCallback(wnd.m_window, GLFWCursorEnterCallback);

#if PLATFORM_EMSCRIPTEN
	emscripten_set_click_callback("#canvas", NULL, 1, EmscriptenMouseCallback);
	emscripten_set_pointerlockchange_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, 0, true, EmscriptenPointerlockchangeCallback);
	emscripten_set_pointerlockerror_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, 0, true, EmscriptenPointerlockerrorCallback);
#endif

	m_mouse.currentPosition.x = (float)wnd.m_windowWidth / 2.0f;
	m_mouse.currentPosition.y = (float)wnd.m_windowHeight / 2.0f;

	LogPrint("InputSystem Create");

	return true;
}
//-----------------------------------------------------------------------------
void InputSystem::Destroy()
{
	// reset state
	for (size_t i = 0; i < MAX_KEYBOARD_KEYS; i++) m_keyboard.keyState[i] = 0;
	for (size_t i = 0; i < MAX_KEY_PRESSED_QUEUE; i++) m_keyboard.keyPressedQueue[i] = 0;
	m_keyboard.keyPressedQueueCount = 0;
	for (size_t i = 0; i < MAX_CHAR_PRESSED_QUEUE; i++) m_keyboard.charPressedQueue[i] = 0;
	m_keyboard.charPressedQueueCount = 0;

	m_mouse.currentPosition = glm::vec2(0.0f);
	m_mouse.previousPosition = glm::vec2(0.0f);
	m_mouse.deltaPosition = glm::vec2(0.0f);
	m_mouse.cursorLocked = false;
	m_mouse.cursorLockRequested = false;
	m_mouse.cursorOnScreen = false;
	for (size_t i = 0; i < MAX_MOUSE_BUTTONS; i++)
	{
		m_mouse.previousButtonState[i] = 0;
		m_mouse.currentButtonState[i] = 0;
	}
	m_mouse.currentWheelMove = glm::vec2(0.0f);
	m_mouse.previousWheelMove = glm::vec2(0.0f);

	LogPrint("InputSystem Destroy");
}
//-----------------------------------------------------------------------------
void InputSystem::Update()
{
	m_keyboard.keyPressedQueueCount = 0;
	m_keyboard.charPressedQueueCount = 0;

	// TODO: если игра свернута - нужно стирать все состояния клавиш и мыши

	m_mouse.previousWheelMove = m_mouse.currentWheelMove;
	m_mouse.currentWheelMove = { 0.0f, 0.0f };

	m_mouse.deltaPosition = m_mouse.currentPosition - m_mouse.previousPosition;
	m_mouse.previousPosition = m_mouse.currentPosition;

	glfwPollEvents();
}
//-----------------------------------------------------------------------------
bool InputSystem::IsKeyPressed(int key)
{
	if (m_keyboard.keyState[key] == Input::BUTTON_PRESS)
	{
		m_keyboard.keyState[key] = Input::BUTTON_REPEAT;
		return true;
	}
	else return false;
}
//-----------------------------------------------------------------------------
bool InputSystem::IsKeyDown(int key) const
{
	if (m_keyboard.keyState[key] >= Input::BUTTON_PRESS) return true;
	else return false;

}
//-----------------------------------------------------------------------------
bool InputSystem::IsKeyReleased(int key)
{
	if (m_keyboard.keyState[key] == Input::BUTTON_RELEASE)
	{
		m_keyboard.keyState[key] = Input::BUTTON_NONE;
		return true;
	}
	else return false;
}
//-----------------------------------------------------------------------------
bool InputSystem::IsKeyUp(int key) const
{
	if (m_keyboard.keyState[key] <= Input::BUTTON_RELEASE) return true;
	else return false;
}
//-----------------------------------------------------------------------------
int InputSystem::GetKeyPressed()
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
int InputSystem::GetCharPressed()
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
bool InputSystem::IsMouseButtonPressed(int button) const
{
	if ((m_mouse.currentButtonState[button] == 1) && (m_mouse.previousButtonState[button] == 0)) return true;
	return false;
}
//-----------------------------------------------------------------------------
bool InputSystem::IsMouseButtonDown(int button) const
{
	if (m_mouse.currentButtonState[button] == 1) return true;
	return false;
}
//-----------------------------------------------------------------------------
bool InputSystem::IsMouseButtonReleased(int button) const
{
	if ((m_mouse.currentButtonState[button] == 0) && (m_mouse.previousButtonState[button] == 1)) return true;
	return false;
}
//-----------------------------------------------------------------------------
bool InputSystem::IsMouseButtonUp(int button) const
{
	return !IsMouseButtonDown(button);
}
//-----------------------------------------------------------------------------
glm::vec2 InputSystem::GetMousePosition() const
{
	return
	{
		m_mouse.currentPosition.x,
		m_mouse.currentPosition.y
	};
}
//-----------------------------------------------------------------------------
glm::vec2 InputSystem::GetMouseDeltaPosition() const
{
	return m_mouse.deltaPosition;
}
//-----------------------------------------------------------------------------
void InputSystem::SetMousePosition(int x, int y)
{
	WindowSystem& wnd = GetWindowSystem();
	m_mouse.currentPosition = { (float)x, (float)y };
	m_mouse.previousPosition = m_mouse.currentPosition;
	glfwSetCursorPos(wnd.m_window, m_mouse.currentPosition.x, m_mouse.currentPosition.y);
}
//-----------------------------------------------------------------------------
float InputSystem::GetMouseWheelMove() const
{
	float result = 0.0f;
	if (fabsf(m_mouse.currentWheelMove.x) > fabsf(m_mouse.currentWheelMove.y)) result = (float)m_mouse.currentWheelMove.x;
	else result = (float)m_mouse.currentWheelMove.y;
	return result;
}
//-----------------------------------------------------------------------------
glm::vec2 InputSystem::GetMouseWheelMoveV() const
{
	return m_mouse.currentWheelMove;
}
//-----------------------------------------------------------------------------
void InputSystem::SetMouseLock(bool lock)
{
	//if (m_mouse.cursorLocked == lock) return;
	LogPrint(std::string("mouse=") + (lock ? "true": "false"));

#if PLATFORM_EMSCRIPTEN
	if (lock) emscripten_request_pointerlock("#canvas", 1);
	else emscripten_exit_pointerlock();
#else
	WindowSystem& wnd = GetWindowSystem();
	glfwSetInputMode(wnd.m_window, GLFW_CURSOR, lock ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
#endif
	SetMousePosition(GetWindowWidth() / 2, GetWindowHeight() / 2);

	m_mouse.cursorLocked = lock;
}
//-----------------------------------------------------------------------------
bool InputSystem::IsCursorOnScreen() const
{
	return m_mouse.cursorOnScreen;
}
//-----------------------------------------------------------------------------
bool InputSystem::IsMouseLock() const
{
	return m_mouse.cursorLocked;
}
//-----------------------------------------------------------------------------
InputSystem& GetInputSystem()
{
	return gInputSystem;
}
//-----------------------------------------------------------------------------