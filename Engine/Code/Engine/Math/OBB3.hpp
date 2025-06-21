#pragma once
#include "Engine/Math/Vec3.hpp"

#include <vector>

struct Mat44;

enum EIGHT_CORNER_POINTS
{
	OBB_BOTTOM_LEFT_BACK,
	OBB_BOTTOM_RIGHT_BACK,
	OBB_TOP_RIGHT_BACK,
	OBB_TOP_LEFT_BACK,
	OBB_BOTTOM_RIGHT_FRONT,
	OBB_BOTTOM_LEFT_FRONT,
	OBB_TOP_LEFT_FRONT,
	OBB_TOP_RIGHT_FRONT,
};

struct OBB3
{
public:
	Vec3 m_center;
	Vec3 m_iBasis;
	Vec3 m_jBasis;
	Vec3 m_kBasis;
	Vec3 m_halfDimensions;

public:
	OBB3() = default;
	~OBB3() = default;

	explicit OBB3(Vec3 const& center, Vec3 const& iBasis, Vec3 const& jBasis, Vec3 const& halfDimensions);

	void GetEightCornerPoints(std::vector<Vec3>& out_eightCornerPoints) const;
	Vec3 GetLocalPositionForWorldPosition(Vec3 const& worldPosition) const;
	Vec3 GetWorldPositionForLocalPosition(Vec3 const& localPosition) const;

	Mat44 GetLocalToWorldTransform() const;
	Mat44 GetWorldToLocalTransform() const;

};