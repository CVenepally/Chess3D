#include "Engine/Math/OBB3.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Mat44.hpp"

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
OBB3::OBB3(Vec3 const& center, Vec3 const& iBasis, Vec3 const& jBasis, Vec3 const& halfDimensions)
	: m_center(center)
	, m_iBasis(iBasis)
	, m_jBasis(jBasis)
	, m_halfDimensions(halfDimensions)
{
	m_kBasis = CrossProduct3D(m_iBasis, m_jBasis);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void OBB3::GetEightCornerPoints(std::vector<Vec3>& out_eightCornerPoints) const
{

	Vec3 bottomCenter = m_center - (m_halfDimensions.z * m_kBasis);

	Vec3 bottomLeftBack   = bottomCenter - (m_halfDimensions.x * m_iBasis) + (m_halfDimensions.y * m_jBasis);
	Vec3 bottomRightBack  = bottomCenter - (m_halfDimensions.x * m_iBasis) - (m_halfDimensions.y * m_jBasis);
	Vec3 bottomLeftFront  = bottomCenter + (m_halfDimensions.x * m_iBasis) + (m_halfDimensions.y * m_jBasis);
	Vec3 bottomRightFront = bottomCenter + (m_halfDimensions.x * m_iBasis) - (m_halfDimensions.y * m_jBasis);

	Vec3 topCenter = m_center + (m_halfDimensions.z * m_kBasis);
	
	Vec3 topLeftBack   = topCenter - (m_halfDimensions.x * m_iBasis) + (m_halfDimensions.y * m_jBasis);
	Vec3 topRightBack  = topCenter - (m_halfDimensions.x * m_iBasis) - (m_halfDimensions.y * m_jBasis);
	Vec3 topLeftFront  = topCenter + (m_halfDimensions.x * m_iBasis) + (m_halfDimensions.y * m_jBasis);
	Vec3 topRightFront = topCenter + (m_halfDimensions.x * m_iBasis) - (m_halfDimensions.y * m_jBasis);

	out_eightCornerPoints.push_back(bottomLeftBack);
	out_eightCornerPoints.push_back(bottomRightBack);
	out_eightCornerPoints.push_back(topRightBack);
	out_eightCornerPoints.push_back(topLeftBack);
	out_eightCornerPoints.push_back(bottomRightFront);
	out_eightCornerPoints.push_back(bottomLeftFront);
	out_eightCornerPoints.push_back(topLeftFront);
	out_eightCornerPoints.push_back(topRightFront);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Vec3 OBB3::GetLocalPositionForWorldPosition(Vec3 const& worldPosition) const
{
	Vec3 centerToWorldPos = worldPosition - m_center;

	float worldPosIAlongBoxIBasis = DotProduct3D(m_iBasis, centerToWorldPos);
	float worldPosJAlongBoxJBasis = DotProduct3D(m_jBasis, centerToWorldPos);
	float worldPosKAlongBoxKBasis = DotProduct3D(m_kBasis, centerToWorldPos);

	return Vec3(worldPosIAlongBoxIBasis, worldPosJAlongBoxJBasis, worldPosKAlongBoxKBasis);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Vec3 OBB3::GetWorldPositionForLocalPosition(Vec3 const& localPosition) const
{
	Vec3 iBasisScaled = localPosition.x * m_iBasis;
	Vec3 jBasisScaled = localPosition.y * m_jBasis;
	Vec3 kBasisScaled = localPosition.z * m_kBasis;

	Vec3 worldPos = m_center + iBasisScaled + jBasisScaled + kBasisScaled;

	return worldPos;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Mat44 OBB3::GetLocalToWorldTransform() const
{
	Mat44 localToWorldTransform;
	localToWorldTransform.SetIJKT3D(m_iBasis, m_jBasis, m_kBasis, m_center);
	return localToWorldTransform;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Mat44 OBB3::GetWorldToLocalTransform() const
{
	Mat44 worldToLocalTransform = GetLocalToWorldTransform();
	return worldToLocalTransform.GetOrthonormalInverse();
}
