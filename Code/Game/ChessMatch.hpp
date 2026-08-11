#pragma once
#include "Engine/Core/EngineCommon.hpp"

#include <string>

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
class ChessBoard;
class Game;
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
enum class MatchState
{
	IDLE = -1,

	PLAYER_ONE_TURN,
	PLAYER_TWO_TURN,
	PLAYER_ONE_WINS,
	PLAYER_TWO_WINS,
	DRAW,

	COUNT
};

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
class ChessMatch
{
	friend class Game;

private:
	ChessMatch(Game* owningGame, int playerOneID, int playerTwoID);
	~ChessMatch();

	void		Update();
	void		Render() const;

	void		RenderUIText() const;

	void		RenderStartBoardToDevConsole() const;
	void		PrintUpdatedBoard(bool didKingDie = false) const;

	std::string GetBoardAsString();

	void		SwitchTurns();
	std::string GetDebugModeString() const;

public:
	MatchState	GetMatchState() const;
	void		SetMatchState(MatchState newMatchState);
	void		ChessMove(std::string from, std::string to, std::string promoteTo = "none");

	static bool OnChessMove(EventArgs& args);
	static bool	OnChessCheats(EventArgs& args);

private:

	Game*		m_owningGame = nullptr;
	ChessBoard* m_chessBoard = nullptr;
	MatchState	m_matchState = MatchState::IDLE;

	std::string m_playerOneName;
	std::string m_playerTwoName;

	bool		m_canTeleport = false;

public:

	int			m_currentTurn = 0;

};