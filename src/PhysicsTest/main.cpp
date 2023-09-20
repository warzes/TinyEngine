#include "stdafx.h"
#include "TestApp.h"
//-----------------------------------------------------------------------------
#if defined(_MSC_VER)
#	pragma comment( lib, "Engine.lib" )
#endif
//-----------------------------------------------------------------------------
int main(
	[[maybe_unused]] int   argc,
	[[maybe_unused]] char* argv[])
{
	// TODO: PhysicsTransform test

	EngineDeviceCreateInfo createInfo;
	createInfo.physics.enable = true;
	auto engineDevice = EngineDevice::Create(createInfo);
	engineDevice->RunApp(std::make_shared<TestApp>());
}
//-----------------------------------------------------------------------------