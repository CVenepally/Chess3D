#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/MathUtils.hpp"



//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
OBB2::OBB2()
{


}



//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
OBB2::~OBB2()
{


}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
OBB2::OBB2(Vec2 center, Vec2 iBasisNormal, Vec2 halfDimensions)
	:m_center(center)
	,m_iBasisNormal(iBasisNormal)
	,m_halfDimension(halfDimensions)
{

	
}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void OBB2::GetCornerPoints(Vec2* out_fourCornerWorldPositions) const
{

	Vec2 jbasisNormal = m_iBasisNormal.GetRotated90Degrees();

	Vec2 bottomLeft		= m_center - (m_iBasisNormal * m_halfDimension.x) - (jbasisNormal * m_halfDimension.y);
	Vec2 bottomRight	= m_center + (m_iBasisNormal * m_halfDimension.x) - (jbasisNormal * m_halfDimension.y);
	Vec2 topRight		= m_center + (m_iBasisNormal * m_halfDimension.x) + (jbasisNormal * m_halfDimension.y);
	Vec2 topLeft		= m_center - (m_iBasisNormal * m_halfDimension.x) + (jbasisNormal * m_halfDimension.y);

	out_fourCornerWorldPositions[BOTTOM_LEFT] = bottomLeft;
	out_fourCornerWorldPositions[BOTTOM_RIGHT] = bottomRight;
	out_fourCornerWorldPositions[TOP_RIGHT] = topRight;
	out_fourCornerWorldPositions[TOP_LEFT] = topLeft;

}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Vec2 const OBB2::GetLocalPosForWorldPos(Vec2 const& worldPos) const
{

	Vec2 distanceVectorCenterToPoint = worldPos - m_center; // CP
	float localPAlongIBasis = DotProduct2D(distanceVectorCenterToPoint, m_iBasisNormal);
	
	Vec2 jBasisNormal = m_iBasisNormal.GetRotated90Degrees();
	float localPAlongJBasis = DotProduct2D(distanceVectorCenterToPoint, jBasisNormal);

	return Vec2(localPAlongIBasis, localPAlongJBasis);


}



//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Vec2 const OBB2::GetWorldPosForLocalPos(Vec2 const& localPos) const
{

	Vec2 iBasisScaled = localPos.x * m_iBasisNormal;
	
	Vec2 jBasisNormal = m_iBasisNormal.GetRotated90Degrees();
	Vec2 jBasisScaled = localPos.y * jBasisNormal;

	Vec2 worldPos = m_center + iBasisScaled + jBasisScaled;

	return worldPos;

}



//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void OBB2::RotateAboutCenter(float rotationDeltaDegrees)
{

	float angle = ATan2Degrees(m_iBasisNormal.y, m_iBasisNormal.x);
	
	float newAngle = angle + rotationDeltaDegrees;

	m_iBasisNormal.x = m_iBasisNormal.x * CosDegrees(newAngle);
	m_iBasisNormal.y = m_iBasisNormal.y * SinDegrees(newAngle);

}
