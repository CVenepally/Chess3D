#pragma once
#include "Engine/Math/Vec2.hpp"
#include <vector>

class CubicHermiteCurve2D;

class CubicBezierCurve2D
{
public:

	CubicBezierCurve2D() = default;
	explicit CubicBezierCurve2D(Vec2 startPos, Vec2 guidePos1, Vec2 guidePos2, Vec2 endPos);
	explicit CubicBezierCurve2D(CubicHermiteCurve2D hermite);
	Vec2  EvaluateAtParametric(float parametricZeroToOne) const;
	float GetApproximateLength(int subdivisions = 64) const;
	Vec2  EvaluateAtApproximateDistance(float distanceAlongCurve, int numSubdivisions = 64);

public:

	Vec2 m_startPos;
	Vec2 m_guidePos1;
	Vec2 m_guidePos2;
	Vec2 m_endPos;

};

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
class CubicHermiteCurve2D
{
public:

	CubicHermiteCurve2D() = default;
	explicit CubicHermiteCurve2D(Vec2 start, Vec2 end, Vec2 initialVelocity, Vec2 finalVelocity);
	explicit CubicHermiteCurve2D(CubicBezierCurve2D hermite);
	Vec2  EvaluateAtParametric(float parametricZeroToOne) const;
	float GetApproximateLength(int subdivisions = 64) const;
	Vec2  EvaluateAtApproximateDistance(float distanceAlongCurve, int numSubdivisions = 64);

public:

	Vec2 m_startPoint;
	Vec2 m_endPoint;

	Vec2 m_initialVelocity;
	Vec2 m_finalVelocity;

};

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
class Spline
{
public:
	
	Spline() = default;
	Spline(std::vector<Vec2> positions);

	void GetHermiteCurves(std::vector<CubicHermiteCurve2D>& out_hermiteCurves);

	Vec2  EvaluateAtParametric(float parametricZeroToOne) const;
	float GetApproximateLength(int subdivisions = 64) const;
	Vec2  EvaluateAtApproximateDistance(float distanceAlongCurve, int numSubdivisions = 64);

public:
	
	std::vector<Vec2> m_positions;
	std::vector<Vec2> m_velocities;
	std::vector<CubicHermiteCurve2D> m_hermiteCurves;

};

float ComputeCubicBezier1D(float a, float b, float c, float d, float t);
float ComputeQuinticBezier1D(float a, float b, float c, float d, float e, float f, float t);
