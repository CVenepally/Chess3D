#pragma once

#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"

struct LineSegment2;
struct AABB2;
struct AABB3;
struct OBB3;
struct Sphere;
struct Cylinder3D;
struct Plane3;

struct Ray2
{
	Ray2() = default;
	Ray2(Vec2 const& startPos, Vec2 const& endPos);
	Ray2(Vec2 const& startPos, Vec2 const& fwdNormal, float maxLength);

	Vec2 m_startPos;
	Vec2 m_forwardNormal;
	float m_maxLength = 1.f;
};

struct Ray3
{
	Ray3() = default;
	Ray3(Vec3 const& startPos, Vec3 const& endPos);
	Ray3(Vec3 const& startPos, Vec3 const& fwdNormal, float maxLength);

	Vec3 m_startPos;
	Vec3 m_fwdNormal;
	float m_maxLength;
};

//------------------------------------------------------------------------------------------------------------------
struct RaycastResult2D
{

	bool m_didImpact = false;
	float m_impactDistance = 0.f;
	Vec2 m_impactPos;
	Vec2 m_impactNormal;

	Vec2 m_rayStartPos;
	Vec2 m_rayForwardNormal;
	float m_rayMaxLength;

	RaycastResult2D() = default;

	explicit RaycastResult2D(bool impactResult, Vec2 rayStartPos, Vec2 rayFwdNormal, float rayLength);
	explicit RaycastResult2D(bool impactResult, float impactDistance, Vec2 impactPos, Vec2 impactNormal, Vec2 rayStartPos, Vec2 rayFwdNormal, float rayLength);

};

//------------------------------------------------------------------------------------------------------------------
struct RaycastResult3D
{

	bool m_didImpact = false;
	float m_impactDistance = 0.f;
	Vec3 m_impactPos;
	Vec3 m_impactNormal;

	Vec3 m_rayStartPos;
	Vec3 m_rayForwardNormal;
	float m_rayMaxLength;

	RaycastResult3D() = default;

	explicit RaycastResult3D(bool impactResult, Vec3 rayStartPos, Vec3 rayFwdNormal, float rayLength);
	explicit RaycastResult3D(bool impactResult, float impactDistance, Vec3 impactPos, Vec3 impactNormal, Vec3 rayStartPos, Vec3 rayFwdNormal, float rayLength);

};

//------------------------------------------------------------------------------------------------------------------
RaycastResult2D RaycastVsDisc2D(Vec2 startPos, Vec2 fwdNormal, float maxDist, Vec2 discCenter, float discRadius);
RaycastResult2D RaycastVsLineSegment2D(Vec2 startPos, Vec2 fwdNormal, float maxDist, LineSegment2 lineSegment);
RaycastResult2D RaycastVsAABB2D(Vec2 startPos, Vec2 fwdNormal, float maxDist, AABB2 box);

RaycastResult3D RaycastVsAABB3D(Vec3 startPos, Vec3 fwdNormal, float maxDist, AABB3 box);
RaycastResult3D RaycastVsSphere3D(Vec3 startPos, Vec3 fwdNormal, float maxDist, Sphere sphere);
RaycastResult3D RaycastVsCylinder3D(Vec3 startPos, Vec3 fwdNormal, float maxDist, Cylinder3D cylinder);
RaycastResult3D RaycastVsOBB3D(Vec3 startPos, Vec3 fwdNormal, float maxDist, OBB3 obb);
RaycastResult3D RaycastVsPlane3D(Vec3 startPos, Vec3 fwdNormal, float maxDist, Plane3 plane);