#pragma once

#include "Engine/Math/Vec3.hpp"

struct Sphere
{
public:

	Vec3    m_center;
	float   m_radius;

public:

	Sphere() = default;
	~Sphere();
	
	explicit Sphere(Vec3 center, float radius);

	Vec3 GetCenter() const;
	void SetCenter(Vec3 newCenter);

};