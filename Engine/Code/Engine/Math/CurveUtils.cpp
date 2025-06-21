#include "Engine/Math/CurveUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/LineSegment2.hpp"

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
float ComputeCubicBezier1D(float a, float b, float c, float d, float t)
{
    float lerpAB = Lerp(a, b, t);
    float lerpBC = Lerp(b, c, t);
    float lerpCD = Lerp(c, d, t);

    float lerpABBC = Lerp(lerpAB, lerpBC, t);
    float lerpBCCD = Lerp(lerpBC, lerpCD, t);

    float finalLerp = Lerp(lerpABBC, lerpBCCD, t);

    return finalLerp;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
float ComputeQuinticBezier1D(float a, float b, float c, float d, float e, float f, float t)
{
	float lerpAB = Lerp(a, b, t);
	float lerpBC = Lerp(b, c, t);
	float lerpCD = Lerp(c, d, t);
	float lerpDE = Lerp(d, e, t);
	float lerpEF = Lerp(e, f, t);

	float lerpG = Lerp(lerpAB, lerpBC, t);
	float lerpH = Lerp(lerpBC, lerpCD, t);

	float lerpI = Lerp(lerpCD, lerpDE, t);
	float lerpJ = Lerp(lerpDE, lerpEF, t);

	float lerpGH = Lerp(lerpG, lerpH, t);
	float lerpHI = Lerp(lerpH, lerpI, t);
	float lerpIJ = Lerp(lerpI, lerpJ, t);

	float lerpGHHI = Lerp(lerpGH, lerpHI, t);
	float lerpHIIJ = Lerp(lerpHI, lerpIJ, t);

	float finalLerp = Lerp(lerpGHHI, lerpHIIJ, t);

	return finalLerp;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
CubicBezierCurve2D::CubicBezierCurve2D(Vec2 startPos, Vec2 guidePos1, Vec2 guidePos2, Vec2 endPos)
	: m_startPos(startPos)
	, m_guidePos1(guidePos1)
	, m_guidePos2(guidePos2)
	, m_endPos(endPos)
{}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
CubicBezierCurve2D::CubicBezierCurve2D(CubicHermiteCurve2D hermite)
{

	m_startPos = hermite.m_startPoint;
	m_endPos = hermite.m_endPoint;

	m_guidePos1 = m_startPos + (hermite.m_initialVelocity * 0.33334f);
	m_guidePos2 = m_endPos - (hermite.m_finalVelocity * 0.33334f);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Vec2 CubicBezierCurve2D::EvaluateAtParametric(float parametricZeroToOne) const
{
	float eX = Lerp(m_startPos.x, m_guidePos1.x, parametricZeroToOne);
	float eY = Lerp(m_startPos.y, m_guidePos1.y, parametricZeroToOne);

	float fX = Lerp(m_guidePos1.x, m_guidePos2.x, parametricZeroToOne); 
	float fY = Lerp(m_guidePos1.y, m_guidePos2.y, parametricZeroToOne);

	float gX = Lerp(m_guidePos2.x, m_endPos.x, parametricZeroToOne);
	float gY = Lerp(m_guidePos2.y, m_endPos.y, parametricZeroToOne);

	float hX = Lerp(eX, fX, parametricZeroToOne);
	float hY = Lerp(eY, fY, parametricZeroToOne);

	float iX = Lerp(fX, gX, parametricZeroToOne);
	float iY = Lerp(fY, gY, parametricZeroToOne);

	float pX = Lerp(hX, iX, parametricZeroToOne);
	float pY = Lerp(hY, iY, parametricZeroToOne);
	
	return Vec2(pX, pY);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
float CubicBezierCurve2D::GetApproximateLength(int subdivisions) const
{
	float length = 0.f;

	for(int segment = 0; segment < subdivisions; ++segment)
	{
		float currentSegmentStartTime = static_cast<float>(segment) / subdivisions;
		float currentSegmentEndTime = static_cast<float>(segment + 1) / subdivisions;

		Vec2 start = EvaluateAtParametric(currentSegmentStartTime);
		Vec2 end = EvaluateAtParametric(currentSegmentEndTime);

		LineSegment2 lineSegment = LineSegment2(start, end);
		
		length += lineSegment.GetLength();
	}
	return length;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Vec2 CubicBezierCurve2D::EvaluateAtApproximateDistance(float distanceAlongCurve, int numSubdivisions)
{
	Vec2 point;

	for(int segment = 0; segment < numSubdivisions; ++segment)
	{
		float currentSegmentStartTime = static_cast<float>(segment) / numSubdivisions;
		float currentSegmentEndTime = static_cast<float>(segment + 1) / numSubdivisions;

		Vec2 start = EvaluateAtParametric(currentSegmentStartTime);
		Vec2 end =	 EvaluateAtParametric(currentSegmentEndTime);

		float length = (end - start).GetLength();

		if(length >= distanceAlongCurve)
		{
			point = end - start;

			point.SetLength(distanceAlongCurve);
			
			point += start;

			break;
		}

		distanceAlongCurve -= length;
	}
	return point;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
CubicHermiteCurve2D::CubicHermiteCurve2D(Vec2 start, Vec2 end, Vec2 initialVelocity, Vec2 finalVelocity)
	: m_startPoint(start)
	, m_endPoint(end)
	, m_initialVelocity(initialVelocity)
	, m_finalVelocity(finalVelocity)
{}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
CubicHermiteCurve2D::CubicHermiteCurve2D(CubicBezierCurve2D bezier)
{

	m_startPoint = bezier.m_startPos;
	m_endPoint = bezier.m_endPos;

	m_initialVelocity = 3.f * (bezier.m_guidePos1 - bezier.m_startPos);
	m_finalVelocity = 3.f * (bezier.m_endPos - bezier.m_guidePos2);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Vec2 CubicHermiteCurve2D::EvaluateAtParametric(float parametricZeroToOne) const
{
	CubicBezierCurve2D bezierCurve = CubicBezierCurve2D(*this);
	return bezierCurve.EvaluateAtParametric(parametricZeroToOne);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
float CubicHermiteCurve2D::GetApproximateLength(int subdivisions) const
{
	CubicBezierCurve2D bezierCurve = CubicBezierCurve2D(*this);
	return bezierCurve.GetApproximateLength(subdivisions);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Vec2 CubicHermiteCurve2D::EvaluateAtApproximateDistance(float distanceAlongCurve, int numSubdivisions)
{
	CubicBezierCurve2D bezierCurve = CubicBezierCurve2D(*this);
	return bezierCurve.EvaluateAtApproximateDistance(distanceAlongCurve, numSubdivisions);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Spline::Spline(std::vector<Vec2> positions)
{
	for(int index = 0; index < static_cast<int>(positions.size()); ++index)
	{
		m_positions.push_back(positions[index]);

		if(index == 0 || index == static_cast<int>(positions.size()) - 1)
		{
			m_velocities.push_back(Vec2::ZERO);
			continue;
		}

		float length = ((positions[index + 1] - positions[index - 1]) * 0.5f).GetLength();

		Vec2 velocity = positions[index] + ((positions[index + 1] - positions[index - 1]).GetNormalized() * length);

		m_velocities.push_back(velocity);
	}
	GetHermiteCurves(m_hermiteCurves);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Spline::GetHermiteCurves(std::vector<CubicHermiteCurve2D>& out_hermiteCurves)
{
	for(int index = 0; index < static_cast<int>(m_positions.size()) - 1; ++index)
	{
		CubicHermiteCurve2D curve;
		curve.m_startPoint = m_positions[index];
		curve.m_endPoint = m_positions[index + 1];

		curve.m_initialVelocity = m_velocities[index];
		curve.m_finalVelocity = m_velocities[index + 1];
	
		out_hermiteCurves.push_back(curve);
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Vec2 Spline::EvaluateAtParametric(float parametricZeroToOne) const
{
	int index = static_cast<int>(parametricZeroToOne);

	if(index >= static_cast<int>(m_hermiteCurves.size()))
	{
		return m_positions[0];
	}

	return m_hermiteCurves[index].EvaluateAtParametric(parametricZeroToOne - index);

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
float Spline::GetApproximateLength(int subdivisions) const
{

	float totalLength = 0.f;

	for(int index = 0; index < static_cast<int>(m_hermiteCurves.size()); ++index)
	{
		totalLength += m_hermiteCurves[index].GetApproximateLength(subdivisions);
	}

	return totalLength;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Vec2 Spline::EvaluateAtApproximateDistance(float distanceAlongCurve, int numSubdivisions)
{
	Vec2 point;
	float distanceLeft = distanceAlongCurve;

	for(int index = 0; index < static_cast<int>(m_hermiteCurves.size()); ++index)
	{
		float length = m_hermiteCurves[index].GetApproximateLength(numSubdivisions);

		if(distanceLeft > length)
		{
			distanceLeft -=length;
		}
		else
		{
			point = m_hermiteCurves[index].EvaluateAtApproximateDistance(distanceLeft, numSubdivisions);
			break;
		}
	}

	return point;
}