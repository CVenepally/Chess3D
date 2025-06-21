#include "Game/ChessBoard.hpp"
#include "Game/ChessMatch.hpp"
#include "Game/ChessPiece.hpp"
#include "Game/ChessPieceDefinition.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"

#include "Engine/Core/DebugRender.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Vertex_PCUTBN.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine/Renderer/IndexBuffer.hpp"
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern Game* g_game;
unsigned int s_indexCount = 0;
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
ChessBoard::ChessBoard(ChessMatch* owningMatch)
	: m_owningMatch(owningMatch)
{
	InitializeBoardBuffers();
	InitializeChessPieces();

	m_boardShader = g_theRenderer->CreateOrGetShader("Data/Shaders/BlinnPhong", InputLayoutType::VERTEX_PCUTBN);
	m_boardTexture	= g_theRenderer->CreateOrGetTextureFromFileNameAndType("Bricks", TextureType::DIFFUSE);
	m_boardNormalMap = g_theRenderer->CreateOrGetTextureFromFileNameAndType("Bricks", TextureType::NORMAL);
	m_boardSgeMap	= g_theRenderer->CreateOrGetTextureFromFileNameAndType("Bricks", TextureType::SGE);

	Vec3 sunDirection = Vec3::MakeFromPolarDegrees(m_sunDirectionYawPitch.m_pitchDegrees, m_sunDirectionYawPitch.m_yawDegrees).GetNormalized();

	m_directionalLight = Light::CreateDirectionalLight(sunDirection, Rgba8(245, 250, 255), 0.5f);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
ChessBoard::~ChessBoard()
{
	if(m_vertexBuffer)
	{
		delete m_vertexBuffer;
		m_vertexBuffer = nullptr;
	}

	if(m_indexBuffer)
	{
		delete m_indexBuffer;
		m_indexBuffer = nullptr;
	}

	for(int index = 0; index < static_cast<int>(ChessTeam::COUNT); ++index)
	{
		for(int pIndex = 0; pIndex < static_cast<int>(m_chessPiecesByTeam[index].size()); ++pIndex)
		{
			m_chessPiecesByTeam[index][pIndex] = nullptr;
		}
	}

	for(int index = 0; index < static_cast<int>(m_allChessPieces.size()); ++index)
	{
		if(m_allChessPieces[index])
		{
			delete m_allChessPieces[index];
			m_allChessPieces[index] = nullptr;

		}
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ChessBoard::Update()
{
	DebugLightControls();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ChessBoard::Render() const
{
 	RenderBoard();
	RenderPieces();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool ChessBoard::MovePiece(IntVec2 const& fromTile, IntVec2 const& toTile, MatchState movingPlayer, bool& out_didKingDie, std::string newPiece)
{
	std::string fromSquare = std::string(1, static_cast<char>(fromTile.x + 97));
	fromSquare.append(std::to_string(fromTile.y + 1));

	std::string toSquare = std::string(1, static_cast<char>(toTile.x + 97));
	toSquare.append(std::to_string(toTile.y + 1));

	// If not check if the Piece belongs to moving player; if not return false
	ChessPiece* pieceOnFromTile = GetPieceOnTile(fromTile);

	if(pieceOnFromTile->m_owningPlayerID != static_cast<int>(movingPlayer))
	{
		g_devConsole->AddLine(DevConsole::ERROR, Stringf("Piece on %s belongs to Player %d!", fromSquare.c_str(), pieceOnFromTile->m_owningPlayerID + 1));
		return false;
	}

	// If toTile is empty, move piece, return true
	if(IsTileEmpty(toTile))
	{
		std::string pieceType = pieceOnFromTile->GetPieceTypeAsString();

		pieceOnFromTile->Occupy(toTile);
		g_devConsole->AddLine(DevConsole::INFO_MAJOR, Stringf("Moved Player %d %s from %s to %s", pieceOnFromTile->m_owningPlayerID + 1, pieceType.c_str(), fromSquare.c_str(), toSquare.c_str()));
		return true;
	}
	else
	{
		// If not, check if the piece is opponent piece; if not, return false

		ChessPiece* pieceOnToTile	= GetPieceOnTile(toTile);
		std::string fromPieceType	= pieceOnFromTile->GetPieceTypeAsString();
		std::string toPieceType		= pieceOnToTile->GetPieceTypeAsString();

		if(pieceOnToTile->m_owningPlayerID == static_cast<int>(movingPlayer))
		{
			g_devConsole->AddLine(DevConsole::ERROR, Stringf("Cannot move %s from %s to %s. %s is occupied by own %s!!", fromPieceType.c_str(), fromSquare.c_str(), toSquare.c_str(), toSquare.c_str(), toPieceType.c_str()));
			return false;
		}
		else
		{
			g_devConsole->AddLine(DevConsole::INFO_MAJOR, Stringf("Player %d has captured Player %d's %s!!", pieceOnFromTile->m_owningPlayerID + 1, pieceOnToTile->m_owningPlayerID + 1, toPieceType.c_str()));
		
			if(pieceOnToTile->GetPieceTypeAsString() == "King")
			{
				out_didKingDie = true;
			}

			pieceOnFromTile->Capture(pieceOnToTile);
			return true;
		}

	}

	return false;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
ChessMoveResult ChessBoard::MovePiece(IntVec2 const& fromTile, IntVec2 const& toTile, MatchState movingPlayer, std::string newPiece)
{
	// get piece on from tile and to tile, if belong to same player return;
	ChessPiece* pieceOnFromTile = GetPieceOnTile(fromTile);
	ChessPiece* pieceOnToTile	= GetPieceOnTile(toTile);


	if(pieceOnToTile && pieceOnFromTile->m_owningPlayerID == pieceOnToTile->m_owningPlayerID)
	{
		std::string fromPieceType = pieceOnFromTile->GetPieceTypeAsString();
		std::string toPieceType = pieceOnToTile->GetPieceTypeAsString();

		std::string fromSquare = std::string(1, static_cast<char>(fromTile.x + 97));
		fromSquare.append(std::to_string(fromTile.y + 1));

		std::string toSquare = std::string(1, static_cast<char>(toTile.x + 97));
		toSquare.append(std::to_string(toTile.y + 1));

		// Check if ToTile is occupied by Own Piece. It feels straightforward to check it in the Piece's Move function but that's just writing to the same code every time and tiring.
		g_devConsole->AddLine(DevConsole::ERROR, Stringf("Cannot move %s from %s to %s. %s is occupied by own %s!!", fromPieceType.c_str(), fromSquare.c_str(), toSquare.c_str(), toSquare.c_str(), toPieceType.c_str()));

		return ChessMoveResult();
	}
	else
	{
		switch(pieceOnFromTile->m_definition->GetPieceType())
		{
			case ChessPieceType::Knight:	return MoveKnight(fromTile, toTile, movingPlayer);
			case ChessPieceType::Bishop:	return MoveBishop(fromTile, toTile, movingPlayer);
			case ChessPieceType::Rook:		return MoveRook(fromTile, toTile, movingPlayer);
			case ChessPieceType::Pawn:		return MovePawn(fromTile, toTile, movingPlayer, newPiece);
			case ChessPieceType::Queen:		return MoveQueen(fromTile, toTile, movingPlayer);
			case ChessPieceType::King:		return MoveKing(fromTile, toTile, movingPlayer);

			default:
				break;
		}
	}


	return ChessMoveResult();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
ChessMoveResult ChessBoard::MoveKnight(IntVec2 const& fromTile, IntVec2 const& toTile, MatchState movingPlayer)
{
	int taxiCabDistance = GetTaxicabDistance2D(fromTile, toTile);

	std::string fromSquare = std::string(1, static_cast<char>(fromTile.x + 97));
	fromSquare.append(std::to_string(fromTile.y + 1));

	std::string toSquare = std::string(1, static_cast<char>(toTile.x + 97));
	toSquare.append(std::to_string(toTile.y + 1));

	if(taxiCabDistance != 3)
	{
		g_devConsole->AddLine(DevConsole::ERROR, Stringf("A Knight must move exactly 3 steps in an L Shape! Example ChessMove from=b1 to=a3"));
		return ChessMoveResult();
	}
	else
	{
		if(abs(fromTile.x - toTile.x) > 2)
		{
			g_devConsole->AddLine(DevConsole::ERROR, Stringf("A Knight cannot move more than 2 steps vertically! Knight must move in an L Shape! Example ChessMove from=a3 to=c4"));
			return ChessMoveResult();
		}

		if(abs(fromTile.y - toTile.y) > 2)
		{
			g_devConsole->AddLine(DevConsole::ERROR, Stringf("A Knight cannot move more than 2 steps horizontally! Knight must move in an L Shape! Example ChessMove from=b1 to=a3"));
			return ChessMoveResult();
		}

		ChessMoveResult moveResult;
		moveResult.m_isValid = true;

		ChessPiece* pieceOnToTile	= GetPieceOnTile(toTile);
		ChessPiece* pieceOnFromTile = GetPieceOnTile(fromTile);

		if(pieceOnToTile)
		{
			if(pieceOnToTile->m_definition->GetPieceType() == ChessPieceType::King)
			{
				moveResult.m_isKingCaptured = true;
			}

			pieceOnFromTile->Capture(pieceOnToTile);
			moveResult.m_isCapture = true;

			g_devConsole->AddLine(DevConsole::INFO_MAJOR, Stringf("Player %d has captured Player %d's %s!!", pieceOnFromTile->m_owningPlayerID + 1, pieceOnToTile->m_owningPlayerID + 1, pieceOnToTile->GetPieceTypeAsString().c_str()));
		}
		else
		{
			pieceOnFromTile->Occupy(toTile);
			
			g_devConsole->AddLine(Rgba8(2, 220, 100), Stringf("Moved Player %d's Knight from %s to %s", static_cast<int>(movingPlayer) + 1, fromSquare.c_str(), toSquare.c_str()));
		}
		
		return moveResult;
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
ChessMoveResult ChessBoard::MoveBishop(IntVec2 const& fromTile, IntVec2 const& toTile, MatchState movingPlayer)
{
	bool isDiagonal = (abs(fromTile.x - toTile.x) == abs(fromTile.y - toTile.y));

	std::string fromSquare = std::string(1, static_cast<char>(fromTile.x + 97));
	fromSquare.append(std::to_string(fromTile.y + 1));

	std::string toSquare = std::string(1, static_cast<char>(toTile.x + 97));
	toSquare.append(std::to_string(toTile.y + 1));

	ChessPiece* pieceOnFromTile = GetPieceOnTile(fromTile);

	if(!isDiagonal)
	{
		g_devConsole->AddLine(DevConsole::ERROR, Stringf("Bishop can only move diagonally!! Example: ChessMove from=f1 to=d3 "));
		return ChessMoveResult();
	}
	else
	{

		if(!IsDiagonalPathEmpty(fromTile, toTile))
		{
			g_devConsole->AddLine(DevConsole::ERROR, Stringf("Bishop cannot hop! They can move diagonally if the path is empty!! Example: ChessMove from=f1 to=d3 "));
			return ChessMoveResult();
		}

		ChessMoveResult moveResult;
		moveResult.m_isValid = true;

		if(IsTileEmpty(toTile))
		{
			pieceOnFromTile->Occupy(toTile);
			g_devConsole->AddLine(Rgba8(2, 220, 100), Stringf("Moved Player %d's Bishop from %s to %s", static_cast<int>(movingPlayer) + 1, fromSquare.c_str(), toSquare.c_str()));
		}
		else
		{
			moveResult.m_isCapture = true;

			ChessPiece* pieceOnToTile = GetPieceOnTile(toTile);

			g_devConsole->AddLine(DevConsole::INFO_MAJOR, Stringf("Player %d has captured Player %d's %s!!", pieceOnFromTile->m_owningPlayerID + 1, pieceOnToTile->m_owningPlayerID + 1, pieceOnToTile->GetPieceTypeAsString().c_str()));

			if(pieceOnToTile->m_definition->GetPieceType() == ChessPieceType::King)
			{
				moveResult.m_isKingCaptured = true;
			}

			pieceOnFromTile->Capture(pieceOnToTile);
			moveResult.m_isCapture = true;
		}
		return moveResult;
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
ChessMoveResult ChessBoard::MoveRook(IntVec2 const& fromTile, IntVec2 const& toTile, MatchState movingPlayer)
{
	bool moveVertical		= (fromTile.x - toTile.x == 0);
	bool moveHorizontal		= (fromTile.y - toTile.y == 0);

	ChessPiece* pieceOnFromTile = GetPieceOnTile(fromTile);

	std::string fromSquare = std::string(1, static_cast<char>(fromTile.x + 97));
	fromSquare.append(std::to_string(fromTile.y + 1));

	std::string toSquare = std::string(1, static_cast<char>(toTile.x + 97));
	toSquare.append(std::to_string(toTile.y + 1));

	if(!moveHorizontal && !moveVertical)
	{
		g_devConsole->AddLine(DevConsole::ERROR, Stringf("Rooks can only move horizontally or vertically!! Example: ChessMove from=a1 to=a3 or ChessMove from=a1 to=c1"));
		return ChessMoveResult();
	}
	else if(moveHorizontal || moveVertical)
	{
		ChessMoveResult moveResult;
		moveResult.m_isValid = true;

		if(moveHorizontal && !IsHorizontalPathEmpty(fromTile, toTile))
		{
			g_devConsole->AddLine(DevConsole::ERROR, Stringf("Rooks cannot hop! Rooks can only move horizontally or vertically along an empty path!!"));
			g_devConsole->AddLine(DevConsole::ERROR, Stringf("Example: ChessMove from=a1 to=a3 or ChessMove from=a1 to=c1"));
			return ChessMoveResult();
		}
	
		if(moveVertical && !IsVerticalPathEmpty(fromTile, toTile))
		{
			g_devConsole->AddLine(DevConsole::ERROR, Stringf("Rooks cannot hop! Rooks can only move horizontally or vertically along an empty path!!"));
			g_devConsole->AddLine(DevConsole::ERROR, Stringf("Example: ChessMove from=a1 to=a3 or ChessMove from=a1 to=c1"));
			return ChessMoveResult();
		}


		if(IsTileEmpty(toTile))
		{
			pieceOnFromTile->Occupy(toTile);
			g_devConsole->AddLine(Rgba8(2, 220, 100), Stringf("Moved Player %d's Rook from %s to %s", static_cast<int>(movingPlayer) + 1, fromSquare.c_str(), toSquare.c_str()));
		}
		else
		{
			moveResult.m_isCapture = true;

			ChessPiece* pieceOnToTile = GetPieceOnTile(toTile);

			if(pieceOnToTile->m_definition->GetPieceType() == ChessPieceType::King)
			{
				moveResult.m_isKingCaptured = true;
			}

			g_devConsole->AddLine(DevConsole::INFO_MAJOR, Stringf("Player %d has captured Player %d's %s!!", pieceOnFromTile->m_owningPlayerID + 1, pieceOnToTile->m_owningPlayerID + 1, pieceOnToTile->GetPieceTypeAsString().c_str()));

			pieceOnFromTile->Capture(pieceOnToTile);
			moveResult.m_isCapture = true;

		}
		return moveResult;
	}

	return ChessMoveResult();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
ChessMoveResult ChessBoard::MovePawn(IntVec2 const& fromTile, IntVec2 const& toTile, MatchState movingPlayer, std::string newPiece)
{		
	ChessPiece* pieceOnFromTile = GetPieceOnTile(fromTile);

	IntVec2 twoForwardLeftRightDiagonalTiles[4];

	std::string fromSquare = std::string(1, static_cast<char>(fromTile.x + 97));
	fromSquare.append(std::to_string(fromTile.y + 1));

	std::string toSquare = std::string(1, static_cast<char>(toTile.x + 97));
	toSquare.append(std::to_string(toTile.y + 1));

	if(pieceOnFromTile->m_owningPlayerID == 0)
	{
		twoForwardLeftRightDiagonalTiles[0] = IntVec2(fromTile.x,		fromTile.y + 1);
		twoForwardLeftRightDiagonalTiles[1] = IntVec2(fromTile.x,		fromTile.y + 2);
		twoForwardLeftRightDiagonalTiles[2] = IntVec2(fromTile.x - 1,	fromTile.y + 1);
		twoForwardLeftRightDiagonalTiles[3] = IntVec2(fromTile.x + 1,	fromTile.y + 1);
	}
	else if(pieceOnFromTile->m_owningPlayerID == 1)
	{
		twoForwardLeftRightDiagonalTiles[0] = IntVec2(fromTile.x,		fromTile.y - 1);
		twoForwardLeftRightDiagonalTiles[1] = IntVec2(fromTile.x,		fromTile.y - 2);
		twoForwardLeftRightDiagonalTiles[2] = IntVec2(fromTile.x + 1,	fromTile.y - 1);
		twoForwardLeftRightDiagonalTiles[3] = IntVec2(fromTile.x - 1,	fromTile.y - 1);
	}

	bool isDiagonalTile = false;
	bool isValidTile	= false;
//	int	 tileIndex = true;

	for(int index = 0; index < 4; ++index)
	{
		if(twoForwardLeftRightDiagonalTiles[index] == toTile)
		{
			isValidTile = true;
			if(index == 2 || index == 3)
			{
				isDiagonalTile = true;
				//tileIndex = index;
			}
			else if(index == 0 || index == 1)
			{
				isDiagonalTile = false;
			}
			break;
		}
	}

	if(!isValidTile)
	{
		g_devConsole->AddLine(DevConsole::ERROR, Stringf("Pawns can only move one step forward (two if first time moving) or one step forward diagonal to capture!!"));
		g_devConsole->AddLine(DevConsole::ERROR, Stringf("Example: ChessMove from=a2 to=a4 or ChessMove from=a3 to=a4"));
		return ChessMoveResult();
	}


	if(isDiagonalTile && IsTileEmpty(toTile))
	{
		ChessMoveResult result;
		result.m_isValid = true;

		if(CheckEnPassant(twoForwardLeftRightDiagonalTiles[0]))
		{
			if(pieceOnFromTile->m_owningPlayerID == 0 && toTile.y == 7)
			{
				if(newPiece != "q" && newPiece != "n" && newPiece != "b" && newPiece != "r")
				{
					g_devConsole->AddLine(DevConsole::ERROR, Stringf("Invalid Move! Provide pawn promotion information if moving to opposing far rank!"));
					g_devConsole->AddLine(DevConsole::ERROR, Stringf("Example ChessMove from=h7 to=h8 promoteTo=q/n/b/r (Queen, Knight, Bishop, Rook)"));
					return ChessMoveResult();
				}
				
				PromotePawnTo(pieceOnFromTile, newPiece);

				result.m_promotion = true;
			}
			else if(pieceOnFromTile->m_owningPlayerID == 1 && toTile.y == 0)
			{
				if(newPiece != "q" && newPiece != "n" && newPiece != "b" && newPiece != "r")
				{
					g_devConsole->AddLine(DevConsole::ERROR, Stringf("Invalid Move! Provide pawn promotion information if moving to opposing far rank!"));
					g_devConsole->AddLine(DevConsole::ERROR, Stringf("Example ChessMove from=a2 to=a1 promoteTo=q/n/b/r (Queen / Knight / Bishop / Rook)"));

					return ChessMoveResult();
				}

				PromotePawnTo(pieceOnFromTile, newPiece);

				result.m_promotion = true;
			}

			ChessPiece* pieceOnForwardTile = GetPieceOnTile(twoForwardLeftRightDiagonalTiles[0]);

			if(pieceOnForwardTile->m_definition->GetPieceType() == ChessPieceType::King)
			{
				result.m_isKingCaptured = true;
			}

			g_devConsole->AddLine(DevConsole::INFO_MAJOR, Stringf("Player %d has captured Player %d's %s via EnPassant!!", pieceOnFromTile->m_owningPlayerID + 1, pieceOnForwardTile->m_owningPlayerID + 1, pieceOnForwardTile->GetPieceTypeAsString().c_str()));

			pieceOnFromTile->Occupy(toTile);
		
			RemovePiece(pieceOnForwardTile);
			result.m_isCapture = true;

			return result;
		}
		else
		{
			g_devConsole->AddLine(DevConsole::ERROR, Stringf("Pawns can only move one step forward (two if first time moving) or one step forward diagonal to capture!!"));
			g_devConsole->AddLine(DevConsole::ERROR, Stringf("Example: ChessMove from=a2 to=a4 or ChessMove from=a3 to=a4"));
			return ChessMoveResult();
		}
	}
	else if(isDiagonalTile && !IsTileEmpty(toTile))
	{
		ChessMoveResult result;
		result.m_isValid = true;

		ChessPiece* pieceOnToTile = GetPieceOnTile(toTile);
		if(pieceOnToTile->m_definition->GetPieceType() == ChessPieceType::King)
		{
			result.m_isKingCaptured = true;
		}

		if(pieceOnFromTile->m_owningPlayerID == 0 && toTile.y == 7)
		{
			if(newPiece != "q" && newPiece != "n" && newPiece != "b" && newPiece != "r")
			{
				g_devConsole->AddLine(DevConsole::ERROR, Stringf("Invalid Move! Provide pawn promotion information if moving to opposing far rank!"));
				g_devConsole->AddLine(DevConsole::ERROR, Stringf("Example ChessMove from=h7 to=h8 promoteTo=q/n/b/r (Queen, Knight, Bishop, Rook)"));
				return ChessMoveResult();
			}

			PromotePawnTo(pieceOnFromTile, newPiece);

			result.m_promotion = true;
		}
		else if(pieceOnFromTile->m_owningPlayerID == 1 && toTile.y == 0)
		{
			if(newPiece != "q" && newPiece != "n" && newPiece != "b" && newPiece != "r")
			{
				g_devConsole->AddLine(DevConsole::ERROR, Stringf("Invalid Move! Provide pawn promotion information if moving to opposing far rank!"));
				g_devConsole->AddLine(DevConsole::ERROR, Stringf("Example ChessMove from=a2 to=a1 promoteTo=q/n/b/r (Queen / Knight / Bishop / Rook)"));

				return ChessMoveResult();
			}

			PromotePawnTo(pieceOnFromTile, newPiece);

			result.m_promotion = true;
		}

		g_devConsole->AddLine(DevConsole::INFO_MAJOR, Stringf("Player %d has captured Player %d's %s!!", pieceOnFromTile->m_owningPlayerID + 1, pieceOnToTile->m_owningPlayerID + 1, pieceOnToTile->GetPieceTypeAsString().c_str()));

		pieceOnFromTile->Capture(pieceOnToTile);
		result.m_isCapture = true;

		return result;
	}

	if(!isDiagonalTile)
	{
		ChessMoveResult result;
		result.m_isValid = true;

		if(toTile == twoForwardLeftRightDiagonalTiles[1])
		{
			if(pieceOnFromTile->m_turnLastMoved == -1)
			{
				if(!IsTileEmpty(toTile))
				{
					g_devConsole->AddLine(DevConsole::ERROR, Stringf("Pawns can only capture diagonal pieces!"));
					return ChessMoveResult();
				}

				if(pieceOnFromTile->m_owningPlayerID == 0 && toTile.y == 7)
				{
					if(newPiece != "q" && newPiece != "n" && newPiece != "b" && newPiece != "r")
					{
						g_devConsole->AddLine(DevConsole::ERROR, Stringf("Invalid Move! Provide pawn promotion information if moving to opposing far rank!"));
						g_devConsole->AddLine(DevConsole::ERROR, Stringf("Example ChessMove from=h7 to=h8 promoteTo=q/n/b/r (Queen, Knight, Bishop, Rook)"));
						return ChessMoveResult();
					}

					PromotePawnTo(pieceOnFromTile, newPiece);

					result.m_promotion = true;
				}
				else if(pieceOnFromTile->m_owningPlayerID == 1 && toTile.y == 0)
				{
					if(newPiece != "q" && newPiece != "n" && newPiece != "b" && newPiece != "r")
					{
						g_devConsole->AddLine(DevConsole::ERROR, Stringf("Invalid Move! Provide pawn promotion information if moving to opposing far rank!"));
						g_devConsole->AddLine(DevConsole::ERROR, Stringf("Example ChessMove from=a2 to=a1 promoteTo=q/n/b/r (Queen / Knight / Bishop / Rook)"));

						return ChessMoveResult();
					}

					PromotePawnTo(pieceOnFromTile, newPiece);

					result.m_promotion = true;
				}


				pieceOnFromTile->Occupy(toTile);
				g_devConsole->AddLine(Rgba8(2, 220, 100), Stringf("Moved Player %d's Pawn from %s to %s", static_cast<int>(movingPlayer) + 1, fromSquare.c_str(), toSquare.c_str()));
				return result;
			}
			else
			{
				g_devConsole->AddLine(DevConsole::ERROR, Stringf("Pawns can only take one step forward after their first turn!"));
				return ChessMoveResult();
			}
		}
		else if(toTile == twoForwardLeftRightDiagonalTiles[0])
		{
			if(!IsTileEmpty(toTile))
			{
				g_devConsole->AddLine(DevConsole::ERROR, Stringf("Pawns can only capture diagonal pieces!"));
				return ChessMoveResult();
			}

			if(pieceOnFromTile->m_owningPlayerID == 0 && toTile.y == 7)
			{
				if(newPiece != "q" && newPiece != "n" && newPiece != "b" && newPiece != "r")
				{
					g_devConsole->AddLine(DevConsole::ERROR, Stringf("Invalid Move! Provide pawn promotion information if moving to opposing far rank!"));
					g_devConsole->AddLine(DevConsole::ERROR, Stringf("Example ChessMove from=h7 to=h8 promoteTo=q/n/b/r (Queen, Knight, Bishop, Rook)"));
					return ChessMoveResult();
				}

				PromotePawnTo(pieceOnFromTile, newPiece);

				result.m_promotion = true;
			}
			else if(pieceOnFromTile->m_owningPlayerID == 1 && toTile.y == 0)
			{
				if(newPiece != "q" && newPiece != "n" && newPiece != "b" && newPiece != "r")
				{
					g_devConsole->AddLine(DevConsole::ERROR, Stringf("Invalid Move! Provide pawn promotion information if moving to opposing far rank!"));
					g_devConsole->AddLine(DevConsole::ERROR, Stringf("Example ChessMove from=a2 to=a1 promoteTo=q/n/b/r (Queen / Knight / Bishop / Rook)"));

					return ChessMoveResult();
				}

				PromotePawnTo(pieceOnFromTile, newPiece);

				result.m_promotion = true;
			}

			pieceOnFromTile->Occupy(toTile);
			g_devConsole->AddLine(Rgba8(2, 220, 100), Stringf("Moved Player %d's Pawn from %s to %s", static_cast<int>(movingPlayer) + 1, fromSquare.c_str(), toSquare.c_str()));
			
			return result;
		}	
	}

	return ChessMoveResult();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
ChessMoveResult ChessBoard::MoveQueen(IntVec2 const& fromTile, IntVec2 const& toTile, MatchState movingPlayer)
{
	bool isDiagonal = (abs(fromTile.x - toTile.x) == abs(fromTile.y - toTile.y));
	bool moveVertical = (fromTile.x - toTile.x == 0);
	bool moveHorizontal = (fromTile.y - toTile.y == 0);

	std::string fromSquare = std::string(1, static_cast<char>(fromTile.x + 97));
	fromSquare.append(std::to_string(fromTile.y + 1));

	std::string toSquare = std::string(1, static_cast<char>(toTile.x + 97));
	toSquare.append(std::to_string(toTile.y + 1));

	ChessPiece* pieceOnFromTile = GetPieceOnTile(fromTile);

	if(!isDiagonal && !moveHorizontal && !moveVertical)
	{
		g_devConsole->AddLine(DevConsole::ERROR, Stringf("Queen can only move diagonally, vertically or horizontally and cannot hop!!")); 
		g_devConsole->AddLine(DevConsole::ERROR, Stringf("Example: ChessMove from=d1 to=d3 or ChessMove from=d1 to=a4 ")); 
		return ChessMoveResult();
	}
	else if(isDiagonal)
	{
		if(!IsDiagonalPathEmpty(fromTile, toTile))
		{
			g_devConsole->AddLine(DevConsole::ERROR, Stringf("Queen cannot hop! She can move to the target tile if the path is empty!!"));
			return ChessMoveResult();
		}

		ChessMoveResult moveResult;
		moveResult.m_isValid = true;

		if(IsTileEmpty(toTile))
		{
			pieceOnFromTile->Occupy(toTile);
			g_devConsole->AddLine(Rgba8(2, 220, 100), Stringf("Moved Player %d's Queen from %s to %s", static_cast<int>(movingPlayer) + 1, fromSquare.c_str(), toSquare.c_str()));
		}
		else
		{
			moveResult.m_isCapture = true;

			ChessPiece* pieceOnToTile = GetPieceOnTile(toTile);

			g_devConsole->AddLine(DevConsole::INFO_MAJOR, Stringf("Player %d has captured Player %d's %s!!", pieceOnFromTile->m_owningPlayerID + 1, pieceOnToTile->m_owningPlayerID + 1, pieceOnToTile->GetPieceTypeAsString().c_str()));

			if(pieceOnToTile->m_definition->GetPieceType() == ChessPieceType::King)
			{
				moveResult.m_isKingCaptured = true;
			}

			pieceOnFromTile->Capture(pieceOnToTile);
			moveResult.m_isCapture = true;
		}
		return moveResult;
	}
	else if(moveHorizontal || moveVertical)
	{
		ChessMoveResult moveResult;
		moveResult.m_isValid = true;

		if(moveHorizontal && !IsHorizontalPathEmpty(fromTile, toTile))
		{
			g_devConsole->AddLine(DevConsole::ERROR, Stringf("Queen cannot hop! She can move to the target tile if the path is empty!!"));
			return ChessMoveResult();
		}

		if(moveVertical && !IsVerticalPathEmpty(fromTile, toTile))
		{
			g_devConsole->AddLine(DevConsole::ERROR, Stringf("Queen cannot hop! She can move to the target tile if the path is empty!!"));
			return ChessMoveResult();
		}


		if(IsTileEmpty(toTile))
		{
			pieceOnFromTile->Occupy(toTile);
			g_devConsole->AddLine(Rgba8(2, 220, 100), Stringf("Moved Player %d's Queen from %s to %s", static_cast<int>(movingPlayer) + 1, fromSquare.c_str(), toSquare.c_str()));
		}
		else
		{
			moveResult.m_isCapture = true;

			ChessPiece* pieceOnToTile = GetPieceOnTile(toTile);

			if(pieceOnToTile->m_definition->GetPieceType() == ChessPieceType::King)
			{
				moveResult.m_isKingCaptured = true;
			}

			g_devConsole->AddLine(DevConsole::INFO_MAJOR, Stringf("Player %d has captured Player %d's %s!!", pieceOnFromTile->m_owningPlayerID + 1, pieceOnToTile->m_owningPlayerID + 1, pieceOnToTile->GetPieceTypeAsString().c_str()));

			pieceOnFromTile->Capture(pieceOnToTile);
			moveResult.m_isCapture = true;
		}
		return moveResult;
	}
	return ChessMoveResult();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
ChessMoveResult ChessBoard::MoveKing(IntVec2 const& fromTile, IntVec2 const& toTile, MatchState movingPlayer)
{
	// see if this is the to tile is 1 tile long or more
	// if 1, see if adjacted tiles to the to tile are occupied by another king
	// if occupied, fail
	// if not perform, free tile, capture and occupy and other general moves
	// if more, see if it is 2 horizontally
	// if not return
	// if yes, see if there is a rook in that direction
	// if yes, see if he hasn't moved before
	// if moved return
	// if no, see if path is clear
	// if clear, castle

	std::string fromSquare = std::string(1, static_cast<char>(fromTile.x + 97));
	fromSquare.append(std::to_string(fromTile.y + 1));

	std::string toSquare = std::string(1, static_cast<char>(toTile.x + 97));
	toSquare.append(std::to_string(toTile.y + 1));

	ChessPiece* pieceOnFromTile = GetPieceOnTile(fromTile);

	int fromTileToTileHorizontalDistance	= abs(fromTile.x - toTile.x);
	int fromTileToTileVerticalDistance		= abs(fromTile.y - toTile.y);

	if(fromTileToTileVerticalDistance > 1 && fromTileToTileHorizontalDistance > 2)
	{
		g_devConsole->AddLine(DevConsole::ERROR, Stringf("King can only take one step in all possible eight direction!"));
		return ChessMoveResult();
	}

	if(fromTileToTileVerticalDistance <= 1 && fromTileToTileHorizontalDistance <= 1)
	{
		ChessMoveResult moveResult;
		moveResult.m_isValid = true;

		std::vector<IntVec2> eightSurroundingTilesForToTile;
		GetUptoEightSurroundingValidTilesForTile(eightSurroundingTilesForToTile, toTile);

		for(IntVec2 const& tileCoord : eightSurroundingTilesForToTile)
		{
			ChessPiece* piece = GetPieceOnTile(tileCoord);

			if(piece && piece->m_definition->GetPieceType() == ChessPieceType::King && piece->m_owningPlayerID != pieceOnFromTile->m_owningPlayerID)
			{
				g_devConsole->AddLine(DevConsole::ERROR, Stringf("Two kings cannot be adjacent to each other!"));
				return ChessMoveResult();
			}
		}

		if(IsTileEmpty(toTile))
		{
			pieceOnFromTile->Occupy(toTile);
			g_devConsole->AddLine(Rgba8(2, 220, 100), Stringf("Moved Player %d's King from %s to %s", static_cast<int>(movingPlayer) + 1, fromSquare.c_str(), toSquare.c_str()));
		}
		else
		{
			moveResult.m_isCapture = true;

			ChessPiece* pieceOnToTile = GetPieceOnTile(toTile);

			if(pieceOnToTile->m_definition->GetPieceType() == ChessPieceType::King)
			{
				moveResult.m_isKingCaptured = true;
			}

			g_devConsole->AddLine(DevConsole::INFO_MAJOR, Stringf("Player %d has captured Player %d's %s!!", pieceOnFromTile->m_owningPlayerID + 1, pieceOnToTile->m_owningPlayerID + 1, pieceOnToTile->GetPieceTypeAsString().c_str()));

			pieceOnFromTile->Capture(pieceOnToTile);
			moveResult.m_isCapture = true;
		}

		return moveResult;
	}
	else if(fromTileToTileHorizontalDistance == 2 && fromTileToTileVerticalDistance == 0)
	{
		ChessMoveResult moveResult;
		moveResult.m_isValid = true;

		// Castling
		if(pieceOnFromTile->m_turnLastMoved > -1)
		{
			g_devConsole->AddLine(DevConsole::ERROR, Stringf("King can only take one step in all possible eight direction!"));
			return ChessMoveResult();
		}

		int moveDirection = fromTile.x - toTile.x;

		if(moveDirection < 0)
		{
			IntVec2 kingSideRookTile = IntVec2(7, toTile.y);

			ChessPiece* kingsideRookTilePiece = GetPieceOnTile(kingSideRookTile);

			if(kingsideRookTilePiece && IsHorizontalPathEmpty(fromTile, kingSideRookTile))
			{
				if(kingsideRookTilePiece->m_definition->GetPieceType() == ChessPieceType::Rook)
				{
					if(kingsideRookTilePiece->m_turnLastMoved != -1)
					{
						g_devConsole->AddLine(DevConsole::ERROR, Stringf("Kingside rook has already moved. Cannot castle."));
						g_devConsole->AddLine(DevConsole::ERROR, Stringf("Castle Possible when king and rook have not moved before"));
						g_devConsole->AddLine(DevConsole::ERROR, Stringf("and no pieces are in between"));
						return ChessMoveResult();
					}

					IntVec2 rookCastleTile = IntVec2(toTile.x - 1, toTile.y);

					pieceOnFromTile->Occupy(toTile);
					kingsideRookTilePiece->Occupy(rookCastleTile);

					std::string rookSquare = std::string(1, static_cast<char>(kingSideRookTile.x + 97));
					rookSquare.append(std::to_string(kingSideRookTile.y + 1));

					std::string rookSquareAfter = std::string(1, static_cast<char>(rookCastleTile.x + 97));
					rookSquareAfter.append(std::to_string(rookCastleTile.y + 1));

					g_devConsole->AddLine(Rgba8(2, 220, 100), Stringf("Moved Player %d's King from %s to %s and Rook from %s to %s by Castling", static_cast<int>(movingPlayer) + 1, fromSquare.c_str(), toSquare.c_str(), rookSquare.c_str(), rookSquareAfter.c_str()));
					return moveResult;
				}
				else
				{
					g_devConsole->AddLine(DevConsole::ERROR, Stringf("No Rook on Kingside. Cannot castle."));
					g_devConsole->AddLine(DevConsole::ERROR, Stringf("Castle Possible when king and rook have not moved before"));
					g_devConsole->AddLine(DevConsole::ERROR, Stringf("and no pieces are in between"));
					return ChessMoveResult();
				}
			}
			else if(kingsideRookTilePiece && !IsHorizontalPathEmpty(fromTile, kingSideRookTile))
			{
				if(kingsideRookTilePiece->m_definition->GetPieceType() == ChessPieceType::Rook)
				{
					g_devConsole->AddLine(DevConsole::ERROR, Stringf("Pieces between King and Rook. Cannot castle."));
					g_devConsole->AddLine(DevConsole::ERROR, Stringf("Castle Possible when king and rook have not moved before"));
					g_devConsole->AddLine(DevConsole::ERROR, Stringf("and no pieces are in between"));
					return ChessMoveResult();

				}
				else
				{
					g_devConsole->AddLine(DevConsole::ERROR, Stringf("No Rook on Kingside. Cannot castle."));
					g_devConsole->AddLine(DevConsole::ERROR, Stringf("Castle Possible when king and rook have not moved before"));
					g_devConsole->AddLine(DevConsole::ERROR, Stringf("and no pieces are in between"));
					return ChessMoveResult();
				}
			}
			else if(!kingsideRookTilePiece)
			{
				g_devConsole->AddLine(DevConsole::ERROR, Stringf("No Rook on Kingside. Cannot castle."));
				g_devConsole->AddLine(DevConsole::ERROR, Stringf("Castle Possible when king and rook have not moved before"));
				g_devConsole->AddLine(DevConsole::ERROR, Stringf("and no pieces are in between"));
				return ChessMoveResult();
			}
		}
		else if(moveDirection > 0)
		{
			IntVec2 queenSideRookTile = IntVec2(0, toTile.y);

			ChessPiece* queenSideRookTilePiece = GetPieceOnTile(queenSideRookTile);

			if(queenSideRookTilePiece && IsHorizontalPathEmpty(fromTile, queenSideRookTile))
			{
				if(queenSideRookTilePiece->m_definition->GetPieceType() == ChessPieceType::Rook)
				{
					if(queenSideRookTilePiece->m_turnLastMoved != -1)
					{
						g_devConsole->AddLine(DevConsole::ERROR, Stringf("Queenside rook has already moved. Cannot castle."));
						g_devConsole->AddLine(DevConsole::ERROR, Stringf("Castle Possible when king and rook have not moved before"));
						g_devConsole->AddLine(DevConsole::ERROR, Stringf("and no pieces are in between"));
						return ChessMoveResult();
					}

					IntVec2 rookCastleTile = IntVec2(toTile.x + 1, toTile.y);

					pieceOnFromTile->Occupy(toTile);
					queenSideRookTilePiece->Occupy(rookCastleTile);

					std::string rookSquare = std::string(1, static_cast<char>(queenSideRookTile.x + 97));
					rookSquare.append(std::to_string(queenSideRookTile.y + 1));

					std::string rookSquareAfter = std::string(1, static_cast<char>(rookCastleTile.x + 97));
					rookSquareAfter.append(std::to_string(rookCastleTile.y + 1));

					g_devConsole->AddLine(Rgba8(2, 220, 100), Stringf("Moved Player %d's King from %s to %s and Rook from %s to %s by Castling", static_cast<int>(movingPlayer) + 1, fromSquare.c_str(), toSquare.c_str(), rookSquare.c_str(), rookSquareAfter.c_str()));
					return moveResult;
				}
				else
				{
					g_devConsole->AddLine(DevConsole::ERROR, Stringf("No Rook on Queenside. Cannot castle."));
					g_devConsole->AddLine(DevConsole::ERROR, Stringf("Castle Possible when king and rook have not moved before"));
					g_devConsole->AddLine(DevConsole::ERROR, Stringf("and no pieces are in between"));
					return ChessMoveResult();
				}
			}
			else if(queenSideRookTilePiece && !IsHorizontalPathEmpty(fromTile, queenSideRookTile))
			{
				if(queenSideRookTilePiece->m_definition->GetPieceType() == ChessPieceType::Rook)
				{
					g_devConsole->AddLine(DevConsole::ERROR, Stringf("Pieces between King and Rook. Cannot castle."));
					g_devConsole->AddLine(DevConsole::ERROR, Stringf("Castle Possible when king and rook have not moved before"));
					g_devConsole->AddLine(DevConsole::ERROR, Stringf("and no pieces are in between"));
					return ChessMoveResult();

				}
				else
				{
					g_devConsole->AddLine(DevConsole::ERROR, Stringf("No Rook on Queenside. Cannot castle."));
					g_devConsole->AddLine(DevConsole::ERROR, Stringf("Castle Possible when king and rook have not moved before"));
					g_devConsole->AddLine(DevConsole::ERROR, Stringf("and no pieces are in between"));
					return ChessMoveResult();
				}
			}
			else if(!queenSideRookTilePiece)
			{
				g_devConsole->AddLine(DevConsole::ERROR, Stringf("No Rook on Queenside. Cannot castle."));
				g_devConsole->AddLine(DevConsole::ERROR, Stringf("Castle Possible when king and rook have not moved before"));
				g_devConsole->AddLine(DevConsole::ERROR, Stringf("and no pieces are in between"));
				return ChessMoveResult();
			}
		}
	}

	return ChessMoveResult();
}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool ChessBoard::IsDiagonalPathEmpty(IntVec2 const& fromTile, IntVec2 const& toTile)
{
	int xChange = (toTile.x > fromTile.x) ? 1 : -1;
	int yChange = (toTile.y > fromTile.y) ? 1 : -1;

	// checking only if the tiles between from and to are free but not the from and to itself
	int x = fromTile.x + xChange;
	int y = fromTile.y + yChange;

	bool isPathEmpty = true;

	while(toTile != IntVec2(x, y))
	{
		isPathEmpty = IsTileEmpty(IntVec2(x, y));

		if(!isPathEmpty)
		{
			return isPathEmpty;
		}

		x += xChange;
		y += yChange;
	}

	return isPathEmpty;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool ChessBoard::IsHorizontalPathEmpty(IntVec2 const& fromTile, IntVec2 const& toTile)
{
	int xChange = (toTile.x > fromTile.x) ? 1 : -1;

	// checking only if the tiles between from and to are free but not the from and to itself
	int x = fromTile.x + xChange;

	bool isPathEmpty = true;

	while(toTile != IntVec2(x, fromTile.y))
	{
		isPathEmpty = IsTileEmpty(IntVec2(x, fromTile.y));

		if(!isPathEmpty)
		{
			return isPathEmpty;
		}

		x += xChange;
	}

	return isPathEmpty;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool ChessBoard::IsVerticalPathEmpty(IntVec2 const& fromTile, IntVec2 const& toTile)
{
	int yChange = (toTile.y > fromTile.y) ? 1 : -1;

	// checking only if the tiles between from and to are free but not the from and to itself
	int y = fromTile.y + yChange;

	bool isPathEmpty = true;

	while(toTile != IntVec2(fromTile.x, y))
	{
		isPathEmpty = IsTileEmpty(IntVec2(fromTile.x, y));

		if(!isPathEmpty)
		{
			return isPathEmpty;
		}

		y += yChange;
	}

	return isPathEmpty;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool ChessBoard::IsTileEmpty(IntVec2 const& boardTile)
{
	for(ChessPiece* piece : m_allChessPieces)
	{
		if(piece && piece->GetTile() == boardTile)
		{
			return false;
		}
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
ChessPiece* ChessBoard::GetPieceOnTile(IntVec2 const& boardTile)
{
	for(ChessPiece* piece : m_allChessPieces)
	{
		if(piece && piece->GetTile() == boardTile)
		{
			return piece;
		}
	}
	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ChessBoard::RenderBoard() const
{
	std::vector<Light> allLights;

	g_theRenderer->BeginRenderEvent("Board Render");
	g_theRenderer->SetModelConstants();
	g_theRenderer->SetLightConstants(m_directionalLight, allLights, g_game->GetCameraPosition(), m_ambientIntensity);
	g_theRenderer->SetBlendMode(BlendMode::OPAQUE);
	g_theRenderer->SetDepthMode(DepthMode::READ_WRITE_LESS_EQUAL);
	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
	g_theRenderer->SetSamplerMode(SamplerMode::POINT_CLAMP);
	g_theRenderer->SetSamplerMode(SamplerMode::BILINEAR_WRAP, 1);
	g_theRenderer->BindTexture(m_boardTexture);
	g_theRenderer->BindTexture(m_boardNormalMap, 1);
	g_theRenderer->BindShader(m_boardShader);
	g_theRenderer->DrawIndexedVertexBuffer(m_vertexBuffer, m_indexBuffer, s_indexCount);

	DebugRenderBoardTiles();
	
	g_theRenderer->EndRenderEvent("Board Render");
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ChessBoard::RenderPieces() const
{
	for(ChessPiece* piece : m_allChessPieces)
	{
		if(piece)
		{
			piece->Render();
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ChessBoard::InitializeChessPieces()
{
	InitializePawns();
	InitializeRooks();
	InitializeKnights();
	InitializeBishops();
	InitializeQueens();
	InitializeKings();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ChessBoard::InitializePawns()
{
	ChessPieceDefinition* definition = ChessPieceDefinition::GetChessPieceDefinitionForType(ChessPieceType::Pawn);
	
	// PLAYER 1
	Mat44	playerOnePieceTransform = Mat44();
	Vec3	playerOnePawnStartPosition = Vec3(0.5f, 1.5f, 0.25f);

	for(int index = 0; index < NUM_PLAYER_ONE_PAWNS; ++index)
	{
		playerOnePieceTransform.SetTranslation3D(playerOnePawnStartPosition);

		ChessPiece* testPiece = new ChessPiece(definition, this, static_cast<int>(ChessTeam::PLAYER_ONE), playerOnePieceTransform);
		m_allChessPieces.push_back(testPiece);
		m_chessPiecesByTeam[static_cast<int>(ChessTeam::PLAYER_ONE)].push_back(testPiece);

		// set position for next pawn
		playerOnePawnStartPosition.x += 1.f;
	}

	// PLAYER 2
	Mat44	playerTwoPieceTransform		= Mat44();
	Vec3	playerTwoPawnStartPosition	= Vec3(0.5f, 6.5f, 0.25f);

	for(int index = 0; index < NUM_PLAYER_TWO_PAWNS; ++index)
	{
		playerTwoPieceTransform.SetTranslation3D(playerTwoPawnStartPosition);

		ChessPiece* testPiece = new ChessPiece(definition, this, static_cast<int>(ChessTeam::PLAYER_TWO), playerTwoPieceTransform);
		m_allChessPieces.push_back(testPiece);
		m_chessPiecesByTeam[static_cast<int>(ChessTeam::PLAYER_TWO)].push_back(testPiece);

		// set position for next pawn
		playerTwoPawnStartPosition.x += 1.f;
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ChessBoard::InitializeKings()
{
	ChessPieceDefinition* definition = ChessPieceDefinition::GetChessPieceDefinitionForType(ChessPieceType::King);

	// PLAYER 1
	Mat44	playerOnePieceTransform = Mat44();
	Vec3	playerOneKingStartPosition = Vec3(4.5f, 0.5f, 0.25f);

	playerOnePieceTransform.SetTranslation3D(playerOneKingStartPosition);

	ChessPiece* playerOneKing = new ChessPiece(definition, this, static_cast<int>(ChessTeam::PLAYER_ONE), playerOnePieceTransform);
	m_allChessPieces.push_back(playerOneKing);
	m_chessPiecesByTeam[static_cast<int>(ChessTeam::PLAYER_ONE)].push_back(playerOneKing);

	// PLAYER 2
	Mat44	playerTwoPieceTransform = Mat44();
	Vec3	playerTwoKingStartPosition = Vec3(4.5f, 7.5f, 0.25f);

	playerTwoPieceTransform.SetTranslation3D(playerTwoKingStartPosition);

	ChessPiece* playerTwoKing = new ChessPiece(definition, this, static_cast<int>(ChessTeam::PLAYER_TWO), playerTwoPieceTransform);
	m_allChessPieces.push_back(playerTwoKing);
	m_chessPiecesByTeam[static_cast<int>(ChessTeam::PLAYER_TWO)].push_back(playerTwoKing);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ChessBoard::InitializeQueens()
{
	ChessPieceDefinition* definition = ChessPieceDefinition::GetChessPieceDefinitionForType(ChessPieceType::Queen);

	// PLAYER 1
	Mat44	playerOnePieceTransform = Mat44();
	Vec3	playerOneQueenStartPosition = Vec3(3.5f, 0.5f, 0.25f);

	playerOnePieceTransform.SetTranslation3D(playerOneQueenStartPosition);

	ChessPiece* playerOneQueen = new ChessPiece(definition, this, static_cast<int>(ChessTeam::PLAYER_ONE), playerOnePieceTransform);
	m_allChessPieces.push_back(playerOneQueen);
	m_chessPiecesByTeam[static_cast<int>(ChessTeam::PLAYER_ONE)].push_back(playerOneQueen);

	// PLAYER 2
	Mat44	playerTwoPieceTransform = Mat44();
	Vec3	playerTwoQueenStartPosition = Vec3(3.5f, 7.5f, 0.25f);

	playerTwoPieceTransform.SetTranslation3D(playerTwoQueenStartPosition);

	ChessPiece* playerTwoQueen = new ChessPiece(definition, this, static_cast<int>(ChessTeam::PLAYER_TWO), playerTwoPieceTransform);
	m_allChessPieces.push_back(playerTwoQueen);
	m_chessPiecesByTeam[static_cast<int>(ChessTeam::PLAYER_TWO)].push_back(playerTwoQueen);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ChessBoard::InitializeRooks()
{
	ChessPieceDefinition* definition = ChessPieceDefinition::GetChessPieceDefinitionForType(ChessPieceType::Rook);

	// PLAYER 1
	Mat44	playerOnePieceTransform = Mat44();
	Vec3	playerOneRookStartPosition = Vec3(0.5f, 0.5f, 0.25f);

	for(int index = 0; index < NUM_PLAYER_ONE_ROOKS; ++index)
	{
		playerOnePieceTransform.SetTranslation3D(playerOneRookStartPosition);

		ChessPiece* rook = new ChessPiece(definition, this, static_cast<int>(ChessTeam::PLAYER_ONE), playerOnePieceTransform);
		m_allChessPieces.push_back(rook);
		m_chessPiecesByTeam[static_cast<int>(ChessTeam::PLAYER_ONE)].push_back(rook);

		// set position for next piece
		playerOneRookStartPosition.x += 7.f;
	}

	// PLAYER 2
	Mat44	playerTwoPieceTransform = Mat44();
	Vec3	playerTwoRookStartPosition = Vec3(0.5f, 7.5f, 0.25f);

	for(int index = 0; index < NUM_PLAYER_TWO_ROOKS; ++index)
	{
		playerTwoPieceTransform.SetTranslation3D(playerTwoRookStartPosition);

		ChessPiece* rook = new ChessPiece(definition, this, static_cast<int>(ChessTeam::PLAYER_TWO), playerTwoPieceTransform);
		m_allChessPieces.push_back(rook);
		m_chessPiecesByTeam[static_cast<int>(ChessTeam::PLAYER_TWO)].push_back(rook);

		// set position for next piece
		playerTwoRookStartPosition.x += 7.f;
	}

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ChessBoard::InitializeKnights()
{
	ChessPieceDefinition* definition = ChessPieceDefinition::GetChessPieceDefinitionForType(ChessPieceType::Knight);

	// PLAYER 1
	Mat44	playerOnePieceTransform = Mat44();
	Vec3	playerOneKnightStartPosition = Vec3(1.5f, 0.5f, 0.25f);

	for(int index = 0; index < NUM_PLAYER_ONE_KNIGHTS; ++index)
	{
		playerOnePieceTransform.SetTranslation3D(playerOneKnightStartPosition);

		ChessPiece* rook = new ChessPiece(definition, this, static_cast<int>(ChessTeam::PLAYER_ONE), playerOnePieceTransform);
		m_allChessPieces.push_back(rook);
		m_chessPiecesByTeam[static_cast<int>(ChessTeam::PLAYER_ONE)].push_back(rook);

		// set position for next piece
		playerOneKnightStartPosition.x += 5.f;
	}

	// PLAYER 2
	Mat44	playerTwoPieceTransform = Mat44();
	Vec3	playerTwoKnightsStartPosition = Vec3(1.5f, 7.5f, 0.25f);

	for(int index = 0; index < NUM_PLAYER_TWO_KNIGHTS; ++index)
	{
		playerTwoPieceTransform.SetTranslation3D(playerTwoKnightsStartPosition);

		ChessPiece* rook = new ChessPiece(definition, this, static_cast<int>(ChessTeam::PLAYER_TWO), playerTwoPieceTransform);
		m_allChessPieces.push_back(rook);
		m_chessPiecesByTeam[static_cast<int>(ChessTeam::PLAYER_TWO)].push_back(rook);

		// set position for next piece
		playerTwoKnightsStartPosition.x += 5.f;
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ChessBoard::InitializeBishops()
{
	ChessPieceDefinition* definition = ChessPieceDefinition::GetChessPieceDefinitionForType(ChessPieceType::Bishop);

	// PLAYER 1
	Mat44	playerOnePieceTransform = Mat44();
	Vec3	playerOneBishopStartPosition = Vec3(2.5f, 0.5f, 0.25f);

	for(int index = 0; index < NUM_PLAYER_ONE_BISHOPS; ++index)
	{
		playerOnePieceTransform.SetTranslation3D(playerOneBishopStartPosition);

		ChessPiece* rook = new ChessPiece(definition, this, static_cast<int>(ChessTeam::PLAYER_ONE), playerOnePieceTransform);
		m_allChessPieces.push_back(rook);
		m_chessPiecesByTeam[static_cast<int>(ChessTeam::PLAYER_ONE)].push_back(rook);

		// set position for next piece
		playerOneBishopStartPosition.x += 3.f;
	}

	// PLAYER 2
	Mat44	playerTwoPieceTransform			= Mat44();
	Vec3	playerTwoBishopStartPosition	= Vec3(2.5f, 7.5f, 0.25f);

	for(int index = 0; index < NUM_PLAYER_TWO_BISHOPS; ++index)
	{
		playerTwoPieceTransform.SetTranslation3D(playerTwoBishopStartPosition);

		ChessPiece* rook = new ChessPiece(definition, this, static_cast<int>(ChessTeam::PLAYER_TWO), playerTwoPieceTransform);
		m_allChessPieces.push_back(rook);
		m_chessPiecesByTeam[static_cast<int>(ChessTeam::PLAYER_TWO)].push_back(rook);

		// set position for next piece
		playerTwoBishopStartPosition.x += 3.f;
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool ChessBoard::CheckEnPassant(IntVec2 const& enPassantTile)
{
	ChessPiece* enPassantPawn = GetPieceOnTile(enPassantTile);

	if(enPassantPawn == nullptr)
	{
		return false;
	}
	else if(enPassantPawn->m_definition->GetPieceType() != ChessPieceType::Pawn)
	{
		return false;
	}
	else if(abs(enPassantPawn->m_currentBoardTile.y - enPassantPawn->m_boardTileLastTurn.y) != 2)
	{
		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ChessBoard::InitializeBoardBuffers()
{
	std::vector<Vertex_PCUTBN> boardVerts;
	std::vector<unsigned int>  indexes;

	for(int y = 0; y < 8; ++y)
	{
		for(int x = 0; x < 8; ++x)
		{
			AABB3 boardTile;
			boardTile.m_mins = Vec3(static_cast<float>(x),			static_cast<float>(y),			0.f);
			boardTile.m_maxs = Vec3(static_cast<float>(x) + 1.f,	static_cast<float>(y) + 1.f,	0.3f);

			Rgba8 color;

			if((x % 2 == 0 && y % 2 == 0) || (x % 2 != 0 && y % 2 != 0))
			{
				color = Rgba8::BLACK;
			}
			else
			{
				color = Rgba8::WHITE;
			}

			AddVertsForAABB3D(boardVerts, indexes, boardTile, color);
		}
	}

	s_indexCount = static_cast<unsigned int>(indexes.size());

	unsigned int vboSize = sizeof(Vertex_PCUTBN) * static_cast<unsigned int>(boardVerts.size());
	unsigned int iboSize = sizeof(unsigned int) * s_indexCount;

	m_vertexBuffer = g_theRenderer->CreateVertexBuffer(sizeof(Vertex_PCUTBN), sizeof(Vertex_PCUTBN));
	m_indexBuffer = g_theRenderer->CreateIndexBuffer(sizeof(unsigned int), sizeof(unsigned int));

	g_theRenderer->CopyCPUToGPU(boardVerts.data(), vboSize, m_vertexBuffer);
	g_theRenderer->CopyCPUToGPU(indexes.data(), iboSize, m_indexBuffer);

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ChessBoard::RemovePiece(ChessPiece* pieceToRemove)
{	
	for(int index = 0; index < static_cast<int>(ChessTeam::COUNT); ++index)
	{
		for(int pIndex = 0; pIndex < static_cast<int>(m_chessPiecesByTeam[index].size()); ++pIndex)
		{
			if(m_chessPiecesByTeam[index][pIndex] == pieceToRemove)
			{
				m_chessPiecesByTeam[index][pIndex] = nullptr;
				break;
			}
		}
	}

	for(int index = 0; index < static_cast<int>(m_allChessPieces.size()); ++index)
	{
		if(m_allChessPieces[index] == pieceToRemove)
		{
			m_allChessPieces[index] = nullptr;
			break;
		}
	}

	delete pieceToRemove;
	pieceToRemove = nullptr;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
int ChessBoard::GetCurrentTurn() const
{
	return m_owningMatch->m_currentTurn;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ChessBoard::GetUptoEightSurroundingValidTilesForTile(std::vector<IntVec2>& out_eightSurrondingTiles, IntVec2 const& tile)
{
	IntVec2 leftCoord			= IntVec2(tile.x - 1, tile.y);
	IntVec2	rightCoord			= IntVec2(tile.x + 1, tile.y);
	IntVec2	topCoord			= IntVec2(tile.x, tile.y + 1);
	IntVec2	bottomCoord			= IntVec2(tile.x, tile.y - 1);
	IntVec2 topLeftCoord		= IntVec2(tile.x - 1, tile.y + 1);
	IntVec2	topRightCoord		= IntVec2(tile.x + 1, tile.y + 1);
	IntVec2	bottomLeftCoord		= IntVec2(tile.x - 1, tile.y - 1);
	IntVec2	bottomRightCoord	= IntVec2(tile.x + 1, tile.y - 1);

	if((leftCoord.x >= 0 && leftCoord.x <= 7) && (leftCoord.y >= 0 && leftCoord.y <= 7))
	{
		out_eightSurrondingTiles.push_back(leftCoord);
	}

	if(rightCoord.x >= 0 && rightCoord.x <= 7 && rightCoord.y >= 0 && rightCoord.y <= 7)
	{
		out_eightSurrondingTiles.push_back(rightCoord);
	}

	if(topCoord.x >= 0 && topCoord.x <= 7 && topCoord.y >= 0 && topCoord.y <= 7)
	{
		out_eightSurrondingTiles.push_back(topCoord);
	}

	if(bottomCoord.x >= 0 && bottomCoord.x <= 7 && bottomCoord.y >= 0 && bottomCoord.y <= 7)
	{
		out_eightSurrondingTiles.push_back(bottomCoord);
	}

	if(topLeftCoord.x >= 0 && topLeftCoord.x <= 7 && topLeftCoord.y >= 0 && topLeftCoord.y <= 7)
	{
		out_eightSurrondingTiles.push_back(topLeftCoord);
	}

	if(topRightCoord.x >= 0 && topRightCoord.x <= 7 && topRightCoord.y >= 0 && topRightCoord.y <= 7)
	{
		out_eightSurrondingTiles.push_back(topRightCoord);
	}

	if(bottomLeftCoord.x >= 0 && bottomLeftCoord.x <= 7 && bottomLeftCoord.y >= 0 && bottomLeftCoord.y <= 7)
	{
		out_eightSurrondingTiles.push_back(bottomLeftCoord);
	}

	if(bottomRightCoord.x >= 0 && bottomRightCoord.x <= 7 && bottomRightCoord.y >= 0 && bottomRightCoord.y <= 7)
	{
		out_eightSurrondingTiles.push_back(bottomRightCoord);
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ChessBoard::DebugLightControls()
{
	if(g_inputSystem->IsKeyDown(KEYCODE_LEFT_ARROW))
	{
		m_sunDirectionYawPitch.m_yawDegrees -= 0.1f;
		Vec3 sunDirection = Vec3::MakeFromPolarDegrees(m_sunDirectionYawPitch.m_pitchDegrees, m_sunDirectionYawPitch.m_yawDegrees).GetNormalized();
		m_directionalLight.SetDirection(sunDirection);
	}
	else if(g_inputSystem->IsKeyDown(KEYCODE_RIGHT_ARROW))
	{
		m_sunDirectionYawPitch.m_yawDegrees += 0.1f;
		Vec3 sunDirection = Vec3::MakeFromPolarDegrees(m_sunDirectionYawPitch.m_pitchDegrees, m_sunDirectionYawPitch.m_yawDegrees).GetNormalized();
		m_directionalLight.SetDirection(sunDirection);
	}
	else if(g_inputSystem->IsKeyDown(KEYCODE_UP_ARROW))
	{
		m_sunDirectionYawPitch.m_pitchDegrees += 0.1f;
		Vec3 sunDirection = Vec3::MakeFromPolarDegrees(m_sunDirectionYawPitch.m_pitchDegrees, m_sunDirectionYawPitch.m_yawDegrees).GetNormalized();
		m_directionalLight.SetDirection(sunDirection);
	}
	else if(g_inputSystem->IsKeyDown(KEYCODE_DOWN_ARROW))
	{
		m_sunDirectionYawPitch.m_pitchDegrees -= 0.1f;
		Vec3 sunDirection = Vec3::MakeFromPolarDegrees(m_sunDirectionYawPitch.m_pitchDegrees, m_sunDirectionYawPitch.m_yawDegrees).GetNormalized();
		m_directionalLight.SetDirection(sunDirection);
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ChessBoard::DebugRenderBoardTiles() const
{
	for(int y = 0; y < 8; ++y)
	{
		for(int x = 0; x < 8; ++x)
		{
			Vec3 postion = Vec3(static_cast<float>(x) + 0.5f, static_cast<float>(y) + 0.5f, 0.5f);
			Vec3 postionA = Vec3(static_cast<float>(x) + 0.5f, static_cast<float>(y) + 0.5f, 0.6f);

			std::string debugText = Stringf("(%d, %d)", x, y);
			DebugAddWorldBillboardText(debugText, postion, 0.1f, Vec2(0.5f, 0.5f), 0.f, Rgba8::RED, Rgba8::RED);

			std::string debugTextA = Stringf("(%c, %d)", x + 65, y + 1);
			DebugAddWorldBillboardText(debugTextA, postionA, 0.1f, Vec2(0.5f, 0.5f), 0.f, Rgba8::GREEN, Rgba8::GREEN);
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ChessBoard::PromotePawnTo(ChessPiece* pawnPiece, std::string const& newPiece)
{
	ChessPieceType newType = ChessPieceType::NONE;

	if(newPiece == "q")
	{
		newType = ChessPieceType::Queen;
	}
	else if(newPiece == "n")
	{
		newType = ChessPieceType::Knight;
	}
	else if(newPiece == "b")
	{
		newType = ChessPieceType::Bishop;
	}
	else if(newPiece == "r")
	{
		newType = ChessPieceType::Rook;
	}

	ChessPieceDefinition* newDef = nullptr;
	
	for(ChessPieceDefinition* def : ChessPieceDefinition::s_pieceDefinitions)
	{
		if(def->GetPieceType() == newType)
		{
			newDef = def;
			break;
		}
	}

	pawnPiece->m_definition = newDef;

}
