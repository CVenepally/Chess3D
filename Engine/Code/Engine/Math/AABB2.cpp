#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/IntVec2.hpp"

const AABB2 AABB2::ZERO_TO_ONE = AABB2(Vec2(0.f, 0.f), Vec2(1.f, 1.f));

AABB2::AABB2()
{}



AABB2::~AABB2()
{}



AABB2::AABB2(AABB2 const& copyFrom)
	: m_mins(copyFrom.m_mins)
	, m_maxs(copyFrom.m_maxs)
{}



AABB2::AABB2(float minX, float minY, float maxX, float maxY)
{
	
	m_mins = Vec2(minX, minY);
	m_maxs = Vec2(maxX, maxY);

}



AABB2::AABB2(Vec2 const& mins, Vec2 const& maxs)
	: m_mins(mins)
	, m_maxs(maxs)
{}


AABB2::AABB2(IntVec2 const& mins, IntVec2 const& maxs) 
{

	m_mins.x = static_cast<float>(mins.x);
	m_mins.y = static_cast<float>(mins.y);

	m_maxs.x = static_cast<float>(maxs.x);
	m_maxs.y = static_cast<float>(maxs.y);

}



bool AABB2::IsPointInside(Vec2 const& point) const
{
	
	if(point.x < m_mins.x || point.y < m_mins.y || point.x > m_maxs.x || point.y > m_maxs.y)
	{
		return false;
	}

	return true;

}



Vec2 const AABB2::GetCenter() const
{

	return (m_mins + m_maxs) * 0.5f;

}



Vec2 const AABB2::GetDimensions() const
{

	return m_maxs - m_mins;

}



Vec2 const AABB2::GetNearestPoint(Vec2 const& referencePosition) const
{

	float x = GetClamped(referencePosition.x, m_mins.x, m_maxs.x);
	float y = GetClamped(referencePosition.y, m_mins.y, m_maxs.y);

	return Vec2(x, y);

}



Vec2 const AABB2::GetPointAtUV(Vec2 const& uv) const
{

	float xAtUV = RangeMap(uv.x, 0.0f, 1.0f, m_mins.x, m_maxs.x);
	float yAtUV = RangeMap(uv.y, 0.0f, 1.0f, m_mins.y, m_maxs.y);

	return Vec2(xAtUV, yAtUV);

}



Vec2 const AABB2::GetUVForPoint(Vec2 const& point) const
{

	float uvAtX = RangeMap(point.x, m_mins.x, m_maxs.x, 0.0f, 1.0f);
	float uvAtY = RangeMap(point.y, m_mins.y, m_maxs.y, 0.0f, 1.0f);

	return Vec2(uvAtX, uvAtY);

}



void AABB2::Translate(Vec2 const& tranlationToApply)
{

	m_mins += tranlationToApply;
	m_maxs += tranlationToApply;

}



void AABB2::SetCenter(Vec2 const& newCenter)
{

	Vec2 pointDifference = newCenter - GetCenter() ;

	m_mins += pointDifference;
	m_maxs += pointDifference;

}



void AABB2::SetDimensions(Vec2 const& newDimensions)
{

	Vec2 center = GetCenter();

	m_mins = Vec2(center.x - newDimensions.x / 2, center.y - newDimensions.y / 2);
	m_maxs = Vec2(center.x + newDimensions.x / 2, center.y + newDimensions.y / 2);

}



void AABB2::StretchToIncludePoint(Vec2 const& point)
{

	if(IsPointInside(point))
	{
		return;
	}

	if(point.x < m_mins.x)
	{
		m_mins.x = point.x;
	}

	if(point.y < m_mins.y)
	{
		m_mins.y = point.y;
	}

	if(point.x > m_maxs.x)
	{
		m_maxs.x = point.x;
	}

	if(point.y > m_maxs.y)
	{
		m_maxs.y = point.y;
	}

}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void AABB2::GetFourCornerPoints(std::vector<Vec2>& out_fourCornerPoints) const
{

	Vec2 bottomRight = Vec2(m_maxs.x, m_mins.y);
	Vec2 topLeft = Vec2(m_mins.x, m_maxs.y);

	out_fourCornerPoints.push_back(m_mins);
	out_fourCornerPoints.push_back(bottomRight);
	out_fourCornerPoints.push_back(m_maxs);
	out_fourCornerPoints.push_back(topLeft);

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
AABB2 AABB2::GetBoxFromUVs(AABB2 boxUVs) const
{
	AABB2 box;

	box.m_mins.x = Lerp(m_mins.x, m_maxs.x, boxUVs.m_mins.x);
	box.m_maxs.x = Lerp(m_mins.x, m_maxs.x, boxUVs.m_maxs.x);
	box.m_mins.y = Lerp(m_mins.y, m_maxs.y, boxUVs.m_mins.y);
	box.m_maxs.y = Lerp(m_mins.y, m_maxs.y, boxUVs.m_maxs.y);

	return box;
}