﻿#pragma once

#include "Core/Logging/LogSystem.h"
#include "EngineTimestamp.h"
#include "Core/IO/FileSystem.h"
#include "Platform/WindowSystem.h"
#include "Platform/InputSystem.h"
#include "RenderAPI/RenderSystem.h"
#include "Graphics/GraphicsSystem.h"
#include "IApp.h"

struct EngineDeviceCreateInfo final
{
	LogCreateInfo log;
	WindowCreateInfo window;
	RenderCreateInfo render;
};

class EngineDevice final
{
public:
	EngineDevice(const EngineDeviceCreateInfo& createInfo);
	~EngineDevice();

	static std::shared_ptr<EngineDevice> Create(const EngineDeviceCreateInfo& createInfo);

	void RunApp(std::shared_ptr<IApp> startApp);

	void Update();
	void Render();
	void Present();

private:
	EngineDevice(EngineDevice&&) = delete;
	EngineDevice(const EngineDevice&) = delete;
	EngineDevice& operator=(EngineDevice&&) = delete;
	EngineDevice& operator=(const EngineDevice&) = delete;

	std::shared_ptr<IApp> m_currentApp = nullptr;
	EngineTimestamp m_timestamp;
};

void ExitRequest();