#pragma once

#pragma warning( disable:4005)
#define DIRECTINPUT_VERSION 0x0800 

// Window Basic Header
#include <WinSock2.h>
#include <windows.h>
#include <assert.h>
#include <tchar.h>
#include <math.h>

// Direct 3D Headers
#include <d3dx11.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx9.h>
#include <dxgi.h> 
#include <d3dcompiler.h>
#include <dinput.h>

// Direct 2D Headers
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <dxgitype.h>

#include<wrl.h>
#include<wrl/client.h>


// Direct Library Link 
#pragma comment( lib, "dinput8.lib" )
#pragma comment(lib, "legacy_stdio_definitions.lib")

#pragma comment( lib, "dxguid.lib" )
#pragma comment( lib, "d3d11.lib" )

#if defined(DEBUG) || defined(_DEBUG)
#pragma comment( lib, "d3dx10d.lib" )
#pragma comment( lib, "d3dx11d.lib" )

#else
#pragma comment( lib, "d3dx10.lib" )
#pragma comment( lib, "d3dx11.lib" )
#pragma comment( lib, "d3dx11.lib" )
#endif

#pragma comment( lib, "dxgi.lib" )
#pragma comment( lib, "d2d1.lib" )
#pragma comment( lib, "dwrite.lib" )

#pragma comment( lib, "d3dcompiler.lib" )
#pragma comment( lib, "winmm.lib" )
#pragma comment( lib, "comctl32.lib" )

