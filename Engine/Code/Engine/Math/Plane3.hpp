#pragma once
#include "Engine/Math/Vec3.hpp"

struct Plane3
{
public:

	Vec3  m_normal;
	float m_distanceFromOrigin;

public:

	float GetAltitudeFromPoint(Vec3 const& point) const;

};