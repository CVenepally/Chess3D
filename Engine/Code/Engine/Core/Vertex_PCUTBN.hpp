#pragma once

#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Rgba8.hpp"

struct Vertex_PCUTBN
{
public:

	Vertex_PCUTBN() = default;
	~Vertex_PCUTBN();

	Vertex_PCUTBN(Vec3 const& position, Rgba8 const& color, Vec2 const& uv, Vec3 const& tangent, Vec3 const& biTangent, Vec3 const& normal);
	Vertex_PCUTBN(float posX, float posY, float posZ, unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha, float uCoord, float vCoord, float tanX, float tanY, float tanZ,
				  float biTanX, float biTanY, float biTanZ, float normalX, float normalY, float normalZ);

public:

	Vec3	m_position;
	Rgba8	m_color;
	Vec2	m_uvCoords;
	Vec3	m_tangent;
	Vec3	m_biTangent;
	Vec3	m_normal;

};