#pragma once

#include "Game/GameCommon.hpp"

#include "Engine/Renderer/Camera.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"

#include <string>
//------------------------------------------------------------------------------------------------------------------
class Timer;
class Clock;
class Entity;
class Player;
class Image;
class Shader;
class ChessMatch;

enum class GameState
{
	ATTRACT,
	MODE_SELECT,
	SERVER_HOST,
	CLIENT_CONNECT,
	LOBBY,
	MATCH
};


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
class Game
{
public:

	Game();
	~Game();

	void						Startup();
	void						Shutdown();
	void						Update();
	void						Render() const;
	void						RenderWorld() const;
	void						RenderScreen() const;


	Vec3 const&					GetCameraPosition() const;
	Vec3						GetCameraForward() const;
	int							GetCurrentPlayerID() const;
	bool						GetIsPlayerGoingFirst(int playerID) const;
	bool						SetInputString(unsigned int charToAppend);

	void						SwitchCurrentPlayer();

	static bool					OnBeginChessMatch(EventArgs& args);
	static bool					OnChessPlayerInfo(EventArgs& args);
	static bool					OnChessSpawnInfo(EventArgs& args);
	static bool					OnRemotePlayerJoined(EventArgs& args);
	static bool					OnChessValidate(EventArgs& args);
	static bool					OnChessResign(EventArgs& args);
	static bool					OnOfferDraw(EventArgs& args);
	static bool					OnAcceptDraw(EventArgs& args);
	static bool					OnRejectDraw(EventArgs& args);
	static bool					OnCharTyped(EventArgs& args);

private:

	void						UpdateCameras();

	void						RenderAttractMode() const;
	void						RenderModeSelect() const;
	void						RenderHostScreen() const;
	void						RenderClientScreen() const;
	void						RenderLobby() const;

	void						RenderGame() const;

	void						KeyboardControls();

	void						TimeManipulation();
	void						BeginChessMatch(Player* firstPlayer, Player* secondPlayer);
	void						ExecuteNetCommands();

	void						AttractModeKeyboardControls();
	void						ModeSelectKeyboardControls();
	void						ServerClientKeyboardControls();
	void						LobbyKeyboardControls();
	void						MatchKeyboardControls();

	void						CreateModeSelectButtons();
	void						CreateServerHostButtons();
	void						CreateClientConnectButtons();
	void						CreateLobbyButtons();
	void						InputKeys();

	std::vector<std::string>	GetButtonNamesForGameState() const;

public:

	bool						m_isDebugMode		= false;
	Clock*						m_gameClock			= nullptr;
	Timer*						m_timerOne			= nullptr;
	ChessMatch*					m_chessMatch		= nullptr;
	bool						m_freeCamera		= true;

	int							m_debugInt			= 0;
	float						m_debugFloat		= 0.f;
	bool						m_isMultiplayer		= false;

	bool						m_wasDrawOffered	= false;
	GameState					m_state				= GameState::ATTRACT;
	Player*						m_localPlayer		= nullptr;

private:

	Player*						m_currentPlayer		= nullptr;
	Player*						m_players[2]		= {nullptr, nullptr};

	Camera						m_screenCamera;

	std::vector<AABB2>			m_activeButtons;

	std::string					m_ip = "127.0.0.1";
	std::string					m_port = "3100";
	std::string					m_inputString;

	int							m_insertionPointPosition = 0;
	bool						m_insertionPointVisible = true;
	bool						m_isEnteringPort = false;
};
