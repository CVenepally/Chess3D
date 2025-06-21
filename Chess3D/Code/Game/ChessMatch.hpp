#pragma once
#include <string>

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
class ChessBoard;
class Game;
class NamedStrings;
typedef NamedStrings EventArgs;
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
enum class MatchState
{
	NONE = -1,

	PLAYER_ONE_TURN,
	PLAYER_TWO_TURN,
	PLAYER_ONE_WINS,
	PLAYER_TWO_WINS,

	COUNT
};

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
class ChessMatch
{
	friend class Game;

private:
	ChessMatch(Game* owningGame);
	~ChessMatch();

	void		Update();
	void		Render() const;

	void		RenderUIText() const;

	void		RenderStartBoardToDevConsole() const;
	void		PrintUpdatedBoard(bool didKingDie = false) const;
	void		SwitchTurns();
	int			GetCurrentPlayerID() const;

public:
	MatchState	GetMatchState() const;
	void		SetMatchState(MatchState newMatchState);
	void		ChessMove(std::string from, std::string to, std::string promoteTo = "none");

	static bool OnChessMove(EventArgs& args);
	static bool	OnChessCheats(EventArgs& args);

private:

	Game*		m_owningGame = nullptr;
	ChessBoard* m_chessBoard = nullptr;
	MatchState	m_matchState = MatchState::NONE;

	bool		m_canTeleport = false;

public:

	int			m_currentTurn = 0;

};