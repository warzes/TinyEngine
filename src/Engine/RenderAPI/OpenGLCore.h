#pragma once

#define OPENGL33 33
#define OPENGL40 40
#define OPENGL41 41
#define OPENGL42 42
#define OPENGL43 43
#define OPENGL44 44
#define OPENGL45 45
#define OPENGL46 46

#define USE_OPENGL_VERSION OPENGL46

namespace OpenGLExtensions
{
	extern uint8_t version;
	extern bool coreDebug;
	extern bool coreDirectStateAccess;
}