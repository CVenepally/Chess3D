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
#include "Engine/Network/NetworkSystem.hpp"
//------------------------------------------------------------------------------------------------------------------
Game* g_game = nullptr;
extern NetworkSystem* g_netSystem;
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

	m_gameClock = new Clock(Clock::GetSystemClock());
	m_timerOne = new Timer(0.6);
	m_timerOne->Start();

	ChessPieceDefinition::InitializeChessPieceDefinition();

	m_players[0] = new Player(this, 0);
	m_players[1] = new Player(this, 1);

	m_players[0]->m_position	= Vec3(4.f, -3.f, 5.f);
	m_players[0]->m_orientation = EulerAngles(90.f, 35.f, 0.f);

	m_players[1]->m_position	= Vec3(4.f, 13.f, 5.f);
	m_players[1]->m_orientation = EulerAngles(-90.f, 35.f, 0.f);

	m_players[0]->m_isGoingFirst = true;

	m_screenCamera.m_viewportBounds.m_mins = Vec2::ZERO;
	m_screenCamera.m_viewportBounds.m_maxs = Vec2(static_cast<float>(g_theWindow->GetClientDimensions().x), static_cast<float>(g_theWindow->GetClientDimensions().y));

	m_currentPlayer = m_players[0];
	m_localPlayer = m_players[0];

	m_activeButtons.reserve(5);

	SubscribeEventCallbackFunction("ChessBegin",		OnBeginChessMatch);
	SubscribeEventCallbackFunction("ChessPlayerInfo",	OnChessPlayerInfo);
	SubscribeEventCallbackFunction("ChessSpawnInfo",	OnChessSpawnInfo);
	SubscribeEventCallbackFunction("PlayerJoined",		OnRemotePlayerJoined);
	SubscribeEventCallbackFunction("ChessValidate",		OnChessValidate);
	SubscribeEventCallbackFunction("ChessResign",		OnChessResign);
	SubscribeEventCallbackFunction("ChessOfferDraw",	OnOfferDraw);
	SubscribeEventCallbackFunction("ChessAcceptDraw",	OnAcceptDraw);
	SubscribeEventCallbackFunction("ChessRejectDraw",	OnRejectDraw);
	SubscribeEventCallbackFunction("CharTyped",			OnCharTyped);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Game::Shutdown()
{
	UnsubscribeEventCallbackFunction("ChessBegin",		OnBeginChessMatch);
	UnsubscribeEventCallbackFunction("ChessPlayerInfo", OnChessPlayerInfo);
	UnsubscribeEventCallbackFunction("ChessSpawnInfo",	OnChessSpawnInfo);
	UnsubscribeEventCallbackFunction("PlayerJoined",	OnRemotePlayerJoined);
	UnsubscribeEventCallbackFunction("ChessResign",		OnChessResign);
	UnsubscribeEventCallbackFunction("ChessOfferDraw",	OnOfferDraw);
	UnsubscribeEventCallbackFunction("ChessAcceptDraw", OnAcceptDraw);
	UnsubscribeEventCallbackFunction("ChessRejectDraw", OnRejectDraw);
	UnsubscribeEventCallbackFunction("CharTyped",		OnCharTyped);

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

	if(m_chessMatch)
	{
		delete m_chessMatch;
		m_chessMatch = nullptr;
	}

	FireEvent("Clear");
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Game::Update()
{
	TimeManipulation();

	while(m_timerOne->DecrementPeriodIfElapsed())
	{
		m_insertionPointVisible = !m_insertionPointVisible;
	}

	KeyboardControls();
	ExecuteNetCommands();

	if(m_state == GameState::MATCH)
	{
		if(m_chessMatch)
		{
			m_chessMatch->Update();
		}
		if(m_isMultiplayer)
		{
			m_localPlayer->Update();
		}
		else
		{
			m_currentPlayer->Update();
		}
	}

	UpdateCameras();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Game::Render() const
{
	RenderWorld();
	RenderScreen();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Game::RenderWorld() const
{
	g_theRenderer->BeginRenderEvent("Game - World Render");

	if(m_isMultiplayer)
	{
		g_theRenderer->BeginCamera(m_localPlayer->m_playerCamera);
	}
	else
	{
		g_theRenderer->BeginCamera(m_currentPlayer->m_playerCamera);
	}

	if(m_state == GameState::MATCH)
	{
		g_theRenderer->ClearScreen(Rgba8(0, 0, 0, 255));
		g_theRenderer->SetDebugConstants(static_cast<float>(m_gameClock->GetTotalSeconds()), m_debugInt, m_debugFloat);
		RenderGame();
	}

	if(m_isDebugMode)
	{
		DebugRenderWorld(m_localPlayer->m_playerCamera);
	}

	if(m_isMultiplayer)
	{
		g_theRenderer->EndCamera(m_localPlayer->m_playerCamera);
	}
	else
	{
		g_theRenderer->EndCamera(m_currentPlayer->m_playerCamera);
	}

	g_theRenderer->EndRenderEvent("Game - World Render");
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Game::RenderScreen() const
{
	g_theRenderer->BeginRenderEvent("Screen Render");
	g_theRenderer->BeginCamera(m_screenCamera);

	switch(m_state)
	{
	case GameState::ATTRACT:
	{
		RenderAttractMode();
		break;
	}
	case GameState::MODE_SELECT:
	{
		RenderModeSelect();
		break;
	}
	case GameState::SERVER_HOST:
	{
		RenderHostScreen();
		break;
	}
	case GameState::CLIENT_CONNECT:
	{
		RenderClientScreen();
		break;
	}
	case GameState::LOBBY:
	{
		RenderLobby();
		break;
	}
	case GameState::MATCH:
	{
		m_chessMatch->RenderUIText();
		break;
	}
	default:
		break;
	}

	DebugRenderScreen(m_screenCamera);

	g_theRenderer->EndCamera(m_screenCamera);
	g_theRenderer->EndRenderEvent("Screen Render");
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Game::OnBeginChessMatch(EventArgs& args)
{
	std::string playerOne = args.GetValue("firstPlayer", g_game->m_localPlayer->m_playerName);
	bool remote = args.GetValue("remote", false);

	Player* first = nullptr;
	Player* second = nullptr;

	if(playerOne == g_game->m_localPlayer->m_playerName)
	{
		g_game->m_currentPlayer = g_game->m_localPlayer;
		first = g_game->m_localPlayer;
		first->m_isGoingFirst = true;

		for(int index = 0; index < 2; ++index)
		{
			if(g_game->m_players[index] && g_game->m_players[index]->m_playerName != playerOne)
			{
				second = g_game->m_players[index];
				second->m_isGoingFirst = false;
				break;
			}
		}
	}
	else
	{
		for(int index = 0; index < 2; ++index)
		{
			if(g_game->m_players[index] && g_game->m_players[index]->m_playerName == playerOne)
			{
				g_game->m_currentPlayer = g_game->m_players[index];
				first = g_game->m_players[index];
				first->m_isGoingFirst = true;
			}
			else
			{
				second = g_game->m_players[index];
				if(second)
				{
					second->m_isGoingFirst = false;
				}
			}
		}
	}

	if(!remote)
	{
		std::string command = Stringf("RemoteCmd cmd=ChessBegin firstPlayer=%s", playerOne.c_str());
		g_devConsole->Execute(command);
	}

	g_game->BeginChessMatch(first, second);

	return true;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Game::OnChessPlayerInfo(EventArgs& args)
{
	std::string name = args.GetValue("name", " ");

	if(name == " ")
	{
		g_devConsole->AddLine(DevConsole::ERROR, "Please give a name! Example: ChessPlayerInfo name=Arno");
		return false;
	}

	bool remote = args.GetValue("remote", false);

	if(!remote)
	{
		g_game->m_localPlayer->m_playerName = name;
		std::string command = Stringf("RemoteCmd cmd=ChessPlayerInfo name=%s playerIdx=%d", name.c_str(), g_game->m_localPlayer->m_playerID);
		g_devConsole->Execute(command);
	}
	else
	{
		int playerID = args.GetValue("playerIdx", -1);

		if(playerID == -1)
		{
			g_devConsole->AddLine(DevConsole::INTRO_SUBTEXT, Stringf("An unknown player has set their name as %s", name.c_str()));
			return false;
		}

		for(int index = 0; index < 2; ++index)
		{
			if(g_game->m_players[index] && g_game->m_players[index]->m_playerID == playerID)
			{
				g_game->m_players[index]->m_playerName = name;
				return false;
			}
		}
	}

	return false;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Game::OnChessSpawnInfo(EventArgs& args)
{
	int playerIdx = args.GetValue("playerIdx", 1);

	g_game->m_localPlayer = g_game->m_players[playerIdx];

	g_game->m_isMultiplayer = true;

// 	g_game->m_chessMatch->m_matchState = MatchState::IDLE;

	return false;
}

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Game::OnRemotePlayerJoined(EventArgs& args)
{
	// check num net clients
	UNUSED(args);

	int netClients = g_netSystem->GetNumConnectedClients();

	if(netClients > 1)
	{
		return false;
	}

	g_devConsole->Execute("RemoteCmd cmd=ChessSpawnInfo playerIdx=1");

//	g_game->m_chessMatch->m_matchState = MatchState::IDLE;

	g_game->m_isMultiplayer = true;

	for(int i = 0; i < 2; ++i)
	{
		g_game->m_players[i]->m_isGoingFirst = false;
	}

	return false;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Game::OnChessValidate(EventArgs& args)
{
	std::string playerOneName	= args.GetValue("player1", " ");
	std::string playerTwoName	= args.GetValue("player2", " ");
	std::string gameState		= args.GetValue("state", " ");
	std::string board			= args.GetValue("board", " ");
	int			turns			= args.GetValue("move", -1);
	bool		remote			= args.GetValue("remote", false);

	std::string localPlayerOneName = g_game->m_chessMatch->m_playerOneName.c_str();
	std::string localPlayerTwoName = g_game->m_chessMatch->m_playerTwoName.c_str();
	std::string localBoard = g_game->m_chessMatch->GetBoardAsString();
	int			localTurns = g_game->m_chessMatch->m_currentTurn;

	std::string localGameState;

	switch(g_game->m_chessMatch->m_matchState)
	{
	case MatchState::PLAYER_ONE_TURN:
	{
		localGameState = "Player1Turn";
		break;
	}
	case MatchState::PLAYER_TWO_TURN:
	{
		localGameState = "Player2Turn";
		break;
	}
	case MatchState::PLAYER_TWO_WINS:
	{
		localGameState = "GameOver";
		break;
	}
	case MatchState::PLAYER_ONE_WINS:
	{
		localGameState = "GameOver";
		break;
	}
	case MatchState::DRAW:
	{
		localGameState = "GameOver";
		break;
	}

	default:
		break;
	}

	if(!remote)
	{
		g_devConsole->AddLine(DevConsole::INFO_MAJOR, "Validating Chess Match Between Client and Server...");
		g_devConsole->AddLine(DevConsole::INFO_MINOR, Stringf("Player 1 Name: %s", localPlayerOneName.c_str()));
		g_devConsole->AddLine(DevConsole::INFO_MINOR, Stringf("Player 2 Name: %s", localPlayerTwoName.c_str()));
		g_devConsole->AddLine(DevConsole::INFO_MINOR, Stringf("Game State: %s", localGameState.c_str()));
		g_devConsole->AddLine(DevConsole::INFO_MINOR, Stringf("Board: %s", localBoard.c_str()));
		g_devConsole->AddLine(DevConsole::INFO_MINOR, Stringf("Turns: %d", localTurns));

		std::string command = Stringf("RemoteCmd cmd=ChessValidate state=%s player1=%s player2=%s move=%d board=%s", localGameState.c_str(), localPlayerOneName.c_str(), localPlayerTwoName.c_str(), localTurns, localBoard.c_str());

		g_devConsole->Execute(command);
	}
	else
	{
		g_devConsole->AddLine(DevConsole::INFO_MAJOR, "Opponent initiated a game state validation...");

		if(playerOneName != localPlayerOneName || playerTwoName != localPlayerTwoName || gameState != localGameState || board != localBoard || turns != localTurns)
		{
			g_devConsole->AddLine(DevConsole::ERROR, "Validation Failed");

			g_devConsole->AddLine(DevConsole::INFO_MAJOR, "Local Game State");
			g_devConsole->AddLine(DevConsole::INFO_MINOR, Stringf("Player 1 Name: %s",	localPlayerOneName.c_str()));
			g_devConsole->AddLine(DevConsole::INFO_MINOR, Stringf("Player 2 Name: %s",	localPlayerTwoName.c_str()));
			g_devConsole->AddLine(DevConsole::INFO_MINOR, Stringf("Game State: %s",		localGameState.c_str()));
			g_devConsole->AddLine(DevConsole::INFO_MINOR, Stringf("Board: %s",			localBoard.c_str()));
			g_devConsole->AddLine(DevConsole::INFO_MINOR, Stringf("Turns: %d",			localTurns));

			g_devConsole->AddLine(DevConsole::INFO_MAJOR, "Opponent Game State");
			g_devConsole->AddLine(DevConsole::INFO_MINOR, Stringf("Player 1 Name: %s",	playerOneName.c_str()));
			g_devConsole->AddLine(DevConsole::INFO_MINOR, Stringf("Player 2 Name: %s",	playerTwoName.c_str()));
			g_devConsole->AddLine(DevConsole::INFO_MINOR, Stringf("Game State: %s",		gameState.c_str()));
			g_devConsole->AddLine(DevConsole::INFO_MINOR, Stringf("Board: %s",			board.c_str()));
			g_devConsole->AddLine(DevConsole::INFO_MINOR, Stringf("Turns: %d",			turns));

			//#ToDo: Echo back to the other player

			g_devConsole->Execute("RemoteCmd cmd=ChessDisconnect reason=VALIDATION_ERROR");
			g_netSystem->Disconnect("VALIDATION_ERROR");
		}
		else
		{
			g_devConsole->AddLine(DevConsole::SUCCESS, "Validation Successful");
		}
	}

	return false;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Game::OnChessResign(EventArgs& args)
{
	bool isRemote = args.GetValue("remote", false);

	std::string firstPlayer;
	std::string secondPlayer;

	for(int index = 0; index < 2; ++index)
	{
		if(g_game->m_players[index]->m_isGoingFirst)
		{
			firstPlayer = g_game->m_players[index]->m_playerName;
		}
		else
		{
			secondPlayer = g_game->m_players[index]->m_playerName;
		}
	}

	bool localIsFirst = g_game->m_localPlayer->m_isGoingFirst;
	bool localIsResigning = !isRemote;

	if(localIsFirst == localIsResigning)
	{
		g_game->m_chessMatch->m_matchState = MatchState::PLAYER_TWO_WINS;
		g_devConsole->AddLine(DevConsole::INTRO_SUBTEXT, Stringf("%s (Player 1) resigned from the game. %s (Player 2) wins by default.", firstPlayer.c_str(), secondPlayer.c_str()));
	}
	else
	{
		g_game->m_chessMatch->m_matchState = MatchState::PLAYER_ONE_WINS;
		g_devConsole->AddLine(DevConsole::INTRO_SUBTEXT, Stringf("%s (Player 2) resigned from the game. %s (Player 1) wins by default.", secondPlayer.c_str(), firstPlayer.c_str()));
	}

	if(localIsResigning)
	{
		g_devConsole->Execute("RemoteCmd cmd=ChessResign");
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Game::OnOfferDraw(EventArgs& args)
{
	bool isRemote = args.GetValue("remote", false);

	if(!isRemote)
	{
		g_devConsole->AddLine(DevConsole::TEXT_HIGHLIGHT, "You Offered to draw. Awaiting opponent response...");
		g_devConsole->Execute("RemoteCmd cmd=ChessOfferDraw");
	}
	else
	{
		g_game->m_wasDrawOffered = true;
		g_devConsole->AddLine(DevConsole::TEXT_HIGHLIGHT, "Your opponent has offered a draw. You may wish to Accept or Reject... (Chess<Accept/Reject>Draw)");
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Game::OnAcceptDraw(EventArgs& args)
{
	bool isRemote = args.GetValue("remote", false);

	if(!isRemote)
	{
		if(!g_game->m_wasDrawOffered)
		{
			g_devConsole->AddLine(DevConsole::ERROR, "The Opponent has not offered a draw yet. You cannot accept or reject it!!");
			return false;
		}

		g_devConsole->AddLine(DevConsole::TEXT_HIGHLIGHT, "Accepted Opponent's Draw Offer!");
		g_game->m_wasDrawOffered = false;
		g_game->m_chessMatch->m_matchState = MatchState::DRAW;
		g_devConsole->Execute("RemoteCmd cmd=ChessAcceptDraw");
	}
	else
	{
		g_devConsole->AddLine(DevConsole::TEXT_HIGHLIGHT, "The Opponent has accepted your draw offer!");
		g_game->m_chessMatch->m_matchState = MatchState::DRAW;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Game::OnRejectDraw(EventArgs& args)
{
	bool isRemote = args.GetValue("remote", false);

	if(!isRemote)
	{
		if(!g_game->m_wasDrawOffered)
		{
			g_devConsole->AddLine(DevConsole::ERROR, "The Opponent has not offered a draw yet. You cannot accept or reject it!!");
			return false;
		}

		g_devConsole->AddLine(DevConsole::ERROR, "REJECTED Opponent's Draw Offer!");
		g_game->m_wasDrawOffered = false;
		g_devConsole->Execute("RemoteCmd cmd=ChessRejectDraw");
	}
	else
	{
		g_devConsole->AddLine(DevConsole::ERROR, "The Opponent has REJECTED your draw offer!");
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Game::InputKeys()
{
	if(g_devConsole->IsOpen())
	{
		return;
	}

	m_insertionPointVisible = true;
	m_timerOne->Stop();
	m_timerOne->Start();

	// insertion point positions
	if(g_inputSystem->WasKeyJustPressed(KEYCODE_LEFT_ARROW))
	{
		if(m_insertionPointPosition > 0)
		{
			m_insertionPointPosition -= 1;
		}
	}

	if(g_inputSystem->WasKeyJustPressed(KEYCODE_RIGHT_ARROW))
	{
		if(m_state == GameState::SERVER_HOST || m_state == GameState::LOBBY)
		{
			if(m_insertionPointPosition < static_cast<int>(m_inputString.length()))
			{
				m_insertionPointPosition += 1;
			}
		}
		
		if(m_state == GameState::CLIENT_CONNECT)
		{
			if(m_isEnteringPort)
			{
				if(m_insertionPointPosition < static_cast<int>(m_port.length()))
				{
					m_insertionPointPosition += 1;
				}
			}
			else
			{
				if(m_insertionPointPosition < static_cast<int>(m_ip.length()))
				{
					m_insertionPointPosition += 1;
				}
			}
		}
	}

	if(g_inputSystem->WasKeyJustPressed(KEYCODE_HOME))
	{
		m_insertionPointPosition = 0;
	}

	if(g_inputSystem->WasKeyJustPressed(KEYCODE_END))
	{
		if(m_state == GameState::SERVER_HOST || m_state == GameState::LOBBY)
		{
			m_insertionPointPosition = static_cast<int>(m_inputString.length());
		}

		if(m_state == GameState::CLIENT_CONNECT)
		{
			if(m_isEnteringPort)
			{
				m_insertionPointPosition = static_cast<int>(m_port.length());
			}
			else
			{
				m_insertionPointPosition = static_cast<int>(m_ip.length());
			}
		}

	}

 	// clearing
 	if(g_inputSystem->WasKeyJustPressed(KEYCODE_BACKSPACE))
 	{
		if(m_state == GameState::SERVER_HOST || m_state == GameState::LOBBY)
		{
			if(m_insertionPointPosition > 0 && static_cast<int>(m_inputString.length()) > 0)
			{
				m_inputString.erase(m_inputString.begin() + m_insertionPointPosition - 1);
				m_insertionPointPosition -= 1;
			}
		}

		if(m_state == GameState::CLIENT_CONNECT)
		{
			if(m_isEnteringPort)
			{
				if(m_insertionPointPosition > 0 && static_cast<int>(m_port.length()) > 0)
				{
					m_port.erase(m_port.begin() + m_insertionPointPosition - 1);
					m_insertionPointPosition -= 1;
				}
			}
			else
			{
				if(m_insertionPointPosition > 0 && static_cast<int>(m_ip.length()) > 0)
				{
					m_ip.erase(m_ip.begin() + m_insertionPointPosition - 1);
					m_insertionPointPosition -= 1;
				}
			}
		}
 	}
 
 	if(g_inputSystem->WasKeyJustPressed(KEYCODE_DELETE))
 	{
		if(m_state == GameState::SERVER_HOST || m_state == GameState::LOBBY)
		{
			if(m_insertionPointPosition < static_cast<int>(m_inputString.length()) && static_cast<int>(m_inputString.length()) > 0)
			{
				m_inputString.erase(m_inputString.begin() + m_insertionPointPosition);
			}
		}

		if(m_state == GameState::CLIENT_CONNECT)
		{
			if(m_isEnteringPort)
			{
				if(m_insertionPointPosition < static_cast<int>(m_port.length()) && static_cast<int>(m_port.length()) > 0)
				{
					m_port.erase(m_port.begin() + m_insertionPointPosition);
				}
			}
			else
			{
				if(m_insertionPointPosition < static_cast<int>(m_ip.length()) && static_cast<int>(m_ip.length()) > 0)
				{
					m_ip.erase(m_ip.begin() + m_insertionPointPosition);
				}
			}
		}
 	}
 
 	// execution
 	if(g_inputSystem->WasKeyJustPressed(KEYCODE_ENTER))
 	{
		if(m_state == GameState::SERVER_HOST || m_state == GameState::LOBBY)
		{
			EventArgs args;
			args.SetValue("name", m_inputString);
			g_eventSystem->FireEvent("ChessPlayerInfo", args);
		}

		if(m_state == GameState::CLIENT_CONNECT)
		{
			EventArgs args;
			args.SetValue("ip", m_ip);
			args.SetValue("port", m_port);
			g_eventSystem->FireEvent("ChessConnect", args);
		}
 	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Game::OnCharTyped(EventArgs& args)
{
	if(!g_devConsole->IsOpen())
	{
		unsigned char charToAppend = static_cast<uchar>(args.GetValue("CharTyped", 0));
		return g_game->SetInputString(charToAppend);
	}

	return false;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Vec3 const& Game::GetCameraPosition() const
{
	return m_currentPlayer->m_playerCamera.m_position;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Vec3 Game::GetCameraForward() const
{
	return m_currentPlayer->GetForwardNormal();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
int Game::GetCurrentPlayerID() const
{
	return m_currentPlayer->m_playerID;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Game::GetIsPlayerGoingFirst(int playerID) const
{
	for(int index = 0; index < 2; ++index)
	{
		if(m_players[index] && m_players[index]->m_playerID == playerID)
		{
			return m_players[index]->m_isGoingFirst;
		}
	}

	return false;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Game::SetInputString(unsigned int charToAppend)
{
	if(m_state != GameState::SERVER_HOST && m_state != GameState::LOBBY && m_state != GameState::CLIENT_CONNECT)
	{
		return false;
	}

	if(charToAppend >= 32 && charToAppend <= 126 && (charToAppend != '`' && charToAppend != '~'))
	{
		std::string appendString(1, static_cast<unsigned char>(charToAppend));

		if(m_state == GameState::SERVER_HOST || m_state == GameState::LOBBY)
		{
			m_inputString.insert(m_insertionPointPosition, appendString);
		}
		if(m_state == GameState::CLIENT_CONNECT)
		{
			if(m_isEnteringPort)
			{
				m_port.insert(m_insertionPointPosition, appendString);
			}
			else
			{
				m_ip.insert(m_insertionPointPosition, appendString);
			}
		}
		m_insertionPointPosition += 1;
		return true;
	}

	return false;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Game::SwitchCurrentPlayer()
{
	if(m_currentPlayer == m_players[0])
	{
		m_currentPlayer = m_players[1];
	}
	else if(m_currentPlayer == m_players[1])
	{
		m_currentPlayer = m_players[0];
	}

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Game::UpdateCameras()
{
	m_screenCamera.m_mode = Camera::eMode_Orthographic;

	m_screenCamera.SetOrthographicView(Vec2(0.f, 0.f), Vec2(g_theWindow->GetClientDimensions()));


	if(m_isMultiplayer)
	{
		m_localPlayer->m_playerCamera.m_mode = Camera::eMode_Perspective;
	}
	else
	{
		m_currentPlayer->m_playerCamera.m_mode = Camera::eMode_Perspective;
	}

	if(m_isMultiplayer)
	{
		m_localPlayer->m_playerCamera.SetPositionAndOrientation(m_localPlayer->m_position, m_localPlayer->m_orientation);
	}
	else
	{
		m_currentPlayer->m_playerCamera.SetPositionAndOrientation(m_currentPlayer->m_position, m_currentPlayer->m_orientation);
	}

	Mat44 cameraToRenderMatrix;
	
	cameraToRenderMatrix.SetIJKT3D(Vec3(0.f, 0.f, 1.f), Vec3(-1.f, 0.f, 0.f), Vec3(0.f, 1.f, 0.f), Vec3(0.f, 0.f, 0.f));

	if(m_isMultiplayer)
	{
		m_localPlayer->m_playerCamera.SetCameraToRenderTransform(cameraToRenderMatrix);
		m_localPlayer->m_playerCamera.SetPerspectiveView(g_theWindow->GetConfig().m_aspectRatio, 60.f, 0.1f, 100.f);
	}
	else
	{
		m_currentPlayer->m_playerCamera.SetCameraToRenderTransform(cameraToRenderMatrix);
		m_currentPlayer->m_playerCamera.SetPerspectiveView(g_theWindow->GetConfig().m_aspectRatio, 60.f, 0.1f, 100.f);
	}


}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Game::RenderAttractMode() const
{
	std::vector<Vertex_PCU> attractModeVerts;

	Texture* attractTexture = g_theRenderer->CreateOrGetTextureFromFileNameAndType("Attract", TextureType::NONE);

	AABB2 screenBounds;
	screenBounds.m_mins = Vec2::ZERO;
	screenBounds.m_maxs = Vec2(g_theWindow->GetClientDimensions());

	AddVertsForAABB2D(attractModeVerts, screenBounds, Rgba8::WHITE);

	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
	g_theRenderer->SetBlendMode(BlendMode::ALPHA);
	g_theRenderer->BindShader(nullptr);
	g_theRenderer->BindTexture(attractTexture);
	g_theRenderer->DrawVertexArray(attractModeVerts);

	std::vector<Vertex_PCU> overlayVerts;

	AddVertsForAABB2D(overlayVerts, screenBounds, Rgba8(0, 0, 0, 80));

	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
	g_theRenderer->SetBlendMode(BlendMode::ALPHA);
	g_theRenderer->BindShader(nullptr);
	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->DrawVertexArray(overlayVerts);

	std::vector<Vertex_PCU> textVerts;

	BitmapFont* font = g_theRenderer->CreateOrGetBitmapFont("Data/Fonts/Alagard");

	font->AddVertsForTextInBox2D(textVerts, "CHESS", screenBounds, 99.f, Rgba8::WHITE, 0.8f, Vec2(0.75f, 0.7f), SHRINK_TO_FIT);
	font->AddVertsForTextInBox2D(textVerts, "34", screenBounds, 99.f, Rgba8::WHITE, 0.8f, Vec2(0.71f, 0.55f), SHRINK_TO_FIT);
	font->AddVertsForTextInBox2D(textVerts, "Press 'Space' to Start", screenBounds, 28.f, Rgba8::WHITE, 0.8f, Vec2(0.77f, 0.1f), SHRINK_TO_FIT);


	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
	g_theRenderer->SetBlendMode(BlendMode::ALPHA);
	g_theRenderer->BindShader(nullptr);
	g_theRenderer->BindTexture(&font->GetTexture());
	g_theRenderer->DrawVertexArray(textVerts);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Game::RenderModeSelect() const
{
	std::vector<Vertex_PCU> attractModeVerts;

	Texture* attractTexture = g_theRenderer->CreateOrGetTextureFromFileNameAndType("Attract", TextureType::NONE);
	BitmapFont* font = g_theRenderer->CreateOrGetBitmapFont("Data/Fonts/Alagard");

	AABB2 screenBounds;
	screenBounds.m_mins = Vec2::ZERO;
	screenBounds.m_maxs = Vec2(g_theWindow->GetClientDimensions());

	AddVertsForAABB2D(attractModeVerts, screenBounds, Rgba8::WHITE);

	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
	g_theRenderer->SetBlendMode(BlendMode::ALPHA);
	g_theRenderer->BindShader(nullptr);
	g_theRenderer->BindTexture(attractTexture);
	g_theRenderer->DrawVertexArray(attractModeVerts);

	std::vector<Vertex_PCU> overlayVerts;

	AddVertsForAABB2D(overlayVerts, screenBounds, Rgba8(0, 0, 0, 80));

	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
	g_theRenderer->SetBlendMode(BlendMode::ALPHA);
	g_theRenderer->BindShader(nullptr);
	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->DrawVertexArray(overlayVerts);

	std::vector<Vertex_PCU> buttonVerts;
	std::vector<Vertex_PCU> buttonTextVerts;
	std::vector<std::string> names = GetButtonNamesForGameState();
	float fontSize = 32.f;

	for(size_t buttonIndex = 0; buttonIndex < m_activeButtons.size(); ++buttonIndex)
	{
		AddVertsForAABB2D(buttonVerts, m_activeButtons[buttonIndex], Rgba8::GREY);
		font->AddVertsForTextInBox2D(buttonTextVerts, names[buttonIndex], m_activeButtons[buttonIndex], fontSize, Rgba8::WHITE, 0.8f);
	}

	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
	g_theRenderer->SetBlendMode(BlendMode::ALPHA);
	g_theRenderer->BindShader(nullptr);
	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->DrawVertexArray(buttonVerts);

	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
	g_theRenderer->SetBlendMode(BlendMode::ALPHA);
	g_theRenderer->BindShader(nullptr);
	g_theRenderer->BindTexture(&font->GetTexture());
	g_theRenderer->DrawVertexArray(buttonTextVerts);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Game::RenderHostScreen() const
{
	std::vector<Vertex_PCU> attractModeVerts;

	Texture* attractTexture = g_theRenderer->CreateOrGetTextureFromFileNameAndType("ClientServer", TextureType::NONE);

	AABB2 screenBounds;
	screenBounds.m_mins = Vec2::ZERO;
	screenBounds.m_maxs = Vec2(g_theWindow->GetClientDimensions());

	AddVertsForAABB2D(attractModeVerts, screenBounds, Rgba8::WHITE);

	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
	g_theRenderer->SetBlendMode(BlendMode::ALPHA);
	g_theRenderer->BindShader(nullptr);
	g_theRenderer->BindTexture(attractTexture);
	g_theRenderer->DrawVertexArray(attractModeVerts);

	std::vector<Vertex_PCU> overlayVerts;

	AddVertsForAABB2D(overlayVerts, screenBounds, Rgba8(0, 0, 0, 150));

	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
	g_theRenderer->SetBlendMode(BlendMode::ALPHA);
	g_theRenderer->BindShader(nullptr);
	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->DrawVertexArray(overlayVerts);

	std::vector<Vertex_PCU> buttonVerts;
	std::vector<Vertex_PCU> buttonTextVerts;
	std::vector<std::string> names = GetButtonNamesForGameState();

	float fontSize = 28.f;
	for(size_t buttonIndex = 0; buttonIndex < m_activeButtons.size(); ++buttonIndex)
	{
		if(buttonIndex == 1 && g_netSystem->GetNumConnectedClients() == 0)
		{
			continue;
		}

		AddVertsForAABB2D(buttonVerts, m_activeButtons[buttonIndex], Rgba8(100, 100, 100, 100));
 		g_gameFont->AddVertsForTextInBox2D(buttonTextVerts, names[buttonIndex], m_activeButtons[buttonIndex], fontSize, Rgba8::WHITE);
	}

	AABB2 textBox = screenBounds.GetBoxFromUVs(0.45f, 0.4f, 0.7f, 0.45f);
	AddVertsForAABB2D(buttonVerts, textBox, Rgba8::WHITE);
	g_gameFont->AddVertsForTextInBox2D(buttonTextVerts, m_inputString, textBox, fontSize, Rgba8::BLACK, 1.f, Vec2(0.f, 0.3f));
	g_gameFont->AddVertsForTextInBox2D(buttonTextVerts, "Host Player Name: ", screenBounds, fontSize, Rgba8::WHITE, 1.f, Vec2(0.25f, 0.42f));

	float charWidth = fontSize;

	Vec2 insertionPointMins = Vec2(textBox.m_mins.x + 2.f + (m_insertionPointPosition * charWidth), textBox.m_mins.y + 5.f);
	Vec2 insertionPointMaxs = Vec2(insertionPointMins.x + 1.f, insertionPointMins.y + fontSize);

	if(m_insertionPointVisible)
	{
		AddVertsForAABB2D(buttonVerts, AABB2(insertionPointMins, insertionPointMaxs), Rgba8::BLACK);
	}

	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
	g_theRenderer->SetBlendMode(BlendMode::ALPHA);
	g_theRenderer->BindShader(nullptr);
	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->DrawVertexArray(buttonVerts);

	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
	g_theRenderer->SetBlendMode(BlendMode::ALPHA);
	g_theRenderer->BindShader(nullptr);
	g_theRenderer->BindTexture(&g_gameFont->GetTexture());
	g_theRenderer->DrawVertexArray(buttonTextVerts);


	std::vector<Vertex_PCU> hostTextVerts;

	std::string serverIP = Stringf("Host Server IP: %s", m_ip.c_str());
	std::string port	 = Stringf("Port: %s", m_port.c_str());
	std::string state;

	g_gameFont->AddVertsForTextInBox2D(hostTextVerts, serverIP, screenBounds, 28.f, Rgba8::WHITE, 1.f, Vec2(0.5f, 0.7f));
	g_gameFont->AddVertsForTextInBox2D(hostTextVerts, port, screenBounds, 28.f, Rgba8::WHITE, 1.f, Vec2(0.5f, 0.65f));

	int numConnectedClients = g_netSystem->GetNumConnectedClients();
	Rgba8 color = Rgba8::WHITE;
	if(numConnectedClients == 0)
	{
		state = "Waiting for players to join...";
	}
	else
	{
		state = Stringf("%d player(s) joined the lobby", numConnectedClients);
		color = Rgba8(0, 255, 100);
	}
	g_gameFont->AddVertsForTextInBox2D(hostTextVerts, state, screenBounds, 18.f, color, 1.f, Vec2(0.5f, 0.25f));

	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
	g_theRenderer->SetBlendMode(BlendMode::ALPHA);
	g_theRenderer->BindShader(nullptr);
	g_theRenderer->BindTexture(&g_gameFont->GetTexture());
	g_theRenderer->DrawVertexArray(hostTextVerts);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Game::RenderClientScreen() const
{
	std::vector<Vertex_PCU> attractModeVerts;

	Texture* attractTexture = g_theRenderer->CreateOrGetTextureFromFileNameAndType("ClientServer", TextureType::NONE);

	AABB2 screenBounds;
	screenBounds.m_mins = Vec2::ZERO;
	screenBounds.m_maxs = Vec2(g_theWindow->GetClientDimensions());

	AddVertsForAABB2D(attractModeVerts, screenBounds, Rgba8::WHITE);

	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
	g_theRenderer->SetBlendMode(BlendMode::ALPHA);
	g_theRenderer->BindShader(nullptr);
	g_theRenderer->BindTexture(attractTexture);
	g_theRenderer->DrawVertexArray(attractModeVerts);

	std::vector<Vertex_PCU> overlayVerts;

	AddVertsForAABB2D(overlayVerts, screenBounds, Rgba8(0, 0, 0, 150));

	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
	g_theRenderer->SetBlendMode(BlendMode::ALPHA);
	g_theRenderer->BindShader(nullptr);
	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->DrawVertexArray(overlayVerts);

	std::vector<Vertex_PCU> buttonVerts;
	std::vector<Vertex_PCU> buttonTextVerts;
	std::vector<std::string> names = GetButtonNamesForGameState();

	float fontSize = 20.f;
	for(size_t buttonIndex = 0; buttonIndex < m_activeButtons.size(); ++buttonIndex)
	{
		if(buttonIndex == 1 && (g_netSystem->GetCurrentNetworkState() != NetworkState::CLIENT_CONNECTED))
		{
			continue;
		}

		if(buttonIndex == 2 && (g_netSystem->GetCurrentNetworkState() == NetworkState::CLIENT_CONNECTING || g_netSystem->GetCurrentNetworkState() == NetworkState::CLIENT_CONNECTED))
		{
			continue;
		}

		AddVertsForAABB2D(buttonVerts, m_activeButtons[buttonIndex], Rgba8(100, 100, 100, 100));
		g_gameFont->AddVertsForTextInBox2D(buttonTextVerts, names[buttonIndex], m_activeButtons[buttonIndex], fontSize, Rgba8::WHITE);
	}

	AABB2 ipTextBox = screenBounds.GetBoxFromUVs(0.45f, 0.6f, 0.7f, 0.65f);
	AddVertsForAABB2D(buttonVerts, ipTextBox, Rgba8::WHITE);

	g_gameFont->AddVertsForTextInBox2D(buttonTextVerts, m_ip, ipTextBox, fontSize, Rgba8::BLACK, 1.f, Vec2(0.f, 0.35f));
	g_gameFont->AddVertsForTextInBox2D(buttonTextVerts, "Server IP: ", screenBounds, fontSize, Rgba8::WHITE, 1.f, Vec2(0.38f, 0.63f));

	AABB2 portTextBox = screenBounds.GetBoxFromUVs(0.45f, 0.52f, 0.55f, 0.57f);
	AddVertsForAABB2D(buttonVerts, portTextBox, Rgba8::WHITE);

	g_gameFont->AddVertsForTextInBox2D(buttonTextVerts, m_port, portTextBox, fontSize, Rgba8::BLACK, 1.f, Vec2(0.f, 0.35f));
	g_gameFont->AddVertsForTextInBox2D(buttonTextVerts, "Port: ", screenBounds, fontSize, Rgba8::WHITE, 1.f, Vec2(0.42f, 0.55f));

	float charWidth = fontSize;

	Vec2 insertionPointMins;

	if(m_isEnteringPort)
	{
		insertionPointMins = Vec2(portTextBox.m_mins.x + 2.f + (m_insertionPointPosition * charWidth), portTextBox.m_mins.y + 8.f);
	}
	else
	{
		insertionPointMins = Vec2(ipTextBox.m_mins.x + 2.f + (m_insertionPointPosition * charWidth), ipTextBox.m_mins.y + 8.f);
	}
	Vec2 insertionPointMaxs = Vec2(insertionPointMins.x + 1.f, insertionPointMins.y + fontSize + 4.f);
	
	if(m_insertionPointVisible)
	{
		AddVertsForAABB2D(buttonVerts, AABB2(insertionPointMins, insertionPointMaxs), Rgba8::BLACK);
	}

	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
	g_theRenderer->SetBlendMode(BlendMode::ALPHA);
	g_theRenderer->BindShader(nullptr);
	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->DrawVertexArray(buttonVerts);

	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
	g_theRenderer->SetBlendMode(BlendMode::ALPHA);
	g_theRenderer->BindShader(nullptr);
	g_theRenderer->BindTexture(&g_gameFont->GetTexture());
	g_theRenderer->DrawVertexArray(buttonTextVerts);

	std::vector<Vertex_PCU> clientTextVerts;

	std::string screenTitle = "Join A Game";
	std::string state;

	g_gameFont->AddVertsForTextInBox2D(clientTextVerts, screenTitle, screenBounds, 32.f, Rgba8::WHITE, 1.f, Vec2(0.5f, 0.8f));

	Rgba8 color = Rgba8::WHITE;

	if(g_netSystem->GetCurrentNetworkState() == NetworkState::CLIENT_CONNECTING)
	{
		state = "Waiting for host to accept...";
	}
	else if(g_netSystem->GetCurrentNetworkState() == NetworkState::CLIENT_CONNECTED)
	{
		state = "Host accepted join request. Press next to continue!";
		color = color = Rgba8(0, 255, 100);
	}

	g_gameFont->AddVertsForTextInBox2D(clientTextVerts, state, screenBounds, 18.f, color, 1.f, Vec2(0.5f, 0.25f));

	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
	g_theRenderer->SetBlendMode(BlendMode::ALPHA);
	g_theRenderer->BindShader(nullptr);
	g_theRenderer->BindTexture(&g_gameFont->GetTexture());
	g_theRenderer->DrawVertexArray(clientTextVerts);

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Game::RenderLobby() const
{
	std::vector<Vertex_PCU> lobbyVerts;

	Texture* attractTexture = g_theRenderer->CreateOrGetTextureFromFileNameAndType("ClientServer", TextureType::NONE);

	AABB2 screenBounds;
	screenBounds.m_mins = Vec2::ZERO;
	screenBounds.m_maxs = Vec2(g_theWindow->GetClientDimensions());

	AddVertsForAABB2D(lobbyVerts, screenBounds, Rgba8::WHITE);

	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
	g_theRenderer->SetBlendMode(BlendMode::ALPHA);
	g_theRenderer->BindShader(nullptr);
	g_theRenderer->BindTexture(attractTexture);
	g_theRenderer->DrawVertexArray(lobbyVerts);

	std::vector<Vertex_PCU> overlayVerts;

	AddVertsForAABB2D(overlayVerts, screenBounds, Rgba8(0, 0, 0, 150));

	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
	g_theRenderer->SetBlendMode(BlendMode::ALPHA);
	g_theRenderer->BindShader(nullptr);
	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->DrawVertexArray(overlayVerts);

	std::vector<Vertex_PCU> buttonVerts;
	std::vector<Vertex_PCU> buttonTextVerts;
	std::vector<std::string> names = GetButtonNamesForGameState();

	float fontSize = 20.f;
	for(size_t buttonIndex = 0; buttonIndex < m_activeButtons.size(); ++buttonIndex)
	{
		AddVertsForAABB2D(buttonVerts, m_activeButtons[buttonIndex], Rgba8(100, 100, 100, 100));
		g_gameFont->AddVertsForTextInBox2D(buttonTextVerts, names[buttonIndex], m_activeButtons[buttonIndex], fontSize, Rgba8::WHITE);
	}

	AABB2 nameTextBox = screenBounds.GetBoxFromUVs(0.15f, 0.58f, 0.3f, 0.615f);
	AddVertsForAABB2D(buttonVerts, nameTextBox, Rgba8::WHITE);

	g_gameFont->AddVertsForTextInBox2D(buttonTextVerts, m_inputString, nameTextBox, fontSize, Rgba8::BLACK, 1.f, Vec2(0.f, 0.35f));
	g_gameFont->AddVertsForTextInBox2D(buttonTextVerts, "Player Name: ", screenBounds, fontSize, Rgba8::WHITE, 1.f, Vec2(0.01f, 0.6f));

	float charWidth = fontSize;

	Vec2 insertionPointMins;
	insertionPointMins = Vec2(nameTextBox.m_mins.x + 2.f + (m_insertionPointPosition * charWidth), nameTextBox.m_mins.y + 4.f);
	Vec2 insertionPointMaxs = Vec2(insertionPointMins.x + 1.f, insertionPointMins.y + fontSize + 0.f);

	if(m_insertionPointVisible)
	{
		AddVertsForAABB2D(buttonVerts, AABB2(insertionPointMins, insertionPointMaxs), Rgba8::BLACK);
	}

	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
	g_theRenderer->SetBlendMode(BlendMode::ALPHA);
	g_theRenderer->BindShader(nullptr);
	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->DrawVertexArray(buttonVerts);

	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
	g_theRenderer->SetBlendMode(BlendMode::ALPHA);
	g_theRenderer->BindShader(nullptr);
	g_theRenderer->BindTexture(&g_gameFont->GetTexture());
	g_theRenderer->DrawVertexArray(buttonTextVerts);

	std::vector<Vertex_PCU> lobbyTextVerts;

	std::string screenTitle = "Lobby";
	std::string state = "Who's going first?";

	g_gameFont->AddVertsForTextInBox2D(lobbyTextVerts, screenTitle, screenBounds, 32.f, Rgba8::WHITE, 1.f, Vec2(0.5f, 0.8f));
	g_gameFont->AddVertsForTextInBox2D(lobbyTextVerts, state, screenBounds, 18.f, Rgba8::WHITE, 1.f, Vec2(0.93f, 0.7f));

	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
	g_theRenderer->SetBlendMode(BlendMode::ALPHA);
	g_theRenderer->BindShader(nullptr);
	g_theRenderer->BindTexture(&g_gameFont->GetTexture());
	g_theRenderer->DrawVertexArray(lobbyTextVerts);
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
void Game::KeyboardControls()
{
	switch(m_state)
	{
	case GameState::ATTRACT:
	{
		AttractModeKeyboardControls();
		break;
	}
	case GameState::MODE_SELECT:
	{
		ModeSelectKeyboardControls();
		break;
	}
	case GameState::SERVER_HOST:
	{
		ServerClientKeyboardControls();
		break;
	}
	case GameState::CLIENT_CONNECT:
	{
		ServerClientKeyboardControls();
		break;
	}
	case GameState::LOBBY:
	{
		LobbyKeyboardControls();
		break;
	}
	case GameState::MATCH:
	{
		MatchKeyboardControls();
		break;
	}
	default:
		break;
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
void Game::BeginChessMatch(Player* firstPlayer, Player* secondPlayer)
{
	if(m_chessMatch)
	{
		delete m_chessMatch;
		m_chessMatch = nullptr;
	}
 
  	firstPlayer->m_position = Vec3(4.f, -3.f, 5.f);
  	firstPlayer->m_orientation = EulerAngles(90.f, 35.f, 0.f);
  
  	secondPlayer->m_position = Vec3(4.f, 13.f, 5.f);
  	secondPlayer->m_orientation = EulerAngles(-90.f, 35.f, 0.f);
  
	if(!m_isMultiplayer)
	{
		m_currentPlayer->m_position = firstPlayer->m_position;
		m_currentPlayer->m_orientation = firstPlayer->m_orientation;
	}

	m_chessMatch = new ChessMatch(this, firstPlayer->m_playerID, secondPlayer->m_playerID);

	m_chessMatch->m_playerOneName = firstPlayer->m_playerName;
	m_chessMatch->m_playerTwoName = secondPlayer->m_playerName;

	m_state = GameState::MATCH;

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Game::ExecuteNetCommands()
{
	std::vector<char> recvBuffer = g_netSystem->GetDataFromReceiveBuffer();

	std::string command;

	for(char byte : recvBuffer)
	{
		if(byte == '\0')
		{
			command = command + " remote=true";

			g_devConsole->Execute(command);
			
			command.clear();
			continue;
		}
		command += byte;
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Game::AttractModeKeyboardControls()
{
	if(g_inputSystem->WasKeyJustPressed(' '))
	{
		CreateModeSelectButtons();
		m_state = GameState::MODE_SELECT;
	}

	if(g_inputSystem->WasKeyJustPressed(KEYCODE_ESC))
	{
		g_theApp->RequestQuit();
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Game::ModeSelectKeyboardControls()
{
	Vec2 mousePosition = g_theWindow->GetNormalizedMouseUV();
	Vec2 windowDim = Vec2(g_theWindow->GetClientDimensions());
 	mousePosition *= windowDim;

	if(g_inputSystem->WasKeyJustPressed(KEYCODE_LEFT_MOUSE))
	{
		for(size_t buttonIndex = 0; buttonIndex < m_activeButtons.size(); ++buttonIndex)
		{
			if(IsPointInsideAABB2D(mousePosition, m_activeButtons[buttonIndex]))
			{
				if(buttonIndex == 0)
				{
					BeginChessMatch(m_players[0], m_players[1]);
					break;
				}
				else if(buttonIndex == 1)
				{
					CreateServerHostButtons();
					FireEvent("ChessListen");

					m_insertionPointPosition = static_cast<int>(m_inputString.length());
					m_ip = g_netSystem->GetServerIPAddress();
					m_port = g_netSystem->GetServerListenPortNumber();
					m_state = GameState::SERVER_HOST;
					break;
				}
				else if(buttonIndex == 2)
				{
					CreateClientConnectButtons();
					m_insertionPointPosition = static_cast<int>(m_ip.length());
					m_state = GameState::CLIENT_CONNECT;
					break;
				}
				else if(buttonIndex == 3)
				{
					FireEvent("Quit");
					break;
				}
			}
		}
	}

	if(g_inputSystem->WasKeyJustPressed(KEYCODE_ESC))
	{
		m_state = GameState::ATTRACT;
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Game::ServerClientKeyboardControls()
{
	InputKeys();

	Vec2 mousePosition = g_theWindow->GetNormalizedMouseUV();
	Vec2 windowDim = Vec2(g_theWindow->GetClientDimensions());
	mousePosition *= windowDim;

	NetworkState netState = g_netSystem->GetCurrentNetworkState();
	int connectedClients = g_netSystem->GetNumConnectedClients();

	if(g_inputSystem->WasKeyJustPressed(KEYCODE_LEFT_MOUSE))
	{
		for(size_t buttonIndex = 0; buttonIndex < m_activeButtons.size(); ++buttonIndex)
		{
			if(IsPointInsideAABB2D(mousePosition, m_activeButtons[buttonIndex]))
			{
				if(buttonIndex == 0)
				{
					FireEvent("ChessDisconnect");
					CreateModeSelectButtons();
					m_state = GameState::MODE_SELECT;
				}

				if(buttonIndex == 1)
				{
					if(m_state == GameState::SERVER_HOST && connectedClients > 0)
					{
						EventArgs args;
						args.SetValue("name", m_inputString);
						g_eventSystem->FireEvent("ChessPlayerInfo", args);

						CreateLobbyButtons();
						m_insertionPointPosition = static_cast<int>(m_inputString.length());
						m_state = GameState::LOBBY;
					}

					if(m_state == GameState::CLIENT_CONNECT && (netState == NetworkState::CLIENT_CONNECTED))
					{
						CreateLobbyButtons();
						m_insertionPointPosition = static_cast<int>(m_inputString.length());
						m_state = GameState::LOBBY;
					}					
				}

				if(m_state == GameState::CLIENT_CONNECT && buttonIndex == 2)
				{
					EventArgs args;
					args.SetValue("ip", m_ip);
					args.SetValue("port", m_port);
					g_eventSystem->FireEvent("ChessConnect", args);
				}
			}
		}
	}

	if(g_inputSystem->WasKeyJustPressed(KEYCODE_ESC))
	{
		FireEvent("ChessDisconnect");
		CreateModeSelectButtons();
		m_state = GameState::MODE_SELECT;
	}

	if(g_inputSystem->WasKeyJustPressed(KEYCODE_TAB))
	{
		if(m_state == GameState::CLIENT_CONNECT)
		{
			m_isEnteringPort = !m_isEnteringPort;

			if(m_isEnteringPort)
			{
				m_insertionPointPosition = static_cast<int>(m_port.size());
			}
			else
			{
				m_insertionPointPosition = static_cast<int>(m_ip.size());
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Game::LobbyKeyboardControls()
{
	InputKeys();

	Vec2 mousePosition = g_theWindow->GetNormalizedMouseUV();
	Vec2 windowDim = Vec2(g_theWindow->GetClientDimensions());
	mousePosition *= windowDim;

	if(g_inputSystem->WasKeyJustPressed(KEYCODE_LEFT_MOUSE))
	{
		for(size_t buttonIndex = 0; buttonIndex < m_activeButtons.size(); ++buttonIndex)
		{
			if(IsPointInsideAABB2D(mousePosition, m_activeButtons[buttonIndex]))
			{
				if(buttonIndex == 0)
				{
					FireEvent("ChessDisconnect");
					CreateModeSelectButtons();
					m_state = GameState::MODE_SELECT;
				}

				if(buttonIndex == 1)
				{
					EventArgs args;
					args.SetValue("name", m_inputString);
					g_eventSystem->FireEvent("ChessPlayerInfo", args);
				}

				if(buttonIndex == 2)
				{
					EventArgs args;
					args.SetValue("firstPlayer", m_players[0]->m_playerName);
					g_eventSystem->FireEvent("ChessBegin", args);
				}
				if(buttonIndex == 3)
				{
					EventArgs args;
					args.SetValue("firstPlayer", m_players[1]->m_playerName);
					g_eventSystem->FireEvent("ChessBegin", args);
				}
			}
		}
	}

	if(g_inputSystem->WasKeyJustPressed(KEYCODE_ESC))
	{
		FireEvent("ChessDisconnect");
		CreateModeSelectButtons();
		m_state = GameState::MODE_SELECT;
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Game::MatchKeyboardControls()
{
	if(g_inputSystem->WasKeyJustPressed(KEYCODE_F1))
	{
		m_isDebugMode = !m_isDebugMode;
	}

	if(g_inputSystem->WasKeyJustPressed(KEYCODE_F4))
	{
		m_freeCamera = !m_freeCamera;
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
	if(g_inputSystem->WasKeyJustPressed(KEYCODE_ESC))
	{
		CreateModeSelectButtons();
		m_state = GameState::MODE_SELECT;
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Game::CreateModeSelectButtons()
{
	m_activeButtons.clear();

	AABB2 screenBounds;
	screenBounds.m_mins = Vec2::ZERO;
	screenBounds.m_maxs = Vec2(g_theWindow->GetClientDimensions());

	AABB2 singlePlayerButton = screenBounds.GetBoxFromUVs(0.6f, 0.6f, 0.8f, 0.7f);
	m_activeButtons.push_back(singlePlayerButton);

	AABB2 hostButton = screenBounds.GetBoxFromUVs(0.6f, 0.48f, 0.8f, 0.58f);
	m_activeButtons.push_back(hostButton);

	AABB2 joinButton = screenBounds.GetBoxFromUVs(0.6f, 0.36f, 0.8f, 0.46f);
	m_activeButtons.push_back(joinButton);

	AABB2 quitButton = screenBounds.GetBoxFromUVs(0.6f, 0.24f, 0.8f, 0.34f);
	m_activeButtons.push_back(quitButton);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Game::CreateServerHostButtons()
{
	m_activeButtons.clear();

	AABB2 screenBounds;
	screenBounds.m_mins = Vec2::ZERO;
	screenBounds.m_maxs = Vec2(g_theWindow->GetClientDimensions());

	AABB2 backButton = screenBounds.GetBoxFromUVs(0.01f, 0.94f, 0.035f, 0.98f);
	m_activeButtons.push_back(backButton);

	AABB2 nextButton = screenBounds.GetBoxFromUVs(0.9f, 0.01f, 0.98f, 0.08f);
	m_activeButtons.push_back(nextButton);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Game::CreateClientConnectButtons()
{
	m_activeButtons.clear();

	AABB2 screenBounds;
	screenBounds.m_mins = Vec2::ZERO;
	screenBounds.m_maxs = Vec2(g_theWindow->GetClientDimensions());

	AABB2 backButton = screenBounds.GetBoxFromUVs(0.01f, 0.94f, 0.035f, 0.98f);
	m_activeButtons.push_back(backButton);

	AABB2 nextButton = screenBounds.GetBoxFromUVs(0.9f, 0.01f, 0.98f, 0.08f);
	m_activeButtons.push_back(nextButton);

	AABB2 connectButton = screenBounds.GetBoxFromUVs(0.45f, 0.3f, 0.55f, 0.35f);
	m_activeButtons.push_back(connectButton);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Game::CreateLobbyButtons()
{
	m_activeButtons.clear();

	AABB2 screenBounds;
	screenBounds.m_mins = Vec2::ZERO;
	screenBounds.m_maxs = Vec2(g_theWindow->GetClientDimensions());

	AABB2 backButton = screenBounds.GetBoxFromUVs(0.01f, 0.94f, 0.035f, 0.98f);
	m_activeButtons.push_back(backButton);

	AABB2 setName = screenBounds.GetBoxFromUVs(0.1f, 0.45f, 0.3f, 0.55f);
	m_activeButtons.push_back(setName);

	AABB2 playerOne = screenBounds.GetBoxFromUVs(0.8f, 0.55f, 0.9f, 0.65f);
	m_activeButtons.push_back(playerOne);

	AABB2 playerTwo = screenBounds.GetBoxFromUVs(0.8f, 0.4f, 0.9f, 0.5f);
	m_activeButtons.push_back(playerTwo);


}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
std::vector<std::string> Game::GetButtonNamesForGameState() const
{
	std::vector<std::string> names;

	switch(m_state)
	{
	case GameState::MODE_SELECT:
	{
		names.push_back("Local Multiplayer");
		names.push_back("Host Game");
		names.push_back("Join Game");
		names.push_back("Quit");
		break;
	}
	case GameState::SERVER_HOST:
	{
		names.push_back("<");
		names.push_back("Next >");
		break;
	}
	case GameState::CLIENT_CONNECT:
	{
		names.push_back("<");
		names.push_back("Next >");
		names.push_back("Connect");
		break;
	}
	case GameState::LOBBY:
		names.push_back("<");
		names.push_back("Set Name");
		names.push_back(Stringf("%s", m_players[0]->m_playerName.c_str()));
		names.push_back(Stringf("%s", m_players[1]->m_playerName.c_str()));
		break;
	case GameState::MATCH:
		break;
	default:
		break;
	}

	return names;
}
