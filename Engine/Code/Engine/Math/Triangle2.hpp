#pragma once
#include "Engine/Math/Vec2.hpp"

struct Triangle2
{

public:

	Vec2 counterClockwisePointOne;
	Vec2 counterClockwisePointTwo;
	Vec2 counterClockwisePointThree;


public:

	Triangle2();
	~Triangle2();

	explicit Triangle2(Vec2 ccOne, Vec2 ccTwo, Vec2 ccThree);

	void Translate(Vec2 translation);
	void SetCenter(Vec2 newCenter);
	void RotateAboutCenter(float rotationDeltaDegrees);

};