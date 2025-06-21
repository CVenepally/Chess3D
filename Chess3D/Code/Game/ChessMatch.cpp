#include "Game/ChessMatch.hpp"
#include "Game/ChessBoard.hpp"
#include "Game/ChessPiece.hpp"
#include "Game/Game.hpp"

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/DevConsole.hpp"
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern Game* g_game;
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
ChessMatch::ChessMatch(Game* owningGame)
	:m_owningGame(owningGame)
{
	m_chessBoard = new ChessBoard(this);
	g_devConsole->AddLine(Rgba8(0, 255, 100), "Welcome to Chess3D! Let the game begin.");

	m_matchState = MatchState::PLAYER_ONE_TURN;
	m_currentTurn = 1;
	RenderStartBoardToDevConsole();

	SubscribeEventCallbackFunction("ChessMove",		OnChessMove);
	SubscribeEventCallbackFunction("ChessCheat",	OnChessCheats);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
ChessMatch::~ChessMatch()
{
	UnsubscribeEventCallbackFunction("ChessMove",	OnChessMove);
	UnsubscribeEventCallbackFunction("ChessCheat", OnChessCheats);
	if(m_chessBoard)
	{
		delete m_chessBoard;
		m_chessBoard = nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ChessMatch::Update()
{
	if(m_chessBoard)
	{
		m_chessBoard->Update();
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ChessMatch::Render() const
{
	if(m_chessBoard)
	{
		m_chessBoard->Render();
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ChessMatch::RenderUIText() const
{
	std::vector<Vertex_PCU> textVerts;
	IntVec2 clientDims = g_theWindow->GetClientDimensions();
	AABB2 textBox = AABB2(Vec2::ZERO, clientDims.GetAsVec2());

	g_gameFont->AddVertsForTextInBox2D(textVerts, "Press '~' to open DevConsole", textBox, 14.f, Rgba8::CYAN, 0.8f, Vec2(0.005f, 0.915f), SHRINK_TO_FIT);
	g_gameFont->AddVertsForTextInBox2D(textVerts, "--------------------------------", textBox, 14.f, Rgba8::CYAN, 0.8f, Vec2(0.005f, 0.898f), SHRINK_TO_FIT);

	std::string matchState;
	std::string cameraControl;

	switch(m_matchState)
	{
	case MatchState::PLAYER_ONE_TURN:
	{
		matchState = "Match State: Player 1's Turn";
		break;
	}
	case MatchState::PLAYER_TWO_TURN:
	{
		matchState = "Match State: Player 2's Turn";
		break;
	}
	case MatchState::PLAYER_ONE_WINS:
	{
		matchState = "Match State: Game Over. Player 1 wins!";
		break;
	}
	case MatchState::PLAYER_TWO_WINS:
	{
		matchState = "Match State: Game Over. Player 2 wins!";
		break;
	}
	default:
		break;
	}

	if(m_owningGame->m_freeCamera)
	{
		cameraControl = "[F4] Camera Mode: Free";
	}
	else
	{
		cameraControl = "[F4] Camera Mode: Fixed / Auto";
	}

	g_gameFont->AddVertsForTextInBox2D(textVerts, Stringf("%s | %s", cameraControl.c_str(), matchState.c_str()), textBox, 14.f, Rgba8::YELLOW, 0.8f, Vec2(0.005f, 0.878f), SHRINK_TO_FIT);
	
	g_theRenderer->BeginRenderEvent("Screen UI Render");
	g_theRenderer->SetModelConstants();
	g_theRenderer->SetBlendMode(BlendMode::ALPHA);
	g_theRenderer->SetDepthMode(DepthMode::READ_WRITE_LESS_EQUAL);
	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
	g_theRenderer->SetSamplerMode(SamplerMode::POINT_CLAMP);
	g_theRenderer->BindTexture(&g_gameFont->GetTexture());
	g_theRenderer->BindShader(nullptr);
	g_theRenderer->DrawVertexArray(textVerts);

	g_theRenderer->EndRenderEvent("UI Render");
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ChessMatch::RenderStartBoardToDevConsole() const
{
	g_devConsole->AddLine(DevConsole::INTRO_SUBTEXT, "###############################################################################");
	g_devConsole->AddLine(DevConsole::INTRO_SUBTEXT, "Player 1's Turn");
	g_devConsole->AddLine(DevConsole::INFO_MAJOR, "  ABCDEFGH ");
	g_devConsole->AddLine(DevConsole::INFO_MAJOR, " +--------+ ");
	g_devConsole->AddLine(DevConsole::INFO_MAJOR, "8|rnbqkbnr|8");
	g_devConsole->AddLine(DevConsole::INFO_MAJOR, "7|pppppppp|7 ");
	g_devConsole->AddLine(DevConsole::INFO_MAJOR, "6|........|6 ");
	g_devConsole->AddLine(DevConsole::INFO_MAJOR, "5|........|5 ");
	g_devConsole->AddLine(DevConsole::INFO_MAJOR, "4|........|4 ");
	g_devConsole->AddLine(DevConsole::INFO_MAJOR, "3|........|3 ");
	g_devConsole->AddLine(DevConsole::INFO_MAJOR, "2|PPPPPPPP|2 ");
	g_devConsole->AddLine(DevConsole::INFO_MAJOR, "1|RNBQKBNR|1 ");
	g_devConsole->AddLine(DevConsole::INFO_MAJOR, " +--------+ ");
	g_devConsole->AddLine(DevConsole::INFO_MAJOR, "  ABCDEFGH ");
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ChessMatch::PrintUpdatedBoard(bool didKingDie) const
{
	std::string rankStrings[8];

	for(int y = 0; y < 8; y++)
	{
		for(int x = 0; x < 8; x++)
		{
			IntVec2 boardTile = IntVec2(x, y);

			if(!m_chessBoard->IsTileEmpty(boardTile))
			{
				ChessPiece* chessPiece = m_chessBoard->GetPieceOnTile(boardTile);

				if(chessPiece)
				{
					std::string pieceGlyph = chessPiece->GetPieceGlyph();
					rankStrings[y] += pieceGlyph;
				}
			}
			else
			{
				rankStrings[y] += ".";
			}
		}
	}

	g_devConsole->AddLine(DevConsole::INTRO_SUBTEXT, "###############################################################################");
	if(didKingDie)
	{
		switch(m_matchState)
		{
			case MatchState::PLAYER_ONE_WINS:
			{
				g_devConsole->AddLine(DevConsole::INTRO_SUBTEXT, Stringf("Player 1 wins the game!"));
				g_devConsole->AddLine(DevConsole::INTRO_SUBTEXT, Stringf("Use 'ChessBegin' to restart"));
				break;
			}
			case MatchState::PLAYER_TWO_WINS:
			{
				g_devConsole->AddLine(DevConsole::INTRO_SUBTEXT, Stringf("Player 2 wins the game!"));
				g_devConsole->AddLine(DevConsole::INTRO_SUBTEXT, Stringf("Use 'ChessBegin' to restart"));
				break;
			}
			case MatchState::COUNT:
				break;
			default:
				break;
		}
	}
	else
	{
		g_devConsole->AddLine(DevConsole::INTRO_SUBTEXT, Stringf("Player %d's Turn", static_cast<int>(m_matchState) + 1));
	}
	g_devConsole->AddLine(DevConsole::INFO_MAJOR, "  ABCDEFGH ");
	g_devConsole->AddLine(DevConsole::INFO_MAJOR, " +--------+ ");
	g_devConsole->AddLine(DevConsole::INFO_MAJOR, Stringf("8|%s|8", rankStrings[7].c_str()));
	g_devConsole->AddLine(DevConsole::INFO_MAJOR, Stringf("7|%s|7", rankStrings[6].c_str()));
	g_devConsole->AddLine(DevConsole::INFO_MAJOR, Stringf("6|%s|6", rankStrings[5].c_str()));
	g_devConsole->AddLine(DevConsole::INFO_MAJOR, Stringf("5|%s|5", rankStrings[4].c_str()));
	g_devConsole->AddLine(DevConsole::INFO_MAJOR, Stringf("4|%s|4", rankStrings[3].c_str()));
	g_devConsole->AddLine(DevConsole::INFO_MAJOR, Stringf("3|%s|3", rankStrings[2].c_str()));
	g_devConsole->AddLine(DevConsole::INFO_MAJOR, Stringf("2|%s|2", rankStrings[1].c_str()));
	g_devConsole->AddLine(DevConsole::INFO_MAJOR, Stringf("1|%s|1", rankStrings[0].c_str()));
	g_devConsole->AddLine(DevConsole::INFO_MAJOR, " +--------+ ");
	g_devConsole->AddLine(DevConsole::INFO_MAJOR, "  ABCDEFGH ");
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ChessMatch::SwitchTurns()
{
	if(m_matchState == MatchState::PLAYER_ONE_TURN)
	{
		m_matchState = MatchState::PLAYER_TWO_TURN;
	}
	else if(m_matchState == MatchState::PLAYER_TWO_TURN)
	{
		m_matchState = MatchState::PLAYER_ONE_TURN;
		m_currentTurn += 1;
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
int ChessMatch::GetCurrentPlayerID() const
{

	switch(m_matchState)
	{
	case MatchState::PLAYER_ONE_TURN: return 0;
	case MatchState::PLAYER_TWO_TURN: return 1;
	default: return -1;
	}

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ChessMatch::ChessMove(std::string from, std::string to, std::string promoteTo)
{
	if(m_matchState == MatchState::PLAYER_ONE_WINS || m_matchState == MatchState::PLAYER_TWO_WINS)
	{
		g_devConsole->AddLine(DevConsole::WARNING, "The match has ended! If you lost and still trying to play, get over it and get good!");
		g_devConsole->AddLine(DevConsole::ERROR, "Use 'ChessBegin' to start a new game");
		return;
	}

	if(from.length() > 2 || to.length() > 2)
	{
		g_devConsole->AddLine(DevConsole::ERROR, "Invalid Command!! Must be two-letters!");
		g_devConsole->AddLine(DevConsole::WARNING, "Ex: from=a1 to=a5");
		return;
	}

	char fromXChar = from[0];
	int fromX = static_cast<int>(fromXChar) - 97;

	std::string fromYStr = from.substr(1);
	int fromY = static_cast<int>(stoi(fromYStr));

	char toXChar = to[0];
	int toX = static_cast<int>(toXChar) - 97;

	std::string toYStr = to.substr(1);
	int toY = static_cast<int>(stoi(toYStr));

	if(fromX < 0 || fromX > 7 || toX < 0 || toX > 7 || fromY < 1 || fromY > 8 || toY < 1 || toY > 8)
	{
		g_devConsole->AddLine(DevConsole::ERROR, "Out of bounds!! Moves should range from A to H, 1 to 8");
		return;
	}

	IntVec2 fromTile	= IntVec2(fromX, fromY - 1);
	IntVec2 toTile		= IntVec2(toX, toY - 1);
	
	if(m_chessBoard->IsTileEmpty(fromTile))
	{
		g_devConsole->AddLine(DevConsole::WARNING, Stringf("No piece found on square %s", from.c_str()));
		return;
	}

	if(m_canTeleport)
	{
		bool didKingDie = false;

		if(m_chessBoard->MovePiece(fromTile, toTile, m_matchState, didKingDie))
		{
			if(didKingDie)
			{
				m_matchState = static_cast<MatchState>(static_cast<int>(m_matchState) + 2);
			}
			else
			{
				SwitchTurns();
			}

			PrintUpdatedBoard(didKingDie);
		}
	}
	else
	{
		ChessMoveResult moveResult = m_chessBoard->MovePiece(fromTile, toTile, m_matchState, promoteTo);

		if(moveResult.m_isValid)
		{
			if(moveResult.m_isKingCaptured)
			{
				m_matchState = static_cast<MatchState>(static_cast<int>(m_matchState) + 2);
			}
			else
			{
				SwitchTurns();
			}
			PrintUpdatedBoard(moveResult.m_isKingCaptured);
		}
	}	
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
MatchState ChessMatch::GetMatchState() const
{
	return m_matchState;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ChessMatch::SetMatchState(MatchState newMatchState)
{
	m_matchState = newMatchState;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool ChessMatch::OnChessMove(EventArgs& args)
{
	std::string fromString = args.GetValue("from", "none");
	std::string toString = args.GetValue("to", "none");
	std::string promoteTo = args.GetValue("promoteTo", "none");

	if(fromString != "none" && toString != "none")
	{
		g_game->m_chessMatch->ChessMove(fromString, toString, promoteTo);
	}
	else
	{
		g_devConsole->AddLine(DevConsole::ERROR, "Invalid Command! Input From and To position. Ex: ChessMove from=e2 to=e4");
		return false;
	}
		 

	return true;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool ChessMatch::OnChessCheats(EventArgs& args)
{
	if(args.IsEmpty())
	{
		std::string teleportStatus = (g_game->m_chessMatch->m_canTeleport == true) ? "True" : "False";

		g_devConsole->AddLine(DevConsole::INFO_MAJOR, "Allowed and active cheats");
		g_devConsole->AddLine(DevConsole::INFO_MAJOR, "------------------------------------------");
		g_devConsole->AddLine(DevConsole::INTRO_TEXT, "	Cheat                   Command                   Status");
		g_devConsole->AddLine(DevConsole::INFO_MINOR, Stringf("Teleport		   ChessCheat Teleport=[True / False]				%s", teleportStatus.c_str()));		
		
		return false;
	}
	else
	{
		std::string teleport = args.GetValue("Teleport", "none");

		if(teleport == "True" || teleport == "False")
		{
			g_game->m_chessMatch->m_canTeleport = (teleport == "True");
			g_devConsole->AddLine(DevConsole::WARNING, Stringf("Teleport cheat active. Pawn promotion will not work."));

		}
		else
		{
			g_devConsole->AddLine(DevConsole::ERROR, Stringf("Invalid Cheat! Use 'ChessCheat' for more information"));
		}
	}

	return false;
}
