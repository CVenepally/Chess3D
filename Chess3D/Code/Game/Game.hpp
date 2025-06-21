#pragma once

#include "Game/GameCommon.hpp"

#include "Engine/Renderer/Camera.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"

//------------------------------------------------------------------------------------------------------------------
class Timer;
class Clock;
class Entity;
class Player;
class Image;
class Shader;
class ChessMatch;

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

	static bool					OnBeginChessMatch(EventArgs& args);
	Vec3 const&					GetCameraPosition() const;
private:
	
	void						UpdateCameras();

	void						RenderAttractMode() const;
	void						RenderGame() const;

	void						InitializeGridVerts(float size);

	void						PrintControlsOnDevConsole();
	void						RenderGrid() const;

	void						KeyboardControls();
	void						XboxControls();

	void						TimeManipulation(); 
	void						BeginChessMatch();

public:

	bool						m_isDebugMode	= false;
	bool						m_isAttractMode = true;
	Clock*						m_gameClock		= nullptr;
	Timer*						m_timerOne		= nullptr;
	ChessMatch*					m_chessMatch	= nullptr;
	bool						m_freeCamera	= false;

private:

	int							m_debugInt		= 0;
	float						m_debugFloat	= 0.f;

	Player*						m_player		= nullptr;
	Timer*						m_timerTwo		= nullptr;

	std::vector<Vertex_PCU>		m_gridVerts;
	Camera						m_screenCamera;
};
