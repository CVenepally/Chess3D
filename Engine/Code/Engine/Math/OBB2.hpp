#pragma once

#include "Engine/Math/Vec2.hpp"

enum OBB2Points
{
	BOTTOM_LEFT,
	BOTTOM_RIGHT,
	TOP_RIGHT,
	TOP_LEFT
};

struct OBB2
{

public:

	Vec2		m_center;
	Vec2		m_iBasisNormal;
	Vec2		m_halfDimension;

public:

	OBB2();
	~OBB2();

	explicit OBB2(Vec2 center, Vec2 iBasisNormal, Vec2 halfDimension);

	void			GetCornerPoints(Vec2* out_fourCornerWorldPositions) const;
	Vec2 const		GetLocalPosForWorldPos(Vec2 const& worldPos) const;
	Vec2 const		GetWorldPosForLocalPos(Vec2 const& localPos) const;
	void			RotateAboutCenter(float rotationDeltaDegrees);

};