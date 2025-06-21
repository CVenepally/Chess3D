#pragma once
#include "Engine/Math/Vec3.hpp"

struct LineSegment3
{
public:

	Vec3 m_start;
	Vec3 m_fwdNormal;
	float m_length;

public:

	LineSegment3();
	~LineSegment3();

	explicit LineSegment3(Vec3 start, Vec3 end);
	explicit LineSegment3(Vec3 start, Vec3 fwdNormal, float length);

	Vec3 GetCenter() const;

	void TranslateLineSegment(Vec3 translation);
	void TranslateEndPoint(Vec3 translation);
	void TranslateStartPoint(Vec3 translation);
	void SetCenter(Vec3 newCenter);

	void RotateAboutCenter(float rotationDeltaDegrees);

};

