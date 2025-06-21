#pragma once

#include "Engine/Math/Vec3.hpp"

struct Cylinder3D
{
public:

	Vec3 m_startPosition;
	float m_height;
	float m_radius;

public:

	Cylinder3D() = default;
	~Cylinder3D();

	explicit Cylinder3D(Vec3 startPos, float height, float radius);

	Vec3 GetCenter() const;
	void SetCenter(Vec3 newCenter);

	Vec3 GetEndPosition() const;

};