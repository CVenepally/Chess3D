#include "Game/Game.hpp"
#include "Game/App.hpp"
#include "Game/Player.hpp"
#include "Game/ChessMatch.hpp"
#include "Game/ChessPieceDefinition.hpp"
#include "Game/GameCommon.hpp"

#include "Engine/Core/Vertex_PCUTBN.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Timer.hpp"
#include "Engine/Core/DebugRender.hpp"
#include "Engine/Math/Sphere.hpp"
//------------------------------------------------------------------------------------------------------------------
Game* g_game = nullptr;
RandomNumberGenerator g_numGenerator;

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Game::Game()
{

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Game::~Game()
{
	 
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Game::Startup()
{
//	PrintControlsOnDevConsole();

	m_isAttractMode = true;

	m_gameClock = new Clock(Clock::GetSystemClock());

	m_timerOne = new Timer(5., m_gameClock);
	
	m_timerTwo = new Timer(0.5, m_gameClock);

	InitializeGridVerts(50);

	ChessPieceDefinition::InitializeChessPieceDefinition();

	m_player = new Player(this);

	m_screenCamera.m_viewportBounds.m_mins = Vec2::ZERO;
	m_screenCamera.m_viewportBounds.m_maxs = Vec2(static_cast<float>(g_theWindow->GetClientDimensions().x), static_cast<float>(g_theWindow->GetClientDimensions().y));

	if(!m_chessMatch)
	{
		BeginChessMatch();
	}

	SubscribeEventCallbackFunction("ChessBegin", OnBeginChessMatch);

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Game::Shutdown()
{
	UnsubscribeEventCallbackFunction("ChessBegin", OnBeginChessMatch);

	ChessPieceDefinition::ClearDefinitions();

	if(m_gameClock)
	{
		delete m_gameClock;
		m_gameClock = nullptr;
	}
	
	if(m_timerOne)
	{
		delete m_timerOne;
		m_timerOne = nullptr;
	}

	if(m_timerTwo)
	{
		delete m_timerTwo;
		m_timerTwo = nullptr;
	}

	if(m_chessMatch)
	{
		delete m_chessMatch;
		m_chessMatch = nullptr;
	}

	m_gridVerts.clear();

	FireEvent("Clear");
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Game::Update()
{

	TimeManipulation();

	KeyboardControls();
	XboxControls();

	if(!m_isAttractMode)
	{
		if(m_chessMatch)
		{
			m_chessMatch->Update();
		}
 		m_player->Update();
	}

	UpdateCameras();
}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Game::Render() const
{
	g_theRenderer->BeginRenderEvent("Game - World Render");
	g_theRenderer->BeginCamera(m_player->m_playerCamera);

	if(!m_isAttractMode)
	{
		g_theRenderer->ClearScreen(Rgba8(0, 0, 0, 255));
		g_theRenderer->SetDebugConstants(m_gameClock->GetDeltaSeconds(), m_debugInt, m_debugFloat);
		RenderGame();
	}

	if(m_isDebugMode)
	{
		DebugRenderWorld(m_player->m_playerCamera);
	}

	g_theRenderer->EndCamera(m_player->m_playerCamera);
	g_theRenderer->EndRenderEvent("Game - World Render");

	g_theRenderer->BeginRenderEvent("Screen Render");
	g_theRenderer->BeginCamera(m_screenCamera);

	if(m_isAttractMode)
	{
		g_theRenderer->ClearScreen(Rgba8(150, 150, 150, 255));
		RenderAttractMode();
	}
	
	if(!m_isAttractMode)
	{
		m_chessMatch->RenderUIText();

		DebugRenderScreen(m_screenCamera);
	}

	g_theRenderer->EndCamera(m_screenCamera);
	g_theRenderer->EndRenderEvent("Screen Render");

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Game::OnBeginChessMatch(EventArgs& args)
{
	UNUSED(args);

	g_game->BeginChessMatch();

	return true;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Vec3 const& Game::GetCameraPosition() const
{
	return m_player->m_playerCamera.m_position;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Game::UpdateCameras()
{
	m_screenCamera.m_mode = Camera::eMode_Orthographic;

	m_screenCamera.SetOrthographicView(Vec2(0.f, 0.f), Vec2(1600.f, 800.f));

	m_player->m_playerCamera.m_mode = Camera::eMode_Perspective;

	if(!m_freeCamera)
	{
		if(m_chessMatch->m_matchState == MatchState::PLAYER_ONE_TURN || m_chessMatch->m_matchState == MatchState::PLAYER_ONE_WINS)
		{
			m_player->m_position = Vec3(4.f, -3.f, 5.f);
			m_player->m_orientation = EulerAngles(90.f, 35.f, 0.f);
		}
		else
		{
			m_player->m_position = Vec3(4.f, 13.f, 5.f);
			m_player->m_orientation = EulerAngles(-90.f, 35.f, 0.f);
		}
	}

	m_player->m_playerCamera.SetPositionAndOrientation(m_player->m_position, m_player->m_orientation);

	Mat44 cameraToRenderMatrix;
	
	cameraToRenderMatrix.SetIJKT3D(Vec3(0.f, 0.f, 1.f), Vec3(-1.f, 0.f, 0.f), Vec3(0.f, 1.f, 0.f), Vec3(0.f, 0.f, 0.f));

	m_player->m_playerCamera.SetCameraToRenderTransform(cameraToRenderMatrix);
	m_player->m_playerCamera.SetPerspectiveView(g_theWindow->GetConfig().m_aspectRatio, 60.f, 0.1f, 100.f);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Game::RenderAttractMode() const
{
	Vec2 ringCenter = Vec2(800.f, 400.f);
	float radius = 60.f;
	float thickness = 5.f;

	if(m_timerOne->GetElapsedFraction() <= 1.)
	{
		radius = Lerp(60.f, 200.f, static_cast<float>(m_timerOne->GetElapsedFraction()));
	}
	else if(m_timerOne->GetElapsedFraction() <= 2.)
	{
		radius = Lerp(200.f, 60.f, static_cast<float>(m_timerOne->GetElapsedFraction()) - 1.f);
	}

	if(m_timerOne->GetElapsedFraction() > 2.)
	{
		m_timerOne->Stop();
		m_timerOne->Start();
	}

	if(m_timerTwo->GetElapsedFraction() <= 1.)
	{
		thickness = Lerp(5.f, 20.f, static_cast<float>(m_timerTwo->GetElapsedFraction()));
	}
	else if(m_timerTwo->GetElapsedFraction() <= 2.)
	{
		thickness = Lerp(20.f, 5.f, static_cast<float>(m_timerTwo->GetElapsedFraction()) - 1.f);
	}

	if(m_timerTwo->GetElapsedFraction() > 2.)
	{
		m_timerTwo->Stop();
		m_timerTwo->Start();
	}

	std::vector<Vertex_PCU> attractModeRingVerts;

	AddVertsForRing(attractModeRingVerts, ringCenter, radius, thickness, Rgba8::ORANGERED);

	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_NONE);
	g_theRenderer->SetBlendMode(BlendMode::ALPHA);
	g_theRenderer->BindShader(nullptr);
	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->DrawVertexArray(attractModeRingVerts);
}

//------------------------------------------------------------------------------------------------------------------
void Game::RenderGame() const
{
//	RenderGrid();
	if(m_chessMatch)
	{
		m_chessMatch->Render();
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Game::InitializeGridVerts(float size)
{

	float gridLineHalfLength = size;
	float gridLineHalfWidth = 0.01f;
	float gridLineHalfHeight = 0.01f;

	Rgba8 xGridLinesColor = Rgba8::GREY;
	Rgba8 yGridLinesColor = Rgba8::GREY;

	AABB3 gridLineAlongX;
	gridLineAlongX.m_mins = Vec3(-gridLineHalfLength, -gridLineHalfWidth, -gridLineHalfHeight);
	gridLineAlongX.m_maxs = Vec3(gridLineHalfLength, gridLineHalfWidth, gridLineHalfHeight);

	AABB3 gridLineAlongY;
	gridLineAlongY.m_mins = Vec3(-gridLineHalfWidth, -gridLineHalfLength, -gridLineHalfHeight);
	gridLineAlongY.m_maxs = Vec3(gridLineHalfWidth, gridLineHalfLength, gridLineHalfHeight);

	for(int gridLineNum = -static_cast<int>(size); gridLineNum <= static_cast<int>(size); ++gridLineNum)
	{

		gridLineAlongX.SetCenter(Vec3(0.f, static_cast<float>(gridLineNum), 0.f));
		gridLineAlongY.SetCenter(Vec3(static_cast<float>(gridLineNum), 0.f, 0.f));

		if(gridLineNum == 0)
		{
			gridLineAlongX.m_mins.y = -0.05f;
			gridLineAlongX.m_maxs.y = 0.05f;

			gridLineAlongY.m_mins.x = -0.05f;
			gridLineAlongY.m_maxs.x = 0.05f;

			xGridLinesColor = Rgba8::RED;
			yGridLinesColor = Rgba8::GREEN;
		}
		else if(gridLineNum % 5 == 0)
		{

			gridLineAlongX.m_mins.y = gridLineNum - 0.02f;
			gridLineAlongX.m_maxs.y = gridLineNum + 0.02f;

			gridLineAlongY.m_mins.x = gridLineNum - 0.02f;
			gridLineAlongY.m_maxs.x = gridLineNum + 0.02f;

			xGridLinesColor = Rgba8(180, 0, 0, 255);
			yGridLinesColor = Rgba8(0, 180, 0, 255);

		}
		else
		{
			gridLineAlongX.m_mins.y = gridLineNum - 0.01f;
			gridLineAlongX.m_maxs.y = gridLineNum + 0.01f;

			gridLineAlongY.m_mins.x = gridLineNum - 0.01f;
			gridLineAlongY.m_maxs.x = gridLineNum + 0.01f;

			xGridLinesColor = Rgba8::GREY;
			yGridLinesColor = Rgba8::GREY;
		}
		AddVertsForAABB3D(m_gridVerts, gridLineAlongX, xGridLinesColor);
		AddVertsForAABB3D(m_gridVerts, gridLineAlongY, yGridLinesColor);
	}

	DebugAddWorldBasis(-1.f, DebugRenderMode::USE_DEPTH);

	EulerAngles xOrientation = EulerAngles(-90.f, 0.f, 0.f);
	Mat44 xOrientationMatrix = xOrientation.GetAsMatrix_IFwd_JLeft_KUp();
	Vec3 xTranslation = Vec3(0.2f, 0.f, 0.3f);
	xOrientationMatrix.SetTranslation3D(xTranslation);

	EulerAngles yOrientation = EulerAngles(180.f, 0.f, 0.f);
	Mat44 yOrientationMatrix = yOrientation.GetAsMatrix_IFwd_JLeft_KUp();
	Vec3 yTranslation = Vec3(0.f, 0.2f, 0.3f);
	yOrientationMatrix.SetTranslation3D(yTranslation);

	EulerAngles zOrientation = EulerAngles(180.f, 0.f, 90.f);
	Mat44 zOrientationMatrix = zOrientation.GetAsMatrix_IFwd_JLeft_KUp();
	Vec3 zTranslation = Vec3(0.f, -0.5f, 0.3f);
	zOrientationMatrix.SetTranslation3D(zTranslation);

	DebugAddWorldText("X - Forward", xOrientationMatrix, 0.125f, Vec2(0.f, 0.f), -1.f, Rgba8::RED, Rgba8::RED, DebugRenderMode::USE_DEPTH);
	DebugAddWorldText("Y - Left", yOrientationMatrix, 0.125f, Vec2(1.f, 0.f), -1.f, Rgba8::GREEN, Rgba8::GREEN, DebugRenderMode::USE_DEPTH);
	DebugAddWorldText("Z - Up", zOrientationMatrix, 0.125f, Vec2(0.f, 0.f), -1.f, Rgba8::BLUE, Rgba8::BLUE, DebugRenderMode::USE_DEPTH);

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Game::PrintControlsOnDevConsole()
{
	g_devConsole->AddLine(DevConsole::INTRO_TEXT, "Chess 3D");
	g_devConsole->AddLine(DevConsole::INTRO_TEXT, "---------------------------------------------------------------------------");
	g_devConsole->AddLine(DevConsole::INTRO_SUBTEXT, "W / S - Move forward / move back");
	g_devConsole->AddLine(DevConsole::INTRO_SUBTEXT, "A / D - Move left / move right");
	g_devConsole->AddLine(DevConsole::INTRO_SUBTEXT, "Z / C - Move up / move down");
	g_devConsole->AddLine(DevConsole::INTRO_SUBTEXT, "Mouse - Look Around");
	g_devConsole->AddLine(DevConsole::INTRO_SUBTEXT, "Shift + any movement key - Increase movement speed");
	g_devConsole->AddLine(DevConsole::INTRO_SUBTEXT, "H - reset position and orientation");
	g_devConsole->AddLine(DevConsole::INTRO_SUBTEXT, "R - reset roll");
	g_devConsole->AddLine(DevConsole::INTRO_SUBTEXT, "---------------------------------------------------------------------------");
	g_devConsole->AddLine(DevConsole::INTRO_SUBTEXT, "Gamepad Left Stick - Move");
	g_devConsole->AddLine(DevConsole::INTRO_SUBTEXT, "Gamepad Right Stick - Look");
	g_devConsole->AddLine(DevConsole::INTRO_SUBTEXT, "Gamepad Left shoulder / Right Shoulder - Move up / move down");
	g_devConsole->AddLine(DevConsole::INTRO_SUBTEXT, "Gamepad A + Left Stick - Increase movement speed");
	g_devConsole->AddLine(DevConsole::INTRO_SUBTEXT, "Gamepad Start - reset position and orientation");
	g_devConsole->AddLine(DevConsole::INTRO_SUBTEXT, "Gamepad Y - reset roll");
	g_devConsole->AddLine(DevConsole::INTRO_SUBTEXT, "---------------------------------------------------------------------------");
	g_devConsole->AddLine(DevConsole::INTRO_TEXT,	 "Debug Render System");
	g_devConsole->AddLine(DevConsole::INTRO_SUBTEXT, "1 - Spawn Line");
	g_devConsole->AddLine(DevConsole::INTRO_SUBTEXT, "2 - Spawn Point on XY-Plane");
	g_devConsole->AddLine(DevConsole::INTRO_SUBTEXT, "3 - Spawn Wireframe Sphere");
	g_devConsole->AddLine(DevConsole::INTRO_SUBTEXT, "4 - Spawn Basis");
	g_devConsole->AddLine(DevConsole::INTRO_SUBTEXT, "5 - Spawn Billboard Text");
	g_devConsole->AddLine(DevConsole::INTRO_SUBTEXT, "6 - Spawn Wireframe Cylinder");
	g_devConsole->AddLine(DevConsole::INTRO_SUBTEXT, "7 - Print Camera orientation on screen");
	g_devConsole->AddLine(DevConsole::INTRO_SUBTEXT, "---------------------------------------------------------------------------");

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Game::RenderGrid() const
{
	g_theRenderer->SetModelConstants();
	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
	g_theRenderer->SetDepthMode(DepthMode::READ_WRITE_LESS_EQUAL);
	g_theRenderer->SetBlendMode(BlendMode::OPAQUE);
	g_theRenderer->BindShader(nullptr);
	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->DrawVertexArray(m_gridVerts);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Game::KeyboardControls()
{

	if(m_isAttractMode)
	{
		if(g_inputSystem->WasKeyJustPressed(' '))
		{
			m_isAttractMode = false;
		}


		if(g_inputSystem->WasKeyJustPressed(KEYCODE_ESC))
		{
			g_theApp->RequestQuit();
		}
	}
	
	if(!m_isAttractMode)
	{
		if(g_inputSystem->WasKeyJustPressed(KEYCODE_F1))
		{
			m_isDebugMode = !m_isDebugMode;
		}

		if(g_inputSystem->WasKeyJustPressed(KEYCODE_F4))
		{
			m_freeCamera = !m_freeCamera;
		}
	
		if(g_inputSystem->WasKeyJustPressed(KEYCODE_ESC))
		{
			m_isAttractMode = true;
		}

		if(g_inputSystem->WasKeyJustPressed('1'))
		{
			m_debugInt = 1;

			if(g_inputSystem->IsKeyDown(KEYCODE_LSHIFT))
			{
				m_debugInt = 11;
			}
		}
		else if(g_inputSystem->WasKeyJustPressed('2'))
		{
			m_debugInt = 2;

			if(g_inputSystem->IsKeyDown(KEYCODE_LSHIFT))
			{
				m_debugInt = 12;
			}
		}
		else if(g_inputSystem->WasKeyJustPressed('3'))
		{
			m_debugInt = 3;

			if(g_inputSystem->IsKeyDown(KEYCODE_LSHIFT))
			{
				m_debugInt = 13;
			}
		}
		else if(g_inputSystem->WasKeyJustPressed('4'))
		{
			m_debugInt = 4;

			if(g_inputSystem->IsKeyDown(KEYCODE_LSHIFT))
			{
				m_debugInt = 14;
			}
		}
		else if(g_inputSystem->WasKeyJustPressed('5'))
		{
			m_debugInt = 5;

			if(g_inputSystem->IsKeyDown(KEYCODE_LSHIFT))
			{
				m_debugInt = 15;
			}
		}
		else if(g_inputSystem->WasKeyJustPressed('6'))
		{
			m_debugInt = 6;

			if(g_inputSystem->IsKeyDown(KEYCODE_LSHIFT))
			{
				m_debugInt = 16;
			}
		}
		else if(g_inputSystem->WasKeyJustPressed('7'))
		{
			m_debugInt = 7;

			if(g_inputSystem->IsKeyDown(KEYCODE_LSHIFT))
			{
				m_debugInt = 17;
			}
		}
		else if(g_inputSystem->WasKeyJustPressed('8'))
		{
			m_debugInt = 8;

			if(g_inputSystem->IsKeyDown(KEYCODE_LSHIFT))
			{
				m_debugInt = 18;
			}
		}
		else if(g_inputSystem->WasKeyJustPressed('9'))
		{
			m_debugInt = 9;
			
			if(g_inputSystem->IsKeyDown(KEYCODE_LSHIFT))
			{
				m_debugInt = 19;
			}
		}
		else if(g_inputSystem->WasKeyJustPressed('0'))
		{
			m_debugInt = 0;

			if(g_inputSystem->IsKeyDown(KEYCODE_LSHIFT))
			{
				m_debugInt = 10;
			}
		}
	}	
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Game::XboxControls()
{
	XboxController xboxController = g_inputSystem->GetController(0);

	if(m_isAttractMode)
	{
		if(xboxController.WasButtonJustPressed(XBOX_BUTTON_START))
		{
			SoundID toGame = g_theAudioSystem->CreateOrGetSound("Data/Audio/ToGame.mp3");
			g_theAudioSystem->StartSound(toGame);

			m_isAttractMode = false;
		}

		if(xboxController.WasButtonJustPressed(XBOX_BUTTON_B))
		{
			g_theApp->RequestQuit();
		}
	}

	if(!m_isAttractMode)
	{
		if(xboxController.WasButtonJustPressed(XBOX_BUTTON_BACK))
		{
			m_isDebugMode = !m_isDebugMode;
		}

		if(xboxController.WasButtonJustPressed(XBOX_BUTTON_B))
		{
			SoundID toAttract = g_theAudioSystem->CreateOrGetSound("Data/Audio/ToAttract.mp3");
			g_theAudioSystem->StartSound(toAttract);

			m_isAttractMode = true;
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------
void Game::TimeManipulation()
{
	if(g_inputSystem->WasKeyJustPressed('T'))
	{
		if(m_gameClock->GetTimeScale() < 1.)
		{
			m_gameClock->SetTimeScale(1.);
		}
		else
		{
			m_gameClock->SetTimeScale(0.1);
		}
	}

	if(g_inputSystem->WasKeyJustPressed('P'))
	{
		m_gameClock->TogglePause();
	}

	if(g_inputSystem->WasKeyJustPressed('O'))
	{
		
		m_gameClock->StepSingleFrame();

	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Game::BeginChessMatch()
{
	if(m_chessMatch)
	{
		delete m_chessMatch;
		m_chessMatch = nullptr;
	}

	m_chessMatch = new ChessMatch(this);
}
