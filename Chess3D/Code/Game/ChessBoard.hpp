#pragma once
#include "Game/ChessMatch.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Renderer/Light.hpp"
#include "Engine/Math/RaycastUtils.hpp"

#include <vector>

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
class ChessPiece;
class VertexBuffer;
class IndexBuffer;
class Texture;
class Shader;

struct IntVec2;

typedef std::vector<ChessPiece*> ChessPieces;

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
enum class ChessTeam
{
	NONE = -1,

	PLAYER_ONE,
	PLAYER_TWO,

	COUNT
};

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
struct ChessMoveResult
{
	bool m_isValid			= false;
	bool m_isCastle			= false;
	bool m_isEnPassant		= false;
	bool m_promotion		= false;
	bool m_isCapture		= false;
	bool m_isKingCaptured	= false;
};

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
class ChessBoard
{
	friend class ChessMatch;

private:

	ChessBoard(ChessMatch* owningMatch);
	~ChessBoard();

	void Update();
	void Render() const;
														
	bool						MovePiece(IntVec2 const& fromTile,	IntVec2 const& toTile, MatchState movingPlayer, bool& out_didKingDie, std::string newPiece = "none");

	ChessMoveResult				MovePiece(IntVec2 const& fromTile,	IntVec2 const& toTile, MatchState movingPlayer, std::string newPiece = "none");
	ChessMoveResult				MoveKnight(IntVec2 const& fromTile,	IntVec2 const& toTile, MatchState movingPlayer);
	ChessMoveResult				MoveBishop(IntVec2 const& fromTile,	IntVec2 const& toTile, MatchState movingPlayer);
	ChessMoveResult				MoveRook(IntVec2 const& fromTile,	IntVec2 const& toTile, MatchState movingPlayer);
	ChessMoveResult				MovePawn(IntVec2 const& fromTile,	IntVec2 const& toTile, MatchState movingPlayer, std::string newPiece = "none");
	ChessMoveResult				MoveQueen(IntVec2 const& fromTile,	IntVec2 const& toTile, MatchState movingPlayer);
	ChessMoveResult				MoveKing(IntVec2 const& fromTile,	IntVec2 const& toTile, MatchState movingPlayer);

	bool						IsDiagonalPathEmpty(IntVec2 const& fromTile, IntVec2 const& toTile);

	/// Checks if tiles between from and to tiles are empty in a horizontal row. Does not check if From and To are empty.
	/// @param fromTile Start Tile
	/// @param toTile Target Tile
	/// @return true if the path is empty
	bool						IsHorizontalPathEmpty(IntVec2 const& fromTile, IntVec2 const& toTile);
	bool						IsVerticalPathEmpty(IntVec2 const& fromTile, IntVec2 const& toTile);

	void						RenderBoard() const;
	void						RenderPieces() const;

	void						InitializeBoardBuffers();
	void						InitializeChessPieces();
	void						InitializePawns();
	void						InitializeKings();
	void						InitializeQueens();
	void						InitializeRooks();
	void						InitializeKnights();
	void						InitializeBishops();

	bool						CheckEnPassant(IntVec2 const& enPassantTile);

	void						DebugLightControls();
	void						DebugRenderBoardTiles() const;

	void						PromotePawnTo(ChessPiece* pawnPiece, std::string const& newPiece);

public:

	bool						IsTileEmpty(IntVec2 const& boardTile);
	ChessPiece*					GetPieceOnTile(IntVec2 const& boardTile);
	void						RemovePiece(ChessPiece* pieceToRemove);
	int							GetCurrentTurn() const;

	void						GetUptoEightSurroundingValidTilesForTile(std::vector<IntVec2>& out_eightSurrondingTiles, IntVec2 const& tile);

private:

	ChessMatch*					m_owningMatch = nullptr;

	ChessPieces					m_allChessPieces;
	ChessPieces					m_chessPiecesByTeam[static_cast<int>(ChessTeam::COUNT)];

	std::string					m_pieceGlyphsOnBoard[64];

	//Rendering
	VertexBuffer*				m_vertexBuffer	= nullptr;
	IndexBuffer*				m_indexBuffer	= nullptr;
	
	Texture*					m_boardTexture		= nullptr;
	Texture*					m_boardNormalMap	= nullptr;
	Texture*					m_boardSgeMap	= nullptr;
	Shader*						m_boardShader		= nullptr;

	EulerAngles					m_sunDirectionYawPitch = EulerAngles(0.f, 0.f, 0.f);
	Light						m_directionalLight; 
	float						m_ambientIntensity = 0.1f;
};