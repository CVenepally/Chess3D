#include "Game/ChessPiece.hpp"
#include "Game/ChessBoard.hpp"
#include "Game/ChessPieceDefinition.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/DebugRender.hpp"

#include "Game/Game.hpp"

#include "Engine/Math/Vec3.hpp"

extern Game* g_game;
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
ChessPiece::ChessPiece(ChessPieceDefinition* definition, ChessBoard* owningBoard, unsigned int owningPlayerID, Mat44 const& startTransform)
	: m_definition(definition)
	, m_owningBoard(owningBoard)
	, m_owningPlayerID(owningPlayerID)
	, m_transform(startTransform)
{
	m_debugCylinder = Cylinder3D(m_transform.GetTranslation3D(), 0.7f, 0.25f);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
ChessPiece::~ChessPiece()
{
	m_definition = nullptr;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ChessPiece::Update()
{

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ChessPiece::Render() const
{
	g_theRenderer->BeginRenderEvent("Piece Render");

	Rgba8 color = m_color;
	
	int bufferIndex = g_game->GetIsPlayerGoingFirst(m_owningPlayerID) ? 0 : 1;

	int specialInt = 0;

	if(m_pieceState == PieceState::HOVERING)
	{
//		color = Rgba8(255, 0, 0, 255);
		specialInt = 1;
		g_theRenderer->SetBlendMode(BlendMode::OPAQUE);
	}
	else if(m_pieceState == PieceState::CHOSEN)
	{
//		color = Rgba8(0, 255, 0, 255);
		specialInt = 2;
		g_theRenderer->SetBlendMode(BlendMode::ALPHA);
	}

	g_theRenderer->SetModelConstants(m_transform, color, specialInt);
	g_theRenderer->SetDepthMode(DepthMode::READ_WRITE_LESS_EQUAL);
	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
	g_theRenderer->SetSamplerMode(SamplerMode::BILINEAR_WRAP, static_cast<int>(TextureType::ALBEDO));
	g_theRenderer->SetSamplerMode(SamplerMode::BILINEAR_WRAP, static_cast<int>(TextureType::NORMAL));
	g_theRenderer->SetSamplerMode(SamplerMode::BILINEAR_WRAP, static_cast<int>(TextureType::ROUGH));
	g_theRenderer->SetSamplerMode(SamplerMode::BILINEAR_WRAP, static_cast<int>(TextureType::METAL));
	g_theRenderer->SetSamplerMode(SamplerMode::POINT_CLAMP,	  static_cast<int>(TextureType::AO));
	g_theRenderer->BindTexture(m_definition->m_diffuseTextures[bufferIndex],	static_cast<int>(TextureType::ALBEDO));
	g_theRenderer->BindTexture(m_definition->m_normalTextures[bufferIndex],		static_cast<int>(TextureType::NORMAL));
	g_theRenderer->BindTexture(m_definition->m_roughTextures[bufferIndex],		static_cast<int>(TextureType::ROUGH));
	g_theRenderer->BindTexture(m_definition->m_metalTextures[bufferIndex],		static_cast<int>(TextureType::METAL));
	g_theRenderer->BindTexture(m_definition->m_aoTextures[bufferIndex],			static_cast<int>(TextureType::AO));
	g_theRenderer->BindShader(m_definition->m_pieceShader);
	g_theRenderer->DrawIndexedVertexBuffer(m_definition->m_vertexBuffer[bufferIndex], m_definition->m_indexBuffer[bufferIndex], m_definition->m_indexCount[bufferIndex]);

	if(g_game->m_isDebugMode)
	{
		DebugAddWorldWireframeCylinder(m_debugCylinder.m_startPosition, m_debugCylinder.m_height, m_debugCylinder.m_radius, 0.f, Rgba8::WHITE, Rgba8::WHITE, DebugRenderMode::USE_DEPTH);
	}

	g_theRenderer->EndRenderEvent("Piece Render");
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
IntVec2 ChessPiece::GetTile() const
{
	Vec3 position = m_transform.GetTranslation3D();
	
	return IntVec2(static_cast<int>(position.x - 0.5f), static_cast<int>(position.y - 0.5f));
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
std::string ChessPiece::GetTileAsChessTile() const
{
	IntVec2 currentTile = GetTile();
	std::string debugTextA = Stringf("%c%d", currentTile.x + 97, currentTile.y + 1);

	return debugTextA;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
std::string ChessPiece::GetPieceTypeAsString() const
{
	std::string pieceType = "none";

	switch(m_definition->m_pieceType)
	{
		case ChessPieceType::Pawn:
		{
			pieceType = "Pawn";
			break;
		}
		case ChessPieceType::King:
		{
			pieceType = "King";
			break;
		}
		case ChessPieceType::Queen:
		{
			pieceType = "Queen";
			break;
		}
		case ChessPieceType::Bishop:
		{
			pieceType = "Bishop";
			break;
		}
		case ChessPieceType::Rook:
		{
			pieceType = "Rook";
			break;
		}
		case ChessPieceType::Knight:
		{
			pieceType = "Knight";
			break;
		}
		default:
			break;
	}

	return pieceType;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
std::string ChessPiece::GetPieceGlyph() const
{
	int index = g_game->GetIsPlayerGoingFirst(m_owningPlayerID) ? 0 : 1;
	return m_definition->m_chessPieceGlyphs[index];
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ChessPiece::Capture(ChessPiece* pieceToCapture)
{
	Occupy(pieceToCapture->GetTile());

	m_owningBoard->RemovePiece(pieceToCapture);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ChessPiece::Occupy(IntVec2 boardTile)
{
	Vec3 position = m_transform.GetTranslation3D();

	m_boardTileLastTurn = IntVec2(static_cast<int>(position.x - 0.5f), static_cast<int>(position.y - 0.5f));

	position = Vec3(static_cast<float>(boardTile.x) + 0.5f, static_cast<float>(boardTile.y) + 0.5f, 0.25f);
	
	m_transform.SetTranslation3D(position);
	m_debugCylinder.m_startPosition = m_transform.GetTranslation3D();

	m_currentBoardTile = boardTile;

	m_turnLastMoved = m_owningBoard->GetCurrentTurn();
}
