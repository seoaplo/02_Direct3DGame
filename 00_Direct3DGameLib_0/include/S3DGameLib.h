#pragma once
#include "SCore.h"
#include "SModel.h"

#ifdef _WIN64
#pragma comment(lib, "fmod64_vc.lib")
#else
#pragma comment(lib, "fmod_vc.lib")
#endif // _WIN64


#if defined(DEBUG) || defined(_DEBUG)

#ifdef _WIN64
#pragma comment( lib, "00_Direct3DGameLib_0_x64_D.lib" )
#else
#pragma comment( lib, "00_Direct3DGameLib_0_x86_D.lib" )
#endif // _WIN64

#else

#ifdef _WIN64
#pragma comment( lib, "00_Direct3DGameLib_0_x64_R.lib" )
#else
#pragma comment( lib, "00_Direct3DGameLib_0_x86_R.lib" )
#endif // _WIN64

#endif
