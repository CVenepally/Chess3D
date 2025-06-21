#pragma once
#include "Engine/Math/Vec2.hpp"
#include <vector>

struct IntVec2;

enum class FourCornerPoints
{
	BOTTOM_LEFT,
	BOTTOM_RIGHT,
	TOP_RIGHT,
	TOP_LEFT
};

struct AABB2
{

public:

	Vec2 m_mins;
	Vec2 m_maxs;

	static const AABB2 ZERO_TO_ONE;


public:

	AABB2();
	~AABB2();

	AABB2(AABB2 const& copyFrom);

	explicit AABB2(float minX, float minY, float maxX, float maxY);
	explicit AABB2(Vec2 const& mins, Vec2 const& maxs);
	explicit AABB2(IntVec2 const& mins, IntVec2 const& maxs);

	bool IsPointInside(Vec2 const& point) const;

	void Translate(Vec2 const& tranlationToApply);
	void SetCenter(Vec2 const& newCenter);
	void SetDimensions(Vec2 const& newDimensions);
	void StretchToIncludePoint(Vec2 const& point);

	AABB2 GetBoxFromUVs(AABB2 boxUVs) const;

	void GetFourCornerPoints(std::vector<Vec2>& out_fourCornerPoints) const;

	Vec2 const GetCenter() const;
	Vec2 const GetDimensions() const;
	Vec2 const GetNearestPoint(Vec2 const& referencePosition) const;
	Vec2 const GetPointAtUV(Vec2 const& uv) const;
	Vec2 const GetUVForPoint(Vec2 const& point) const;



};
