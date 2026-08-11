#include "Game/App.hpp"

#include "Engine/Core/Time.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/XMLUtils.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/NamedProperties.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/DebugRender.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Window/Window.hpp"
#include "Engine/Network/NetworkSystem.hpp"
#include "Game/Game.hpp"


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
App*			g_theApp			= nullptr;
Renderer*		g_theRenderer		= nullptr;
AudioSystem*	g_theAudioSystem	= nullptr;
BitmapFont*		g_gameFont			= nullptr;
extern Game*	g_game;

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

	NetworkConfig netConfig;
	netConfig.m_hostAddress = "127.0.0.1";
	netConfig.m_maxClients = 4;
	g_netSystem = new NetworkSystem(netConfig);

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
	g_netSystem->Startup();
	DebugRenderSystemStartup(debugConfig);
	g_gameFont = g_theRenderer->CreateOrGetBitmapFontWithFontName("Font");

	g_game->Startup();

	SubscribeEventCallbackFunction("Quit", RequestQuitEvent);
	SubscribeEventCallbackFunction("ChessListen", OnChessListen);
	SubscribeEventCallbackFunction("ChessConnect", OnChessConnect);
	SubscribeEventCallbackFunction("ChessDisconnect", OnChessDisconnect);
	SubscribeEventCallbackFunction("ChessServerInfo", OnChessServerInfo);
}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void App::Shutdown()
{
	UnsubscribeEventCallbackFunction("ChessServerInfo", OnChessServerInfo);
	UnsubscribeEventCallbackFunction("ChessDisconnect", OnChessDisconnect);
	UnsubscribeEventCallbackFunction("ChessConnect", OnChessConnect);
	UnsubscribeEventCallbackFunction("ChessListen", OnChessListen);
	UnsubscribeEventCallbackFunction("Quit", RequestQuitEvent);

	g_game->Shutdown();
	g_game = nullptr;

	g_netSystem->Shutdown();
	g_netSystem = nullptr;

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
	g_netSystem->BeginFrame();

	DebugRenderBeginFrame();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void App::EndFrame()
{
	DebugRenderEndFrame();

	g_netSystem->EndFrame();
	g_devConsole->EndFrame();
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
	g_devConsole->Render(AABB2(Vec2(0.f, 0.f), Vec2(g_theWindow->GetClientDimensions())), g_theRenderer);
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
	if(!g_theWindow->IsWindowActive() || g_devConsole->IsOpen() || g_game->m_state != GameState::MATCH)
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

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool App::OnChessListen(EventArgs& args)
{
	UNUSED(args);
	g_netSystem->StartServer();
	return false;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool App::OnChessConnect(EventArgs& args)
{
	std::string ipAddress = args.GetValue("ip", "127.0.0.1");
	std::string port = args.GetValue("port", "3100");

	g_netSystem->StartClient(ipAddress, port);
	return false;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool App::OnChessDisconnect(EventArgs& args)
{
	std::string reason = args.GetValue("reason", " ");

	g_netSystem->Disconnect(reason);

	g_game->m_isMultiplayer = false;

	return false;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool App::OnChessServerInfo(EventArgs& args)
{
	UNUSED(args);

	g_devConsole->AddLine(DevConsole::INFO_MINOR, Stringf("IP Address: %s", g_netSystem->GetIPAddress().c_str()));
	g_devConsole->AddLine(DevConsole::INFO_MINOR, Stringf("Port Number: %s", g_netSystem->GetNetPort().c_str()));
	g_devConsole->AddLine(DevConsole::INFO_MINOR, Stringf("Num Clients Connected: %d", g_netSystem->GetNumConnectedClients()));

	return false;
}

