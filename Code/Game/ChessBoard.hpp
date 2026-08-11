#pragma once
#include "Game/ChessMatch.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Renderer/Light.hpp"
#include "Engine/Math/RaycastUtils.hpp"
#include "Game/EngineBuildPreferences.hpp"

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
enum class PlayerState
{
	HOLDING_PIECE,
	CHOOSING_PIECE
};

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
struct ChessRaycastResult
{
	RaycastResult3D m_raycastResult;
	ChessPiece*		m_hitChessPiece = nullptr;
	AABB3			m_hitTile;
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

	ChessBoard(ChessMatch* owningMatch, int playerOneID, int playerTwoID);
	~ChessBoard();

	void						Update();
	
	void						Render() const;
	void						RenderBoard() const;
	void						RenderPieces() const;
	void						RenderGhostPiece() const;
	void						RenderHoveredTile() const;
	void						RenderCrosshair() const;

	void						ChooseChessPiece();
	void						MoveChessPiece();

	bool						MovePiece(IntVec2 const& fromTile, IntVec2 const& toTile, int movingPlayerID, bool& out_didKingDie);
	ChessMoveResult				MovePiece(IntVec2 const& fromTile, IntVec2 const& toTile, int movingPlayerID, std::string newPiece = "none");
	ChessMoveResult				MoveKnight(IntVec2 const& fromTile, IntVec2 const& toTile, int movingPlayerID);
	ChessMoveResult				MoveBishop(IntVec2 const& fromTile, IntVec2 const& toTile, int movingPlayerID);
	ChessMoveResult				MoveRook(IntVec2 const& fromTile, IntVec2 const& toTile, int movingPlayerID);
	ChessMoveResult				MovePawn(IntVec2 const& fromTile, IntVec2 const& toTile, int movingPlayerID, std::string newPiece = "none");
	ChessMoveResult				MoveQueen(IntVec2 const& fromTile, IntVec2 const& toTile, int movingPlayerID);
	ChessMoveResult				MoveKing(IntVec2 const& fromTile, IntVec2 const& toTile, int movingPlayerID);

	ChessMoveResult				ValidateMovePiece(IntVec2 const& fromTile, IntVec2 const& toTile, std::string newPiece = "none");
	ChessMoveResult				ValidateMoveKnight(IntVec2 const& fromTile, IntVec2 const& toTile);
	ChessMoveResult				ValidateMoveBishop(IntVec2 const& fromTile, IntVec2 const& toTile);
	ChessMoveResult				ValidateMoveRook(IntVec2 const& fromTile, IntVec2 const& toTile);
	ChessMoveResult				ValidateMovePawn(IntVec2 const& fromTile, IntVec2 const& toTile, std::string newPiece = "none");
	ChessMoveResult				ValidateMoveQueen(IntVec2 const& fromTile, IntVec2 const& toTile);
	ChessMoveResult				ValidateMoveKing(IntVec2 const& fromTile, IntVec2 const& toTile);

	void						PromotePawnTo(ChessPiece* pawnPiece, std::string const& newPiece);
	bool						CheckEnPassant(IntVec2 const& enPassantTile);

	ChessRaycastResult			RaycastVsChessSet();
	ChessRaycastResult			RaycastVsAllPieces();
	ChessRaycastResult			RaycastVsTiles();

	bool						IsDiagonalPathEmpty(IntVec2 const& fromTile, IntVec2 const& toTile);
	bool						IsHorizontalPathEmpty(IntVec2 const& fromTile, IntVec2 const& toTile);
	bool						IsVerticalPathEmpty(IntVec2 const& fromTile, IntVec2 const& toTile);

	void						InitializeBoardBuffers();
	void						InitializeChessPieces(int playerOneID, int playerTwoID);
	void						InitializePawns(int playerOneID, int playerTwoID);
	void						InitializeKings(int playerOneID, int playerTwoID);
	void						InitializeQueens(int playerOneID, int playerTwoID);
	void						InitializeRooks(int playerOneID, int playerTwoID);
	void						InitializeKnights(int playerOneID, int playerTwoID);
	void						InitializeBishops(int playerOneID, int playerTwoID);

	void						DebugLightControls();
	void						DebugRenderBoardTiles() const;

public:

	ChessPiece*					GetPieceOnTile(IntVec2 const& boardTile);
	bool						IsTileEmpty(IntVec2 const& boardTile);
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
	Texture*					m_boardSgeMap		= nullptr;

	Texture*					m_boardRoughTexture	= nullptr;
	Texture*					m_boardMetalTexture	= nullptr;
	Texture*					m_boardAOTexture	= nullptr;


	Shader*						m_boardShader		= nullptr;

	// Light
	Light						m_sunLight; 
	std::vector<Light>			m_allLights;

	float						m_ambientIntensity = 0.3f;

	std::vector<AABB3>			m_boardTiles;

	PlayerState					m_playerState = PlayerState::CHOOSING_PIECE;

	ChessPiece*					m_heldPiece = nullptr;
	ChessPiece*					m_hoveredPiece = nullptr;
	AABB3						m_hoveringTile = AABB3();
	bool						m_renderGhostPiece = false;
// 	IntVec2						m_destinationTile = IntVec2(-1, -1);
};