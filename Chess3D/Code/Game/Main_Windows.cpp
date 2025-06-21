#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <windows.h>			// #include this (massive, platform-specific) header in VERY few places (and .CPPs only)
#include <math.h>
#include <cassert>
#include <crtdbg.h>

#include "Game/App.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"


extern App* g_theApp;
extern InputSystem* g_inputSystem;


//-----------------------------------------------------------------------------------------------
int WINAPI WinMain( HINSTANCE applicationInstanceHandle, HINSTANCE, LPSTR commandLineString, int )
{
	UNUSED(commandLineString);
	UNUSED(applicationInstanceHandle);
	
	g_theApp = new App();
	g_theApp->Startup();

	g_theApp->RunMainFrame();
	
	 g_theApp->Shutdown();
	 delete g_theApp;
	 g_theApp = nullptr;

	return 0;
}


