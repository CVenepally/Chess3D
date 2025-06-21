#include "Engine/Math/Cylinder3D.hpp"

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Cylinder3D::Cylinder3D(Vec3 startPos, float height, float radius)
	: m_startPosition(startPos)
	, m_height(height)
	, m_radius(radius)
{

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Cylinder3D::~Cylinder3D()
{

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Vec3 Cylinder3D::GetCenter() const
{
	return Vec3(m_startPosition.x, m_startPosition.y, m_startPosition.z + (m_height * 0.5f));
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Cylinder3D::SetCenter(Vec3 newCenter)
{

	m_startPosition = Vec3(newCenter.x, newCenter.y, newCenter.z - (m_height * 0.5f));

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Vec3 Cylinder3D::GetEndPosition() const
{
	return Vec3(m_startPosition.x, m_startPosition.y, m_startPosition.z + m_height);
}
