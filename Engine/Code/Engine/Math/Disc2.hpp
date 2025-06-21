#pragma once

#include "Engine/Math/Vec2.hpp"

struct Disc2
{

public:

	Vec2 m_center;
	float m_radius;

public:

	Disc2();
	~Disc2();

	explicit Disc2(Vec2 center, float radius);

	void Translation(Vec2 translation);
	void SetCenter(Vec2 newCenter);

};