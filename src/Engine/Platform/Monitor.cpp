#include "stdafx.h"
#include "Monitor.h"
#include "Window.h"
#include "Core/Logging/Log.h"
//-----------------------------------------------------------------------------
int Monitor::GetCount()
{
#if PLATFORM_DESKTOP
	int monitorCount;
	glfwGetMonitors(&monitorCount);
	return monitorCount;
#else
	return 1;
#endif
}
//-----------------------------------------------------------------------------
int Monitor::GetCurrentId()
{
	int index = 0;

#if PLATFORM_DESKTOP
	int monitorCount;
	GLFWmonitor **monitors = glfwGetMonitors(&monitorCount);
	GLFWmonitor *monitor = NULL;

	if( monitorCount > 1 )
	{
		if( IsWindowFullscreen() )
		{
			// Get the handle of the monitor that the specified window is in full screen on
			monitor = glfwGetWindowMonitor((GLFWwindow*)GetWindowHandle());

			for( int i = 0; i < monitorCount; i++ )
			{
				if( monitors[i] == monitor )
				{
					index = i;
					break;
				}
			}
		}
		else
		{
			int x = 0;
			int y = 0;

			glfwGetWindowPos((GLFWwindow*)GetWindowHandle(), &x, &y);

			for( int i = 0; i < monitorCount; i++ )
			{
				int mx = 0;
				int my = 0;

				int width = 0;
				int height = 0;

				monitor = monitors[i];
				glfwGetMonitorWorkarea(monitor, &mx, &my, &width, &height);

				if( (x >= mx) &&
					(x < (mx + width)) &&
					(y >= my) &&
					(y < (my + height)) )
				{
					index = i;
					break;
				}
			}
		}
	}
#endif

	return index;
}
//-----------------------------------------------------------------------------
glm::vec2 Monitor::GetPosition(int monitorId)
{
#if PLATFORM_DESKTOP
	int monitorCount;
	GLFWmonitor **monitors = glfwGetMonitors(&monitorCount);

	if( (monitorId >= 0) && (monitorId < monitorCount) )
	{
		int x, y;
		glfwGetMonitorPos(monitors[monitorId], &x, &y);

		return { (float)x, (float)y };
	}
	else LogWarning("GLFW: Failed to find selected monitor");
#endif
	return { 0.0f, 0.0f };
}
//-----------------------------------------------------------------------------
int Monitor::GetWidth(int monitorId)
{
#if PLATFORM_DESKTOP
	int monitorCount;
	GLFWmonitor **monitors = glfwGetMonitors(&monitorCount);

	if( (monitorId >= 0) && (monitorId < monitorCount) )
	{
		const GLFWvidmode *mode = glfwGetVideoMode(monitors[monitorId]);

		if( mode ) return mode->width;
		else LogWarning("GLFW: Failed to find video mode for selected monitor");
	}
	else LogWarning("GLFW: Failed to find selected monitor");
#endif
#if PLATFORM_ANDROID
	if( CORE.Android.app->window != NULL )
	{
		return ANativeWindow_getWidth(CORE.Android.app->window);
	}
#endif
	return 0;
}
//-----------------------------------------------------------------------------
int Monitor::GetHeight(int monitorId)
{
#if PLATFORM_DESKTOP
	int monitorCount;
	GLFWmonitor **monitors = glfwGetMonitors(&monitorCount);

	if( (monitorId >= 0) && (monitorId < monitorCount) )
	{
		const GLFWvidmode *mode = glfwGetVideoMode(monitors[monitorId]);

		if( mode ) return mode->height;
		else LogWarning("GLFW: Failed to find video mode for selected monitor");
	}
	else LogWarning("GLFW: Failed to find selected monitor");
#endif
#if PLATFORM_ANDROID
	if( CORE.Android.app->window != NULL )
	{
		return ANativeWindow_getHeight(CORE.Android.app->window);
	}
#endif
	return 0;
}
//-----------------------------------------------------------------------------
int Monitor::GetPhysicalWidth(int monitorId)
{
#if PLATFORM_DESKTOP
	int monitorCount;
	GLFWmonitor **monitors = glfwGetMonitors(&monitorCount);

	if( (monitorId >= 0) && (monitorId < monitorCount) )
	{
		int physicalWidth;
		glfwGetMonitorPhysicalSize(monitors[monitorId], &physicalWidth, NULL);
		return physicalWidth;
	}
	else LogWarning("GLFW: Failed to find selected monitor");
#endif
	return 0;
}
//-----------------------------------------------------------------------------
int Monitor::GetPhysicalHeight(int monitorId)
{
#if PLATFORM_DESKTOP
	int monitorCount;
	GLFWmonitor **monitors = glfwGetMonitors(&monitorCount);

	if( (monitorId >= 0) && (monitorId < monitorCount) )
	{
		int physicalHeight;
		glfwGetMonitorPhysicalSize(monitors[monitorId], NULL, &physicalHeight);
		return physicalHeight;
	}
	else LogWarning("GLFW: Failed to find selected monitor");
#endif
	return 0;
}
//-----------------------------------------------------------------------------
int Monitor::GetRefreshRate(int monitorId)
{
#if PLATFORM_DESKTOP
	int monitorCount;
	GLFWmonitor **monitors = glfwGetMonitors(&monitorCount);

	if( (monitorId >= 0) && (monitorId < monitorCount) )
	{
		const GLFWvidmode *vidmode = glfwGetVideoMode(monitors[monitorId]);
		return vidmode->refreshRate;
	}
	else LogWarning("GLFW: Failed to find selected monitor");
#endif
	return 0;
}
//-----------------------------------------------------------------------------
const char* Monitor::GetName(int monitorId)
{
#if PLATFORM_DESKTOP
	int monitorCount;
	GLFWmonitor **monitors = glfwGetMonitors(&monitorCount);

	if( (monitorId >= 0) && (monitorId < monitorCount) )
	{
		return glfwGetMonitorName(monitors[monitorId]);
	}
	else LogWarning("GLFW: Failed to find selected monitor");
#endif
	return "";
}
//-----------------------------------------------------------------------------
glm::vec2 Monitor::GetScaleDPI()
{
	glm::vec2 scale = { 1.0f, 1.0f };

#if PLATFORM_DESKTOP
	float xdpi = 1.0;
	float ydpi = 1.0;
	glm::vec2 windowPos = GetWindow().GetPosition();

	int monitorCount = 0;
	GLFWmonitor **monitors = glfwGetMonitors(&monitorCount);

	// Check window monitor
	for( int i = 0; i < monitorCount; i++ )
	{
		glfwGetMonitorContentScale(monitors[i], &xdpi, &ydpi);

		int xpos, ypos, width, height;
		glfwGetMonitorWorkarea(monitors[i], &xpos, &ypos, &width, &height);

		if( (windowPos.x >= xpos) && (windowPos.x < xpos + width) &&
			(windowPos.y >= ypos) && (windowPos.y < ypos + height) )
		{
			scale.x = xdpi;
			scale.y = ydpi;
			break;
		}
	}
#endif

	return scale;
}
//-----------------------------------------------------------------------------