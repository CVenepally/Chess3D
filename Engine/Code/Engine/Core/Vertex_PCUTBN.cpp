#include "Engine/Core/Vertex_PCUTBN.hpp"

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Vertex_PCUTBN::~Vertex_PCUTBN()
{
	
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Vertex_PCUTBN::Vertex_PCUTBN(Vec3 const& position, Rgba8 const& color, Vec2 const& uv, Vec3 const& tangent, Vec3 const& biTangent, Vec3 const& normal)
	: m_position(position)
	, m_color(color)
	, m_uvCoords(uv)
	, m_tangent(tangent)
	, m_biTangent(biTangent)
	, m_normal(normal)
{}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Vertex_PCUTBN::Vertex_PCUTBN(float posX, float posY, float posZ, unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha, float uCoord, float vCoord, float tanX, float tanY, float tanZ, float biTanX, float biTanY, float biTanZ, float normalX, float normalY, float normalZ)
	: m_position(Vec3(posX, posY, posZ))
	, m_color(Rgba8(red, green, blue, alpha))
	, m_uvCoords(Vec2(uCoord, vCoord))
	, m_tangent(Vec3(tanX, tanY, tanZ))
	, m_biTangent(Vec3(biTanX, biTanY, biTanZ))
	, m_normal(Vec3(normalX, normalY, normalZ))
{}

