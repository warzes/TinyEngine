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
extern Input gInput;
extern Window gWindow;
extern RenderSystem gRenderSystem;
extern GraphicsSystem gGraphicsSystem;
//-----------------------------------------------------------------------------
EngineDevice::~EngineDevice()
{
	close();
}
//-----------------------------------------------------------------------------
std::shared_ptr<EngineDevice> EngineDevice::Create(const EngineDeviceCreateInfo& createInfo)
{
	auto ptr = std::make_shared<EngineDevice>();
	ptr->init(createInfo);
	return ptr;
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
		m_timestamp.PreviousFrameTimestamp = EngineTimestamp::GetTime();

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
void EngineDevice::init(const EngineDeviceCreateInfo& createInfo)
{
	gLogSystem.Create(createInfo.log);
	LogPrint("EngineDevice Create");

	if( !gWindow.Create(createInfo.window))
		return;

	gInput.Create();
	gRenderSystem.Create(createInfo.render);
	gGraphicsSystem.Create();

	isExitRequested = false;
}
//-----------------------------------------------------------------------------
void EngineDevice::close()
{
	LogPrint("EngineDevice Destroy");
	
	gGraphicsSystem.Destroy();
	gRenderSystem.Destroy();
	gWindow.Destroy();
	gLogSystem.Destroy();
}
//-----------------------------------------------------------------------------
void EngineDevice::update()
{
	gWindow.Update();
	m_timestamp.Update();
	gInput.Update();

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
	gWindow.Present();
}
//-----------------------------------------------------------------------------
void ExitRequest()
{
	isExitRequested = true;
}
//-----------------------------------------------------------------------------