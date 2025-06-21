#pragma once

#include "ThirdParty/tinyXML2/tinyxml2.h"
#include <vector>
#include <string>
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
class VertexBuffer;
class IndexBuffer;
class Shader;
class Texture;

struct Vec3;
struct Vertex_PCUTBN;

typedef tinyxml2::XMLElement XmlElement;

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
enum class ChessPieceType
{
	NONE = -1,

	Pawn,
	King,
	Queen,
	Rook,
	Knight,
	Bishop,

	COUNT
};

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
class ChessPieceDefinition
{
	friend class ChessPiece;

private:

	ChessPieceDefinition() = default;
	explicit ChessPieceDefinition(XmlElement const& chessPieceDefElement);
	~ChessPieceDefinition();

	void										InitPieceType(std::string pieceType);

	void										PopulatePieceBuffers();
	void										PopulatePawnBuffer();
	void										PopulateKingBuffer();
	void										PopulateQueenBuffer();
	void										PopulateRookBuffer();
	void										PopulateKnightBuffer();
	void										PopulateBishopBuffer();

	void										AddVertsForPawn(std::vector<Vertex_PCUTBN>& verts,		std::vector<unsigned int>& indexes);
	void										AddVertsForKing(std::vector<Vertex_PCUTBN>& verts,		std::vector<unsigned int>& indexes);
	void										AddVertsForQueen(std::vector<Vertex_PCUTBN>& verts,		std::vector<unsigned int>& indexes);
	void										AddVertsForRook(std::vector<Vertex_PCUTBN>& verts,		std::vector<unsigned int>& indexes);
	void										AddVertsForKnight(std::vector<Vertex_PCUTBN>& verts,	std::vector<unsigned int>& indexes, Vec3 const& fwdVector);
	void										AddVertsForBishop(std::vector<Vertex_PCUTBN>& verts,	std::vector<unsigned int>& indexes);

public:
	static ChessPieceDefinition*				GetChessPieceDefinitionForType(ChessPieceType type);
	static void									InitializeChessPieceDefinition();
	static void									ClearDefinitions();
	void										SetPieceType(ChessPieceType newPieceType);
	ChessPieceType								GetPieceType() const;

	static std::vector<ChessPieceDefinition*>	s_pieceDefinitions;

private:

	ChessPieceType								m_pieceType			= ChessPieceType::NONE;

	VertexBuffer*								m_vertexBuffer[2]	= {nullptr, nullptr};
	IndexBuffer*								m_indexBuffer[2]	= {nullptr, nullptr};
	unsigned int								m_indexCount[2]		= {};
	std::string									m_chessPieceGlyphs[2];

	Shader*										m_pieceShader			= nullptr;
	Texture*									m_diffuseTextures[2]	= {};
	Texture*									m_normalTextures[2]		= {};
	Texture*									m_sgeTextures[2]		= {};

};