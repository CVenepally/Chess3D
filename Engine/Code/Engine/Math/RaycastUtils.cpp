#include "Engine/Math/RaycastUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/LineSegment2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/OBB3.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/Sphere.hpp"
#include "Engine/Math/Cylinder3D.hpp"
#include "Engine/Math/Plane3.hpp"

#include <math.h>
#include <vector>


//------------------------------------------------------------------------------------------------------------------
RaycastResult2D::RaycastResult2D(bool impactResult, Vec2 rayStartPos, Vec2 rayFwdNormal, float rayLength)
	: m_didImpact(impactResult)
	, m_rayStartPos(rayStartPos)
	, m_rayForwardNormal(rayFwdNormal)
	, m_rayMaxLength(rayLength)
{

}



//------------------------------------------------------------------------------------------------------------------
RaycastResult2D::RaycastResult2D(bool impactResult, float impactDistance, Vec2 impactPos, Vec2 impactNormal, Vec2 rayStartPos, Vec2 rayFwdNormal, float rayLength)
	: m_didImpact(impactResult)
	, m_impactDistance(impactDistance)
	, m_impactPos(impactPos)
	, m_impactNormal(impactNormal)
	, m_rayStartPos(rayStartPos)
	, m_rayForwardNormal(rayFwdNormal)
	, m_rayMaxLength(rayLength)
{

}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
RaycastResult2D RaycastVsLineSegment2D(Vec2 startPos, Vec2 fwdNormal, float maxDist, LineSegment2 lineSegment)
{

	Vec2 jBasis = fwdNormal.GetRotated90Degrees();
	Vec2 rayToSegStartVector = lineSegment.m_start - startPos;
	Vec2 rayToSegEndVector = lineSegment.m_end - startPos;

	float rayToSegStartJProjection = DotProduct2D(rayToSegStartVector, jBasis);
	float rayToSegEndJProjection = DotProduct2D(rayToSegEndVector, jBasis);

	if(rayToSegStartJProjection * rayToSegEndJProjection >= 0.f)
	{
		return RaycastResult2D(false, startPos, fwdNormal, maxDist);
	}

	float rayToSegStartIProjection = DotProduct2D(rayToSegStartVector, fwdNormal);
	float rayToSegEndIProjection = DotProduct2D(rayToSegEndVector, fwdNormal);

	if(rayToSegStartIProjection >= maxDist && rayToSegEndIProjection >= maxDist)
	{
		return RaycastResult2D(false, startPos, fwdNormal, maxDist);
	}

	if(rayToSegStartIProjection <= 0 && rayToSegEndIProjection <= 0)
	{
		return RaycastResult2D(false, startPos, fwdNormal, 0.f);
	}

	float fraction = rayToSegStartJProjection / (rayToSegStartJProjection - rayToSegEndJProjection);

	float impactDist = rayToSegStartIProjection + fraction * (rayToSegEndIProjection - rayToSegStartIProjection);

	if(impactDist <= 0.f || impactDist >= maxDist)
	{
		return RaycastResult2D(false, startPos, fwdNormal, maxDist);
	}

	Vec2 impactPos = startPos + (fwdNormal * impactDist);
	Vec2 impactNormal = (lineSegment.m_end - lineSegment.m_start).GetRotated90Degrees().GetNormalized();

	if(DotProduct2D(fwdNormal, impactNormal) > 0)
	{
		impactNormal = -impactNormal;
	}

	return RaycastResult2D(true, impactDist, impactPos, impactNormal, startPos, fwdNormal, maxDist);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
RaycastResult2D RaycastVsAABB2D(Vec2 startPos, Vec2 fwdNormal, float maxDist, AABB2 box)
{
	
	Vec2 endPos = startPos + (fwdNormal * maxDist);

	if(box.IsPointInside(startPos))
	{
		return RaycastResult2D(true, 0.f, startPos, fwdNormal, startPos, fwdNormal, maxDist);
	}

	float stepForwardX = 1.f / fwdNormal.x;
	float stepForwardY = 1.f / fwdNormal.y;

	float startToMinX = box.m_mins.x - startPos.x;
	float startToMaxX = box.m_maxs.x - startPos.x;
	float startToMinY = box.m_mins.y - startPos.y;
	float startToMaxY = box.m_maxs.y - startPos.y;

	float impactDistanceToMinX = startToMinX * stepForwardX;
	float impactDistanceToMaxX = startToMaxX * stepForwardX;	
	float impactDistanceToMinY = startToMinY * stepForwardY;
	float impactDistanceToMaxY = startToMaxY * stepForwardY;

	float xEntryTimeFraction;
	float xExitTimeFraction;
	float yEntryTimeFraction;
	float yExitTimeFraction;

	if(impactDistanceToMinX < impactDistanceToMaxX)
	{
		xEntryTimeFraction = impactDistanceToMinX;
		xExitTimeFraction = impactDistanceToMaxX;
	}
	else
	{
		xEntryTimeFraction = impactDistanceToMaxX;
		xExitTimeFraction = impactDistanceToMinX;
	}

	if(impactDistanceToMinY < impactDistanceToMaxY)
	{
		yEntryTimeFraction = impactDistanceToMinY;
		yExitTimeFraction = impactDistanceToMaxY;
	}
	else
	{
		yEntryTimeFraction = impactDistanceToMaxY;
		yExitTimeFraction = impactDistanceToMinY;
	}

	FloatRange xRanges = FloatRange(xEntryTimeFraction, xExitTimeFraction);
	FloatRange yRanges = FloatRange(yEntryTimeFraction, yExitTimeFraction);
	FloatRange lineRange = FloatRange(0.f, maxDist);

	bool isOverlapping = xRanges.IsOverlappingWith(yRanges);

	if(!isOverlapping)
	{
		return RaycastResult2D(false, startPos, fwdNormal, maxDist);
	}

	FloatRange overlappingRange = xRanges.GetOverlapRange(yRanges);

	if(!lineRange.IsOverlappingWith(overlappingRange))
	{
		return RaycastResult2D(false, startPos, fwdNormal, maxDist);
	}

	float impactDistance = overlappingRange.m_min;
	Vec2 impactPos = startPos + (fwdNormal * impactDistance);
	Vec2 impactNormal;

	if(xRanges.m_min > yRanges.m_min)
	{
		if(fwdNormal.x < 0)
		{
			impactNormal = Vec2(1.0f, 0.0f);
		}
		else
		{
			impactNormal = Vec2(-1.0f, 0.0f);
		}
	}
	else
	{
		if(fwdNormal.y < 0)
		{
			impactNormal = Vec2(0.0f, 1.0f);
		}
		else
		{
			impactNormal = Vec2(0.0f, -1.0f);
		}
	}

	return RaycastResult2D(true, impactDistance, impactPos, impactNormal, startPos, fwdNormal, maxDist);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
RaycastResult3D RaycastVsAABB3D(Vec3 startPos, Vec3 fwdNormal, float maxDist, AABB3 box)
{

	RaycastResult3D result;
	result.m_rayStartPos = startPos;
	result.m_rayForwardNormal = fwdNormal;
	result.m_rayMaxLength = maxDist;

	if(IsPointInsideAABB3D(startPos, box))
	{
		result.m_didImpact = true;
		result.m_impactDistance = 0.f;
		result.m_impactPos = startPos;
		result.m_impactNormal = fwdNormal;
		
		return result;
	}

	float stepForwardX = 1 / fwdNormal.x;
	float stepForwardY = 1 / fwdNormal.y;
	float stepForwardZ = 1 / fwdNormal.z;

	float startToMinX = box.m_mins.x - startPos.x;
	float startToMaxX = box.m_maxs.x - startPos.x;

	float startToMinY = box.m_mins.y - startPos.y;
	float startToMaxY = box.m_maxs.y - startPos.y;
	
	float startToMinZ = box.m_mins.z - startPos.z;
	float startToMaxZ = box.m_maxs.z - startPos.z;

	float impactDistanceToMinX = startToMinX * stepForwardX;
	float impactDistanceToMaxX = startToMaxX * stepForwardX;

	float impactDistanceToMinY = startToMinY * stepForwardY;
	float impactDistanceToMaxY = startToMaxY * stepForwardY;

	float impactDistanceToMinZ = startToMinZ * stepForwardZ;
	float impactDistanceToMaxZ = startToMaxZ * stepForwardZ;

	float xEntryTimeFraction;
	float xExitTimeFraction;
	float yEntryTimeFraction;
	float yExitTimeFraction;
	float zEntryTimeFraction;
	float zExitTimeFraction;

	if(impactDistanceToMinX < impactDistanceToMaxX)
	{
		xEntryTimeFraction = impactDistanceToMinX;
		xExitTimeFraction = impactDistanceToMaxX;
	}
	else
	{
		xEntryTimeFraction = impactDistanceToMaxX;
		xExitTimeFraction = impactDistanceToMinX;
	}

	if(impactDistanceToMinY < impactDistanceToMaxY)
	{
		yEntryTimeFraction = impactDistanceToMinY;
		yExitTimeFraction = impactDistanceToMaxY;
	}
	else
	{
		yEntryTimeFraction = impactDistanceToMaxY;
		yExitTimeFraction = impactDistanceToMinY;
	}

	if(impactDistanceToMinZ < impactDistanceToMaxZ)
	{
		zEntryTimeFraction = impactDistanceToMinZ;
		zExitTimeFraction = impactDistanceToMaxZ;
	}
	else
	{
		zEntryTimeFraction = impactDistanceToMaxZ;
		zExitTimeFraction = impactDistanceToMinZ;
	}


	FloatRange xRanges = FloatRange(xEntryTimeFraction, xExitTimeFraction);
	FloatRange yRanges = FloatRange(yEntryTimeFraction, yExitTimeFraction);
	FloatRange zRanges = FloatRange(zEntryTimeFraction, zExitTimeFraction);
	FloatRange lineRange = FloatRange(0.f, maxDist);

	bool isOverlapping = xRanges.IsOverlappingWith(yRanges);

	if(!isOverlapping)
	{
		return RaycastResult3D(false, startPos, fwdNormal, maxDist);
	}

	FloatRange overlappingRange = xRanges.GetOverlapRange(yRanges);
	
	isOverlapping = zRanges.IsOverlappingWith(overlappingRange);

	if(!isOverlapping)
	{
		return RaycastResult3D(false, startPos, fwdNormal, maxDist);
	}
	
	overlappingRange = overlappingRange.GetOverlapRange(zRanges);

	if(!lineRange.IsOverlappingWith(overlappingRange))
	{
		return RaycastResult3D(false, startPos, fwdNormal, maxDist);
	}

	float impactDistance = overlappingRange.m_min;
	Vec3 impactPos = startPos + (fwdNormal * impactDistance);
	Vec3 impactNormal;

	if(xRanges.m_min > yRanges.m_min && xRanges.m_min > zRanges.m_min)
	{
		if(fwdNormal.x < 0)
		{
			impactNormal = Vec3(1.0f, 0.0f, 0.f);
		}
		else
		{
			impactNormal = Vec3(-1.0f, 0.0f, 0.f);
		}
	}
	else if(yRanges.m_min > zRanges.m_min)
	{
		if(fwdNormal.y < 0)
		{
			impactNormal = Vec3(0.0f, 1.0f, 0.f);
		}
		else
		{
			impactNormal = Vec3(0.0f, -1.0f, 0.f);
		}
	}
	else
	{
		if(fwdNormal.z < 0)
		{
			impactNormal = Vec3(0.0f, 0.0f, 1.f);
		}
		else
		{
			impactNormal = Vec3(0.0f, 0.0f, -1.f);
		}

	}

	result.m_didImpact = true;
	result.m_impactPos = impactPos;
	result.m_impactDistance = impactDistance;
	result.m_impactNormal = impactNormal;

	return result;
}

//------------------------------------------------------------------------------------------------------------------
RaycastResult2D RaycastVsDisc2D(Vec2 startPos, Vec2 fwdNormal, float maxDist, Vec2 discCenter, float discRadius)
{

	if(IsPointInsideDisc2D(startPos, discCenter, discRadius))
	{
		return RaycastResult2D(true, 0.f, startPos, fwdNormal, startPos, fwdNormal, maxDist);
	}

	Vec2 rayStartToDiscCenterVector = discCenter - startPos;
	Vec2 jBasis = fwdNormal.GetRotated90Degrees();

	float altitude = DotProduct2D(rayStartToDiscCenterVector, jBasis);

	if(altitude >= discRadius || altitude <= -discRadius)
	{
		return RaycastResult2D(false, startPos, fwdNormal, maxDist);
	}

	float startCenterVectorProjectionOnRay = DotProduct2D(rayStartToDiscCenterVector, fwdNormal);

	float adjust = sqrtf((discRadius * discRadius) - (altitude * altitude));

	float impactDistance = startCenterVectorProjectionOnRay - adjust;

	if(impactDistance >= maxDist || impactDistance < 0.f)
	{
		return RaycastResult2D(false, startPos, fwdNormal, maxDist);
	}

	Vec2 impactPosition = startPos + (fwdNormal * impactDistance);
	Vec2 impactNormal = (impactPosition - discCenter).GetNormalized();

	return RaycastResult2D(true, impactDistance, impactPosition, impactNormal, startPos, fwdNormal, maxDist);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
RaycastResult3D RaycastVsSphere3D(Vec3 startPos, Vec3 fwdNormal, float maxDist, Sphere sphere)
{
	RaycastResult3D result;
	result.m_rayStartPos = startPos;
	result.m_rayForwardNormal = fwdNormal;
	result.m_rayMaxLength = maxDist;

	if(IsPointInsideSphere(startPos, sphere))
	{
		result.m_didImpact = true;
		result.m_impactDistance = 0.f;
		result.m_impactPos = startPos;
		result.m_impactNormal = fwdNormal;

		return result;
	}


	Vec3 startToCenter = sphere.m_center - startPos;
	Vec3 startToCenterProjectedOnI = GetProjectedOnto3D(startToCenter, fwdNormal);
	Vec3 startToCenterProjectedOnJ = startToCenter - startToCenterProjectedOnI;

	float altitude = startToCenterProjectedOnJ.GetLength();

	if(altitude >= sphere.m_radius || altitude <= -sphere.m_radius)
	{
		result.m_didImpact = false;
		result.m_impactDistance = 0.f;
		result.m_impactPos = startPos;
		result.m_impactNormal = fwdNormal;

		return result;
	}

	float startToCenterOnILength = DotProduct3D(startToCenter, fwdNormal);

	float adjust = sqrtf((sphere.m_radius * sphere.m_radius) - (altitude * altitude));

	result.m_impactDistance = startToCenterOnILength - adjust;

	if(result.m_impactDistance > maxDist || result.m_impactDistance < 0.f)
	{
		result.m_didImpact = false;
		result.m_impactPos = startPos;
		result.m_impactNormal = fwdNormal;

		return result;
	}

	result.m_didImpact = true;
	result.m_impactPos = startPos + (fwdNormal * result.m_impactDistance);
	result.m_impactNormal = (result.m_impactPos - sphere.m_center).GetNormalized();

	return result;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
RaycastResult3D RaycastVsCylinder3D(Vec3 startPos, Vec3 fwdNormal, float maxDist, Cylinder3D cylinder)
{
	RaycastResult3D result;
	result.m_rayStartPos = startPos;
	result.m_rayForwardNormal = fwdNormal;
	result.m_rayMaxLength = maxDist;

	if(IsPointInsideCylinder3D(startPos, cylinder))
	{
		result.m_didImpact = true;
		result.m_impactDistance = 0.f;
		result.m_impactPos = startPos;
		result.m_impactNormal = - fwdNormal;

		return result;
	}
	
	Vec3 rayEndPosition = startPos + fwdNormal * maxDist;

	float distanceScale = 1.f / maxDist;

	float zStep = 1.f / fwdNormal.z;

	float bottomZImpactDistance = (cylinder.m_startPosition.z - startPos.z) * zStep;
	float topZImpactDistance = ((cylinder.m_startPosition.z + cylinder.m_height) - startPos.z) * zStep;

	float zBottomImpactTime = bottomZImpactDistance * distanceScale;
	float zTopImpactTime = topZImpactDistance * distanceScale;

	float zEntryTime;
	float zExitTime;

	if(zBottomImpactTime < zTopImpactTime)
	{
		zEntryTime = zBottomImpactTime;
		zExitTime = zTopImpactTime;
	}
	else
	{
		zEntryTime = zTopImpactTime;
		zExitTime = zBottomImpactTime;
	}

	// cylinder's disc against ray
	
	Vec3 endPosition = startPos + fwdNormal * maxDist;

	Vec2 fwdNrmlXY = Vec2(fwdNormal.x, fwdNormal.y).GetNormalized();

	Vec2 startPosXY = Vec2(startPos.x, startPos.y);
	Vec2 endPosXY = Vec2(endPosition.x, endPosition.y);
	Vec2 cylinderCenterXY = Vec2(cylinder.m_startPosition.x, cylinder.m_startPosition.y);

	float maxDistXY = (endPosXY - startPosXY).GetLength();

	float distScaleXY = 1.f / maxDistXY;

	Vec2 startToCylinderCenter = cylinderCenterXY - startPosXY;

	Vec2 jBasisXY = fwdNrmlXY.GetRotated90Degrees();

	float altitude = DotProduct2D(startToCylinderCenter, jBasisXY);

	if(altitude >= cylinder.m_radius || altitude <= -cylinder.m_radius)
	{
		result.m_didImpact = false;
		result.m_impactDistance = 0.f;
		result.m_impactPos = startPos;
		result.m_impactNormal = fwdNormal;

		return result;
	}
	
	float startToCenterProjectionOnFwdNrml = DotProduct2D(startToCylinderCenter, fwdNrmlXY);

	float adjust = sqrtf((cylinder.m_radius * cylinder.m_radius) - (altitude * altitude));

	float entryDistance = startToCenterProjectionOnFwdNrml - adjust;
	float exitDistance = startToCenterProjectionOnFwdNrml + adjust;

	float entryTime = entryDistance * distScaleXY;
	float exitTime = exitDistance * distScaleXY;

	FloatRange zRanges = FloatRange(zEntryTime, zExitTime);
	FloatRange xyRanges = FloatRange(entryTime, exitTime);

	if(!zRanges.IsOverlappingWith(xyRanges))
	{
		result.m_didImpact = false;
		result.m_impactDistance = 0.f;
		result.m_impactPos = startPos;
		result.m_impactNormal = fwdNormal;

		return result;
	}

	FloatRange overalappingRange = zRanges.GetOverlapRange(xyRanges);

	result.m_impactDistance = maxDist * overalappingRange.m_min;

	if(result.m_impactDistance >= maxDist || result.m_impactDistance < 0.f)
	{
		result.m_didImpact = false;
		result.m_impactDistance = 0.f;
		result.m_impactPos = startPos;
		result.m_impactNormal = fwdNormal;

		return result;
	}

	result.m_impactPos = startPos + fwdNormal * result.m_impactDistance;
	result.m_didImpact = true;

	if(zRanges.m_min > xyRanges.m_min)
	{
		if(fwdNormal.z < 0.f)
		{
			result.m_impactNormal = Vec3(0.f, 0.f, 1.f);
		}
		else
		{
			result.m_impactNormal = Vec3(0.f, 0.f, -1.f);
		}
	}
	else
	{
		result.m_impactNormal = (result.m_impactPos - Vec3(cylinder.m_startPosition.x, cylinder.m_startPosition.y, result.m_impactPos.z)).GetNormalized();
	}

	return result;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
RaycastResult3D RaycastVsOBB3D(Vec3 startPos, Vec3 fwdNormal, float maxDist, OBB3 obb)
{

	Mat44 worldToLocalTransform = obb.GetWorldToLocalTransform();
	Mat44 localToWorldTransform = obb.GetLocalToWorldTransform();

	Vec3 localPosition = worldToLocalTransform.TransformPosition3D(startPos);
	Vec3 localForward  = worldToLocalTransform.TransformVectorQuantity3D(fwdNormal);

	std::vector<Vec3> obbEightCornerPoints;
	obb.GetEightCornerPoints(obbEightCornerPoints);
	
	AABB3 obbAsAABB;
	obbAsAABB.m_mins = -obb.m_halfDimensions;
	obbAsAABB.m_maxs = obb.m_halfDimensions;

	RaycastResult3D rayResult = RaycastVsAABB3D(localPosition, localForward, maxDist, obbAsAABB);
	rayResult.m_rayStartPos = localToWorldTransform.TransformPosition3D(rayResult.m_rayStartPos);
	rayResult.m_impactPos = localToWorldTransform.TransformPosition3D(rayResult.m_impactPos);
	rayResult.m_rayForwardNormal = localToWorldTransform.TransformVectorQuantity3D(rayResult.m_rayForwardNormal);
	rayResult.m_impactNormal = localToWorldTransform.TransformVectorQuantity3D(rayResult.m_impactNormal);
	
	return rayResult;

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
RaycastResult3D RaycastVsPlane3D(Vec3 startPos, Vec3 fwdNormal, float maxDist, Plane3 plane)
{
	RaycastResult3D result;
	result.m_rayStartPos = startPos;
	result.m_rayForwardNormal = fwdNormal;
	result.m_rayMaxLength = maxDist;
	result.m_impactDistance = FLT_MAX;

	float altitudeFromStart = plane.GetAltitudeFromPoint(startPos);
	Vec3 rayEnd = startPos + fwdNormal * maxDist;

	float altitudeFromEnd = plane.GetAltitudeFromPoint(rayEnd);

	if(altitudeFromEnd * altitudeFromStart > 0.f)
	{
		result.m_didImpact = false;
	}
	else
	{
		if(IsPointInFrontOfPlane(startPos, plane))
		{
			result.m_impactNormal = plane.m_normal;
		}
		else
		{
			result.m_impactNormal = -plane.m_normal;
		}

		result.m_impactDistance = -altitudeFromStart / DotProduct3D(fwdNormal, plane.m_normal);
		result.m_impactPos = startPos + fwdNormal * result.m_impactDistance;
		result.m_didImpact = true;
	}

	return result;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Ray2::Ray2(Vec2 const& startPos, Vec2 const& endPos)
	: m_startPos(startPos)
{
	m_maxLength = (endPos - startPos).GetLength();
	m_forwardNormal = (endPos - startPos).GetNormalized();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Ray2::Ray2(Vec2 const& startPos, Vec2 const& fwdNormal, float maxLength)
	: m_startPos(startPos)
	, m_forwardNormal(fwdNormal)
	, m_maxLength(maxLength)
{
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Ray3::Ray3(Vec3 const& startPos, Vec3 const& endPos)
	: m_startPos(startPos)
{	
	m_maxLength = (endPos - startPos).GetLength();
	m_fwdNormal = (endPos - startPos).GetNormalized();

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Ray3::Ray3(Vec3 const& startPos, Vec3 const& fwdNormal, float maxLength)
	: m_startPos(startPos)
	, m_fwdNormal(fwdNormal)
	, m_maxLength(maxLength)
{}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
RaycastResult3D::RaycastResult3D(bool impactResult, Vec3 rayStartPos, Vec3 rayFwdNormal, float rayLength)
	: m_didImpact(impactResult)
	, m_rayStartPos(rayStartPos)
	, m_rayForwardNormal(rayFwdNormal)
	, m_rayMaxLength(rayLength)
{

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
RaycastResult3D::RaycastResult3D(bool impactResult, float impactDistance, Vec3 impactPos, Vec3 impactNormal, Vec3 rayStartPos, Vec3 rayFwdNormal, float rayLength)
	: m_didImpact(impactResult)
	, m_impactDistance(impactDistance)
	, m_impactNormal(impactNormal)
	, m_rayStartPos(rayStartPos)
	, m_rayForwardNormal(rayFwdNormal)
	, m_rayMaxLength(rayLength)
{}
