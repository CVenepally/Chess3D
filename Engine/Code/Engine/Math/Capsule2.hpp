#pragma once

#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/LineSegment2.hpp"

struct Capsule2 
{

public:

	Vec2 m_start;
	Vec2 m_end;
	float m_radius;

public:

	Capsule2();
	~Capsule2();

	explicit Capsule2(Vec2 start, Vec2 end, float radius);
	explicit Capsule2(LineSegment2 lineSegment, float radius);

	void Translate(Vec2 translation);
	void SetCenter(Vec2 newCenter);

	void RotateAboutCenter(float rotationDeltaDegrees);

};


