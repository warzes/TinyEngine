#include "stdafx.h"
#include "EngineDevice.h"
//-----------------------------------------------------------------------------
#if defined(_MSC_VER)
#	pragma comment( lib, "3rdparty.lib" )
#endif

physic
terrain
skeletal anim
//-----------------------------------------------------------------------------
bool isExitRequested = true;
//-----------------------------------------------------------------------------
extern LogSystem gLogSystem;
extern InputSystem gInputSystem;
extern WindowSystem gWindowSystem;
extern RenderSystem gRenderSystem;
extern GraphicsSystem gGraphicsSystem;
#if PLATFORM_EMSCRIPTEN
EngineDevice* thisEngineDevice = nullptr;
#endif
//-----------------------------------------------------------------------------
#if PLATFORM_EMSCRIPTEN
void mainLoop()
{
	thisEngineDevice->Update();
	thisEngineDevice->Render();
	thisEngineDevice->Present();
	// TODO: exit command
}
#endif
//-----------------------------------------------------------------------------
EngineDevice::EngineDevice(const EngineDeviceCreateInfo& createInfo)
{
	if (!gLogSystem.Create(createInfo.log)) return;

	if (!gWindowSystem.Create(createInfo.window)) return;
	if (!gInputSystem.Create()) return;

	if (!gRenderSystem.Create(createInfo.render)) return;
	if (!gGraphicsSystem.Create()) return;

	m_timestamp.PreviousFrameTimestamp = EngineTimestamp::GetTime();
#if PLATFORM_EMSCRIPTEN
	thisEngineDevice = this;
#endif
	isExitRequested = false;

	LogPrint("EngineDevice Create");
}
//-----------------------------------------------------------------------------
EngineDevice::~EngineDevice()
{
	LogPrint("EngineDevice Destroy");

	gGraphicsSystem.Destroy();
	gRenderSystem.Destroy();
	gWindowSystem.Destroy();
	gLogSystem.Destroy();
}
//-----------------------------------------------------------------------------
std::shared_ptr<EngineDevice> EngineDevice::Create(const EngineDeviceCreateInfo& createInfo)
{
	return std::make_shared<EngineDevice>(createInfo);
}
//-----------------------------------------------------------------------------
void EngineDevice::RunApp(std::shared_ptr<IApp> app)
{
	assert(app);
	assert(!isExitRequested);
	if (!app || isExitRequested) return;
#if PLATFORM_EMSCRIPTEN
	assert(thisEngineDevice);
	if (!thisEngineDevice) return;
#endif

	LogPrint("EngineDevice RunApp");

	// Init
	m_currentApp = app;
	if( m_currentApp->Create() )
	{
#if PLATFORM_EMSCRIPTEN
		emscripten_set_main_loop(mainLoop, 0, true);
#else
		while( !isExitRequested )
		{
			Update();
			Render();
			Present();
		}
#endif
	}
	// Destroy App
	m_currentApp->Destroy();
	m_currentApp = nullptr;
}
//-----------------------------------------------------------------------------
void EngineDevice::Update()
{
	gWindowSystem.Update();
	m_timestamp.Update();
	gInputSystem.Update();

	m_currentApp->Update(static_cast<float>(m_timestamp.ElapsedTime));

	m_timestamp.UpdateAverageFrameTime();
}
//-----------------------------------------------------------------------------
void EngineDevice::Render()
{
	m_currentApp->Render();
}
//-----------------------------------------------------------------------------
void EngineDevice::Present()
{
	gWindowSystem.Present();
}
//-----------------------------------------------------------------------------
void ExitRequest()
{
	isExitRequested = true;
}
//-----------------------------------------------------------------------------