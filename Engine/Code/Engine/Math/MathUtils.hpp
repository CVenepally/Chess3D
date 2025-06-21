#pragma once

#include "Engine/Math/Vec2.hpp"

enum class BillboardType
{
	NONE = -1,

	WORLD_UP_FACING,
	WORLD_UP_OPPOSING,
	FULL_FACING,
	FULL_OPPOSING,

	COUNT
};

struct Vec3;
struct Vec4;
struct Mat44;
struct IntVec2;
struct AABB2;
struct AABB3;
struct OBB2;
struct OBB3;
struct Sphere;
struct Cylinder3D;
struct LineSegment2;
struct Capsule2;
struct Triangle2;
struct Plane3;

// Degrees and radians
float ConvertDegreesToRadians(float deg);
float ConvertRadiansToDegrees(float rad);
float CosDegrees(float deg);
float SinDegrees(float deg);
float ATan2Degrees(float y, float x);
float GetShortestAngularDispDegrees(float startDegrees, float endDegrees);
float GetTurnedTowardDegrees(float currentDegrees, float goalDegrees, float maxDeltaDegrees);
float GetDegreesBetweenVectors2D(Vec2 const& a, Vec2 const& b);


// Clamp and Lerp
float   GetClamped(float value, float minValue, float maxValue);
double	GetClamped(double value, double minValue, double maxValue);
int		GetClamped(int value, int minValue, int maxValue);
float   GetClampedZeroToOne(float value);
float   Lerp(float start, float end, float fractionTowardEnd);
float   GetFractionWithinRange(float value, float rangeStart, float rangeEnd);
float   RangeMap(float inValue, float inStart, float inEnd, float outStart, float outEnd);
float   RangeMapClamped(float inValue, float inStart, float inEnd, float outStart, float outEnd);
int     RoundDownToInt(float value);


// Vector Functions
float       GetVectorDistance2D(Vec2 const& vecA, Vec2 const& vecB);
float       GetVectorDistanceSquared2D(Vec2 const& vecA, Vec2 const& vecB);
float       GetProjectedLength2D(Vec2 const& vectorToProject, Vec2 const& vectorToProjectOnto);
float       GetProjectedLength3D(Vec3 const& vectorToProject, Vec3 const& vectorToProjectOnto);
int         GetTaxicabDistance2D(IntVec2 const& pointA, IntVec2 const& pointB);

Vec2 const  GetProjectedOnto2D(Vec2 const& vectorToProject, Vec2 const& vectorToProjectOnto); 
Vec3 const  GetProjectedOnto3D(Vec3 const& vectorToProject, Vec3 const& vectorToProjectOnto); 

float GetVectorDistance3D(Vec3 const& vecA, Vec3 const& vecB);
float GetVectorDistanceSquared3D(Vec3 const& vecA, Vec3 const& vecB);
float GetXYDistance3D(Vec3 const& vecA, Vec3 const& vecB);
float GetXYDistanceSquared3D(Vec3 const& vecA, Vec3 const& vecB);

bool DoDiscsOverlap(Vec2 const& centerA, float radiusA, Vec2 const& centerB, float radiusB);
bool DoSpheresOverlap(Vec3 const& centerA, float radiusA, Vec3 const& centerB, float radiusB);
bool DoSpheresOverlap(Sphere const& sphereA, Sphere const& sphereB);

bool DoAABB2Overlap(AABB2 const& box1, AABB2 const& box2);
bool DoAABB3Overlap(AABB3 const& box1, AABB3 const& box2);

bool DoCylindersOverlap(Cylinder3D const& cylinderA, Cylinder3D const& cylinderB);

bool DoesSphereAndAABB3Overlap(Sphere const& sphere, AABB3 const& box);
bool DoesSphereAndOBB3Overlap(Sphere const& sphere, OBB3 const& box);
bool DoesCylinderAndBoxOverlap(Cylinder3D const& cylinder, AABB3 const& box);
bool DoesCylinderAndSphereOverlap(Cylinder3D const& cylinder, Sphere const& sphere);

bool IsPointInsideDisc2D(Vec2 const& point, Vec2 const& discCenter, float discRadius);
bool IsPointInsideOrientedSector2D(Vec2 const& point, Vec2 const& sectorTip, float sectorForwardDegrees, float sectorApertureDegree, float sectorRadius); 
bool IsPointInsideDirectedSector2D(Vec2 const& point, Vec2 const& sectorTip, Vec2 const& sectorForwardNormal, float sectorApertureDegree, float sectorRadius);

bool IsPointInsideCapsule2D(Vec2 const& point, Capsule2 const& capsule);
bool IsPointInsideCapsule2D(Vec2 const& point, Vec2 const& boneStart, Vec2 const& boneEnd, float radius);
bool IsPointInsideAABB2D(Vec2 const& point, AABB2 const& box);
bool IsPointInsideOBB2D(Vec2 const& point, OBB2 const& box);
bool IsPointInsideTriangle2D(Vec2 const& referencePoint, Vec2 const& triCCW0, Vec2 const& triCCW1, Vec2 const& triCCW2);
bool IsPointInsideTriangle2D(Vec2 const& referencePoint, Triangle2 const& triangle);
bool IsPointInsideAABB3D(Vec3 const& point, AABB3 const& box);
bool IsPointInsideSphere(Vec3 const& point, Sphere const& sphere);
bool IsPointInsideCylinder3D(Vec3 const& point, Cylinder3D const& cylinder);
bool IsPointInsideOBB3D(Vec3 const& point, OBB3 const& obb);

bool IsPointInFrontOfPlane(Vec3 const& referencePoint, Plane3 const& plane);

Vec2 const GetNearestPointOnDisc2D(Vec2 const& referencePosition, Vec2 const& discCenter, float discRadius);
Vec2 const GetNearestPointOnAABB2D(Vec2 const& referencePosition, AABB2 const& box);
Vec2 const GetNearestPointOnInfiniteLine2D(Vec2 const& referencePosition, LineSegment2 const& infiniteLine);
Vec2 const GetNearestPointOnInfiniteLine2D(Vec2 const& referencePosition, Vec2 const& pointOnLine, Vec2 const& anotherPointOnLine);
Vec2 const GetNearestPointOnLineSegment2D(Vec2 const& referencePosition, LineSegment2 const& lineSegment);
Vec2 const GetNearestPointOnLineSegment2D(Vec2 const& referencePosition, Vec2 const& lineSegEnd, Vec2 const& lineSegStart);
Vec2 const GetNearestPointOnCapsule2D(Vec2 const& referencePosition, Capsule2 const& capsule);
Vec2 const GetNearestPointOnCapsule2D(Vec2 const& referencePosition, Vec2 const& boneStart, Vec2 const& boneEnd, float radius);
Vec2 const GetNearestPointOnOBB2D(Vec2 const& referencePosition, OBB2 const& orientedBox);
Vec2 const GetNearestPointOnTriangle2D(Vec2 const& referencePoint, Vec2 const& triCCW0, Vec2 const& triCCW1, Vec2 const& triCCW2);
Vec2 const GetNearestPointOnTriangle2D(Vec2 const& referencePoint, Triangle2 const& triangle);

Vec3 const GetNearestPointOnAABB3D(Vec3 const& referencePoint, AABB3 const& box);
Vec3 const GetNearestPointOnSphere(Vec3 const& referencePoint, Sphere const& sphere);
Vec3 const GetNearestPointOnCylinder3D(Vec3 const& referencePoint, Cylinder3D const& cylinder);
Vec3 const GetNearestPointOnOBB3D(Vec3 const& referencePoint, OBB3 const& cylinder);
Vec3 const GetNearestPointOnPlane3D(Vec3 const& referencePoint, Plane3 const& plane);

bool PushDiscOutOfFixedPoint2D(Vec2& mobileDiscCenter, float discRadius, Vec2 const& fixedPointPos);
bool PushDiscOutOfFixedDisc2D(Vec2& mobileDiscCenter, float discRadius, Vec2 const& fixedDiscCenter, float fixedDiscRadius); 
bool PushDiscsOutOfEachOther2D(Vec2& aCenter, float aRadius, Vec2& bCenter, float bRadius);
bool PushDiscOutOfAABB2D(Vec2& mobileDiscCenter, float discRadius, AABB2 const& fixedBox);

bool BounceDiscOutOfFixedDisc(Vec2& mobileDiscCenter, float discRadius, Vec2 const& fixedDiscCenter, float fixedDiscRadius, Vec2& discVelocity, float elasticity, float friction);
bool BounceDiscOutOfDisc(Vec2& discACenter, float discARadius, Vec2& discAVelocity, float discAElasticity, Vec2& discBCenter, float discBRadius, Vec2& discBVelocity, float discBElasticity);
bool BounceDiscOutOfFixedPoint(Vec2& mobileDiscCenter, float discRadius, Vec2& discVelocity, float elasticity, Vec2 const& fixedPoint, float friction);

// Transform utilities
void TransformPosition2D(Vec2& posToTransform, float uniformScale, float rotationDegrees, Vec2 const& moveDistance);
void TransformPosition2D(Vec2& posToTransform, Vec2 const& iBasis, Vec2 const& jBasis, Vec2 const& moveDistance); 

void TransformPositionXY3D(Vec3& posToTransform, float uniformScale, float rotationDegrees, Vec2 const& moveDistance);
void TransformPositionXY3D(Vec3& posToTransform, Vec2 const& iBasis, Vec2 const& jBasis, Vec2 const& moveDistance); 

void TransformPosition3D(Vec3& posToTransform, Mat44 const& transform);


// Dot and Cross Product
float DotProduct2D(Vec2 const& vecA, Vec2 const& vecB);
float DotProduct3D(Vec3 const& vecA, Vec3 const& vecB);
float DotProduct4D(Vec4 const& vecA, Vec4 const& vecB);
float CrossProduct2D(Vec2 const& vecA, Vec2 const& vecB);
Vec3 CrossProduct3D(Vec3 const& vecA, Vec3 const& vecB);


float NormalizeByte(unsigned char value);
unsigned char DenormalizeByte(float value);

Mat44 GetLookAtTransform(Vec3 currentPosition, Vec3 targetPosition);
Mat44 GetBillboardTransform(BillboardType billboardType, Mat44 const& targetTransform, Vec3 const& billboardPosition, Vec2 const& billboardScale = Vec2(1.f, 1.f));