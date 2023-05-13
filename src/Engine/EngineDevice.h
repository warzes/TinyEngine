﻿#pragma once

#include "LogSystem.h"
#include "EngineTimestamp.h"
#include "FileSystem.h"
#include "Window.h"
#include "Input.h"
#include "RenderSystem.h"
#include "GraphicsSystem.h"
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
	EngineDevice() = default;
	~EngineDevice();

	static std::shared_ptr<EngineDevice> Create(const EngineDeviceCreateInfo& createInfo);

	void RunApp(std::shared_ptr<IApp> startApp);

private:
	EngineDevice(EngineDevice&&) = delete;
	EngineDevice(const EngineDevice&) = delete;
	EngineDevice& operator=(EngineDevice&&) = delete;
	EngineDevice& operator=(const EngineDevice&) = delete;

	void init(const EngineDeviceCreateInfo& createInfo);
	void close();

	void update();
	void render();
	void present();

	std::shared_ptr<IApp> m_currentApp = nullptr;
	EngineTimestamp m_timestamp;
};

void ExitRequest();