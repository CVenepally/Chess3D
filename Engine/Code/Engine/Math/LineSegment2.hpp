#pragma once
#include "Engine/Math/Vec2.hpp"

struct LineSegment2
{
public:

	Vec2 m_start;
	Vec2 m_end;


public:

	LineSegment2();
	~LineSegment2();

	explicit LineSegment2(Vec2 start, Vec2 end);

	void TranslateLineSegment(Vec2 translation);
	void TranslateEndPoint(Vec2 translation);
	void TranslateStartPoint(Vec2 translation);
	void SetCenter(Vec2 newCenter);

	float GetLength();

	void RotateAboutCenter(float rotationDeltaDegrees);

};

