#include "stdafx.h"
#include "EngineDevice.h"
//-----------------------------------------------------------------------------
#if defined(_MSC_VER)
#	pragma comment( lib, "3rdparty.lib" )
#endif
//-----------------------------------------------------------------------------
bool isExitRequested = true;
//-----------------------------------------------------------------------------
extern LogSystem gLogSystem;
extern InputSystem gInputSystem;
extern WindowSystem gWindowSystem;
extern RenderSystem gRenderSystem;
extern GraphicsSystem gGraphicsSystem;
//-----------------------------------------------------------------------------
EngineDevice::EngineDevice(const EngineDeviceCreateInfo& createInfo)
{
	if (!gLogSystem.Create(createInfo.log)) return;
	LogPrint("EngineDevice Create");
	if (!gWindowSystem.Create(createInfo.window)) return;
	if (!gInputSystem.Create()) return;

	if (!gRenderSystem.Create(createInfo.render)) return;
	if (!gGraphicsSystem.Create()) return;

	m_timestamp.PreviousFrameTimestamp = EngineTimestamp::GetTime();
	isExitRequested = false;
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
	if( !app || isExitRequested ) return;

	LogPrint("EngineDevice RunApp");

	// Init
	m_currentApp = app;
	if( m_currentApp->Create() )
	{
		while( !isExitRequested )
		{
			update();
			render();
			present();
		}
	}
	// Destroy App
	m_currentApp->Destroy();
	m_currentApp = nullptr;
}
//-----------------------------------------------------------------------------
void EngineDevice::update()
{
	gWindowSystem.Update();
	m_timestamp.Update();
	gInputSystem.Update();

	m_currentApp->Update(static_cast<float>(m_timestamp.ElapsedTime));

	m_timestamp.UpdateAverageFrameTime();
}
//-----------------------------------------------------------------------------
void EngineDevice::render()
{
	m_currentApp->Render();
}
//-----------------------------------------------------------------------------
void EngineDevice::present()
{
	gWindowSystem.Present();
}
//-----------------------------------------------------------------------------
void ExitRequest()
{
	isExitRequested = true;
}
//-----------------------------------------------------------------------------