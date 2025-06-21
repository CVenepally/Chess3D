#pragma once
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Cylinder3D.hpp"

#include <string>
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
class ChessPieceDefinition;
class ChessBoard;

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
class ChessPiece
{
	friend class ChessBoard;

private:

	ChessPiece() = default;
	explicit ChessPiece(ChessPieceDefinition* definition, ChessBoard* owningBoard, unsigned int owningPlayerID, Mat44 const& startTransform);
	~ChessPiece();

	void			Update();
	void			Render() const;

	IntVec2			GetTile() const;
	std::string		GetPieceTypeAsString() const;

	void			Capture(ChessPiece* pieceToCapture);
	void			Occupy(IntVec2 boardTile);

public:
	std::string		GetPieceGlyph() const;

private:
	ChessPieceDefinition*			m_definition		= nullptr;
	ChessBoard*						m_owningBoard		= nullptr;
	int								m_owningPlayerID	= -1;
	Mat44							m_transform;
	Rgba8							m_color				= Rgba8::WHITE;

	int								m_turnLastMoved		= -1;
	IntVec2							m_currentBoardTile	= IntVec2::ZERO;
	IntVec2							m_boardTileLastTurn	= IntVec2::ZERO;


	Cylinder3D						m_debugCylinder;
};