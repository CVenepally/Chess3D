#include "Game/App.hpp"

#include "Engine/Core/Time.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/XMLUtils.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/DebugRender.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Window/Window.hpp"
#include "Game/Game.hpp"


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
App* g_theApp = nullptr;
Renderer* g_theRenderer = nullptr;
AudioSystem* g_theAudioSystem = nullptr;
BitmapFont* g_gameFont = nullptr;
extern Game* g_game;

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
App::App()
{

}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
App::~App()
{

}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void App::Startup()
{

	LoadConfigFile("Data/GameConfig.xml");


	InputConfig inputConfig;
	g_inputSystem = new InputSystem(inputConfig);

	WindowConfig windowConfig;
	windowConfig.m_aspectRatio = g_gameConfigBlackboard.GetValue("windowAspect", 1.f);
	windowConfig.m_theInputSystem = g_inputSystem;
	windowConfig.m_windowTitle = "Chess3D";
	g_theWindow = new Window(windowConfig);

	RenderConfig renderConfig;
	renderConfig.m_window = g_theWindow;
	g_theRenderer = new Renderer(renderConfig);

	EventSystemConfig eventSystemConfig;
	g_eventSystem = new EventSystem(eventSystemConfig);

	DevConsoleConfig devConsoleConfig;
	devConsoleConfig.m_fontSize = 20.f;
	devConsoleConfig.m_fontAspect = 0.7f;
	devConsoleConfig.m_fontFileNamePath = "Data/Fonts/ButlerFont";
	g_devConsole = new DevConsole(devConsoleConfig);

	AudioConfig audioConfig;
	g_theAudioSystem = new AudioSystem(audioConfig);

	DegbugRenderConfig debugConfig;
	debugConfig.m_renderer = g_theRenderer;
	debugConfig.m_fontName = "Font";

	g_game = new Game();


	g_eventSystem->Startup();
	g_devConsole->Startup();
	g_inputSystem->Startup();
	g_theWindow->Startup();
	g_theRenderer->Startup();
	g_theAudioSystem->Startup();
	DebugRenderSystemStartup(debugConfig);
	g_gameFont = g_theRenderer->CreateOrGetBitmapFontWithFontName("Font");

	g_game->Startup();

	SubscribeEventCallbackFunction("Quit", RequestQuitEvent);
}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void App::Shutdown()
{

	UnsubscribeEventCallbackFunction("Quit", RequestQuitEvent);

	g_game->Shutdown();
	g_game = nullptr;

	g_theAudioSystem->Shutdown();
	g_theAudioSystem = nullptr;

	g_theRenderer->Shutdown();
	g_theRenderer = nullptr;

	g_theWindow->Shutdown();
	g_theWindow = nullptr;

	g_inputSystem->Shutdown();
	g_inputSystem = nullptr;

	g_devConsole->Shutdown();
	g_devConsole = nullptr;

	g_eventSystem->Shutdown();
	g_eventSystem = nullptr;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void App::BeginFrame()
{
	Clock::TickSystemClock();

	g_inputSystem->BeginFrame();
	g_theWindow->BeginFrame();
	g_theRenderer->BeginFrame();
	g_theAudioSystem->BeginFrame();
	g_devConsole->BeginFrame();

	DebugRenderBeginFrame();

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void App::EndFrame()
{

	DebugRenderEndFrame();

	g_theAudioSystem->EndFrame();
	g_theRenderer->EndFrame();
	g_theWindow->EndFrame();
	g_inputSystem->EndFrame();

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void App::RunMainFrame()
{
	while(!g_theApp->isQuitting())
	{
		g_theApp->RunFrame();
	}
}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void App::RunFrame()
{
	
	BeginFrame();
	Update();
	Render();
	EndFrame();

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void App::Update()
{

	SetCursorVisibility();

	if(g_inputSystem->WasKeyJustPressed(KEYCODE_F11))
	{
		LoadConfigFile("Data/GameConfig.xml");
	}

	if(g_inputSystem->WasKeyJustPressed(KEYCODE_TILDE))
	{
		g_devConsole->ToggleMode(OPEN_FULL);
	}

	g_game->Update();

	if(g_inputSystem->WasKeyJustPressed(KEYCODE_F8))
	{
		ResetGame();
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void App::Render() const
{
	g_theRenderer->ClearScreen(Rgba8(0, 0, 0, 255));
	g_game->Render();
	g_devConsole->Render(AABB2(Vec2(0.f, 0.f), Vec2(1600.f, 800.f)), g_theRenderer);

}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void App::ResetGame()
{
	
	g_game->Shutdown();
	g_game->Startup();

}

//------------------------------------------------------------------------------------------------------------------
void App::SetCursorVisibility()
{
	if(!g_theWindow->IsWindowActive() || g_devConsole->IsOpen() || g_game->m_isAttractMode)
	{
		g_inputSystem->SetCursorMode(CursorMode::POINTER);
	}
	else
	{
		g_inputSystem->SetCursorMode(CursorMode::FPS);
	}
}

//------------------------------------------------------------------------------------------------------------------
void App::LoadConfigFile(char const* configFilePath)
{

	XmlDocument configFile;

	XmlResult result = configFile.LoadFile(configFilePath);

	if(result == tinyxml2::XML_SUCCESS)
	{
		XmlElement* configRootElement = configFile.RootElement();

		if(configRootElement)
		{
			g_gameConfigBlackboard.PopulateFromXMLElementAttributes(*configRootElement);
		}
		else
		{
			DebuggerPrintf("Config from \"%s\" could not be found. Config root missing or invalid\n", configFilePath);
		}
	}
	else
	{
		DebuggerPrintf("Config File from \"%s\" could not be loaded\n", configFilePath);

	}

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool App::isQuitting() const
{

	return m_isQuitting;

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void App::RequestQuit()
{

	m_isQuitting = true;

}


//------------------------------------------------------------------------------------------------------------------
bool App::RequestQuitEvent(EventArgs& args)
{

	UNUSED(args);

	g_theApp->RequestQuit();

	return false;

}