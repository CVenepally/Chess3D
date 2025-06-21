#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Plane3.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/OBB3.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/Cylinder3D.hpp"
#include "Engine/Math/Sphere.hpp"
#include "Engine/Math/LineSegment2.hpp"
#include "Engine/Math/Capsule2.hpp"
#include "Engine/Math/Triangle2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Core/EngineCommon.hpp"

#include <math.h>

float ConvertDegreesToRadians(float deg)
{

    return deg * (3.1415926f/180.f);

}



float ConvertRadiansToDegrees(float rad)
{

    return rad * (180.f/3.1415926f);

}



float CosDegrees(float deg)
{
    return cosf(deg * (3.1415926f / 180.f));
}



float SinDegrees(float deg)
{

    return sinf(deg * (3.1415926f / 180.f));

}



float ATan2Degrees(float y, float x)
{

    return atan2f(y, x) * (180.f / 3.1415926f);

}



float GetShortestAngularDispDegrees(float startDegrees, float endDegrees)
{

    float displacement = endDegrees - startDegrees;

    while(displacement > 180.f)
    {
        displacement -= 360.f;
    }

    while(displacement < -180.f)
    {
        displacement += 360.f;
    }

    return displacement;

}



float GetTurnedTowardDegrees(float currentDegrees, float goalDegrees, float maxDeltaDegrees)
{

    float angularDisplacement = GetShortestAngularDispDegrees(currentDegrees, goalDegrees);

    if(fabsf(angularDisplacement) < maxDeltaDegrees)
    {
        return goalDegrees;
    }

    if(angularDisplacement > 0.f)
    {
        return currentDegrees + maxDeltaDegrees;
    }

    return currentDegrees - maxDeltaDegrees;

}

float GetDegreesBetweenVectors2D(Vec2 const& a, Vec2 const& b)
{
    Vec2 normalizedA = a.GetNormalized();
    Vec2 normalizedB = b.GetNormalized();

    float abDotProduct = DotProduct2D(normalizedA, normalizedB);

    abDotProduct = GetClamped(abDotProduct, -1.f, 1.f);

    float angleInRadians = acosf(abDotProduct);

    float angleInDegrees = ConvertRadiansToDegrees(angleInRadians);

    return angleInDegrees;
}



float GetClamped(float value, float minValue, float maxValue)
{

    if(value >= minValue && value <= maxValue)
    {
        return value;
    }

    if(value < minValue)
    {
        return minValue;
    }
  
    return maxValue;

}

double GetClamped(double value, double minValue, double maxValue)
{
	if(value >= minValue && value <= maxValue)
	{
		return value;
	}

	if(value < minValue)
	{
		return minValue;
	}

	return maxValue;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
int GetClamped(int value, int minValue, int maxValue)
{
	if(value >= minValue && value <= maxValue)
	{
		return value;
	}

	if(value < minValue)
	{
		return minValue;
	}

	return maxValue;
}


float GetClampedZeroToOne(float value)
{

    if(value > 0.f && value < 1.f)
    {
        return value;
    }
    
    if(value <= 0.f)
    {
        return 0.f;
    }
    
    return 1.f;

}


float Lerp(float start, float end, float fractionTowardEnd)
{

    float range = end - start;
    float fractionValue = start + (range * fractionTowardEnd);

    return fractionValue;

}


float GetFractionWithinRange(float value, float rangeStart, float rangeEnd)
{

    float valueStartDiff = value - rangeStart;
    float range = rangeEnd - rangeStart;

    float fraction = valueStartDiff / range;

    return fraction;

}



float RangeMap(float inValue, float inStart, float inEnd, float outStart, float outEnd)
{

    float fraction = GetFractionWithinRange(inValue, inStart, inEnd);

    return Lerp(outStart, outEnd, fraction);

}



float RangeMapClamped(float inValue, float inStart, float inEnd, float outStart, float outEnd)
{

    if(inValue < inStart)
    {
        return outStart;
    }
    
    if(inValue > inEnd)
    {
        return outEnd;
    }
    
    float clampValue = GetClamped(inValue, inStart, inEnd);

    return RangeMap(clampValue, inStart, inEnd, outStart, outEnd);

}



int RoundDownToInt(float value)
{

    return static_cast<int>(floor(value));

}



float GetVectorDistance2D(Vec2 const& vecA, Vec2 const& vecB)
{

    float deltaX = vecB.x - vecA.x;
    float deltaY = vecB.y - vecA.y; 

    float distance = sqrtf((deltaX * deltaX) + (deltaY * deltaY));

    return distance;

}



float GetVectorDistanceSquared2D(Vec2 const& vecA, Vec2 const& vecB)
{

    float deltaX = vecB.x - vecA.x;
    float deltaY = vecB.y - vecA.y;

    float distance = (deltaX * deltaX) + (deltaY * deltaY);

    return distance;

}



int GetTaxicabDistance2D(IntVec2 const& pointA, IntVec2 const& pointB)
{

    IntVec2 taxiCabVector = pointA - pointB;

    return abs(taxiCabVector.x) + abs(taxiCabVector.y);

}



float GetProjectedLength2D(Vec2 const& vectorToProject, Vec2 const& vectorToProjectOnto)
{

    Vec2 normalizedVector = vectorToProjectOnto.GetNormalized();

    return DotProduct2D(vectorToProject, normalizedVector);

}


float GetProjectedLength3D(Vec3 const& vectorToProject, Vec3 const& vectorToProjectOnto)
{
	Vec3 normalizedVector = vectorToProjectOnto.GetNormalized();

	return DotProduct3D(vectorToProject, normalizedVector);
}



Vec2 const GetProjectedOnto2D(Vec2 const& vectorToProject, Vec2 const& vectorToProjectOnto)
{
    Vec2 normalVector = vectorToProjectOnto.GetNormalized();

    float normalVectorLength = GetProjectedLength2D(vectorToProject, normalVector);
    
    return normalVector * normalVectorLength;
}



Vec3 const GetProjectedOnto3D(Vec3 const& vectorToProject, Vec3 const& vectorToProjectOnto)
{
	Vec3 normalVector = vectorToProjectOnto.GetNormalized();

	float normalVectorLength = GetProjectedLength3D(vectorToProject, normalVector);

	return normalVector * normalVectorLength;
}



float GetVectorDistance3D(Vec3 const& vecA, Vec3 const& vecB)
{

    float deltaX = vecB.x - vecA.x;
    float deltaY = vecB.y - vecA.y;
    float deltaZ = vecB.z - vecA.z;

    float distance = sqrtf((deltaX * deltaX) + (deltaY * deltaY) + (deltaZ * deltaZ));

    return distance;

}



float GetVectorDistanceSquared3D(Vec3 const& vecA, Vec3 const& vecB)
{

    float deltaX = vecB.x - vecA.x;
    float deltaY = vecB.y - vecA.y;
    float deltaZ = vecB.z - vecA.z;

    float distance = (deltaX * deltaX) + (deltaY * deltaY) + (deltaZ * deltaZ);

    return distance;

}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
float GetXYDistance3D(Vec3 const& vecA, Vec3 const& vecB)
{

    float deltaX = vecB.x - vecA.x;
    float deltaY = vecB.y - vecA.y;

    float distance = sqrtf((deltaX * deltaX) + (deltaY * deltaY));

    return distance;

}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
float GetXYDistanceSquared3D(Vec3 const& vecA, Vec3 const& vecB)
{

    float deltaX = vecB.x - vecA.x;
    float deltaY = vecB.y - vecA.y;

    float distance = (deltaX * deltaX) + (deltaY * deltaY);

    return distance;

}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool DoDiscsOverlap(Vec2 const& centerA, float radiusA, Vec2 const& centerB, float radiusB)
{
    
    float radiiSum = (radiusA + radiusB) * (radiusA + radiusB);
    float cDistance = GetVectorDistanceSquared2D(centerA, centerB);

    if(cDistance <= radiiSum)
    {
        return true;
    }
    
    return false;


}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool DoSpheresOverlap(Vec3 const& centerA, float radiusA, Vec3 const& centerB, float radiusB)
{

    float radiiSum = (radiusA + radiusB) * (radiusA + radiusB);
    float cDistance = GetVectorDistanceSquared3D(centerA, centerB);

    if(cDistance <= radiiSum)
    {
        return true;
    }

    return false;

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool DoSpheresOverlap(Sphere const& sphereA, Sphere const& sphereB)
{
    return DoSpheresOverlap(sphereA.m_center, sphereA.m_radius, sphereB.m_center, sphereB.m_radius);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool DoAABB2Overlap(AABB2 const& box1, AABB2 const& box2)
{

    std::vector<Vec2> boxOneCornerPoints;
    std::vector<Vec2> boxTwoCornerPoints;

    box1.GetFourCornerPoints(boxOneCornerPoints);
    box2.GetFourCornerPoints(boxTwoCornerPoints);

    return box1.IsPointInside(boxTwoCornerPoints[BOTTOM_LEFT]) || box1.IsPointInside(boxTwoCornerPoints[BOTTOM_RIGHT]) || box1.IsPointInside(boxTwoCornerPoints[TOP_RIGHT]) || box1.IsPointInside(boxTwoCornerPoints[TOP_LEFT]) ||
           box2.IsPointInside(boxOneCornerPoints[BOTTOM_LEFT]) || box2.IsPointInside(boxOneCornerPoints[BOTTOM_RIGHT]) || box2.IsPointInside(boxOneCornerPoints[TOP_RIGHT]) || box2.IsPointInside(boxOneCornerPoints[TOP_LEFT]);
 
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool DoAABB3Overlap(AABB3 const& box1, AABB3 const& box2)
{

    FloatRange boxOneXRange = FloatRange(box1.m_mins.x, box1.m_maxs.x);
    FloatRange boxTwoXRange = FloatRange(box2.m_mins.x, box2.m_maxs.x);
    bool isXOveralpping = boxOneXRange.IsOverlappingWith(boxTwoXRange);

    FloatRange boxOneYRange = FloatRange(box1.m_mins.y, box1.m_maxs.y);
    FloatRange boxTwoYRange = FloatRange(box2.m_mins.y, box2.m_maxs.y);
	bool isYOveralpping = boxOneYRange.IsOverlappingWith(boxTwoYRange);

    FloatRange boxOneZRange = FloatRange(box1.m_mins.z, box1.m_maxs.z);
    FloatRange boxTwoZRange = FloatRange(box2.m_mins.z, box2.m_maxs.z);
	bool isZOveralpping = boxOneZRange.IsOverlappingWith(boxTwoZRange);

    return isXOveralpping && isYOveralpping && isZOveralpping;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool DoCylindersOverlap(Cylinder3D const& cylinderA, Cylinder3D const& cylinderB)
{
    FloatRange cylinderAHeightRange = FloatRange(cylinderA.m_startPosition.z, cylinderA.m_startPosition.z + cylinderA.m_height);
    FloatRange cylinderBHeightRange = FloatRange(cylinderB.m_startPosition.z, cylinderB.m_startPosition.z + cylinderB.m_height);

    if(cylinderAHeightRange.IsOverlappingWith(cylinderBHeightRange))
    {

        Vec2 cylinderAStart = Vec2(cylinderA.m_startPosition.x, cylinderA.m_startPosition.y);
        Vec2 cylinderBStart = Vec2(cylinderB.m_startPosition.x, cylinderB.m_startPosition.y);

        return DoDiscsOverlap(cylinderAStart, cylinderA.m_radius, cylinderBStart, cylinderB.m_radius);
    }
    
    return false;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool DoesSphereAndAABB3Overlap(Sphere const& sphere, AABB3 const& box)
{
    Vec3 pointOnBox = GetNearestPointOnAABB3D(sphere.m_center, box);

    return IsPointInsideSphere(pointOnBox, sphere);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool DoesSphereAndOBB3Overlap(Sphere const& sphere, OBB3 const& box)
{
	Vec3 pointOnBox = GetNearestPointOnOBB3D(sphere.m_center, box);

	return IsPointInsideSphere(pointOnBox, sphere);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool DoesCylinderAndBoxOverlap(Cylinder3D const& cylinder, AABB3 const& box)
{
 
 	FloatRange cylinderHeightRange = FloatRange(cylinder.m_startPosition.z, cylinder.m_startPosition.z + cylinder.m_height);
 	FloatRange boxHeightRange = FloatRange(box.m_mins.z, box.m_maxs.z);
 
 	if(cylinderHeightRange.IsOverlappingWith(boxHeightRange))
 	{
 
 		Vec2 cylinderStartXY = Vec2(cylinder.m_startPosition.x, cylinder.m_startPosition.y);
 		AABB2 box2D = AABB2(box.m_mins.x, box.m_mins.y, box.m_maxs.x, box.m_maxs.y);
 
         Vec2 boxNearestPoint = box2D.GetNearestPoint(cylinderStartXY);
 
 		return IsPointInsideDisc2D(boxNearestPoint, cylinderStartXY, cylinder.m_radius);
 	}
 
     return false;

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool DoesCylinderAndSphereOverlap(Cylinder3D const& cylinder, Sphere const& sphere)
{

    Vec3 nearestPoint = GetNearestPointOnCylinder3D(sphere.m_center, cylinder);

    return IsPointInsideSphere(nearestPoint, sphere);

}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool IsPointInsideDisc2D(Vec2 const& point, Vec2 const& discCenter, float discRadius)
{
    if(GetVectorDistanceSquared2D(point, discCenter) > discRadius * discRadius)
    {
        return false;
    }

    return true;
}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool IsPointInsideOrientedSector2D(Vec2 const& point, Vec2 const& sectorTip, float sectorForwardDegrees, float sectorApertureDegree, float sectorRadius)
{

    if(GetVectorDistanceSquared2D(point, sectorTip) > sectorRadius * sectorRadius)
    {
        return false;
    }

    Vec2 tipToPoint = point - sectorTip;

    Vec2 forwardNormal = Vec2(CosDegrees(sectorForwardDegrees), SinDegrees(sectorForwardDegrees));

	if(GetDegreesBetweenVectors2D(tipToPoint, forwardNormal) > sectorApertureDegree / 2)
	{
		return false;
	}

	return true;

}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool IsPointInsideDirectedSector2D(Vec2 const& point, Vec2 const& sectorTip, Vec2 const& sectorForwardNormal, float sectorApertureDegree, float sectorRadius)
{
	if(GetVectorDistanceSquared2D(point, sectorTip) > sectorRadius * sectorRadius)
	{
		return false;
	}

    Vec2 tipToPoint = point - sectorTip;

    if(GetDegreesBetweenVectors2D(tipToPoint, sectorForwardNormal) > sectorApertureDegree / 2)
    {
        return false;
    }

    return true;

}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool IsPointInsideCapsule2D(Vec2 const& point, Capsule2 const& capsule)
{
    Vec2 boneStart = capsule.m_start;
    Vec2 boneEnd = capsule.m_end;
    float radius = capsule.m_radius;

    return IsPointInsideCapsule2D(point, boneStart, boneEnd, radius);
}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool IsPointInsideCapsule2D(Vec2 const& point, Vec2 const& boneStart, Vec2 const& boneEnd, float radius)
{

    Vec2 pointOnBone = GetNearestPointOnLineSegment2D(point, boneEnd, boneStart);

    Vec2 pointBoneDistance = pointOnBone - point;
    
    float distance = pointBoneDistance.GetLength();

    return distance < radius;
}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool IsPointInsideAABB2D(Vec2 const& point, AABB2 const& box)
{
    return box.IsPointInside(point);
}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool IsPointInsideOBB2D(Vec2 const& point, OBB2 const& box)
{
    Vec2 pointLocalCoords = box.GetLocalPosForWorldPos(point);

    if(pointLocalCoords.x >= box.m_halfDimension.x)
    {
        return false;
    }

	if(pointLocalCoords.x <= -box.m_halfDimension.x)
	{
		return false;
	}

	if(pointLocalCoords.y >= box.m_halfDimension.y)
	{
		return false;
	}

	if(pointLocalCoords.y <= -box.m_halfDimension.y)
	{
		return false;
	}

    return true;

}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool IsPointInsideTriangle2D(Vec2 const& referencePoint, Vec2 const& triCCW0, Vec2 const& triCCW1, Vec2 const& triCCW2)
{

    Vec2 aToPointDistance = referencePoint - triCCW0;
    Vec2 aToBDistance = triCCW1 - triCCW0;
    Vec2 perpendicularToAB = aToBDistance.GetRotated90Degrees();

    if(DotProduct2D(aToPointDistance, perpendicularToAB) <= 0.f)
    {
        return false;
    }

	Vec2 bToPointDistance = referencePoint - triCCW1;
	Vec2 bToCDistance = triCCW2 - triCCW1;
	Vec2 perpendicularToBC = bToCDistance.GetRotated90Degrees();

	if(DotProduct2D(bToPointDistance, perpendicularToBC) <= 0.f)
	{
		return false;
	}


	Vec2 cToPointDistance = referencePoint - triCCW2;
	Vec2 cToADistance = triCCW0 - triCCW2;
	Vec2 perpendicularToCA = cToADistance.GetRotated90Degrees();

	if(DotProduct2D(cToPointDistance, perpendicularToCA) <= 0.f)
	{
		return false;
	}

    return true;

}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool IsPointInsideTriangle2D(Vec2 const& referencePoint, Triangle2 const& triangle)
{
    Vec2 triCCW0 = triangle.counterClockwisePointOne;
    Vec2 triCCW1 = triangle.counterClockwisePointTwo;
    Vec2 triCCW2 = triangle.counterClockwisePointThree;

    return IsPointInsideTriangle2D(referencePoint, triCCW0, triCCW1, triCCW2);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool IsPointInsideAABB3D(Vec3 const& point, AABB3 const& box)
{
    return (point.x > box.m_mins.x && point.y > box.m_mins.y && point.z > box.m_mins.z) && (point.x < box.m_maxs.x && point.y < box.m_maxs.y && point.z < box.m_maxs.z);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool IsPointInsideSphere(Vec3 const& point, Sphere const& sphere)
{
	if(GetVectorDistanceSquared3D(point, sphere.m_center) > sphere.m_radius * sphere.m_radius)
	{
		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool IsPointInsideCylinder3D(Vec3 const& point, Cylinder3D const& cylinder)
{

    if(IsPointInsideDisc2D(Vec2(point.x, point.y), Vec2(cylinder.m_startPosition.x, cylinder.m_startPosition.y), cylinder.m_radius))
    {
        if(point.z > cylinder.m_startPosition.z && point.z < cylinder.m_startPosition.z + cylinder.m_height)
        {
            return true;
        }
    }
    return false;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool IsPointInsideOBB3D(Vec3 const& point, OBB3 const& obb)
{
	Vec3 pointLocalCoords = obb.GetLocalPositionForWorldPosition(point);

	if(pointLocalCoords.x >= obb.m_halfDimensions.x)
	{
		return false;
	}
	else if(pointLocalCoords.x <= -obb.m_halfDimensions.x)
	{
		return false;
	}
	else if(pointLocalCoords.y >= obb.m_halfDimensions.y)
	{
		return false;
	}
	else if(pointLocalCoords.y <= -obb.m_halfDimensions.y)
	{
		return false;
	}
	else if(pointLocalCoords.z >= obb.m_halfDimensions.z)
	{
		return false;
	}
	else if(pointLocalCoords.z <= -obb.m_halfDimensions.z)
	{
		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool IsPointInFrontOfPlane(Vec3 const& referencePoint, Plane3 const& plane)
{
    return plane.GetAltitudeFromPoint(referencePoint) > 0.f;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Vec2 const GetNearestPointOnDisc2D(Vec2 const& referencePosition, Vec2 const& discCenter, float discRadius)
{

    Vec2 centerToPointDistanceVector =  referencePosition - discCenter;

    if(centerToPointDistanceVector.GetLengthSquared() <= discRadius * discRadius)
    {
        return referencePosition;
    }
    
    centerToPointDistanceVector.SetLength(discRadius);

    
    return centerToPointDistanceVector + discCenter;

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Vec2 const GetNearestPointOnCapsule2D(Vec2 const& referencePosition, Capsule2 const& capsule)
{
	Vec2 boneStart = capsule.m_start;
	Vec2 boneEnd = capsule.m_end;
	float radius = capsule.m_radius;

    return GetNearestPointOnCapsule2D(referencePosition, boneStart, boneEnd, radius);
}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Vec2 const GetNearestPointOnCapsule2D(Vec2 const& referencePosition, Vec2 const& boneStart, Vec2 const& boneEnd, float radius)
{
    Vec2 pointOnBone = GetNearestPointOnLineSegment2D(referencePosition, boneEnd, boneStart);

    Vec2 distanceFromBoneToPoint = referencePosition - pointOnBone;

    float clampedLength = GetClamped(distanceFromBoneToPoint.GetLength(), 0.f, radius);

    distanceFromBoneToPoint.SetLength(clampedLength);

    return pointOnBone + distanceFromBoneToPoint;
}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Vec2 const GetNearestPointOnInfiniteLine2D(Vec2 const& referencePosition, LineSegment2 const& infiniteLine)
{

    Vec2 pointOnLine = infiniteLine.m_start;
    Vec2 anotherPointOnLine = infiniteLine.m_end;


    return GetNearestPointOnInfiniteLine2D(referencePosition, pointOnLine, anotherPointOnLine);
}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Vec2 const GetNearestPointOnInfiniteLine2D(Vec2 const& referencePosition, Vec2 const& pointOnLine, Vec2 const& anotherPointOnLine)
{
	Vec2 startToEnd = anotherPointOnLine - pointOnLine;

	Vec2 startToPoint = referencePosition - pointOnLine;

	Vec2 nearestPoint = GetProjectedOnto2D(startToPoint, startToEnd);

	return pointOnLine + nearestPoint;
}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Vec2 const GetNearestPointOnLineSegment2D(Vec2 const& referencePosition, LineSegment2 const& lineSegment)
{
    Vec2 lineSegStart = lineSegment.m_start;
    Vec2 lineSegEnd = lineSegment.m_end;

    return GetNearestPointOnLineSegment2D(referencePosition, lineSegEnd, lineSegStart);
}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Vec2 const GetNearestPointOnLineSegment2D(Vec2 const& referencePosition, Vec2 const& lineSegEnd, Vec2 const& lineSegStart)
{

    Vec2 startToEnd = lineSegEnd - lineSegStart;

    Vec2 startToPoint = referencePosition - lineSegStart;

    if(DotProduct2D(startToEnd, startToPoint) <= 0.f)
    {
        return lineSegStart;
    }

    Vec2 endToPoint = referencePosition - lineSegEnd;

    if(DotProduct2D(startToEnd, endToPoint) >= 0.f)
    {
        return lineSegEnd;
    }

    Vec2 nearestPoint = GetProjectedOnto2D(startToPoint, startToEnd);

    return lineSegStart + nearestPoint;

}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Vec2 const GetNearestPointOnAABB2D(Vec2 const& referencePosition, AABB2 const& box)
{
    return box.GetNearestPoint(referencePosition);
}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Vec2 const GetNearestPointOnOBB2D(Vec2 const& referencePosition, OBB2 const& orientedBox)
{
    Vec2 localPosition = orientedBox.GetLocalPosForWorldPos(referencePosition);

    float x = GetClamped(localPosition.x, -orientedBox.m_halfDimension.x, orientedBox.m_halfDimension.x);
    float y = GetClamped(localPosition.y, -orientedBox.m_halfDimension.y, orientedBox.m_halfDimension.y);

    return orientedBox.GetWorldPosForLocalPos(Vec2(x, y));
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Vec2 const GetNearestPointOnTriangle2D(Vec2 const& referencePoint, Vec2 const& triCCW0, Vec2 const& triCCW1, Vec2 const& triCCW2)
{

    if(IsPointInsideTriangle2D(referencePoint, triCCW0, triCCW1, triCCW2))
    {
        return referencePoint;
    }

    LineSegment2 edgeOne = LineSegment2(triCCW0, triCCW1);
    LineSegment2 edgeTwo = LineSegment2(triCCW1, triCCW2);
    LineSegment2 edgeThree = LineSegment2(triCCW2, triCCW0);

    Vec2 nearestPointOnEdgeOne = GetNearestPointOnLineSegment2D(referencePoint, edgeOne);
    Vec2 nearestPointOnEdgeTwo = GetNearestPointOnLineSegment2D(referencePoint, edgeTwo);
    Vec2 nearestPointOnEdgeThree = GetNearestPointOnLineSegment2D(referencePoint, edgeThree);

    Vec2 distanceFromPointOne = referencePoint - nearestPointOnEdgeOne;
    Vec2 distanceFromPointTwo = referencePoint - nearestPointOnEdgeTwo;
    Vec2 distanceFromPointThree = referencePoint - nearestPointOnEdgeThree;

	Vec2 trueNearestPoint = nearestPointOnEdgeOne;

    if(distanceFromPointOne.GetLength() > distanceFromPointTwo.GetLength())
    {
        if(distanceFromPointTwo.GetLength() > distanceFromPointThree.GetLength())
        {
            trueNearestPoint = nearestPointOnEdgeThree;
        }
        else
        {
            trueNearestPoint = nearestPointOnEdgeTwo;
        }
    }
    else
    {
		if(distanceFromPointOne.GetLength() > distanceFromPointThree.GetLength())
		{
			trueNearestPoint = nearestPointOnEdgeThree;
		}
        else
        {
            trueNearestPoint = nearestPointOnEdgeOne;
        }
    }

    return trueNearestPoint;

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Vec2 const GetNearestPointOnTriangle2D(Vec2 const& referencePoint, Triangle2 const& triangle)
{

    Vec2 ccw0 = triangle.counterClockwisePointOne;
    Vec2 ccw1 = triangle.counterClockwisePointTwo;
    Vec2 ccw2 = triangle.counterClockwisePointThree;

    return GetNearestPointOnTriangle2D(referencePoint, ccw0, ccw1, ccw2);

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Vec3 const GetNearestPointOnAABB3D(Vec3 const& referencePoint, AABB3 const& box)
{
    Vec3 nearestPoint;

    nearestPoint.x = GetClamped(referencePoint.x, box.m_mins.x, box.m_maxs.x);
    nearestPoint.y = GetClamped(referencePoint.y, box.m_mins.y, box.m_maxs.y);
    nearestPoint.z = GetClamped(referencePoint.z, box.m_mins.z, box.m_maxs.z);

    return nearestPoint;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Vec3 const GetNearestPointOnSphere(Vec3 const& referencePoint, Sphere const& sphere)
{

	Vec3 centerToPointDistanceVector = referencePoint - sphere.m_center;

	if(centerToPointDistanceVector.GetLengthSquared() <= sphere.m_radius * sphere.m_radius)
	{
		return referencePoint;
	}

	centerToPointDistanceVector.SetLength(sphere.m_radius);


	return centerToPointDistanceVector + sphere.m_center;

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Vec3 const GetNearestPointOnCylinder3D(Vec3 const& referencePoint, Cylinder3D const& cylinder)
{

    Vec3 nearestPoint = referencePoint;

    if(IsPointInsideCylinder3D(referencePoint, cylinder))
    {
        return referencePoint;
    }

	if(referencePoint.z < cylinder.m_startPosition.z)
	{
        nearestPoint.z = cylinder.m_startPosition.z;
	}
    
    if(referencePoint.z > cylinder.m_startPosition.z + cylinder.m_height)
	{
        nearestPoint.z = cylinder.m_startPosition.z + cylinder.m_height;
	}
    
    Vec2 discNearestPoint = GetNearestPointOnDisc2D(Vec2(nearestPoint.x, nearestPoint.y), Vec2(cylinder.m_startPosition.x, cylinder.m_startPosition.y), cylinder.m_radius);

    nearestPoint.x = discNearestPoint.x;
    nearestPoint.y = discNearestPoint.y;

    return nearestPoint;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Vec3 const GetNearestPointOnOBB3D(Vec3 const& referencePoint, OBB3 const& obb)
{
    Vec3 referencePointInBoxLocal = obb.GetLocalPositionForWorldPosition(referencePoint);

    float nearestX = GetClamped(referencePointInBoxLocal.x, -obb.m_halfDimensions.x, obb.m_halfDimensions.x);
    float nearestY = GetClamped(referencePointInBoxLocal.y, -obb.m_halfDimensions.y, obb.m_halfDimensions.y);
    float nearestZ = GetClamped(referencePointInBoxLocal.z, -obb.m_halfDimensions.z, obb.m_halfDimensions.z);

    return obb.GetWorldPositionForLocalPosition(Vec3(nearestX, nearestY, nearestZ));
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Vec3 const GetNearestPointOnPlane3D(Vec3 const& referencePoint, Plane3 const& plane)
{
    float distanceFromOriginToPoint = DotProduct3D(referencePoint, plane.m_normal);
    float altitude = distanceFromOriginToPoint - plane.m_distanceFromOrigin;

    Vec3 altitudeVector = plane.m_normal * altitude;

    Vec3 nearestPoint = referencePoint - altitudeVector;

    return nearestPoint;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool PushDiscOutOfFixedPoint2D(Vec2& mobileDiscCenter, float discRadius, Vec2 const& fixedPointPos)
{

    Vec2 pointAndCenterDistanceVector = mobileDiscCenter - fixedPointPos;
    
    if(pointAndCenterDistanceVector.GetLengthSquared() >= discRadius * discRadius)
    {
        return false;
    }

    pointAndCenterDistanceVector.SetLength(discRadius);

    mobileDiscCenter = fixedPointPos + pointAndCenterDistanceVector;

    return true;

}

bool PushDiscOutOfFixedDisc2D(Vec2& mobileDiscCenter, float discRadius, Vec2 const& fixedDiscCenter, float fixedDiscRadius)
{
	
    Vec2 centerDistanceVector = mobileDiscCenter - fixedDiscCenter;

	if(centerDistanceVector.GetLengthSquared() >= (discRadius + fixedDiscRadius) * (discRadius + fixedDiscRadius))
	{
		return false;
	}

	centerDistanceVector.SetLength(fixedDiscRadius + discRadius);

	mobileDiscCenter = fixedDiscCenter + centerDistanceVector;

	return true;

}

bool PushDiscsOutOfEachOther2D(Vec2& bottomOrLeftCircleCenter, float bottomOrLeftCircleRadius, Vec2& topOrRightCircleCenter, float topOrRightCircleRadius)
{

    Vec2 centerDistanceVector = topOrRightCircleCenter - bottomOrLeftCircleCenter;
    
    if(centerDistanceVector.GetLengthSquared() >= (bottomOrLeftCircleRadius + topOrRightCircleRadius) * (bottomOrLeftCircleRadius + topOrRightCircleRadius))
    {
	    return false;
    }
    
    float overlap = (bottomOrLeftCircleRadius + topOrRightCircleRadius) - centerDistanceVector.GetLength();

    centerDistanceVector.SetLength(overlap / 2);

    bottomOrLeftCircleCenter -= centerDistanceVector;
    topOrRightCircleCenter += centerDistanceVector;

    return true;

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool PushDiscOutOfAABB2D(Vec2& mobileDiscCenter, float discRadius, AABB2 const& fixedBox)
{
    Vec2 pointOnBox = fixedBox.GetNearestPoint(mobileDiscCenter);

    return PushDiscOutOfFixedPoint2D(mobileDiscCenter, discRadius, pointOnBox);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool BounceDiscOutOfFixedDisc(Vec2& mobileDiscCenter, float discRadius, Vec2 const& fixedDiscCenter, float fixedDiscRadius, Vec2& discVelocity, float elasticity, float friction)
{  
    if(!PushDiscOutOfFixedDisc2D(mobileDiscCenter, discRadius, fixedDiscCenter, fixedDiscRadius))
    {
		return false;
    }
    
	Vec2 normal = (fixedDiscCenter - mobileDiscCenter).GetNormalized();

	Vec2 velocityAlongNormal = normal * DotProduct2D(normal, discVelocity);
	Vec2 tangentialVelocity = discVelocity - velocityAlongNormal;
	discVelocity = (tangentialVelocity)+(-velocityAlongNormal * elasticity * friction);
	return true;
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool BounceDiscOutOfDisc(Vec2& discACenter, float discARadius, Vec2& discAVelocity, float discAElasticity, Vec2& discBCenter, float discBRadius, Vec2& discBVelocity, float discBElasticity)
{
    if(!PushDiscsOutOfEachOther2D(discACenter, discARadius, discBCenter, discBRadius))
    {    
        return false;
    }

	Vec2 bVelReltaiveToA = discAVelocity - discBVelocity;

    float relativeDirection = DotProduct2D(discACenter - discBCenter, bVelReltaiveToA);

    if(relativeDirection > 0.f)
    {
        return false;
    }

	Vec2 normal = (discBCenter - discACenter).GetNormalized();

	Vec2 discAVelocityAlongNormal = normal * DotProduct2D(normal, discAVelocity);
	Vec2 discATangentialVelocity = discAVelocity - discAVelocityAlongNormal;

	Vec2 discBVelocityAlongNormal = normal * DotProduct2D(normal, discBVelocity);
	Vec2 discBTangentialVelocity = discBVelocity - discBVelocityAlongNormal;

	discAVelocity = (discATangentialVelocity)+(discBVelocityAlongNormal * (discAElasticity * discBElasticity));
	discBVelocity = (discBTangentialVelocity)+(discAVelocityAlongNormal * (discBElasticity * discAElasticity));

    return true;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool BounceDiscOutOfFixedPoint(Vec2& mobileDiscCenter, float discRadius, Vec2& discVelocity, float elasticity, Vec2 const& fixedPoint, float friction)
{
	if(!PushDiscOutOfFixedPoint2D(mobileDiscCenter, discRadius, fixedPoint))
	{
		return false;
	}

	Vec2 normal = (fixedPoint - mobileDiscCenter).GetNormalized();

	Vec2 velocityAlongNormal = normal * DotProduct2D(normal, discVelocity);
	Vec2 tangentialVelocity = discVelocity - velocityAlongNormal;
	discVelocity = (tangentialVelocity)+(-velocityAlongNormal * elasticity * friction);

	return true;

}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------


void TransformPosition2D(Vec2& posToTransform, float uniformScale, float rotationDegrees, Vec2 const& moveDistance)
{

    posToTransform *= uniformScale;
    
    float r = sqrtf((posToTransform.x * posToTransform.x) + (posToTransform.y * posToTransform.y));
    float angle = ATan2Degrees(posToTransform.y, posToTransform.x);

    angle += rotationDegrees;

    posToTransform.x = r * CosDegrees(angle);
    posToTransform.y = r * SinDegrees(angle);

    posToTransform += moveDistance;

}

void TransformPosition2D(Vec2& posToTransform, Vec2 const& iBasis, Vec2 const& jBasis, Vec2 const& moveDistance)
{
   
    Vec2 iInWorld = posToTransform.x * iBasis;
    Vec2 jInWorld = posToTransform.y * jBasis;

    posToTransform = moveDistance + (iInWorld + jInWorld);

}



void TransformPositionXY3D(Vec3& posToTransform, float scaleXY, float rotationDegrees, Vec2 const& moveDistance)
{
    posToTransform.x *= scaleXY;
    posToTransform.y *= scaleXY;

    float r = sqrtf((posToTransform.x * posToTransform.x) + (posToTransform.y * posToTransform.y));
    float angle = ATan2Degrees(posToTransform.y, posToTransform.x);

    angle += rotationDegrees;

    posToTransform.x = r * CosDegrees(angle);
    posToTransform.y = r * SinDegrees(angle);

    posToTransform.x += moveDistance.x;
    posToTransform.y += moveDistance.y;

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void TransformPositionXY3D(Vec3& posToTransform, Vec2 const& iBasis, Vec2 const& jBasis, Vec2 const& moveDistance)
{

	Vec2 iInWorld = posToTransform.x * iBasis;
	Vec2 jInWorld = posToTransform.y * jBasis;

	posToTransform.x = moveDistance.x + (iInWorld.x + jInWorld.x);
	posToTransform.y = moveDistance.y + (iInWorld.y + jInWorld.y);

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void TransformPosition3D(Vec3& posToTransform, Mat44 const& transform)
{
    transform.TransformPosition3D(posToTransform);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Mat44 GetLookAtTransform(Vec3 currentPosition, Vec3 targetPosition)
{
    Mat44 lookAtMatrix;

    Vec3 directionVector = targetPosition - currentPosition;
    Vec3 iBasis = directionVector.GetNormalized();
    Vec3 jBasis;
    Vec3 kBasis;

    Vec3 X = Vec3(1.f, 0.f, 0.f);
    Vec3 Y = Vec3(0.f, 1.f, 0.f);
    Vec3 Z = Vec3(0.f, 0.f, 1.f);


    if(abs(DotProduct3D(iBasis, Z)) < 0.999f)
    {
        Vec3 crossZI = CrossProduct3D(Z, iBasis);
        jBasis = crossZI.GetNormalized();
        kBasis = CrossProduct3D(iBasis, jBasis);
    }
    else
    {
        Vec3 crossIY = CrossProduct3D(iBasis, Y);
        kBasis = crossIY.GetNormalized();
        jBasis = CrossProduct3D(kBasis, iBasis);

    }

    lookAtMatrix.SetIJK3D(iBasis, jBasis, kBasis);

    return lookAtMatrix;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Mat44 GetBillboardTransform(BillboardType billboardType, Mat44 const& targetTransform, Vec3 const& billboardPosition, Vec2 const& billboardScale)
{

    UNUSED(billboardScale);

    Mat44 billboardTransform;

    switch(billboardType)
    {
        case BillboardType::WORLD_UP_FACING:
        {
            
            Vec3 targetIBasis;
            Vec3 targetJBasis;
            Vec3 targetKBasis;
            Vec3 targetTranslation;

            targetTransform.GetIJKT3D(targetIBasis, targetJBasis, targetKBasis, targetTranslation);

            Vec3 transformForward = targetTranslation - billboardPosition;
            transformForward.z = 0.f;

            transformForward = transformForward.GetNormalized();

            Vec3 transformLeft = CrossProduct3D(targetKBasis, transformForward);
            billboardTransform.SetIJK3D(transformForward, transformLeft, Vec3::UP);
            
            return billboardTransform;
        }
        case BillboardType::WORLD_UP_OPPOSING:
		{
			Vec3 targetIBasis;
			Vec3 targetJBasis;
			Vec3 targetKBasis;
			Vec3 targetTranslation;

			targetTransform.GetIJKT3D(targetIBasis, targetJBasis, targetKBasis, targetTranslation);

			Vec3 transformForward = -targetIBasis;
			transformForward.z = 0.f;

			transformForward = transformForward.GetNormalized();

			Vec3 transformUp = targetKBasis;

			Vec3 transformLeft = CrossProduct3D(transformUp, transformForward);
			billboardTransform.SetIJK3D(transformForward, transformLeft, transformUp);

			return billboardTransform;
		}

        case BillboardType::FULL_FACING:
        {

			Vec3 targetTranslation = targetTransform.GetTranslation3D();
            return GetLookAtTransform(billboardPosition, targetTranslation);

        }

        case BillboardType::FULL_OPPOSING:
        {
			Vec3 transformIBasis = - targetTransform.GetIBasis3D();
			Vec3 transformJBasis = - targetTransform.GetJBasis3D();
			Vec3 transformKBasis = targetTransform.GetKBasis3D();

            billboardTransform.SetIJK3D(transformIBasis, transformJBasis, transformKBasis);

            return billboardTransform;

        }
        
        default:
            return billboardTransform;
    }

}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
float DotProduct2D(Vec2 const& vecA, Vec2 const& vecB)
{

    float xProd = vecA.x * vecB.x;
    float yProd = vecA.y * vecB.y;

    float dotProduct = xProd + yProd;

    return dotProduct;

}



float DotProduct3D(Vec3 const& vecA, Vec3 const& vecB)
{

	float xProd = vecA.x * vecB.x;
	float yProd = vecA.y * vecB.y;
	float zProd = vecA.z * vecB.z;

	float dotProduct = xProd + yProd + zProd;

	return dotProduct;

}

//------------------------------------------------------------------------------------------------------------------
float DotProduct4D(Vec4 const& vecA, Vec4 const& vecB)
{

	float xProd = vecA.x * vecB.x;
	float yProd = vecA.y * vecB.y;
	float zProd = vecA.z * vecB.z;
	float wProd = vecA.w * vecB.w;

	float dotProduct = xProd + yProd + zProd + wProd;

	return dotProduct;

}

//------------------------------------------------------------------------------------------------------------------
float CrossProduct2D(Vec2 const& vecA, Vec2 const& vecB)
{
    float zScalar = (vecA.x * vecB.y) - (vecA.y * vecB.x);

    return zScalar;
}


//------------------------------------------------------------------------------------------------------------------
Vec3 CrossProduct3D(Vec3 const& vecA, Vec3 const& vecB)
{
    float iComp = (vecA.y * vecB.z) - (vecA.z * vecB.y);
    float jComp = (vecA.x * vecB.z) - (vecA.z * vecB.x);
    float kComp = (vecA.x * vecB.y) - (vecA.y * vecB.x);

    return Vec3(iComp, -jComp, kComp);
}

//------------------------------------------------------------------------------------------------------------------
float NormalizeByte(unsigned char value)
{
    float inputToFloat = static_cast<float>(value);

    float normalizedByte = RangeMapClamped(inputToFloat, 0.f, 255.f, 0.f, 1.f);

    return normalizedByte;
}

//------------------------------------------------------------------------------------------------------------------s
unsigned char DenormalizeByte(float value)
{
    float clampedFloat = GetClamped(value * 256.f, 0.f, 255.f);

    unsigned char denormalized = static_cast<unsigned char>(clampedFloat);

    return denormalized;

}
