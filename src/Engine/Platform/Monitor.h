#pragma once

namespace Monitor
{
	int GetCount();
	int GetCurrentId();
	glm::vec2 GetPosition(int monitorId);
	int GetWidth(int monitorId);
	int GetHeight(int monitorId);
	int GetPhysicalWidth(int monitorId);
	int GetPhysicalHeight(int monitorId);
	int GetRefreshRate(int monitorId);

	const char *GetName(int monitorId);

	// Get window scale DPI factor for current monitor
	glm::vec2 GetScaleDPI();
}