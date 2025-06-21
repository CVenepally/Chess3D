#include "Game/ChessPieceDefinition.hpp"
#include "Game/GameCommon.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/XMLUtils.hpp"
#include "Engine/Core/DebugRender.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Vertex_PCUTBN.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/Sphere.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/OBB3.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine/Renderer/IndexBuffer.hpp"
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
std::vector<ChessPieceDefinition*> ChessPieceDefinition::s_pieceDefinitions;

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
ChessPieceDefinition::ChessPieceDefinition(XmlElement const& chessPieceDefElement)
{
	std::string pieceType = ParseXmlAttribute(chessPieceDefElement, "pieceType", "Horse");

 	InitPieceType(pieceType);

	m_chessPieceGlyphs[0] = ParseXmlAttribute(chessPieceDefElement, "playerOneGlyph", ".");
	m_chessPieceGlyphs[1] = ParseXmlAttribute(chessPieceDefElement, "playerTwoGlyph", ".");

	std::string shader = ParseXmlAttribute(chessPieceDefElement, "shader", "Data/Shaders/Default");
	m_pieceShader = g_theRenderer->CreateOrGetShader(shader.c_str(), InputLayoutType::VERTEX_PCUTBN);

	//get texture names
	std::string textureOne = ParseXmlAttribute(chessPieceDefElement, "textureOne", "FunkyBricks");
	std::string textureTwo = ParseXmlAttribute(chessPieceDefElement, "textureTwo", "FunkyBricks");

	// Load Diffuse Textures
	m_diffuseTextures[0] = g_theRenderer->CreateOrGetTextureFromFileNameAndType(textureOne, TextureType::DIFFUSE);
	m_diffuseTextures[1] = g_theRenderer->CreateOrGetTextureFromFileNameAndType(textureTwo, TextureType::DIFFUSE);

	// Load Normal Textures
	m_normalTextures[0] = g_theRenderer->CreateOrGetTextureFromFileNameAndType(textureOne, TextureType::NORMAL);
	m_normalTextures[1] = g_theRenderer->CreateOrGetTextureFromFileNameAndType(textureTwo, TextureType::NORMAL);

	// Load Spec Gloss Emit Textures
	m_sgeTextures[0] = g_theRenderer->CreateOrGetTextureFromFileNameAndType(textureOne, TextureType::SGE);
	m_sgeTextures[1] = g_theRenderer->CreateOrGetTextureFromFileNameAndType(textureTwo, TextureType::SGE);


	PopulatePieceBuffers();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
ChessPieceDefinition::~ChessPieceDefinition()
{
	for(int index = 0; index < 2; ++index)
	{
		delete m_vertexBuffer[index];
		m_vertexBuffer[index] = nullptr;

		delete m_indexBuffer[index];
		m_indexBuffer[index] = nullptr;
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ChessPieceDefinition::PopulatePieceBuffers()
{
	switch(m_pieceType)
	{
		case ChessPieceType::Pawn:
		{
			PopulatePawnBuffer();
			break;
		}
		case ChessPieceType::King:
		{
			PopulateKingBuffer();
			break;
		}
		case ChessPieceType::Queen:
		{
			PopulateQueenBuffer();
			break;
		}
		case ChessPieceType::Rook:
		{
			PopulateRookBuffer();
			break;
		}
		case ChessPieceType::Knight:
		{
			PopulateKnightBuffer();
			break;
		}
		case ChessPieceType::Bishop:
		{
			PopulateBishopBuffer();
			break;
		}
		default:
			break;
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ChessPieceDefinition::PopulatePawnBuffer()
{
	std::vector<Vertex_PCUTBN>	playerOneVerts;
	std::vector<unsigned int>	playerOneIndexes;

	AddVertsForPawn(playerOneVerts, playerOneIndexes);
	m_indexCount[0] = static_cast<unsigned int>(playerOneIndexes.size());

	unsigned int vboSize = sizeof(Vertex_PCUTBN) * static_cast<unsigned int>(playerOneVerts.size());
	unsigned int iboSize = sizeof(unsigned int) * m_indexCount[0];

	m_vertexBuffer[0]	= g_theRenderer->CreateVertexBuffer(sizeof(Vertex_PCUTBN), sizeof(Vertex_PCUTBN));
	m_indexBuffer[0]	= g_theRenderer->CreateIndexBuffer(sizeof(unsigned int), sizeof(unsigned int));

	g_theRenderer->CopyCPUToGPU(playerOneVerts.data(), vboSize, m_vertexBuffer[0]);
	g_theRenderer->CopyCPUToGPU(playerOneIndexes.data(), iboSize, m_indexBuffer[0]);

	std::vector<Vertex_PCUTBN>	playerTwoVerts;
	std::vector<unsigned int>	playerTwoIndexes;

	AddVertsForPawn(playerTwoVerts, playerTwoIndexes);
	m_indexCount[1] = static_cast<unsigned int>(playerTwoIndexes.size());

	vboSize = sizeof(Vertex_PCUTBN) * static_cast<unsigned int>(playerTwoVerts.size());
	iboSize = sizeof(unsigned int) * m_indexCount[1];

	m_vertexBuffer[1]	= g_theRenderer->CreateVertexBuffer(sizeof(Vertex_PCUTBN), sizeof(Vertex_PCUTBN));
	m_indexBuffer[1]	= g_theRenderer->CreateIndexBuffer(sizeof(unsigned int), sizeof(unsigned int));

	g_theRenderer->CopyCPUToGPU(playerTwoVerts.data(),	 vboSize, m_vertexBuffer[1]);
	g_theRenderer->CopyCPUToGPU(playerTwoIndexes.data(), iboSize, m_indexBuffer[1]);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ChessPieceDefinition::PopulateKingBuffer()
{
	std::vector<Vertex_PCUTBN>	playerOneVerts;
	std::vector<unsigned int>	playerOneIndexes;

	AddVertsForKing(playerOneVerts, playerOneIndexes);
	m_indexCount[0] = static_cast<unsigned int>(playerOneIndexes.size());

	unsigned int vboSize = sizeof(Vertex_PCUTBN) * static_cast<unsigned int>(playerOneVerts.size());
	unsigned int iboSize = sizeof(unsigned int) * m_indexCount[0];

	m_vertexBuffer[0] = g_theRenderer->CreateVertexBuffer(sizeof(Vertex_PCUTBN), sizeof(Vertex_PCUTBN));
	m_indexBuffer[0] = g_theRenderer->CreateIndexBuffer(sizeof(unsigned int), sizeof(unsigned int));

	g_theRenderer->CopyCPUToGPU(playerOneVerts.data(), vboSize, m_vertexBuffer[0]);
	g_theRenderer->CopyCPUToGPU(playerOneIndexes.data(), iboSize, m_indexBuffer[0]);

	std::vector<Vertex_PCUTBN>	playerTwoVerts;
	std::vector<unsigned int>	playerTwoIndexes;

	AddVertsForKing(playerTwoVerts, playerTwoIndexes);
	m_indexCount[1] = static_cast<unsigned int>(playerTwoIndexes.size());

	vboSize = sizeof(Vertex_PCUTBN) * static_cast<unsigned int>(playerTwoVerts.size());
	iboSize = sizeof(unsigned int) * m_indexCount[1];

	m_vertexBuffer[1] = g_theRenderer->CreateVertexBuffer(sizeof(Vertex_PCUTBN), sizeof(Vertex_PCUTBN));
	m_indexBuffer[1] = g_theRenderer->CreateIndexBuffer(sizeof(unsigned int), sizeof(unsigned int));

	g_theRenderer->CopyCPUToGPU(playerTwoVerts.data(), vboSize, m_vertexBuffer[1]);
	g_theRenderer->CopyCPUToGPU(playerTwoIndexes.data(), iboSize, m_indexBuffer[1]);

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ChessPieceDefinition::PopulateQueenBuffer()
{
	std::vector<Vertex_PCUTBN>	playerOneVerts;
	std::vector<unsigned int>	playerOneIndexes;

	AddVertsForQueen(playerOneVerts, playerOneIndexes);
	m_indexCount[0] = static_cast<unsigned int>(playerOneIndexes.size());

	unsigned int vboSize = sizeof(Vertex_PCUTBN) * static_cast<unsigned int>(playerOneVerts.size());
	unsigned int iboSize = sizeof(unsigned int) * m_indexCount[0];

	m_vertexBuffer[0] = g_theRenderer->CreateVertexBuffer(sizeof(Vertex_PCUTBN), sizeof(Vertex_PCUTBN));
	m_indexBuffer[0] = g_theRenderer->CreateIndexBuffer(sizeof(unsigned int), sizeof(unsigned int));

	g_theRenderer->CopyCPUToGPU(playerOneVerts.data(), vboSize, m_vertexBuffer[0]);
	g_theRenderer->CopyCPUToGPU(playerOneIndexes.data(), iboSize, m_indexBuffer[0]);

	std::vector<Vertex_PCUTBN>	playerTwoVerts;
	std::vector<unsigned int>	playerTwoIndexes;

	AddVertsForQueen(playerTwoVerts, playerTwoIndexes);
	m_indexCount[1] = static_cast<unsigned int>(playerTwoIndexes.size());

	vboSize = sizeof(Vertex_PCUTBN) * static_cast<unsigned int>(playerTwoVerts.size());
	iboSize = sizeof(unsigned int) * m_indexCount[1];

	m_vertexBuffer[1] = g_theRenderer->CreateVertexBuffer(sizeof(Vertex_PCUTBN), sizeof(Vertex_PCUTBN));
	m_indexBuffer[1] = g_theRenderer->CreateIndexBuffer(sizeof(unsigned int), sizeof(unsigned int));

	g_theRenderer->CopyCPUToGPU(playerTwoVerts.data(), vboSize, m_vertexBuffer[1]);
	g_theRenderer->CopyCPUToGPU(playerTwoIndexes.data(), iboSize, m_indexBuffer[1]);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ChessPieceDefinition::PopulateRookBuffer()
{
	std::vector<Vertex_PCUTBN>	playerOneVerts;
	std::vector<unsigned int>	playerOneIndexes;

	AddVertsForRook(playerOneVerts, playerOneIndexes);
	m_indexCount[0] = static_cast<unsigned int>(playerOneIndexes.size());

	unsigned int vboSize = sizeof(Vertex_PCUTBN) * static_cast<unsigned int>(playerOneVerts.size());
	unsigned int iboSize = sizeof(unsigned int) * m_indexCount[0];

	m_vertexBuffer[0] = g_theRenderer->CreateVertexBuffer(sizeof(Vertex_PCUTBN), sizeof(Vertex_PCUTBN));
	m_indexBuffer[0] = g_theRenderer->CreateIndexBuffer(sizeof(unsigned int), sizeof(unsigned int));

	g_theRenderer->CopyCPUToGPU(playerOneVerts.data(), vboSize, m_vertexBuffer[0]);
	g_theRenderer->CopyCPUToGPU(playerOneIndexes.data(), iboSize, m_indexBuffer[0]);

	std::vector<Vertex_PCUTBN>	playerTwoVerts;
	std::vector<unsigned int>	playerTwoIndexes;

	AddVertsForRook(playerTwoVerts, playerTwoIndexes);
	m_indexCount[1] = static_cast<unsigned int>(playerTwoIndexes.size());

	vboSize = sizeof(Vertex_PCUTBN) * static_cast<unsigned int>(playerTwoVerts.size());
	iboSize = sizeof(unsigned int) * m_indexCount[1];

	m_vertexBuffer[1] = g_theRenderer->CreateVertexBuffer(sizeof(Vertex_PCUTBN), sizeof(Vertex_PCUTBN));
	m_indexBuffer[1] = g_theRenderer->CreateIndexBuffer(sizeof(unsigned int), sizeof(unsigned int));

	g_theRenderer->CopyCPUToGPU(playerTwoVerts.data(), vboSize, m_vertexBuffer[1]);
	g_theRenderer->CopyCPUToGPU(playerTwoIndexes.data(), iboSize, m_indexBuffer[1]);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ChessPieceDefinition::PopulateKnightBuffer()
{
	std::vector<Vertex_PCUTBN>	playerOneVerts;
	std::vector<unsigned int>	playerOneIndexes;

	AddVertsForKnight(playerOneVerts, playerOneIndexes, Vec3::LEFT);
	m_indexCount[0] = static_cast<unsigned int>(playerOneIndexes.size());

	unsigned int vboSize = sizeof(Vertex_PCUTBN) * static_cast<unsigned int>(playerOneVerts.size());
	unsigned int iboSize = sizeof(unsigned int) * m_indexCount[0];

	m_vertexBuffer[0] = g_theRenderer->CreateVertexBuffer(sizeof(Vertex_PCUTBN), sizeof(Vertex_PCUTBN));
	m_indexBuffer[0] = g_theRenderer->CreateIndexBuffer(sizeof(unsigned int), sizeof(unsigned int));

	g_theRenderer->CopyCPUToGPU(playerOneVerts.data(), vboSize, m_vertexBuffer[0]);
	g_theRenderer->CopyCPUToGPU(playerOneIndexes.data(), iboSize, m_indexBuffer[0]);

	std::vector<Vertex_PCUTBN>	playerTwoVerts;
	std::vector<unsigned int>	playerTwoIndexes;

	AddVertsForKnight(playerTwoVerts, playerTwoIndexes, Vec3::RIGHT);
	m_indexCount[1] = static_cast<unsigned int>(playerTwoIndexes.size());

	vboSize = sizeof(Vertex_PCUTBN) * static_cast<unsigned int>(playerTwoVerts.size());
	iboSize = sizeof(unsigned int) * m_indexCount[1];

	m_vertexBuffer[1] = g_theRenderer->CreateVertexBuffer(sizeof(Vertex_PCUTBN), sizeof(Vertex_PCUTBN));
	m_indexBuffer[1] = g_theRenderer->CreateIndexBuffer(sizeof(unsigned int), sizeof(unsigned int));

	g_theRenderer->CopyCPUToGPU(playerTwoVerts.data(), vboSize, m_vertexBuffer[1]);
	g_theRenderer->CopyCPUToGPU(playerTwoIndexes.data(), iboSize, m_indexBuffer[1]);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ChessPieceDefinition::PopulateBishopBuffer()
{
	std::vector<Vertex_PCUTBN>	playerOneVerts;
	std::vector<unsigned int>	playerOneIndexes;

	AddVertsForBishop(playerOneVerts, playerOneIndexes);
	m_indexCount[0] = static_cast<unsigned int>(playerOneIndexes.size());

	unsigned int vboSize = sizeof(Vertex_PCUTBN) * static_cast<unsigned int>(playerOneVerts.size());
	unsigned int iboSize = sizeof(unsigned int) * m_indexCount[0];

	m_vertexBuffer[0] = g_theRenderer->CreateVertexBuffer(sizeof(Vertex_PCUTBN), sizeof(Vertex_PCUTBN));
	m_indexBuffer[0] = g_theRenderer->CreateIndexBuffer(sizeof(unsigned int), sizeof(unsigned int));

	g_theRenderer->CopyCPUToGPU(playerOneVerts.data(), vboSize, m_vertexBuffer[0]);
	g_theRenderer->CopyCPUToGPU(playerOneIndexes.data(), iboSize, m_indexBuffer[0]);

	std::vector<Vertex_PCUTBN>	playerTwoVerts;
	std::vector<unsigned int>	playerTwoIndexes;

	AddVertsForBishop(playerTwoVerts, playerTwoIndexes);
	m_indexCount[1] = static_cast<unsigned int>(playerTwoIndexes.size());

	vboSize = sizeof(Vertex_PCUTBN) * static_cast<unsigned int>(playerTwoVerts.size());
	iboSize = sizeof(unsigned int) * m_indexCount[1];

	m_vertexBuffer[1] = g_theRenderer->CreateVertexBuffer(sizeof(Vertex_PCUTBN), sizeof(Vertex_PCUTBN));
	m_indexBuffer[1] = g_theRenderer->CreateIndexBuffer(sizeof(unsigned int), sizeof(unsigned int));

	g_theRenderer->CopyCPUToGPU(playerTwoVerts.data(), vboSize, m_vertexBuffer[1]);
	g_theRenderer->CopyCPUToGPU(playerTwoIndexes.data(), iboSize, m_indexBuffer[1]);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ChessPieceDefinition::AddVertsForPawn(std::vector<Vertex_PCUTBN>& verts, std::vector<unsigned int>& indexes)
{
	float startZ = 0.f;

	float baseDiscHeight = 0.1f;
	float baseDiscRadius = 0.2f;

	Cylinder3D baseDisc = Cylinder3D(Vec3::ZERO, baseDiscHeight, baseDiscRadius);
	AddVertsForIndexedZCylinder3D(verts, indexes, baseDisc, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 32);
	startZ += baseDiscHeight;

	float secondBaseDiscHeight = 0.05f;
	float secondBaseDiscRadius = 0.15f;

	Cylinder3D secondBase = Cylinder3D(Vec3(0.f, 0.f, startZ), secondBaseDiscHeight, secondBaseDiscRadius);
	AddVertsForIndexedZCylinder3D(verts, indexes, secondBase, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 32);
	startZ += secondBaseDiscHeight;

	float pillarHeight = 0.25f;
	float pillarRadius = 0.1f;

	Cylinder3D pillar = Cylinder3D(Vec3(0.f, 0.f, startZ), pillarHeight, pillarRadius);
	AddVertsForIndexedZCylinder3D(verts, indexes, pillar, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 32);
	startZ += pillarHeight;

	float topBaseOneHeight = 0.05f;
	float topBaseOneRadius = 0.15f;

	Cylinder3D topBaseOne = Cylinder3D(Vec3(0.f, 0.f, startZ), topBaseOneHeight, topBaseOneRadius);
	AddVertsForIndexedZCylinder3D(verts, indexes, topBaseOne, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 32);
	startZ += topBaseOneHeight;

	float topBaseTwoHeight = 0.05f;
	float topBaseTwoRadius = 0.2f;

	Cylinder3D topBaseTwo = Cylinder3D(Vec3(0.f, 0.f, startZ), topBaseTwoHeight, topBaseTwoRadius);
	AddVertsForIndexedZCylinder3D(verts, indexes, topBaseTwo, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 32);

	startZ += 0.18f;
	AddVertsForIndexedSphere3D(verts, indexes, Sphere(Vec3(0.f, 0.f, startZ), 0.18f), Rgba8::WHITE, AABB2::ZERO_TO_ONE, 32, 16);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ChessPieceDefinition::AddVertsForKing(std::vector<Vertex_PCUTBN>& verts, std::vector<unsigned int>& indexes)
{
	float startZ = 0.f;

	float baseDiscHeight = 0.1f;
	float baseDiscRadius = 0.25f;

	Cylinder3D baseDisc = Cylinder3D(Vec3::ZERO, baseDiscHeight, baseDiscRadius);
	AddVertsForIndexedZCylinder3D(verts, indexes, baseDisc, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 32);
	startZ += baseDiscHeight;

	float secondBaseDiscHeight = 0.05f;
	float secondBaseDiscRadius = 0.2f;

	Cylinder3D secondBase = Cylinder3D(Vec3(0.f, 0.f, startZ), secondBaseDiscHeight, secondBaseDiscRadius);
	AddVertsForIndexedZCylinder3D(verts, indexes, secondBase, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 32);
	startZ += secondBaseDiscHeight;

	float pillarHeight = 0.45f;
	float pillarRadius = 0.15f;

	Cylinder3D pillar = Cylinder3D(Vec3(0.f, 0.f, startZ), pillarHeight, pillarRadius);
	AddVertsForIndexedZCylinder3D(verts, indexes, pillar, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 32);
	startZ += pillarHeight;

	float topBaseOneHeight = 0.05f;
	float topBaseOneRadius = 0.2f;

	Cylinder3D topBaseOne = Cylinder3D(Vec3(0.f, 0.f, startZ), topBaseOneHeight, topBaseOneRadius);
	AddVertsForIndexedZCylinder3D(verts, indexes, topBaseOne, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 32);
	startZ += topBaseOneHeight;

	float topBaseTwoHeight = 0.05f;
	float topBaseTwoRadius = 0.25f;

	Cylinder3D topBaseTwo = Cylinder3D(Vec3(0.f, 0.f, startZ), topBaseTwoHeight, topBaseTwoRadius);
	AddVertsForIndexedZCylinder3D(verts, indexes, topBaseTwo, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 32);
	
	startZ += 0.18f; 

	AABB3 vertical = AABB3(Vec3(-0.05f, -0.05f, -0.15f), Vec3(0.05f, 0.05f, 0.15f));
	vertical.SetCenter(Vec3(0.f, 0.f, startZ));

	AABB3 horizontal = AABB3(Vec3(-0.15f, -0.05f, -0.05f), Vec3(0.15f, 0.05f, 0.05f));
	horizontal.SetCenter(Vec3(0.f, 0.f, startZ));
	
	AddVertsForAABB3D(verts, indexes, vertical);
	AddVertsForAABB3D(verts, indexes, horizontal);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ChessPieceDefinition::AddVertsForQueen(std::vector<Vertex_PCUTBN>& verts, std::vector<unsigned int>& indexes)
{
	float startZ = 0.f;

	float baseDiscHeight = 0.1f;
	float baseDiscRadius = 0.25f;

	Cylinder3D baseDisc = Cylinder3D(Vec3::ZERO, baseDiscHeight, baseDiscRadius);
	AddVertsForIndexedZCylinder3D(verts, indexes, baseDisc, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 32);
	startZ += baseDiscHeight;

	float secondBaseDiscHeight = 0.05f;
	float secondBaseDiscRadius = 0.2f;

	Cylinder3D secondBase = Cylinder3D(Vec3(0.f, 0.f, startZ), secondBaseDiscHeight, secondBaseDiscRadius);
	AddVertsForIndexedZCylinder3D(verts, indexes, secondBase, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 32);
	startZ += secondBaseDiscHeight;

	float pillarHeight = 0.54f;
	float pillarRadius = 0.15f;

	Cylinder3D pillar = Cylinder3D(Vec3(0.f, 0.f, startZ), pillarHeight, pillarRadius);
	AddVertsForIndexedZCylinder3D(verts, indexes, pillar, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 32);
	startZ += pillarHeight;

	float topBaseOneHeight = 0.05f;
	float topBaseOneRadius = 0.2f;

	Cylinder3D topBaseOne = Cylinder3D(Vec3(0.f, 0.f, startZ), topBaseOneHeight, topBaseOneRadius);
	AddVertsForIndexedZCylinder3D(verts, indexes, topBaseOne, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 32);
	startZ += topBaseOneHeight;

	float topBaseTwoHeight = 0.05f;
	float topBaseTwoRadius = 0.25f;

	Cylinder3D topBaseTwo = Cylinder3D(Vec3(0.f, 0.f, startZ), topBaseTwoHeight, topBaseTwoRadius);
	AddVertsForIndexedZCylinder3D(verts, indexes, topBaseTwo, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 32);

	float obbCommonCenterZ = startZ + topBaseTwoHeight + 0.03f;

	startZ += 0.18f;
	AddVertsForIndexedSphere3D(verts, indexes, Sphere(Vec3(0.f, 0.f, startZ), 0.15f), Rgba8::WHITE, AABB2::ZERO_TO_ONE, 32, 16);

	Vec3 obbCenter = Vec3(0.f, 0.f, obbCommonCenterZ);
	OBB3 obb = OBB3(obbCenter, Vec3::FORWARD, Vec3::LEFT, Vec3(0.25f, 0.05f, 0.1f));
	AddVertsForIndexedOBB3D(verts, indexes, obb);

	obb.m_iBasis = obb.m_iBasis.GetRotatedAboutZDegrees(45.f);
	obb.m_jBasis = obb.m_jBasis.GetRotatedAboutZDegrees(45.f);
	AddVertsForIndexedOBB3D(verts, indexes, obb);

	obb.m_iBasis = obb.m_iBasis.GetRotatedAboutZDegrees(45.f);
	obb.m_jBasis = obb.m_jBasis.GetRotatedAboutZDegrees(45.f);
	AddVertsForIndexedOBB3D(verts, indexes, obb);

	obb.m_iBasis = obb.m_iBasis.GetRotatedAboutZDegrees(45.f);
	obb.m_jBasis = obb.m_jBasis.GetRotatedAboutZDegrees(45.f);
	AddVertsForIndexedOBB3D(verts, indexes, obb);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ChessPieceDefinition::AddVertsForRook(std::vector<Vertex_PCUTBN>& verts, std::vector<unsigned int>& indexes)
{
	float startZ = 0.f;

	float baseDiscHeight = 0.1f;
	float baseDiscRadius = 0.25f;

	Cylinder3D baseDisc = Cylinder3D(Vec3::ZERO, baseDiscHeight, baseDiscRadius);
	AddVertsForIndexedZCylinder3D(verts, indexes, baseDisc, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 32);
	startZ += baseDiscHeight;

	float secondBaseDiscHeight = 0.05f;
	float secondBaseDiscRadius = 0.2f;

	Cylinder3D secondBase = Cylinder3D(Vec3(0.f, 0.f, startZ), secondBaseDiscHeight, secondBaseDiscRadius);
	AddVertsForIndexedZCylinder3D(verts, indexes, secondBase, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 32);
	startZ += secondBaseDiscHeight;

	float pillarHeight = 0.35f;
	float pillarRadius = 0.15f;

	Cylinder3D pillar = Cylinder3D(Vec3(0.f, 0.f, startZ), pillarHeight, pillarRadius);
	AddVertsForIndexedZCylinder3D(verts, indexes, pillar, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 32);
	startZ += pillarHeight;

	float topBaseOneHeight = 0.05f;
	float topBaseOneRadius = 0.2f;

	Cylinder3D topBaseOne = Cylinder3D(Vec3(0.f, 0.f, startZ), topBaseOneHeight, topBaseOneRadius);
	AddVertsForIndexedZCylinder3D(verts, indexes, topBaseOne, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 32);
	startZ += topBaseOneHeight;

	float topBaseTwoHeight = 0.05f;
	float topBaseTwoRadius = 0.25f;

	Cylinder3D topBaseTwo = Cylinder3D(Vec3(0.f, 0.f, startZ), topBaseTwoHeight, topBaseTwoRadius);
	AddVertsForIndexedZCylinder3D(verts, indexes, topBaseTwo, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 32);

	float obbCommonCenterZ = startZ + topBaseTwoHeight + 0.1f;

	Vec3 obbCenter = Vec3(0.2f, 0.f, obbCommonCenterZ);
	OBB3 obb = OBB3(obbCenter, Vec3::FORWARD, Vec3::LEFT, Vec3(0.03f, 0.07f, 0.1f));
	AddVertsForIndexedOBB3D(verts, indexes, obb);

	obb.m_center = Vec3(-0.2f, 0.f, obbCommonCenterZ);
	AddVertsForIndexedOBB3D(verts, indexes, obb);

	obb.m_center = Vec3(0.f, 0.2f, obbCommonCenterZ);
	obb.m_iBasis = obb.m_iBasis.GetRotatedAboutZDegrees(90.f);
	obb.m_jBasis = obb.m_jBasis.GetRotatedAboutZDegrees(90.f);
	AddVertsForIndexedOBB3D(verts, indexes, obb);

	obb.m_center = Vec3(0.f, -0.2f, obbCommonCenterZ);
	AddVertsForIndexedOBB3D(verts, indexes, obb);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ChessPieceDefinition::AddVertsForKnight(std::vector<Vertex_PCUTBN>& verts, std::vector<unsigned int>& indexes, Vec3 const& fwdVector)
{
	float startZ = 0.f;

	float baseDiscHeight = 0.1f;
	float baseDiscRadius = 0.25f;

	Cylinder3D baseDisc = Cylinder3D(Vec3::ZERO, baseDiscHeight, baseDiscRadius);
	AddVertsForIndexedZCylinder3D(verts, indexes, baseDisc, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 32);
	startZ += baseDiscHeight;

	float secondBaseDiscHeight = 0.05f;
	float secondBaseDiscRadius = 0.2f;

	Cylinder3D secondBase = Cylinder3D(Vec3(0.f, 0.f, startZ), secondBaseDiscHeight, secondBaseDiscRadius);
	AddVertsForIndexedZCylinder3D(verts, indexes, secondBase, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 32);
	startZ += secondBaseDiscHeight;

	float pillarHeight = 0.1f;
	float pillarRadius = 0.15f;

	Cylinder3D pillar = Cylinder3D(Vec3(0.f, 0.f, startZ), pillarHeight, pillarRadius);
	AddVertsForIndexedZCylinder3D(verts, indexes, pillar, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 32);
	startZ += pillarHeight;

	float topBaseOneHeight = 0.05f;
	float topBaseOneRadius = 0.2f;

	Cylinder3D topBaseOne = Cylinder3D(Vec3(0.f, 0.f, startZ), topBaseOneHeight, topBaseOneRadius);
	AddVertsForIndexedZCylinder3D(verts, indexes, topBaseOne, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 32);
	startZ += topBaseOneHeight + 0.25f;

	AABB3 knightBody = AABB3(Vec3(-0.12f, -0.12f, -0.25f), Vec3(0.12f, 0.12f, 0.25f));
	knightBody.SetCenter(Vec3(0.f, 0.f, startZ));
	AddVertsForAABB3D(verts, indexes, knightBody);

	Vec3 knightFaceCenter = (fwdVector * 0.2f) + Vec3(0.f, 0.f, startZ + 0.1f);

	AABB3 knightFace = AABB3(Vec3(-0.12f, -0.15f, -0.075f), Vec3(0.12f, 0.15f, 0.075f));
	knightFace.SetCenter(knightFaceCenter);
	AddVertsForAABB3D(verts, indexes, knightFace);


}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ChessPieceDefinition::AddVertsForBishop(std::vector<Vertex_PCUTBN>& verts, std::vector<unsigned int>& indexes)
{
	float startZ = 0.f;

	float baseDiscHeight = 0.1f;
	float baseDiscRadius = 0.25f;

	Cylinder3D baseDisc = Cylinder3D(Vec3::ZERO, baseDiscHeight, baseDiscRadius);
	AddVertsForIndexedZCylinder3D(verts, indexes, baseDisc, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 32);
	startZ += baseDiscHeight;

	float secondBaseDiscHeight = 0.05f;
	float secondBaseDiscRadius = 0.2f;

	Cylinder3D secondBase = Cylinder3D(Vec3(0.f, 0.f, startZ), secondBaseDiscHeight, secondBaseDiscRadius);
	AddVertsForIndexedZCylinder3D(verts, indexes, secondBase, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 32);
	startZ += secondBaseDiscHeight;

	float pillarHeight = 0.35f;
	float pillarRadius = 0.15f;

	Cylinder3D pillar = Cylinder3D(Vec3(0.f, 0.f, startZ), pillarHeight, pillarRadius);
	AddVertsForIndexedZCylinder3D(verts, indexes, pillar, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 32);
	startZ += pillarHeight;

	float topDiscHeight = 0.05f;
	float topDiscRadius = 0.2f;

	Cylinder3D topBaseTwo = Cylinder3D(Vec3(0.f, 0.f, startZ), topDiscHeight, topDiscRadius);
	AddVertsForIndexedZCylinder3D(verts, indexes, topBaseTwo, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 32);

	startZ += topDiscHeight + 0.1f;

	float headRadius = 0.18f;
	Sphere head = Sphere( Vec3(0.f, 0.f, startZ), headRadius);
	AddVertsForIndexedSphere3D(verts, indexes, head, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 32, 16);

	startZ += headRadius + 0.03f;

	float tipRadius = 0.05f;

	Sphere tip = Sphere(Vec3(0.f, 0.f, startZ), tipRadius);
	AddVertsForIndexedSphere3D(verts, indexes, tip, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 32, 16);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ChessPieceDefinition::InitPieceType(std::string pieceType)
{
	if(pieceType == "Pawn")
	{
		m_pieceType = ChessPieceType::Pawn;
	}
	else if(pieceType == "King")
	{
		m_pieceType = ChessPieceType::King;
	}
	else if(pieceType == "Queen")
	{
		m_pieceType = ChessPieceType::Queen;
	}
	else if(pieceType == "Rook")
	{
		m_pieceType = ChessPieceType::Rook;
	}
	else if(pieceType == "Knight")
	{
		m_pieceType = ChessPieceType::Knight;
	}	
	else if(pieceType == "Bishop")
	{
		m_pieceType = ChessPieceType::Bishop;
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
ChessPieceDefinition* ChessPieceDefinition::GetChessPieceDefinitionForType(ChessPieceType type)
{
	for(ChessPieceDefinition* def : s_pieceDefinitions)
	{
		if(def && def->m_pieceType == type)
		{
			return def;
		}
	}

	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ChessPieceDefinition::InitializeChessPieceDefinition()
{
	XmlDocument pieceDefinition;

	char const* filePath = "Data/Definitions/ChessPieceDefinitions.xml";
	XmlResult result = pieceDefinition.LoadFile(filePath);
	GUARANTEE_OR_DIE(result == tinyxml2::XML_SUCCESS, "Could not open ChessPieceDefinitions.xml");

	XmlElement* rootElement = pieceDefinition.RootElement();
	GUARANTEE_OR_DIE(rootElement, "Failed to access root element. Root should be 'Definitions'");

	XmlElement* pieceDefElement = rootElement->FirstChildElement();
	
	while(pieceDefElement)
	{
		std::string elementName = pieceDefElement->Name();
		GUARANTEE_OR_DIE(elementName == "Definition", "Child Element should be Definition");

		ChessPieceDefinition* newActorDefintition = new ChessPieceDefinition(*pieceDefElement);
		s_pieceDefinitions.push_back(newActorDefintition);

		pieceDefElement = pieceDefElement->NextSiblingElement();
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ChessPieceDefinition::ClearDefinitions()
{
	for(ChessPieceDefinition* def : ChessPieceDefinition::s_pieceDefinitions)
	{
		delete def;
		def = nullptr;
	}
	s_pieceDefinitions.clear();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ChessPieceDefinition::SetPieceType(ChessPieceType newPieceType)
{
	m_pieceType = newPieceType;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
ChessPieceType ChessPieceDefinition::GetPieceType() const
{
	return m_pieceType;
}
