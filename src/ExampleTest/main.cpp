#include "stdafx.h"
// Basic Example
#include "001_Triangle.h"
#include "002_DinamicVertex.h"
#include "003_BufferMapping.h"
#include "004_Quad.h"
#include "005_BasicTexture.h"
#include "006_Multitexturing.h"
#include "007_LightMaps.h"
#include "008_AlphaMapping.h"
#include "009_DiffuseLighting.h"
#include "010_AmbientLighting.h"
#include "011_SpecularLighting.h"
#include "012_MultiplePointLights.h"
#include "013_OtherMultipleLights.h"
#include "014_NormalMapping.h"
#include "015_SpecularMapping.h"
#include "016_BasicObjModel.h"
#include "017_Framebuffer.h"
//-----------------------------------------------------------------------------
#if defined(_MSC_VER)
#	pragma comment( lib, "Engine.lib" )
#endif
//-----------------------------------------------------------------------------
int main(
	[[maybe_unused]] int   argc,
	[[maybe_unused]] char* argv[])
{
	while( 1 )
	{
		std::cout << "Select Example (q - exit):" << std::endl;
		std::cout << "Basic:" << std::endl;
		std::cout << "    b1 - Basic Draw Triangle" << std::endl;
		std::cout << "    b2 - Dinamic Vertex" << std::endl;
		std::cout << "    b3 - Buffer Mapping" << std::endl;
		std::cout << "    b4 - Draw Quad" << std::endl;
		std::cout << "    b5 - Draw Basic Texture" << std::endl;
		std::cout << "    b6 - Multitexturing" << std::endl;
		std::cout << "    b7 - Light Maps" << std::endl;
		std::cout << "    b8 - Alpha Mapping" << std::endl;
		std::cout << "    b9 - Diffuse Lighting" << std::endl;
		std::cout << "    b10 - Ambient Lighting" << std::endl;
		std::cout << "    b11 - Specular Lighting" << std::endl;
		std::cout << "    b12 - Multiple Point Lights" << std::endl;
		std::cout << "    b13 - Other Multiple Lights" << std::endl;
		std::cout << "    b14 - Normal Mapping Texture" << std::endl;
		std::cout << "    b15 - Specular Mapping" << std::endl;
		std::cout << "    b16 - Basic ObjModel Loader" << std::endl;
		std::cout << "    b17 - Framebuffer" << std::endl;

		std::cout << std::endl;

		std::string read;
		std::cin >> read;

		if( read == "q" )
			break;

#define START_SCENE(arg, x) \
		if( read == arg ) \
		{ \
			auto engineDevice = EngineDevice::Create({}); \
			engineDevice->RunApp(std::make_shared<x>()); \
		}
		START_SCENE("b1", _001Triangle);
		START_SCENE("b2", _002DinamicVertex);
		START_SCENE("b3", _003BufferMapping);
		START_SCENE("b4", _004Quad);
		START_SCENE("b5", _005BasicTexture);
		START_SCENE("b6", _006Multitexturing);
		START_SCENE("b7", _007LightMaps);
		START_SCENE("b8", _008AlphaMapping);
		START_SCENE("b9", _009DiffuseLighting);
		START_SCENE("b10", _010AmbientLighting);
		START_SCENE("b11", _011SpecularLighting);
		START_SCENE("b12", _012MultiplePointLights);
		START_SCENE("b13", _013OtherMultipleLights);
		START_SCENE("b14", _014NormalMapping);
		START_SCENE("b15", _015SpecularMapping);
		START_SCENE("b16", _016BasicObjModel);
		START_SCENE("b17", _017Framebuffer);

#undef START_SCENE
	}}
//-----------------------------------------------------------------------------