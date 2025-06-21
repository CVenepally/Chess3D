#include "Engine/Math/AABB3.hpp"


//------------------------------------------------------------------------------------------------------------------
AABB3::AABB3(AABB3 const& copyFrom)
{

	m_mins = copyFrom.m_mins;
	m_maxs = copyFrom.m_maxs;

}


//------------------------------------------------------------------------------------------------------------------
AABB3::AABB3(float minX, float minY, float minZ, float maxX, float maxY, float maxZ)
{

	m_mins = Vec3(minX, minY, minZ);
	m_maxs = Vec3(maxX, maxY, maxZ);

}

//------------------------------------------------------------------------------------------------------------------
AABB3::AABB3(Vec3 mins, Vec3 maxs)
	: m_mins(mins)
	, m_maxs(maxs)
{

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void AABB3::GetCornerPoints(std::vector<Vec3>& out_CornerPoints) const
{

	Vec3 pointA = m_mins;
	Vec3 pointB = Vec3(m_mins.x, m_mins.y, m_maxs.z);
	Vec3 pointC = Vec3(m_mins.x, m_maxs.y, m_maxs.z);
	Vec3 pointD = Vec3(m_mins.x, m_maxs.y, m_mins.z);
	
	Vec3 pointE = Vec3(m_maxs.x, m_maxs.y, m_mins.z);
	Vec3 pointF = Vec3(m_maxs.x, m_mins.y, m_mins.z);
	Vec3 pointG = Vec3(m_maxs.x, m_mins.y, m_maxs.z);
	Vec3 pointH = m_maxs;

	out_CornerPoints.push_back(pointA);
	out_CornerPoints.push_back(pointB);
	out_CornerPoints.push_back(pointC);
	out_CornerPoints.push_back(pointD);
	out_CornerPoints.push_back(pointE);
	out_CornerPoints.push_back(pointF);
	out_CornerPoints.push_back(pointG);
	out_CornerPoints.push_back(pointH);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Vec3 AABB3::GetCenter() const
{

	return (m_mins + m_maxs) * 0.5f;

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void AABB3::SetCenter(Vec3 newCenter)
{

	Vec3 centerDifference = newCenter - GetCenter();

	m_mins += centerDifference;
	m_maxs += centerDifference;

}

