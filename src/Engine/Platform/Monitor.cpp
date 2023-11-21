#include "stdafx.h"
#include "Monitor.h"
#include "WindowSystem.h"
#include "Core/Logging/Log.h"
//-----------------------------------------------------------------------------
int Monitor::GetCount()
{
#if PLATFORM_DESKTOP
	int monitorCount = 0;
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
	int monitorCount = 0;
	GLFWmonitor **monitors = glfwGetMonitors(&monitorCount);
	GLFWmonitor *monitor = nullptr;

	if( monitorCount > 1 )
	{
		if( IsWindowFullscreen() )
		{
			// Get the handle of the monitor that the specified window is in full screen on
			monitor = glfwGetWindowMonitor(GetGLFWWindow());

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
			// In case the window is between two monitors, we use below logic to try to detect the "current monitor" for that window, note that this is probably an overengineered solution for a very side case trying to match SDL behaviour

			int closestDist = 0x7FFFFFFF;

			// Window center position
			int wcx = 0;
			int wcy = 0;
			glfwGetWindowPos(GetGLFWWindow(), &wcx, &wcy);
			wcx += (int)GetWindowWidth() / 2;
			wcy += (int)GetWindowHeight() / 2;

			for (int i = 0; i < monitorCount; i++)
			{
				// Monitor top-left position
				int mx = 0;
				int my = 0;

				monitor = monitors[i];
				glfwGetMonitorPos(monitor, &mx, &my);
				const GLFWvidmode* mode = glfwGetVideoMode(monitor);

				if (mode)
				{
					const int right = mx + mode->width - 1;
					const int bottom = my + mode->height - 1;

					if ((wcx >= mx) && (wcx <= right) && (wcy >= my) && (wcy <= bottom))
					{
						index = i;
						break;
					}

					int xclosest = wcx;
					if (wcx < mx) xclosest = mx;
					else if (wcx > right) xclosest = right;

					int yclosest = wcy;
					if (wcy < my) yclosest = my;
					else if (wcy > bottom) yclosest = bottom;

					int dx = wcx - xclosest;
					int dy = wcy - yclosest;
					int dist = (dx * dx) + (dy * dy);
					if (dist < closestDist)
					{
						index = i;
						closestDist = dist;
					}
				}
				else LogWarning("GLFW: Failed to find video mode for selected monitor");
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
	int monitorCount = 0;
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
	int width = 0;
	int monitorCount = 0;
	GLFWmonitor **monitors = glfwGetMonitors(&monitorCount);

	if( (monitorId >= 0) && (monitorId < monitorCount) )
	{
		const GLFWvidmode *mode = glfwGetVideoMode(monitors[monitorId]);

		if( mode ) width = mode->width;
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
	return width;
}
//-----------------------------------------------------------------------------
int Monitor::GetHeight(int monitorId)
{
#if PLATFORM_DESKTOP
	int height = 0;
	int monitorCount = 0;
	GLFWmonitor **monitors = glfwGetMonitors(&monitorCount);

	if( (monitorId >= 0) && (monitorId < monitorCount) )
	{
		const GLFWvidmode *mode = glfwGetVideoMode(monitors[monitorId]);

		if( mode ) height = mode->height;
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
	return height;
}
//-----------------------------------------------------------------------------
int Monitor::GetPhysicalWidth(int monitorId)
{
#if PLATFORM_DESKTOP
	int physicalWidth = 0;
	int monitorCount = 0;
	GLFWmonitor **monitors = glfwGetMonitors(&monitorCount);

	if( (monitorId >= 0) && (monitorId < monitorCount) ) 
		glfwGetMonitorPhysicalSize(monitors[monitorId], &physicalWidth, nullptr);
	else LogWarning("GLFW: Failed to find selected monitor");
#endif
	return physicalWidth;
}
//-----------------------------------------------------------------------------
int Monitor::GetPhysicalHeight(int monitorId)
{
#if PLATFORM_DESKTOP
	int physicalHeight = 0;
	int monitorCount = 0;
	GLFWmonitor **monitors = glfwGetMonitors(&monitorCount);

	if( (monitorId >= 0) && (monitorId < monitorCount) ) 
		glfwGetMonitorPhysicalSize(monitors[monitorId], nullptr, &physicalHeight);
	else LogWarning("GLFW: Failed to find selected monitor");
#endif
	return physicalHeight;
}
//-----------------------------------------------------------------------------
int Monitor::GetRefreshRate(int monitorId)
{
#if PLATFORM_DESKTOP
	int refresh = 0;
	int monitorCount = 0;
	GLFWmonitor **monitors = glfwGetMonitors(&monitorCount);

	if( (monitorId >= 0) && (monitorId < monitorCount) )
	{
		const GLFWvidmode *vidmode = glfwGetVideoMode(monitors[monitorId]);
		refresh = vidmode->refreshRate;
	}
	else LogWarning("GLFW: Failed to find selected monitor");
#endif
	return refresh;
}
//-----------------------------------------------------------------------------
const char* Monitor::GetName(int monitorId)
{
#if PLATFORM_DESKTOP
	int monitorCount = 0;
	GLFWmonitor **monitors = glfwGetMonitors(&monitorCount);

	if( (monitorId >= 0) && (monitorId < monitorCount) )
		return glfwGetMonitorName(monitors[monitorId]);
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
	glm::vec2 windowPos = GetWindowSystem().GetPosition();

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