#include "VertexUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RaycastUtils.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/Vertex_PCUTBN.hpp"
#include "Engine/Math/Capsule2.hpp"
#include "Engine/Math/LineSegment2.hpp"
#include "Engine/Math/LineSegment3.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/OBB3.hpp"
#include "Engine/Math/Disc2.hpp"
#include "Engine/Math/Sphere.hpp"
#include "Engine/Math/Cylinder3D.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/Triangle2.hpp"
#include "Engine/Core/DebugRender.hpp"

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void TransformVertexArrayXY3D(int numVerts, Vertex_PCU* verts, float uniformScaleXY, float rotationDegreesAboutZ, Vec2 const& translationXY)
{

	for(int vertex = 0; vertex < numVerts; ++vertex)
	{
		Vec3& position = verts[vertex].m_position;
		
		TransformPositionXY3D(position, uniformScaleXY, rotationDegreesAboutZ, translationXY);
	}

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void TransformVertexArrayXY3D(std::vector<Vertex_PCU>& verts, float uniformScaleXY, float rotationDegreesAboutZ, Vec2 const& translationXY)
{

	for(int vertex = 0; vertex < static_cast<int>(verts.size()); ++vertex)
	{
		Vec3& position = verts[vertex].m_position;

		TransformPositionXY3D(position, uniformScaleXY, rotationDegreesAboutZ, translationXY);
	}

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void TransformVertexArray3D(std::vector<Vertex_PCU>& verts, Mat44 const& transform)
{
	for(int vertex = 0; vertex < static_cast<int>(verts.size()); ++vertex)
	{
		Vec3& position = verts[vertex].m_position;

		position = transform.TransformPosition3D(position);
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void TransformVertexArray3DForPartialVector(std::vector<Vertex_PCU>& verts, Mat44 const& transform, int startPos, int endPos)
{

	for(int vertex = startPos; vertex <= endPos; ++vertex)
	{
		Vec3& position = verts[vertex].m_position;

		position = transform.TransformPosition3D(position);
	}

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
AABB2 GetVertexBounds(std::vector<Vertex_PCU>& verts)
{
	AABB2 vertexBounds;
	vertexBounds.m_mins = Vec2(verts[0].m_position.x, verts[0].m_position.y);
	vertexBounds.m_maxs = Vec2(verts[0].m_position.x, verts[0].m_position.y);

	for(size_t index = 0; index < verts.size(); ++index)
	{
		if(verts[index].m_position.x < vertexBounds.m_mins.x)
		{
			vertexBounds.m_mins.x = verts[index].m_position.x;
		}

		if(verts[index].m_position.x > vertexBounds.m_maxs.x)
		{
			vertexBounds.m_maxs.x = verts[index].m_position.x;
		}

		if(verts[index].m_position.y < vertexBounds.m_mins.y)
		{
			vertexBounds.m_mins.y = verts[index].m_position.y;
		}

		if(verts[index].m_position.y > vertexBounds.m_maxs.y)
		{
			vertexBounds.m_maxs.y = verts[index].m_position.y;
		}
	}

	return vertexBounds;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void AddVertsForCapsule2D(std::vector<Vertex_PCU>& verts, Vec2 const& boneStart, Vec2 const& boneEnd, float radius, Rgba8 const& color)
{
	
	Vec2 forwardStep = boneEnd - boneStart;
	forwardStep.SetLength(radius);

	Vec2 leftStep = forwardStep.GetRotated90Degrees();

	Vec2 endBoxLeftPoint = boneEnd + leftStep;
	Vec2 endBoxRightPoint = boneEnd - leftStep;
	
	Vec2 startBoxLeftPoint = boneStart + leftStep;
	Vec2 startBoxRightPoint = boneStart - leftStep;

	verts.push_back(Vertex_PCU(endBoxLeftPoint, color));  
	verts.push_back(Vertex_PCU(startBoxLeftPoint, color)); 
	verts.push_back(Vertex_PCU(startBoxRightPoint, color)); 
	
	verts.push_back(Vertex_PCU(startBoxRightPoint, color)); 
	verts.push_back(Vertex_PCU(endBoxRightPoint, color));  
	verts.push_back(Vertex_PCU(endBoxLeftPoint, color));


	constexpr int NUM_SLICES = 32;
	constexpr float DEGREES_PER_SLICE = 180.f / NUM_SLICES;

	float boneForwardOrientationTheta = forwardStep.GetOrientationDegrees(); // angle the capsule is facing
	float endCapsuleStartAngle = boneForwardOrientationTheta - 90.f; // starts from right end goes to left
	float startCapsuleStartAngle = endCapsuleStartAngle + 180.f; // starts from left end goes to right

	for(int sliceNum = 0; sliceNum < NUM_SLICES; ++sliceNum)
	{
		float sliceStartTheta = endCapsuleStartAngle + (DEGREES_PER_SLICE * static_cast<float>(sliceNum));
		float sliceEndTheta = sliceStartTheta + DEGREES_PER_SLICE;

		Vec2 sliceStartDisp = Vec2::MakeFromPolarDegrees(sliceStartTheta, radius);
		Vec2 sliceEndDisp = Vec2::MakeFromPolarDegrees(sliceEndTheta, radius);

		Vec2 sliceStartOuterPos = boneEnd + sliceStartDisp;
		Vec2 sliceEndOuterPos = boneEnd + sliceEndDisp;

		verts.push_back(Vertex_PCU(boneEnd, color));
		verts.push_back(Vertex_PCU(sliceStartOuterPos, color));
		verts.push_back(Vertex_PCU(sliceEndOuterPos, color));
	}


	for(int sliceNum = 0; sliceNum < NUM_SLICES; ++sliceNum)
	{
		float sliceStartTheta = startCapsuleStartAngle + (DEGREES_PER_SLICE * static_cast<float>(sliceNum));
		float sliceEndTheta = sliceStartTheta + DEGREES_PER_SLICE;

		Vec2 sliceStartDisp = Vec2::MakeFromPolarDegrees(sliceStartTheta, radius);
		Vec2 sliceEndDisp = Vec2::MakeFromPolarDegrees(sliceEndTheta, radius);

		Vec2 sliceStartOuterPos = boneStart + sliceStartDisp;
		Vec2 sliceEndOuterPos = boneStart + sliceEndDisp;

		verts.push_back(Vertex_PCU(boneStart, color));
		verts.push_back(Vertex_PCU(sliceStartOuterPos, color));
		verts.push_back(Vertex_PCU(sliceEndOuterPos, color));
	}

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void AddVertsForCapsule2D(std::vector<Vertex_PCU>& verts, Capsule2 const& capsule, Rgba8 const& color)
{

	Vec2 boneStart = capsule.m_start;
	Vec2 boneEnd = capsule.m_end;
	float radius = capsule.m_radius;

	AddVertsForCapsule2D(verts, boneStart, boneEnd, radius, color);

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void AddVertsForDisc2D(std::vector<Vertex_PCU>&verts, Vec2 const& center, float radius, Rgba8 const& color)
{

	constexpr int NUM_SLICES = 32;
	constexpr float DEGREES_PER_SLICE = 360.f / NUM_SLICES;


	for(int sliceNum = 0; sliceNum < NUM_SLICES; ++sliceNum)
	{
		float sliceStartAngle = DEGREES_PER_SLICE * static_cast<float>(sliceNum);
		float sliceEndAngle = sliceStartAngle + DEGREES_PER_SLICE;

		Vec2 sliceStartDisp = Vec2::MakeFromPolarDegrees(sliceStartAngle, radius);
		Vec2 sliceEndDisp = Vec2::MakeFromPolarDegrees(sliceEndAngle, radius);
		
		Vec2 sliceStartOuterPos = center + sliceStartDisp;
		Vec2 sliceEndOuterPos = center + sliceEndDisp;

		verts.push_back(Vertex_PCU(center, color));
		verts.push_back(Vertex_PCU(sliceStartOuterPos, color));
		verts.push_back(Vertex_PCU(sliceEndOuterPos, color));
	}

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void AddVertsForDisc2D(std::vector<Vertex_PCU>& verts, Disc2 disc, Rgba8 const& color)
{

	Vec2 center = disc.m_center;
	float radius = disc.m_radius;

	AddVertsForDisc2D(verts, center, radius, color);

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void AddVertsForAABB2D(std::vector<Vertex_PCU>& verts, AABB2 const& bounds, Rgba8 const& color)
{
	Vec2 bottomLeft = bounds.m_mins;
	Vec2 topRight = bounds.m_maxs;
	Vec2 topLeft = Vec2(bottomLeft.x, topRight.y);
	Vec2 bottomRight = Vec2(topRight.x, bottomLeft.y);

	verts.push_back(Vertex_PCU(bottomLeft,	color, Vec2(0.f, 0.f)));
	verts.push_back(Vertex_PCU(bottomRight, color, Vec2(1.f, 0.f)));
	verts.push_back(Vertex_PCU(topRight,	color, Vec2(1.f, 1.f)));

	verts.push_back(Vertex_PCU(bottomLeft,	color, Vec2(0.f, 0.f)));
	verts.push_back(Vertex_PCU(topRight,	color, Vec2(1.f, 1.f)));
	verts.push_back(Vertex_PCU(topLeft,		color, Vec2(0.f, 1.f)));
}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void AddVertsForAABB2D(std::vector<Vertex_PCU>& verts, AABB2 const& bounds, Rgba8 const& color, Vec2 const& minUVs, Vec2 const& maxUVs)
{
	Vec2 bottomLeft = bounds.m_mins;
	Vec2 topRight = bounds.m_maxs;
	Vec2 topLeft = Vec2(bottomLeft.x, topRight.y);
	Vec2 bottomRight = Vec2(topRight.x, bottomLeft.y);

	Vec2 bottomLeftUVs = minUVs;
	Vec2 topRightUVs = maxUVs;
	Vec2 topLeftUVs = Vec2(minUVs.x, maxUVs.y);
	Vec2 bottomRightUVs = Vec2(maxUVs.x, minUVs.y);

	verts.push_back(Vertex_PCU(bottomLeft, color, bottomLeftUVs));
	verts.push_back(Vertex_PCU(bottomRight, color, bottomRightUVs));
	verts.push_back(Vertex_PCU(topRight, color, topRightUVs));

	verts.push_back(Vertex_PCU(bottomLeft, color, bottomLeftUVs));
	verts.push_back(Vertex_PCU(topRight, color, topRightUVs));
	verts.push_back(Vertex_PCU(topLeft, color, topLeftUVs));
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void AddVertsForAABB2D(std::vector<Vertex_PCU>& verts, AABB2 const& bounds, Rgba8 const& color, AABB2 UVs)
{
	AddVertsForAABB2D(verts, bounds, color, UVs.m_mins, UVs.m_maxs);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void AddVertsForOBB2D(std::vector<Vertex_PCU>&verts, OBB2 const& box, Rgba8 const& color)
{

	Vec2 cornerPoints[4] = {};

	box.GetCornerPoints(cornerPoints);

	verts.push_back(Vertex_PCU(cornerPoints[BOTTOM_LEFT], color, Vec2(0.f, 0.f)));
	verts.push_back(Vertex_PCU(cornerPoints[BOTTOM_RIGHT], color, Vec2(1.f, 0.f)));
	verts.push_back(Vertex_PCU(cornerPoints[TOP_RIGHT], color, Vec2(1.f, 1.f)));

	verts.push_back(Vertex_PCU(cornerPoints[BOTTOM_LEFT], color, Vec2(0.f, 0.f)));
	verts.push_back(Vertex_PCU(cornerPoints[TOP_RIGHT], color, Vec2(1.f, 1.f)));
	verts.push_back(Vertex_PCU(cornerPoints[TOP_LEFT], color, Vec2(0.f, 1.f)));
	
}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void AddVertsForLineSegment2D(std::vector<Vertex_PCU>&verts, Vec2 const& start, Vec2 const& end, float thickness, Rgba8 const& color)
{

	Vec2 forwardStep = end - start;
	forwardStep.SetLength(thickness);

	Vec2 leftStep = forwardStep.GetRotated90Degrees();

	Vec2 endBoxLeftPoint = end + leftStep;
	Vec2 endBoxRightPoint = end - leftStep;

	Vec2 startBoxLeftPoint = start + leftStep;
	Vec2 startBoxRightPoint = start - leftStep;

	verts.push_back(Vertex_PCU(endBoxLeftPoint, color));
	verts.push_back(Vertex_PCU(startBoxLeftPoint, color));
	verts.push_back(Vertex_PCU(startBoxRightPoint, color));

	verts.push_back(Vertex_PCU(startBoxRightPoint, color));
	verts.push_back(Vertex_PCU(endBoxRightPoint, color));
	verts.push_back(Vertex_PCU(endBoxLeftPoint, color));

}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void AddVertsForLineSegment2D(std::vector<Vertex_PCU>& verts, Vec2 const& start, Vec2 const& end, float thickness, Rgba8 const& startColor, Rgba8 const& endColor)
{

	Vec2 forwardStep = end - start;
	forwardStep.SetLength(thickness);

	Vec2 leftStep = forwardStep.GetRotated90Degrees();

	Vec2 endBoxLeftPoint = end + leftStep;
	Vec2 endBoxRightPoint = end - leftStep;

	Vec2 startBoxLeftPoint = start + leftStep;
	Vec2 startBoxRightPoint = start - leftStep;

	verts.push_back(Vertex_PCU(endBoxLeftPoint, endColor));
	verts.push_back(Vertex_PCU(startBoxLeftPoint, startColor));
	verts.push_back(Vertex_PCU(startBoxRightPoint, startColor));

	verts.push_back(Vertex_PCU(startBoxRightPoint, startColor));
	verts.push_back(Vertex_PCU(endBoxRightPoint, endColor));
	verts.push_back(Vertex_PCU(endBoxLeftPoint, endColor));

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void AddVertsForLineSegment2D(std::vector<Vertex_PCU>&verts, LineSegment2 const& lineSegment, float thickness, Rgba8 const& color)
{
	Vec2 start = lineSegment.m_start;
	Vec2 end = lineSegment.m_end;
	AddVertsForLineSegment2D(verts, start, end, thickness, color);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void AddVertsForTriangle2D(std::vector<Vertex_PCU>& verts, Vec2 const& ccOne, Vec2 const& ccTwo, Vec2 const& ccThree, Rgba8 const& color)
{
	verts.push_back(Vertex_PCU(ccOne, color));
	verts.push_back(Vertex_PCU(ccTwo, color));
	verts.push_back(Vertex_PCU(ccThree, color));
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void AddVertsForTriangle2D(std::vector<Vertex_PCU>& verts, Vec2 const& ccOne, Vec2 const& ccTwo, Vec2 const& ccThree, Rgba8 const& color, AABB2 UVs)
{
	verts.push_back(Vertex_PCU(ccOne, color));
	verts.push_back(Vertex_PCU(ccTwo, color));
	verts.push_back(Vertex_PCU(ccThree, color));
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void AddVertsForTriangle2D(std::vector<Vertex_PCU>& verts, Triangle2 const& triangle, Rgba8 const& color)
{
	verts.push_back(Vertex_PCU(triangle.counterClockwisePointOne, color));
	verts.push_back(Vertex_PCU(triangle.counterClockwisePointTwo, color));
	verts.push_back(Vertex_PCU(triangle.counterClockwisePointThree, color));
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void AddVertsForRay2(std::vector<Vertex_PCU>& verts, Vec2 tailPos, Vec2 tipPos, float arrowSize, float lineThickness, Rgba8 const& color)
{

	AddVertsForLineSegment2D(verts, tailPos, tipPos, lineThickness, color);

	float lineAngle = (tailPos - tipPos).GetOrientationDegrees();

	Vec2 arrowHeadLeft = tipPos + Vec2::MakeFromPolarDegrees(lineAngle - 45.f, arrowSize);
	Vec2 arrowHeadRight = tipPos + Vec2::MakeFromPolarDegrees(lineAngle + 45.f, arrowSize);
	
	AddVertsForLineSegment2D(verts, tipPos, arrowHeadLeft, lineThickness, color);
	AddVertsForLineSegment2D(verts, tipPos, arrowHeadRight, lineThickness, color);

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void AddVertsForRing(std::vector<Vertex_PCU>& verts, Vec2 const& center, float radius, float thickness, Rgba8 const& color)
{

	float halfThickness = 0.5f * thickness;
	float innerCircleRadius = radius - halfThickness;
	float outerCircleRadius = radius + halfThickness;

	constexpr int NUM_SIDES = 32;
	constexpr float ANGLE_BETWEEEN_SIDES = 360.f / (float)NUM_SIDES;
	
	for(int sideNum = 0; sideNum < NUM_SIDES; ++sideNum)
	{
		float startDegrees = ANGLE_BETWEEEN_SIDES * static_cast<float>(sideNum);
		float endDegrees = ANGLE_BETWEEEN_SIDES * static_cast<float>(sideNum + 1);
		float cosStart = CosDegrees(startDegrees);
		float sinStart = SinDegrees(startDegrees);
		float cosEnd = CosDegrees(endDegrees);
		float sinEnd = SinDegrees(endDegrees);

		Vec3 innerStartPos = Vec3(center.x + innerCircleRadius * cosStart, center.y + innerCircleRadius * sinStart, 0.f);
		Vec3 outerStartPos = Vec3(center.x + outerCircleRadius * cosStart, center.y + outerCircleRadius * sinStart, 0.f);
		Vec3 innerEndPos = Vec3(center.x + innerCircleRadius * cosEnd, center.y + innerCircleRadius * sinEnd, 0.f);
		Vec3 outerEndPos = Vec3(center.x + outerCircleRadius * cosEnd, center.y + outerCircleRadius * sinEnd, 0.f);

		verts.push_back(Vertex_PCU(innerEndPos, color));
		verts.push_back(Vertex_PCU(innerStartPos, color));
		verts.push_back(Vertex_PCU(outerStartPos, color));

		verts.push_back(Vertex_PCU(outerEndPos, color));
		verts.push_back(Vertex_PCU(innerEndPos, color));
		verts.push_back(Vertex_PCU(outerStartPos, color));
	
	}

}

//------------------------------------------------------------------------------------------------------------------
void AddVertsForQuad3D(std::vector<Vertex_PCU>& verts, Vec3 const& bottomLeft, Vec3 const& bottomRight, Vec3 const& topRight, Vec3 const& topLeft, Rgba8 const& color, AABB2 const& UVs)
{
	verts.push_back(Vertex_PCU(bottomLeft, color, UVs.m_mins));
	verts.push_back(Vertex_PCU(bottomRight, color, Vec2(UVs.m_maxs.x, UVs.m_mins.y)));
	verts.push_back(Vertex_PCU(topRight, color, UVs.m_maxs));

	verts.push_back(Vertex_PCU(bottomLeft, color, UVs.m_mins));
	verts.push_back(Vertex_PCU(topRight, color, UVs.m_maxs));
	verts.push_back(Vertex_PCU(topLeft, color, Vec2(UVs.m_mins.x, UVs.m_maxs.y)));
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void AddVertsForQuad3D(std::vector<Vertex_PCUTBN>& verts, std::vector<unsigned int>& indices, Vec3 const& bottomLeft, Vec3 const& bottomRight, Vec3 const& topRight, Vec3 const& topLeft, Rgba8 const& color, AABB2 const& UVs)
{
	Vec3 tangent	= (bottomRight - bottomLeft).GetNormalized();
	Vec3 biTangent	= (topLeft - bottomLeft).GetNormalized();

	Vec3 bottomLeftNormal	= CrossProduct3D((bottomRight - bottomLeft), (topLeft - bottomLeft)).GetNormalized();
	Vec3 bottomRightNormal	= CrossProduct3D((topRight - bottomRight), (bottomLeft - bottomRight)).GetNormalized();
	Vec3 topRightNormal		= CrossProduct3D((topLeft - topRight), (bottomRight - topRight)).GetNormalized();
	Vec3 topLeftNormal		= CrossProduct3D((bottomLeft - topLeft), (topRight - topLeft)).GetNormalized();

	int startIndex = static_cast<unsigned int>(verts.size());

	verts.push_back(Vertex_PCUTBN(bottomLeft,	color,	UVs.m_mins,							tangent, biTangent, bottomLeftNormal));
	verts.push_back(Vertex_PCUTBN(bottomRight,	color,	Vec2(UVs.m_maxs.x, UVs.m_mins.y),	tangent, biTangent, bottomRightNormal));
	verts.push_back(Vertex_PCUTBN(topRight,		color,	UVs.m_maxs,							tangent, biTangent, topRightNormal));
	verts.push_back(Vertex_PCUTBN(topLeft,		color,	Vec2(UVs.m_mins.x, UVs.m_maxs.y),	tangent, biTangent, topLeftNormal));

	indices.push_back(startIndex);
	indices.push_back(startIndex + 1);
	indices.push_back(startIndex + 2);
	indices.push_back(startIndex);
	indices.push_back(startIndex + 2);
	indices.push_back(startIndex + 3);

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void AddVertsForRoundedQuad3D_VPCU(std::vector<Vertex_PCU>& verts, Vec3 bottomLeft, Vec3 bottomRight, Vec3 topRight, Vec3 topLeft, Rgba8 const& color, AABB2 const& UVs)
{
	float bottomLengthHalf = (bottomRight - bottomLeft).GetLength() * 0.5f;
	float topLengthHalf = (topRight - topLeft).GetLength() * 0.5f;

	Vec3 bottomMiddle = bottomLeft + (bottomRight - bottomLeft).GetNormalized() * bottomLengthHalf;
	Vec3 topMiddle = topLeft + (topRight - topLeft).GetNormalized() * topLengthHalf;

	Vec2 bottomMiddleUVs = Vec2((UVs.m_maxs.x - UVs.m_mins.x) * 0.5f, UVs.m_mins.y);
	Vec2 topMiddleUVs = Vec2(UVs.m_maxs.x, (UVs.m_maxs.y - UVs.m_mins.y) * 0.5f);

	verts.push_back(Vertex_PCU(bottomLeft, color, UVs.m_mins));
	verts.push_back(Vertex_PCU(bottomMiddle, color, bottomMiddleUVs));
	verts.push_back(Vertex_PCU(topMiddle, color, topMiddleUVs));

	verts.push_back(Vertex_PCU(bottomLeft, color, UVs.m_mins));
	verts.push_back(Vertex_PCU(topMiddle, color, topMiddleUVs));
	verts.push_back(Vertex_PCU(topLeft, color, Vec2(UVs.m_mins.x, UVs.m_maxs.y)));

	verts.push_back(Vertex_PCU(bottomMiddle, color, bottomMiddleUVs));
	verts.push_back(Vertex_PCU(bottomRight, color, Vec2(UVs.m_maxs.x, UVs.m_mins.y)));
	verts.push_back(Vertex_PCU(topRight, color, UVs.m_maxs));

	verts.push_back(Vertex_PCU(bottomMiddle, color, bottomMiddleUVs));
	verts.push_back(Vertex_PCU(topRight, color, UVs.m_maxs));
	verts.push_back(Vertex_PCU(topMiddle, color, topMiddleUVs));
}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void AddVertsForRoundedQuad3D(std::vector<Vertex_PCUTBN>& verts, Vec3 const& bottomLeft, Vec3 const& bottomRight, Vec3 const& topRight, Vec3 const& topLeft, Rgba8 const& color, AABB2 const& UVs)
{
	Vec3 defaultTangent = Vec3(0.f, 0.f, 0.f);
	Vec3 defaultBiTangent = Vec3(0.f, 0.f, 0.f);

	float bottomLengthHalf = (bottomRight - bottomLeft).GetLength() * 0.5f;
	float topLengthHalf    = (topRight - topLeft).GetLength() * 0.5f;

	Vec3 bottomMiddle = bottomLeft + (bottomRight - bottomLeft).GetNormalized() * bottomLengthHalf;
	Vec3 topMiddle	  = topLeft + (topRight - topLeft).GetNormalized() * topLengthHalf;

	Vec3 bottomLeftToBottomRightNormal = (bottomRight - bottomLeft).GetNormalized();
	Vec3 bottomLeftToTopLeftNormal = (topLeft - bottomLeft).GetNormalized();
	
	Vec3 bottomMiddleToBottomRightNormal = (bottomRight - bottomMiddle).GetNormalized();
	Vec3 bottomMiddleToTopMiddleNormal = (topMiddle - bottomMiddle).GetNormalized();
	Vec3 bottomMiddleNormal = CrossProduct3D(bottomMiddleToBottomRightNormal, bottomMiddleToTopMiddleNormal).GetNormalized();

	Vec3 topMiddleToTopRightNormal = (topRight - topMiddle).GetNormalized();
	Vec3 topMiddleToBottomMiddleNormal = (bottomMiddle - topMiddle).GetNormalized();
	Vec3 topMiddleNormal = CrossProduct3D(topMiddleToTopRightNormal, topMiddleToBottomMiddleNormal).GetNormalized();

	Vec2 bottomMiddleUVs = Vec2((UVs.m_maxs.x - UVs.m_mins.x) * 0.5f, UVs.m_mins.y);
	Vec2 topMiddleUVs = Vec2(UVs.m_maxs.x , (UVs.m_maxs.y - UVs.m_mins.y) * 0.5f);

	verts.push_back(Vertex_PCUTBN(bottomLeft, color, UVs.m_mins, defaultTangent, defaultBiTangent, -bottomLeftToBottomRightNormal));
	verts.push_back(Vertex_PCUTBN(bottomMiddle, color, bottomMiddleUVs, defaultTangent, defaultBiTangent, bottomMiddleNormal));
	verts.push_back(Vertex_PCUTBN(topMiddle, color, topMiddleUVs, defaultTangent, defaultBiTangent, topMiddleNormal));

	verts.push_back(Vertex_PCUTBN(bottomLeft, color, UVs.m_mins, defaultTangent, defaultBiTangent, -bottomLeftToBottomRightNormal));
	verts.push_back(Vertex_PCUTBN(topMiddle, color, topMiddleUVs, defaultTangent, defaultBiTangent, topMiddleNormal));
	verts.push_back(Vertex_PCUTBN(topLeft, color, Vec2(UVs.m_mins.x, UVs.m_maxs.y), defaultTangent, defaultBiTangent, -bottomLeftToBottomRightNormal));

	verts.push_back(Vertex_PCUTBN(bottomMiddle, color, bottomMiddleUVs, defaultTangent, defaultBiTangent, bottomMiddleNormal));
	verts.push_back(Vertex_PCUTBN(bottomRight, color, Vec2(UVs.m_maxs.x, UVs.m_mins.y), defaultTangent, defaultBiTangent, bottomLeftToBottomRightNormal));
	verts.push_back(Vertex_PCUTBN(topRight, color, UVs.m_maxs, defaultTangent, defaultBiTangent, bottomLeftToBottomRightNormal));

	verts.push_back(Vertex_PCUTBN(bottomMiddle, color, bottomMiddleUVs, defaultTangent, defaultBiTangent, bottomMiddleNormal));
	verts.push_back(Vertex_PCUTBN(topRight, color, UVs.m_maxs, defaultTangent, defaultBiTangent, bottomLeftToBottomRightNormal));
	verts.push_back(Vertex_PCUTBN(topMiddle, color, topMiddleUVs, defaultTangent, defaultBiTangent, topMiddleNormal));
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void AddVertsForRoundedQuad3D(std::vector<Vertex_PCUTBN>&verts, std::vector<unsigned int>&indices, Vec3 const& bottomLeft, Vec3 const& bottomRight, Vec3 const& topRight, Vec3 const& topLeft, Rgba8 const& color, AABB2 const& UVs)
{

	unsigned int startIndex = static_cast<unsigned int>(verts.size());

	Vec3 defaultTangent = Vec3(0.f, 0.f, 0.f);
	Vec3 defaultBiTangent = Vec3(0.f, 0.f, 0.f);

	float bottomLengthHalf = (bottomRight - bottomLeft).GetLength() * 0.5f;
	float topLengthHalf = (topRight - topLeft).GetLength() * 0.5f;

	Vec3 bottomMiddle = bottomLeft + (bottomRight - bottomLeft).GetNormalized() * bottomLengthHalf;
	Vec3 topMiddle = topLeft + (topRight - topLeft).GetNormalized() * topLengthHalf;

	Vec3 bottomLeftToBottomRightNormal = (bottomRight - bottomLeft).GetNormalized();
	Vec3 bottomLeftToTopLeftNormal = (topLeft - bottomLeft).GetNormalized();

	Vec3 bottomMiddleToBottomRightNormal = (bottomRight - bottomMiddle).GetNormalized();
	Vec3 bottomMiddleToTopMiddleNormal = (topMiddle - bottomMiddle).GetNormalized();
	Vec3 bottomMiddleNormal = CrossProduct3D(bottomMiddleToBottomRightNormal, bottomMiddleToTopMiddleNormal).GetNormalized();

	Vec3 topMiddleToTopRightNormal = (topRight - topMiddle).GetNormalized();
	Vec3 topMiddleToBottomMiddleNormal = (bottomMiddle - topMiddle).GetNormalized();
	Vec3 topMiddleNormal = CrossProduct3D(topMiddleToTopRightNormal, topMiddleToBottomMiddleNormal).GetNormalized();

	Vec2 uvSize = UVs.m_maxs - UVs.m_mins;

	Vec2 bottomMiddleUVs = Vec2(UVs.m_mins.x + uvSize.x * 0.5f, UVs.m_mins.y);
	Vec2 topMiddleUVs = Vec2(UVs.m_mins.x + uvSize.x * 0.5f, UVs.m_maxs.y);

	verts.push_back(Vertex_PCUTBN(bottomLeft, color, UVs.m_mins, defaultTangent, defaultBiTangent, -bottomLeftToBottomRightNormal));
	verts.push_back(Vertex_PCUTBN(bottomMiddle, color, bottomMiddleUVs, defaultTangent, defaultBiTangent, bottomMiddleNormal));
	verts.push_back(Vertex_PCUTBN(topMiddle, color, topMiddleUVs, defaultTangent, defaultBiTangent, topMiddleNormal));
	verts.push_back(Vertex_PCUTBN(topLeft, color, Vec2(UVs.m_mins.x, UVs.m_maxs.y), defaultTangent, defaultBiTangent, -bottomLeftToBottomRightNormal));
	verts.push_back(Vertex_PCUTBN(bottomRight, color, Vec2(UVs.m_maxs.x, UVs.m_mins.y), defaultTangent, defaultBiTangent, bottomLeftToBottomRightNormal));
	verts.push_back(Vertex_PCUTBN(topRight, color, UVs.m_maxs, defaultTangent, defaultBiTangent, bottomLeftToBottomRightNormal));

	indices.push_back(startIndex);
	indices.push_back(startIndex + 1);
	indices.push_back(startIndex + 2);

	indices.push_back(startIndex);
	indices.push_back(startIndex + 2);
	indices.push_back(startIndex + 3);

	indices.push_back(startIndex + 1);
	indices.push_back(startIndex + 4);
	indices.push_back(startIndex + 5);

	indices.push_back(startIndex + 1);
	indices.push_back(startIndex + 5);
	indices.push_back(startIndex + 2);

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void AddVertsForSphere3D(std::vector<Vertex_PCU>& verts, Vec3 const& center, float radius, Rgba8 const& color, AABB2 const& UVs, int numSlices, int numStacks)
{

	// #NOTE: If you ever comeback and do not understand a single line, go refer SD2 Notes from page 7 to 11

	float pitchScale = 1.f / static_cast<float>(numStacks);
	float yawScale	 = 1.f / static_cast<float>(numSlices);

	float uScale = UVs.GetDimensions().x / numSlices;
	float vScale = UVs.GetDimensions().y / numStacks;
	
	std::vector<Vec3> bottomLeftPoints;

	float currentPitchAngle		= -90.f;
	float angleChangePerStack   = 180.f * pitchScale;

	float minYawAngle		  = 0.f;
	float currentYawAngle	  = 0.f;
	float angleChangePerSlice = 360.f * yawScale;

	for(int stackIndex = 0; stackIndex <= numStacks; ++stackIndex)
	{
		for(int sliceIndex = 0; sliceIndex < numSlices; ++sliceIndex)
		{
			Vec3 bottomLeftPoint = center + Vec3::MakeFromPolarDegrees(currentPitchAngle, currentYawAngle, radius);
			bottomLeftPoints.push_back(bottomLeftPoint);
			
			currentYawAngle += angleChangePerSlice;
		}

		currentYawAngle = minYawAngle;
		currentPitchAngle += angleChangePerStack;
	}

	for(int stackIndex = 0; stackIndex < numStacks; ++stackIndex)
	{
		for(int sliceIndex = 0; sliceIndex < numSlices; ++sliceIndex)
		{
			if(stackIndex > 0 && stackIndex < numStacks - 1)
			{
				int bottomLeftPointIndex = numSlices * stackIndex + sliceIndex;
				
				int bottomRightPointIndex;

				if(sliceIndex != numSlices - 1)
				{
					bottomRightPointIndex = bottomLeftPointIndex + 1;
				}
				else
				{
					bottomRightPointIndex = bottomLeftPointIndex - (numSlices - 1);
				}

				int topRightPointIndex = bottomRightPointIndex + numSlices;
				int topLeftPointIndex = bottomLeftPointIndex + numSlices;

 				AABB2 quadUVs;
				quadUVs.m_mins.x = sliceIndex * uScale;
 				quadUVs.m_mins.y = stackIndex * vScale;
 				quadUVs.m_maxs.x = (sliceIndex + 1 ) * uScale;
 				quadUVs.m_maxs.y = (stackIndex + 1) * vScale;

				AddVertsForQuad3D(verts, bottomLeftPoints[bottomLeftPointIndex], bottomLeftPoints[bottomRightPointIndex], bottomLeftPoints[topRightPointIndex], bottomLeftPoints[topLeftPointIndex], color, quadUVs);

			}

			if(stackIndex == 0)
			{
				int bottomPointIndex = sliceIndex; // ccw1

				int topRightPointIndex; // ccw2

				if(sliceIndex != numSlices - 1)
				{
					topRightPointIndex = bottomPointIndex + numSlices + 1;
				}
				else
				{
					topRightPointIndex = bottomPointIndex - (numSlices - 1);
				}

				int topLeftPointIndex = bottomPointIndex + numSlices;

				AABB2 triUVs;
				triUVs.m_mins.x = sliceIndex * uScale;
				triUVs.m_mins.y = stackIndex * vScale;
				triUVs.m_maxs.x = (sliceIndex + 1) * uScale;
				triUVs.m_maxs.y = (stackIndex + 1) * vScale;

				verts.push_back(Vertex_PCU(bottomLeftPoints[bottomPointIndex],	 color, triUVs.m_mins));
				verts.push_back(Vertex_PCU(bottomLeftPoints[topRightPointIndex], color, triUVs.m_maxs));
				verts.push_back(Vertex_PCU(bottomLeftPoints[topLeftPointIndex],	 color, Vec2(triUVs.m_mins.x, triUVs.m_maxs.y)));

			}
			
			
			if(stackIndex == numStacks - 1)
			{
				int bottomLeftPointIndex = numSlices * stackIndex + sliceIndex;

				int bottomRightPointIndex;

				if(sliceIndex != numSlices - 1)
				{
					bottomRightPointIndex = bottomLeftPointIndex + 1;
				}
				else
				{
					bottomRightPointIndex = bottomLeftPointIndex - (numSlices - 1);
				}

				int topPointIndex = bottomLeftPointIndex + numSlices;

				AABB2 triUVs;
				triUVs.m_mins.x = sliceIndex * uScale;
				triUVs.m_mins.y = stackIndex * vScale;
				triUVs.m_maxs.x = (sliceIndex + 1) * uScale;
				triUVs.m_maxs.y = (stackIndex + 1) * vScale;

				verts.push_back(Vertex_PCU(bottomLeftPoints[bottomLeftPointIndex],	color, triUVs.m_mins));
				verts.push_back(Vertex_PCU(bottomLeftPoints[bottomRightPointIndex], color, Vec2(triUVs.m_maxs.x, triUVs.m_mins.y)));
				verts.push_back(Vertex_PCU(bottomLeftPoints[topPointIndex],			color, Vec2(triUVs.m_mins.x, triUVs.m_maxs.y)));
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void AddVertsForSphere3D(std::vector<Vertex_PCU>& verts, Sphere const& sphere, Rgba8 const& color, AABB2 const& UVs, int numSlices, int numStacks)
{

	AddVertsForSphere3D(verts, sphere.m_center, sphere.m_radius, color, UVs, numSlices, numStacks);

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void AddVertsForIndexedSphere3D(std::vector<Vertex_PCUTBN>& verts, std::vector<unsigned int>& indexes, Sphere const& sphere, Rgba8 const& color, AABB2 const& UVs, int numSlices, int numStacks, bool debugTangents)
{
	// #NOTE: If you ever comeback and do not understand a single line, go refer SD2 Notes from page 7 to 11

	Vec3 tempTangent = Vec3::ZERO;
	Vec3 tempBitangent = Vec3::ZERO;

	float pitchScale = 1.f / static_cast<float>(numStacks);
	float yawScale = 1.f / static_cast<float>(numSlices);

	float uScale = UVs.GetDimensions().x / numSlices;
	float vScale = UVs.GetDimensions().y / numStacks;

	std::vector<Vec3> bottomLeftPoints;

	float currentPitchAngle = -90.f;
	float angleChangePerStack = 180.f * pitchScale;

	float minYawAngle = 0.f;
	float currentYawAngle = 0.f;
	float angleChangePerSlice = 360.f * yawScale;

	for(int stackIndex = 0; stackIndex <= numStacks; ++stackIndex)
	{
		for(int sliceIndex = 0; sliceIndex < numSlices; ++sliceIndex)
		{
			Vec3 bottomLeftPoint = sphere.m_center + Vec3::MakeFromPolarDegrees(currentPitchAngle, currentYawAngle, sphere.m_radius);
			bottomLeftPoints.push_back(bottomLeftPoint);

			currentYawAngle += angleChangePerSlice;
		}

		currentYawAngle = minYawAngle;
		currentPitchAngle += angleChangePerStack;
	}

	for(int stackIndex = 0; stackIndex < numStacks; ++stackIndex)
	{
		for(int sliceIndex = 0; sliceIndex < numSlices; ++sliceIndex)
		{
			if(stackIndex > 0 && stackIndex < numStacks - 1)
			{
				int bottomLeftPointIndex = numSlices * stackIndex + sliceIndex;

				int bottomRightPointIndex;

				if(sliceIndex != numSlices - 1)
				{
					bottomRightPointIndex = bottomLeftPointIndex + 1;
				}
				else
				{
					bottomRightPointIndex = bottomLeftPointIndex - (numSlices - 1);
				}

				int topRightPointIndex = bottomRightPointIndex + numSlices;
				int topLeftPointIndex = bottomLeftPointIndex + numSlices;

				AABB2 quadUVs;
				quadUVs.m_mins.x = sliceIndex * uScale;
				quadUVs.m_mins.y = stackIndex * vScale;
				quadUVs.m_maxs.x = (sliceIndex + 1) * uScale;
				quadUVs.m_maxs.y = (stackIndex + 1) * vScale;

				Vec3 bottomLeft		= bottomLeftPoints[bottomLeftPointIndex];
				Vec3 bottomRight	= bottomLeftPoints[bottomRightPointIndex];
				Vec3 topRight		= bottomLeftPoints[topRightPointIndex];
				Vec3 topLeft		= bottomLeftPoints[topLeftPointIndex];

				Vec3 bottomLeftNormal	= (bottomLeft  - sphere.m_center).GetNormalized();
				Vec3 bottomRightNormal	= (bottomRight - sphere.m_center).GetNormalized();
				Vec3 topRightNormal		= (topRight    - sphere.m_center).GetNormalized();
				Vec3 topLeftNormal		= (topLeft     - sphere.m_center).GetNormalized();

				Vec3 tangentBottomLeft;
				Vec3 tangentBottomRight;
				Vec3 tangentTopLeft;
				Vec3 tangentTopRight;

				Vec3 arbitraryBottomLeft = (bottomLeftNormal.z < 0.999f) ? Vec3::UP : Vec3::FORWARD;
				Vec3 arbitraryBottomRight = (bottomRightNormal.z < 0.999f) ? Vec3::UP : Vec3::FORWARD;
				Vec3 arbitraryTopLeft = (topLeftNormal.z < 0.999f) ? Vec3::UP : Vec3::FORWARD;
				Vec3 arbitraryTopRight = (topRightNormal.z < 0.999f) ? Vec3::UP : Vec3::FORWARD;
				
				if(arbitraryBottomLeft == Vec3::UP)
				{
					tangentBottomLeft = CrossProduct3D(arbitraryBottomLeft, bottomLeftNormal).GetNormalized();
				}
				else
				{
					tangentBottomLeft = CrossProduct3D(bottomLeftNormal, arbitraryBottomLeft).GetNormalized();
				}

				if(arbitraryBottomRight == Vec3::UP)
				{
					tangentBottomRight = CrossProduct3D(arbitraryBottomRight, bottomRightNormal).GetNormalized();
				}
				else
				{
					tangentBottomRight = CrossProduct3D(bottomRightNormal, arbitraryBottomRight).GetNormalized();
				}

				if(arbitraryTopLeft == Vec3::UP)
				{
					tangentTopLeft = CrossProduct3D(arbitraryTopLeft, topLeftNormal).GetNormalized();
				}
				else
				{
					tangentTopLeft = CrossProduct3D(topLeftNormal, arbitraryTopLeft).GetNormalized();
				}

				if(arbitraryTopRight == Vec3::UP)
				{
					tangentTopRight = CrossProduct3D(arbitraryTopRight, topRightNormal).GetNormalized();
				}
				else
				{
					tangentTopRight = CrossProduct3D(topRightNormal, arbitraryTopRight).GetNormalized();
				}

				Vec3 bitangentBottomLeft	= CrossProduct3D(bottomLeftNormal, tangentBottomLeft).GetNormalized();
				Vec3 bitangentBottomRight	= CrossProduct3D(bottomRightNormal, tangentBottomRight).GetNormalized();
				Vec3 bitangentTopLeft		= CrossProduct3D(topLeftNormal, tangentTopLeft).GetNormalized();
				Vec3 bitangentTopRight		= CrossProduct3D(topRightNormal, tangentTopRight).GetNormalized();

				unsigned int startIndex = static_cast<unsigned int>(verts.size());

				if(debugTangents)
				{
					DebugAddWorldArrow(bottomLeft, bottomLeftNormal, 0.03f, 0.001f, 10000.f, Rgba8::BLUE);
					DebugAddWorldArrow(bottomLeft, tangentBottomLeft, 0.03f, 0.001f, 10000.f, Rgba8::RED);
					DebugAddWorldArrow(bottomLeft, bitangentBottomLeft, 0.03f, 0.001f, 10000.f, Rgba8::GREEN);

					DebugAddWorldArrow(bottomRight, bottomRightNormal, 0.03f, 0.001f, 10000.f, Rgba8::BLUE);
					DebugAddWorldArrow(bottomRight, tangentBottomRight, 0.03f, 0.001f, 10000.f, Rgba8::RED);
					DebugAddWorldArrow(bottomRight, bitangentBottomRight, 0.03f, 0.001f, 10000.f, Rgba8::GREEN);

					DebugAddWorldArrow(topRight, topRightNormal, 0.03f, 0.001f, 10000.f, Rgba8::BLUE);
					DebugAddWorldArrow(topRight, tangentTopRight, 0.03f, 0.001f, 10000.f, Rgba8::RED);
					DebugAddWorldArrow(topRight, bitangentTopRight, 0.03f, 0.001f, 10000.f, Rgba8::GREEN);

					DebugAddWorldArrow(topLeft, topLeftNormal, 0.03f, 0.001f, 10000.f, Rgba8::BLUE);
					DebugAddWorldArrow(topLeft, tangentTopLeft, 0.03f, 0.001f, 10000.f, Rgba8::RED);
					DebugAddWorldArrow(topLeft, bitangentTopLeft, 0.03f, 0.001f, 10000.f, Rgba8::GREEN);
				}
				
				verts.push_back(Vertex_PCUTBN(bottomLeft, color, quadUVs.m_mins, tangentBottomLeft, bitangentBottomLeft, bottomLeftNormal));
				verts.push_back(Vertex_PCUTBN(bottomRight, color, Vec2(quadUVs.m_maxs.x, quadUVs.m_mins.y), tangentBottomRight, bitangentBottomRight, bottomRightNormal));
				verts.push_back(Vertex_PCUTBN(topRight, color, quadUVs.m_maxs, tangentTopRight, bitangentTopRight, topRightNormal));
				verts.push_back(Vertex_PCUTBN(topLeft, color, Vec2(quadUVs.m_mins.x, quadUVs.m_maxs.y), tangentTopLeft, bitangentTopLeft, topLeftNormal));

				indexes.push_back(startIndex);
				indexes.push_back(startIndex + 1);
				indexes.push_back(startIndex + 2);
				indexes.push_back(startIndex);
				indexes.push_back(startIndex + 2);
				indexes.push_back(startIndex + 3);
			}

			if(stackIndex == 0)
			{
				int bottomPointIndex = sliceIndex; // ccw1

				int topRightPointIndex; // ccw2

				if(sliceIndex != numSlices - 1)
				{
					topRightPointIndex = bottomPointIndex + numSlices + 1;
				}
				else
				{
					topRightPointIndex = numSlices;
				}

				int topLeftPointIndex = bottomPointIndex + numSlices;

				AABB2 triUVs;
				triUVs.m_mins.x = sliceIndex * uScale;
				triUVs.m_mins.y = stackIndex * vScale;
				triUVs.m_maxs.x = (sliceIndex + 1) * uScale;
				triUVs.m_maxs.y = (stackIndex + 1) * vScale;

				unsigned int startIndex = static_cast<unsigned int>(verts.size());

				Vec3 posA = bottomLeftPoints[bottomPointIndex];
				Vec3 posB = bottomLeftPoints[topRightPointIndex];
				Vec3 posC = bottomLeftPoints[topLeftPointIndex];

				Vec3 normalOnA = Vec3::DOWN;
				Vec3 normalOnB = (posB - sphere.m_center).GetNormalized();
				Vec3 normalOnC = (posC - sphere.m_center).GetNormalized();

				Vec3 tangentOnA;
				Vec3 tangentOnB;
				Vec3 tangentOnC;

				Vec3 arbitraryB = (normalOnB.z < 1.f) ? Vec3::UP : Vec3::FORWARD;
				Vec3 arbitraryC = (normalOnC.z < 1.f) ? Vec3::UP : Vec3::FORWARD;

				tangentOnA = Vec3::ZERO;

				if(arbitraryB == Vec3::UP)
				{
					tangentOnB = CrossProduct3D(arbitraryB, normalOnB).GetNormalized();
				}
				else
				{
					tangentOnB = CrossProduct3D(normalOnB, arbitraryB).GetNormalized();
				}

				if(arbitraryC == Vec3::UP)
				{
					tangentOnC = CrossProduct3D(arbitraryC, normalOnC).GetNormalized();
				}
				else
				{
					tangentOnC = CrossProduct3D(normalOnC, arbitraryC).GetNormalized();
				}

																	
				Vec3 biTangentOnA = CrossProduct3D(normalOnA, tangentOnA).GetNormalized();
				Vec3 biTangentOnB = CrossProduct3D(normalOnB, tangentOnB).GetNormalized();
				Vec3 biTangentOnC = CrossProduct3D(normalOnC, tangentOnC).GetNormalized();

				if(debugTangents)
				{
					DebugAddWorldArrow(posA, normalOnA, 0.03f, 0.001f, 10000.f, Rgba8::BLUE);
					DebugAddWorldArrow(posA, tangentOnA, 0.03f, 0.001f, 10000.f, Rgba8::RED);
					DebugAddWorldArrow(posA, biTangentOnA, 0.03f, 0.001f, 10000.f, Rgba8::GREEN);

					DebugAddWorldArrow(posB, normalOnB, 0.03f, 0.001f, 10000.f, Rgba8::BLUE);
					DebugAddWorldArrow(posB, tangentOnB, 0.03f, 0.001f, 10000.f, Rgba8::RED);
					DebugAddWorldArrow(posB, biTangentOnB, 0.03f, 0.001f, 10000.f, Rgba8::GREEN);

					DebugAddWorldArrow(posC, normalOnC, 0.03f, 0.001f, 10000.f, Rgba8::BLUE);
					DebugAddWorldArrow(posC, tangentOnC, 0.03f, 0.001f, 10000.f, Rgba8::RED);
					DebugAddWorldArrow(posC, biTangentOnC, 0.03f, 0.001f, 10000.f, Rgba8::GREEN);
				}


				verts.push_back(Vertex_PCUTBN(posA, color, triUVs.m_mins, tangentOnA, biTangentOnA, normalOnA));
				verts.push_back(Vertex_PCUTBN(posB, color, triUVs.m_maxs, tangentOnB, biTangentOnB, normalOnB));
				verts.push_back(Vertex_PCUTBN(posC, color, Vec2(triUVs.m_mins.x, triUVs.m_maxs.y), tangentOnC, biTangentOnC, normalOnC));

				indexes.push_back(startIndex);
				indexes.push_back(startIndex + 1);
				indexes.push_back(startIndex + 2);
			}

			if(stackIndex == numStacks - 1)
			{
				int bottomLeftPointIndex = numSlices * stackIndex + sliceIndex;

				int bottomRightPointIndex;

				if(sliceIndex != numSlices - 1)
				{
					bottomRightPointIndex = bottomLeftPointIndex + 1;
				}
				else
				{
					bottomRightPointIndex = bottomLeftPointIndex - (numSlices - 1);
				}

				int topPointIndex = bottomLeftPointIndex + numSlices;

				AABB2 triUVs;
				triUVs.m_mins.x = sliceIndex * uScale;
				triUVs.m_mins.y = stackIndex * vScale;
				triUVs.m_maxs.x = (sliceIndex + 1) * uScale;
				triUVs.m_maxs.y = (stackIndex + 1) * vScale;

				unsigned int topFaceStartIndex = static_cast<unsigned int>(verts.size());

				Vec3 posD = bottomLeftPoints[bottomLeftPointIndex];
				Vec3 posE = bottomLeftPoints[bottomRightPointIndex];
				Vec3 posF = bottomLeftPoints[topPointIndex];

				Vec3 normalOnD = (posD - sphere.m_center).GetNormalized();
				Vec3 normalOnE = (posE - sphere.m_center).GetNormalized();
				Vec3 normalOnF = Vec3::UP;

				Vec3 tangentOnD;
				Vec3 tangentOnE;
				Vec3 tangentOnF;

				Vec3 arbitraryD = (normalOnD.z < 0.999f) ? Vec3::UP : Vec3::FORWARD;
				Vec3 arbitraryE = (normalOnE.z < 0.999f) ? Vec3::UP : Vec3::FORWARD;
				Vec3 arbitraryF = (normalOnF.z < 0.999f) ? Vec3::UP : Vec3::FORWARD;

				if(arbitraryD == Vec3::UP)
				{
					tangentOnD = CrossProduct3D(arbitraryD, normalOnD).GetNormalized();
				}
				else
				{
					tangentOnD = CrossProduct3D(normalOnD, arbitraryD).GetNormalized();
				}

				if(arbitraryE == Vec3::UP)
				{
					tangentOnE = CrossProduct3D(arbitraryE, normalOnE).GetNormalized();
				}
				else
				{
					tangentOnE = CrossProduct3D(normalOnE, arbitraryE).GetNormalized();
				}

				tangentOnF = Vec3::ZERO;

				Vec3 biTangentOnD = CrossProduct3D(normalOnD, tangentOnD).GetNormalized();
				Vec3 biTangentOnE = CrossProduct3D(normalOnE, tangentOnE).GetNormalized();
				Vec3 biTangentOnF = CrossProduct3D(normalOnF, tangentOnF).GetNormalized();

				verts.push_back(Vertex_PCUTBN(posD, color, triUVs.m_mins,						   tangentOnD, biTangentOnD, normalOnD));
				verts.push_back(Vertex_PCUTBN(posE, color, Vec2(triUVs.m_maxs.x, triUVs.m_mins.y), tangentOnE, biTangentOnE, normalOnE));
				verts.push_back(Vertex_PCUTBN(posF, color, Vec2(triUVs.m_mins.x, triUVs.m_maxs.y), tangentOnF, biTangentOnF, normalOnF));

				indexes.push_back(topFaceStartIndex);
				indexes.push_back(topFaceStartIndex + 1);
				indexes.push_back(topFaceStartIndex + 2);
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void AddVertsForAABB3D(std::vector<Vertex_PCU>& verts, AABB3 const& bounds, Rgba8 const& color, AABB2 const& UVs)
{
	std::vector<Vec3> eightCornerPoints;

	bounds.GetCornerPoints(eightCornerPoints);

	// Positive X Face
	AddVertsForQuad3D(verts, eightCornerPoints[POINT_F], eightCornerPoints[POINT_E], eightCornerPoints[POINT_H], eightCornerPoints[POINT_G], color, UVs);

	// Negative X Face
	AddVertsForQuad3D(verts, eightCornerPoints[POINT_D], eightCornerPoints[POINT_A], eightCornerPoints[POINT_B], eightCornerPoints[POINT_C], color, UVs);

	// Positive Y Face
	AddVertsForQuad3D(verts, eightCornerPoints[POINT_E], eightCornerPoints[POINT_D], eightCornerPoints[POINT_C], eightCornerPoints[POINT_H], color, UVs);

	// Negative Y Face
	AddVertsForQuad3D(verts, eightCornerPoints[POINT_A], eightCornerPoints[POINT_F], eightCornerPoints[POINT_G], eightCornerPoints[POINT_B], color, UVs);

	// Positive Z Face
	AddVertsForQuad3D(verts, eightCornerPoints[POINT_B], eightCornerPoints[POINT_G], eightCornerPoints[POINT_H], eightCornerPoints[POINT_C], color, UVs);

	// Negative Z Face
	AddVertsForQuad3D(verts, eightCornerPoints[POINT_F], eightCornerPoints[POINT_A], eightCornerPoints[POINT_D], eightCornerPoints[POINT_E], color, UVs);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void AddVertsForAABB3D(std::vector<Vertex_PCUTBN>& verts, std::vector<unsigned int>& indices, AABB3 const& bounds, Rgba8 const& color, AABB2 const& UVs)
{

	std::vector<Vec3> eightCornerPoints;
	bounds.GetCornerPoints(eightCornerPoints);

	AddVertsForQuad3D(verts, indices, eightCornerPoints[POINT_F], eightCornerPoints[POINT_E], eightCornerPoints[POINT_H], eightCornerPoints[POINT_G], color, UVs);

	// Negative X Face
	AddVertsForQuad3D(verts, indices, eightCornerPoints[POINT_D], eightCornerPoints[POINT_A], eightCornerPoints[POINT_B], eightCornerPoints[POINT_C], color, UVs);

	// Positive Y Face
	AddVertsForQuad3D(verts, indices, eightCornerPoints[POINT_E], eightCornerPoints[POINT_D], eightCornerPoints[POINT_C], eightCornerPoints[POINT_H], color, UVs);

	// Negative Y Face
	AddVertsForQuad3D(verts, indices, eightCornerPoints[POINT_A], eightCornerPoints[POINT_F], eightCornerPoints[POINT_G], eightCornerPoints[POINT_B], color, UVs);

	// Positive Z Face
	AddVertsForQuad3D(verts, indices, eightCornerPoints[POINT_B], eightCornerPoints[POINT_G], eightCornerPoints[POINT_H], eightCornerPoints[POINT_C], color, UVs);

	// Negative Z Face
	AddVertsForQuad3D(verts, indices, eightCornerPoints[POINT_F], eightCornerPoints[POINT_A], eightCornerPoints[POINT_D], eightCornerPoints[POINT_E], color, UVs);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void AddVertsForOBB3D(std::vector<Vertex_PCU>& verts, OBB3 const& bounds, Rgba8 const& color, AABB2 const& UVs)
{
	std::vector<Vec3> eightCornerPoints;
	bounds.GetEightCornerPoints(eightCornerPoints);

	// Negative X Face
	AddVertsForQuad3D(verts, eightCornerPoints[OBB_BOTTOM_LEFT_BACK], eightCornerPoints[OBB_BOTTOM_RIGHT_BACK], eightCornerPoints[OBB_TOP_RIGHT_BACK], eightCornerPoints[OBB_TOP_LEFT_BACK], color, UVs);

	// Positive X Face
	AddVertsForQuad3D(verts, eightCornerPoints[OBB_BOTTOM_RIGHT_FRONT], eightCornerPoints[OBB_BOTTOM_LEFT_FRONT], eightCornerPoints[OBB_TOP_LEFT_FRONT], eightCornerPoints[OBB_TOP_RIGHT_FRONT], color, UVs);

	// Positive Y Face
	AddVertsForQuad3D(verts, eightCornerPoints[OBB_BOTTOM_LEFT_FRONT], eightCornerPoints[OBB_BOTTOM_LEFT_BACK], eightCornerPoints[OBB_TOP_LEFT_BACK], eightCornerPoints[OBB_TOP_LEFT_FRONT], color, UVs);

	// Negative Y Face
	AddVertsForQuad3D(verts, eightCornerPoints[OBB_BOTTOM_RIGHT_BACK], eightCornerPoints[OBB_BOTTOM_RIGHT_FRONT], eightCornerPoints[OBB_TOP_RIGHT_FRONT], eightCornerPoints[OBB_TOP_RIGHT_BACK], color, UVs);

	// Positive Z Face
	AddVertsForQuad3D(verts, eightCornerPoints[OBB_TOP_LEFT_BACK], eightCornerPoints[OBB_TOP_RIGHT_BACK], eightCornerPoints[OBB_TOP_RIGHT_FRONT], eightCornerPoints[OBB_TOP_LEFT_FRONT], color, UVs);

	// Negative Z Face
	AddVertsForQuad3D(verts, eightCornerPoints[OBB_BOTTOM_LEFT_FRONT], eightCornerPoints[OBB_BOTTOM_RIGHT_FRONT], eightCornerPoints[OBB_BOTTOM_RIGHT_BACK], eightCornerPoints[OBB_BOTTOM_LEFT_BACK], color, UVs);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void AddVertsForIndexedOBB3D(std::vector<Vertex_PCUTBN>& verts, std::vector<unsigned int>& indices, OBB3 const& bounds, Rgba8 const& color, AABB2 const& UVs)
{
	std::vector<Vec3> eightCornerPoints;
	bounds.GetEightCornerPoints(eightCornerPoints);

	// Negative X Face
	AddVertsForQuad3D(verts, indices, eightCornerPoints[OBB_BOTTOM_LEFT_BACK], eightCornerPoints[OBB_BOTTOM_RIGHT_BACK], eightCornerPoints[OBB_TOP_RIGHT_BACK], eightCornerPoints[OBB_TOP_LEFT_BACK], color, UVs);

	// Positive X Face
	AddVertsForQuad3D(verts, indices, eightCornerPoints[OBB_BOTTOM_RIGHT_FRONT], eightCornerPoints[OBB_BOTTOM_LEFT_FRONT], eightCornerPoints[OBB_TOP_LEFT_FRONT], eightCornerPoints[OBB_TOP_RIGHT_FRONT], color, UVs);

	// Positive Y Face
	AddVertsForQuad3D(verts, indices, eightCornerPoints[OBB_BOTTOM_LEFT_FRONT], eightCornerPoints[OBB_BOTTOM_LEFT_BACK], eightCornerPoints[OBB_TOP_LEFT_BACK], eightCornerPoints[OBB_TOP_LEFT_FRONT], color, UVs);

	// Negative Y Face
	AddVertsForQuad3D(verts, indices, eightCornerPoints[OBB_BOTTOM_RIGHT_BACK], eightCornerPoints[OBB_BOTTOM_RIGHT_FRONT], eightCornerPoints[OBB_TOP_RIGHT_FRONT], eightCornerPoints[OBB_TOP_RIGHT_BACK], color, UVs);

	// Positive Z Face
	AddVertsForQuad3D(verts, indices, eightCornerPoints[OBB_TOP_LEFT_BACK], eightCornerPoints[OBB_TOP_RIGHT_BACK], eightCornerPoints[OBB_TOP_RIGHT_FRONT], eightCornerPoints[OBB_TOP_LEFT_FRONT], color, UVs);

	// Negative Z Face
	AddVertsForQuad3D(verts, indices, eightCornerPoints[OBB_BOTTOM_LEFT_FRONT], eightCornerPoints[OBB_BOTTOM_RIGHT_FRONT], eightCornerPoints[OBB_BOTTOM_RIGHT_BACK], eightCornerPoints[OBB_BOTTOM_LEFT_BACK], color, UVs);

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void AddVertsForCylinder3D(std::vector<Vertex_PCU>& verts, float height, float radius, Rgba8 const& color, AABB2 const& UVs, int numSlices)
{

	float pitchScale = 1.f / static_cast<float>(numSlices);
	float currentPitchAngle = 0.f;
	float angleChangePerSlice = 360.f * pitchScale;

	float uScale = UVs.GetDimensions().x / numSlices;

	Vec3 startPosition = Vec3(0.f, 0.f, 0.f);
	Vec3 heightVec = Vec3(height, 0.f, 0.f);

	std::vector<Vec3> bottomLeftPoints;

	for(int sliceIndex = 0; sliceIndex < numSlices; ++sliceIndex)
	{
		Vec3 bottomLeftPoint = startPosition + Vec3::MakeFromPolarDegrees(currentPitchAngle, 90.f, radius);
		bottomLeftPoints.push_back(bottomLeftPoint);

		currentPitchAngle += angleChangePerSlice;
	}

	for(int sliceIndex = 0; sliceIndex < numSlices; ++sliceIndex)
	{
		int bottomLeftPointIndex = sliceIndex;
		int bottomRightPointIndex = (sliceIndex + 1) % numSlices;

		// bottom face
		verts.push_back(Vertex_PCU(startPosition, color));
		verts.push_back(Vertex_PCU(bottomLeftPoints[bottomRightPointIndex], color));
		verts.push_back(Vertex_PCU(bottomLeftPoints[bottomLeftPointIndex], color));

		// quad
		AABB2 quadUVs;
		quadUVs.m_mins.x = uScale * sliceIndex;
		quadUVs.m_mins.y = 0.f;
		quadUVs.m_maxs.x = (sliceIndex + 1) * uScale;
		quadUVs.m_maxs.y = 1.f;
		AddVertsForQuad3D(verts, bottomLeftPoints[bottomLeftPointIndex], bottomLeftPoints[bottomRightPointIndex], bottomLeftPoints[bottomRightPointIndex] + heightVec, bottomLeftPoints[bottomLeftPointIndex] + heightVec, color, quadUVs);

		// top face
		verts.push_back(Vertex_PCU(bottomLeftPoints[bottomLeftPointIndex] + heightVec, color));
		verts.push_back(Vertex_PCU(bottomLeftPoints[bottomRightPointIndex] + heightVec, color));
		verts.push_back(Vertex_PCU(startPosition + heightVec, color));
	}

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void AddVertsForZCylinder3D(std::vector<Vertex_PCU>& verts, Cylinder3D const& cylinder, Rgba8 const& color, AABB2 const& UVs, int numSlices)
{
	float yawScale = 1.f / static_cast<float>(numSlices);

	float uScale = UVs.GetDimensions().x / numSlices;

	std::vector<Vec3> bottomLeftPoints;
	std::vector<Vec3> topLeftPoints;

	Vec3 startPosition = cylinder.m_startPosition;
	Vec3 endPosition = startPosition + Vec3(0.f, 0.f, cylinder.m_height);
	float radius = cylinder.m_radius;

	float currentYawAngle = 0.f;
	float angleChangePerSlice = 360.f * yawScale;

	for(int sliceIndex = 0; sliceIndex < numSlices; ++sliceIndex)
	{
		Vec3 bottomLeftPoint = startPosition + Vec3::MakeFromPolarDegrees(0.f, currentYawAngle, radius);
		bottomLeftPoints.push_back(bottomLeftPoint);

		Vec3 topLeftPoint = endPosition + Vec3::MakeFromPolarDegrees(0.f, currentYawAngle, radius);
		topLeftPoints.push_back(topLeftPoint);

		currentYawAngle += angleChangePerSlice;
	}

	currentYawAngle = 0.f;

	for(int sliceIndex = 0; sliceIndex < numSlices; ++sliceIndex)
	{
		int bottomLeftPointIndex = sliceIndex;
		int bottomRightPointIndex = (sliceIndex + 1) % numSlices;
		int topRightPointIndex = (sliceIndex + 1) % numSlices;
		int topLeftPointIndex = sliceIndex;

		Vec2 currentFaceUVs;
		currentFaceUVs.x = (UVs.m_maxs.x - UVs.m_mins.x) * (0.5f * CosDegrees(currentYawAngle) + 0.5f);
		currentFaceUVs.y = (UVs.m_maxs.y - UVs.m_mins.y) * (0.5f * SinDegrees(currentYawAngle) + 0.5f);

		Vec2 nextFaceUVs;
		nextFaceUVs.x = (UVs.m_maxs.x - UVs.m_mins.x) * (0.5f * CosDegrees(currentYawAngle + angleChangePerSlice) + 0.5f);
		nextFaceUVs.y = (UVs.m_maxs.y - UVs.m_mins.y) * (0.5f * SinDegrees(currentYawAngle + angleChangePerSlice) + 0.5f);

	
		// bottom face
		verts.push_back(Vertex_PCU(startPosition, color, UVs.GetCenter()));
		verts.push_back(Vertex_PCU(bottomLeftPoints[bottomRightPointIndex], color, nextFaceUVs));
		verts.push_back(Vertex_PCU(bottomLeftPoints[bottomLeftPointIndex], color, currentFaceUVs));

		// quad
	
		AABB2 quadUVs;
		quadUVs.m_mins.x = uScale * sliceIndex;
		quadUVs.m_mins.y = 0.f;
		quadUVs.m_maxs.x = (sliceIndex + 1) * uScale;
		quadUVs.m_maxs.y = 1.f;

		AddVertsForQuad3D(verts, bottomLeftPoints[bottomLeftPointIndex], bottomLeftPoints[bottomRightPointIndex], topLeftPoints[topRightPointIndex], topLeftPoints[topLeftPointIndex], color, quadUVs);

		// top face
		verts.push_back(Vertex_PCU(topLeftPoints[topLeftPointIndex], color, currentFaceUVs));
		verts.push_back(Vertex_PCU(topLeftPoints[topRightPointIndex], color, nextFaceUVs));
		verts.push_back(Vertex_PCU(endPosition, color, Vec2(0.5f, 0.5f)));

		currentYawAngle += angleChangePerSlice;
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void AddVertsForIndexedZCylinder3D(std::vector<Vertex_PCUTBN>& verts, std::vector<unsigned int>& indexes, Cylinder3D const& cylinder, Rgba8 const& color, AABB2 const& UVs, int numSlices)
{
	Vec3 tempTangent	= Vec3::ZERO;
	Vec3 tempBitangent	= Vec3::ZERO;

	float yawScale	= 1.f / static_cast<float>(numSlices);

	float uScale	= UVs.GetDimensions().x / numSlices;

	std::vector<Vec3> bottomLeftPoints;
	std::vector<Vec3> topLeftPoints;

	Vec3 startPosition	= cylinder.m_startPosition;
	Vec3 endPosition	= startPosition + Vec3(0.f, 0.f, cylinder.m_height);
	float radius		= cylinder.m_radius;

	float currentYawAngle		= 0.f;
	float angleChangePerSlice	= 360.f * yawScale;

	for(int sliceIndex = 0; sliceIndex < numSlices; ++sliceIndex)
	{
		Vec3 bottomLeftPoint = startPosition + Vec3::MakeFromPolarDegrees(0.f, currentYawAngle, radius);
		bottomLeftPoints.push_back(bottomLeftPoint);

		Vec3 topLeftPoint = endPosition + Vec3::MakeFromPolarDegrees(0.f, currentYawAngle, radius);
		topLeftPoints.push_back(topLeftPoint);

		currentYawAngle += angleChangePerSlice;
	}

	currentYawAngle = 0.f;

	for(int sliceIndex = 0; sliceIndex < numSlices; ++sliceIndex)
	{
		int bottomLeftPointIndex	= sliceIndex;
		int bottomRightPointIndex	= (sliceIndex + 1) % numSlices;
		int topRightPointIndex		= (sliceIndex + 1) % numSlices;
		int topLeftPointIndex		= sliceIndex;

		Vec2 currentFaceUVs;
		currentFaceUVs.x = (UVs.m_maxs.x - UVs.m_mins.x) * (0.5f * CosDegrees(currentYawAngle) + 0.5f);
		currentFaceUVs.y = (UVs.m_maxs.y - UVs.m_mins.y) * (0.5f * SinDegrees(currentYawAngle) + 0.5f);

		Vec2 nextFaceUVs;
		nextFaceUVs.x = (UVs.m_maxs.x - UVs.m_mins.x) * (0.5f * CosDegrees(currentYawAngle + angleChangePerSlice) + 0.5f);
		nextFaceUVs.y = (UVs.m_maxs.y - UVs.m_mins.y) * (0.5f * SinDegrees(currentYawAngle + angleChangePerSlice) + 0.5f);

		// bottom face
		int bottomFaceIndex = static_cast<unsigned int>(verts.size());

		Vec3 posA = startPosition;
		Vec3 posB = bottomLeftPoints[bottomRightPointIndex];
		Vec3 posC = bottomLeftPoints[bottomLeftPointIndex];

		Vec3 abVec = posB - posA;
		Vec3 acVec = posC - posA;
		Vec3 bcVec = posC - posB;
		Vec3 baVec = posA - posB;
		Vec3 caVec = posA - posC;
		Vec3 cbVec = posB - posC;

		Vec3 normalOnA = Vec3::DOWN;
		Vec3 normalOnB = Vec3::DOWN;
		Vec3 normalOnC = Vec3::DOWN;

		Vec3 tangentOnA = Vec3::FORWARD;
		Vec3 tangentOnB = Vec3::FORWARD;
		Vec3 tangentOnC = Vec3::FORWARD;

		Vec3 bitangentOnA = Vec3::LEFT;
		Vec3 bitangentOnB = Vec3::LEFT;
		Vec3 bitangentOnC = Vec3::LEFT;

		verts.push_back(Vertex_PCUTBN(posA, color, UVs.GetCenter(), tangentOnA, bitangentOnA, normalOnA));
		verts.push_back(Vertex_PCUTBN(posB, color, nextFaceUVs,		tangentOnB, bitangentOnB, normalOnB));
		verts.push_back(Vertex_PCUTBN(posC, color, currentFaceUVs,	tangentOnC, bitangentOnC, normalOnC));

		indexes.push_back(bottomFaceIndex);
		indexes.push_back(bottomFaceIndex + 1);
		indexes.push_back(bottomFaceIndex + 2);

		// quad
		AABB2 quadUVs;
		quadUVs.m_mins.x = uScale * sliceIndex;
		quadUVs.m_mins.y = 0.f;
		quadUVs.m_maxs.x = (sliceIndex + 1) * uScale;
		quadUVs.m_maxs.y = 1.f;

		Vec3 bottomLeft		= bottomLeftPoints[bottomLeftPointIndex];
		Vec3 bottomRight	= bottomLeftPoints[bottomRightPointIndex];
		Vec3 topRight		= topLeftPoints[topRightPointIndex];
		Vec3 topLeft		= topLeftPoints[topLeftPointIndex];

		Vec3 bottomLeftNormal	= (bottomLeft - startPosition).GetNormalized();
		Vec3 bottomRightNormal	= (bottomRight - startPosition).GetNormalized();
		Vec3 topRightNormal		= (topRight	- endPosition).GetNormalized();
		Vec3 topLeftNormal		= (topLeft - endPosition).GetNormalized();

		Vec3 bottomLeftTangent	= bottomLeftNormal.GetRotatedAboutZDegrees(90.f).GetNormalized();
		Vec3 bottomRightTangent = bottomRightNormal.GetRotatedAboutZDegrees(90.f).GetNormalized();
		Vec3 topRightTangent	= topRightNormal.GetRotatedAboutZDegrees(90.f).GetNormalized();
		Vec3 topLeftTangent		= topLeftNormal.GetRotatedAboutZDegrees(90.f).GetNormalized();

		Vec3 bottomLeftBitangent	= CrossProduct3D(bottomLeftNormal, bottomLeftTangent).GetNormalized();
		Vec3 bottomRightBitangent	= CrossProduct3D(bottomRightNormal, bottomRightTangent).GetNormalized();
		Vec3 topRightBitangent		= CrossProduct3D(topRightNormal, topRightTangent).GetNormalized();
		Vec3 topLeftBitangent		= CrossProduct3D(topLeftNormal, topLeftTangent).GetNormalized();

		int startIndex = static_cast<unsigned int>(verts.size());

		verts.push_back(Vertex_PCUTBN(bottomLeft,	color, quadUVs.m_mins,								bottomLeftTangent,	bottomLeftBitangent,	bottomLeftNormal));
		verts.push_back(Vertex_PCUTBN(bottomRight,	color, Vec2(quadUVs.m_maxs.x, quadUVs.m_mins.y),	bottomRightTangent, bottomRightBitangent,	bottomRightNormal));
		verts.push_back(Vertex_PCUTBN(topRight,		color, quadUVs.m_maxs,								topRightTangent,	topRightBitangent,		topRightNormal));
		verts.push_back(Vertex_PCUTBN(topLeft,		color, Vec2(quadUVs.m_mins.x, quadUVs.m_maxs.y),	topLeftTangent,		topLeftBitangent,		topLeftNormal));

		indexes.push_back(startIndex);
		indexes.push_back(startIndex + 1);
		indexes.push_back(startIndex + 2);
		indexes.push_back(startIndex);
		indexes.push_back(startIndex + 2);
		indexes.push_back(startIndex + 3);

		// top face
		int topFaceStartIndex = static_cast<unsigned int>(verts.size());
		
		Vec3 posD = topLeftPoints[topLeftPointIndex];
		Vec3 posE = topLeftPoints[topRightPointIndex];
		Vec3 posF = endPosition;

		Vec3 deVec = posE - posD;
		Vec3 dfVec = posF - posD;
		Vec3 efVec = posF - posE;
		Vec3 edVec = posD - posE;
		Vec3 fdVec = posD - posF;
		Vec3 feVec = posE - posF;

		Vec3 normalOnD = CrossProduct3D(deVec, dfVec).GetNormalized();
		Vec3 normalOnE = CrossProduct3D(efVec, edVec).GetNormalized();
		Vec3 normalOnF = CrossProduct3D(fdVec, feVec).GetNormalized();

		Vec3 tangentOnD = Vec3::FORWARD;
		Vec3 tangentOnE = Vec3::FORWARD;
		Vec3 tangentOnF = Vec3::FORWARD;

		Vec3 bitangentOnD = Vec3::LEFT;
		Vec3 bitangentOnE = Vec3::LEFT;
		Vec3 bitangentOnF = Vec3::LEFT;

		verts.push_back(Vertex_PCUTBN(posD, color, currentFaceUVs,		tangentOnD, bitangentOnD, normalOnD));
		verts.push_back(Vertex_PCUTBN(posE, color, nextFaceUVs,			tangentOnE, bitangentOnE, normalOnE));
		verts.push_back(Vertex_PCUTBN(posF, color, Vec2(0.5f, 0.5f),	tangentOnF, bitangentOnF, normalOnF));

		indexes.push_back(topFaceStartIndex);
		indexes.push_back(topFaceStartIndex + 1);
		indexes.push_back(topFaceStartIndex + 2);
		
		currentYawAngle += angleChangePerSlice;
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void AddVertsForCone3D(std::vector<Vertex_PCU>& verts, float height, float radius, Rgba8 const& color, AABB2 const& UVs, int numSlices)
{
	float pitchScale = 1.f / static_cast<float>(numSlices);
	float currentPitchAngle = 0.f;
	float angleChangePerSlice = 360.f * pitchScale;

	float uScale = UVs.GetDimensions().x / numSlices;

	Vec3 startPosition = Vec3(0.f, 0.f, 0.f);
	Vec3 heightVec = Vec3(height, 0.f, 0.f);

	std::vector<Vec3> bottomLeftPoints;

	for(int sliceIndex = 0; sliceIndex < numSlices; ++sliceIndex)
	{
		Vec3 bottomLeftPoint = startPosition + Vec3::MakeFromPolarDegrees(currentPitchAngle, 90.f, radius);
		bottomLeftPoints.push_back(bottomLeftPoint);

		currentPitchAngle += angleChangePerSlice;
	}

	for(int sliceIndex = 0; sliceIndex < numSlices; ++sliceIndex)
	{
		int bottomLeftPointIndex = sliceIndex;
		int bottomRightPointIndex = (sliceIndex + 1) % numSlices;

		// bottom face
		verts.push_back(Vertex_PCU(startPosition, color));
		verts.push_back(Vertex_PCU(bottomLeftPoints[bottomRightPointIndex], color));
		verts.push_back(Vertex_PCU(bottomLeftPoints[bottomLeftPointIndex], color));

		// tip 

		AABB2 triUV;
		triUV.m_mins.y = 0.f;
		triUV.m_maxs.y = 1.f;
		triUV.m_mins.x = sliceIndex * uScale;
		triUV.m_maxs.x = (sliceIndex + 1) * uScale;
		verts.push_back(Vertex_PCU(bottomLeftPoints[bottomLeftPointIndex], color));
		verts.push_back(Vertex_PCU(bottomLeftPoints[bottomRightPointIndex], color));
		verts.push_back(Vertex_PCU(startPosition + heightVec, color, UVs.m_maxs));
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void AddVertsForIndexedCone3D(std::vector<Vertex_PCUTBN>& verts, std::vector<unsigned int>& indexes, float height, float radius, Rgba8 const& color, AABB2 const& UVs, int numSlices)
{
	Vec3 tempTangent	= Vec3::ZERO;
	Vec3 tempBitangent	= Vec3::ZERO;

	float pitchScale			= 1.f / static_cast<float>(numSlices);
	float currentPitchAngle		= 0.f;
	float angleChangePerSlice	= 360.f * pitchScale;

	float uScale = UVs.GetDimensions().x / numSlices;

	Vec3 startPosition	= Vec3(-2.f, 0.f, 5.f);
	Vec3 heightVec		= Vec3(height, 0.f, 0.f);

	std::vector<Vec3> bottomLeftPoints;

	for(int sliceIndex = 0; sliceIndex < numSlices; ++sliceIndex)
	{
		Vec3 bottomLeftPoint = startPosition + Vec3::MakeFromPolarDegrees(currentPitchAngle, 90.f, radius);
		bottomLeftPoints.push_back(bottomLeftPoint);

		currentPitchAngle += angleChangePerSlice;
	}

	for(int sliceIndex = 0; sliceIndex < numSlices; ++sliceIndex)
	{
		unsigned int startIndexForBuffer = static_cast<unsigned int>(verts.size());

		int bottomLeftPointIndex	= sliceIndex;
		int bottomRightPointIndex	= (sliceIndex + 1) % numSlices;

		Vec2 currentFaceUVs;
		currentFaceUVs.x = (UVs.m_maxs.x - UVs.m_mins.x) * (0.5f * CosDegrees(currentPitchAngle) + 0.5f);
		currentFaceUVs.y = (UVs.m_maxs.y - UVs.m_mins.y) * (0.5f * SinDegrees(currentPitchAngle) + 0.5f);

		Vec2 nextFaceUVs;
		nextFaceUVs.x = (UVs.m_maxs.x - UVs.m_mins.x) * (0.5f * CosDegrees(currentPitchAngle + angleChangePerSlice) + 0.5f);
		nextFaceUVs.y = (UVs.m_maxs.y - UVs.m_mins.y) * (0.5f * SinDegrees(currentPitchAngle + angleChangePerSlice) + 0.5f);

		Vec3 posA = startPosition;
		Vec3 posB = bottomLeftPoints[bottomRightPointIndex];
		Vec3 posC = bottomLeftPoints[bottomLeftPointIndex];
		Vec3 posD = startPosition + heightVec;

		Vec3 abVec = posB - posA;
		Vec3 acVec = posC - posA;
		Vec3 bcVec = posC - posB;
		Vec3 baVec = posA - posB;
		Vec3 caVec = posA - posC;
		Vec3 cbVec = posB - posC;

		Vec3 normalOnA = CrossProduct3D(abVec, acVec).GetNormalized();
		Vec3 normalOnB = CrossProduct3D(bcVec, baVec).GetNormalized();
		Vec3 normalOnC = CrossProduct3D(caVec, cbVec).GetNormalized();

		// bottom face
		verts.push_back(Vertex_PCUTBN(posA, color, UVs.GetCenter(), tempTangent, tempBitangent,	normalOnA));
		verts.push_back(Vertex_PCUTBN(posB, color, nextFaceUVs,		tempTangent, tempBitangent,	normalOnB));
		verts.push_back(Vertex_PCUTBN(posC, color, currentFaceUVs,	tempTangent, tempBitangent,	normalOnC));

		// tip 

		AABB2 triUV;
		triUV.m_mins.y = 0.f;
		triUV.m_maxs.y = 1.f;
		triUV.m_mins.x = sliceIndex * uScale;
		triUV.m_maxs.x = (sliceIndex + 1) * uScale;

		Vec3 normalOnTipC = (posC - posA).GetNormalized();
		Vec3 normalOnTipB = (posB - posA).GetNormalized();
		Vec3 normalOnTip  = (posD - posA).GetNormalized();

		verts.push_back(Vertex_PCUTBN(posC, color, triUV.m_mins, tempTangent, tempBitangent, normalOnTipC));
		verts.push_back(Vertex_PCUTBN(posB, color, Vec2(triUV.m_maxs.x, triUV.m_mins.y), tempTangent, tempBitangent, normalOnTipB));
		verts.push_back(Vertex_PCUTBN(posD, color, Vec2(triUV.m_mins.x, triUV.m_maxs.y), tempTangent, tempBitangent, normalOnTip));

		indexes.push_back(startIndexForBuffer);
		indexes.push_back(startIndexForBuffer + 1);
		indexes.push_back(startIndexForBuffer + 2);
		indexes.push_back(startIndexForBuffer + 3);
		indexes.push_back(startIndexForBuffer + 4);
		indexes.push_back(startIndexForBuffer + 5);
	}

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void AddVertsForCone3D(std::vector<Vertex_PCU>& verts, float height, float radius, Rgba8 const& topColor, Rgba8 const& baseColor, AABB2 const& UVs, int numSlices)
{
	float pitchScale = 1.f / static_cast<float>(numSlices);
	float currentPitchAngle = 0.f;
	float angleChangePerSlice = 360.f * pitchScale;

	float uScale = UVs.GetDimensions().x / numSlices;

	Vec3 startPosition = Vec3(0.f, 0.f, 0.f);
	Vec3 heightVec = Vec3(height, 0.f, 0.f);

	std::vector<Vec3> bottomLeftPoints;

	for(int sliceIndex = 0; sliceIndex < numSlices; ++sliceIndex)
	{
		Vec3 bottomLeftPoint = startPosition + Vec3::MakeFromPolarDegrees(currentPitchAngle, 90.f, radius);
		bottomLeftPoints.push_back(bottomLeftPoint);

		currentPitchAngle += angleChangePerSlice;
	}

	for(int sliceIndex = 0; sliceIndex < numSlices; ++sliceIndex)
	{
		int bottomLeftPointIndex = sliceIndex;
		int bottomRightPointIndex = (sliceIndex + 1) % numSlices;

		// bottom face
		verts.push_back(Vertex_PCU(startPosition, baseColor));
		verts.push_back(Vertex_PCU(bottomLeftPoints[bottomRightPointIndex], baseColor));
		verts.push_back(Vertex_PCU(bottomLeftPoints[bottomLeftPointIndex], baseColor));

		// tip 

		AABB2 triUV;
		triUV.m_mins.y = 0.f;
		triUV.m_maxs.y = 1.f;
		triUV.m_mins.x = sliceIndex * uScale;
		triUV.m_maxs.x = (sliceIndex + 1) * uScale;
		verts.push_back(Vertex_PCU(bottomLeftPoints[bottomLeftPointIndex], topColor));
		verts.push_back(Vertex_PCU(bottomLeftPoints[bottomRightPointIndex], topColor));
		verts.push_back(Vertex_PCU(startPosition + heightVec, topColor, UVs.m_maxs));
	}

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void AddVertsForArrow3D(std::vector<Vertex_PCU>& verts, Vec3 const& startPos, Vec3 const& fwdNormal, float length, float radius, Rgba8 const& color, AABB2 const& UVs, int numSlices)
{

	std::vector<Vertex_PCU> tailVerts;
	std::vector<Vertex_PCU> headVerts;

	float headLength = length * 0.3f;
	float tailLength = length - headLength;

	Vec3 headStartPoint = startPos + (tailLength * fwdNormal);
	Vec3 endPosition = startPos + length * fwdNormal;

	AddVertsForCylinder3D(tailVerts, tailLength, radius, color, UVs, numSlices);

	Mat44 tailLookAtMatrix = GetLookAtTransform(startPos, headStartPoint);

	Mat44 tailTranslationMatrix = Mat44::MakeTranslation3D(startPos);

	tailLookAtMatrix.SetTranslation3D(startPos);

	TransformVertexArray3D(tailVerts, tailLookAtMatrix);


	AddVertsForCone3D(headVerts, headLength, radius + (radius * 0.5f), color, UVs, numSlices);

	Mat44 headLookAtMatrix = GetLookAtTransform(headStartPoint, endPosition);

	Mat44 headTranslationMatrix = Mat44::MakeTranslation3D(headStartPoint);

	headLookAtMatrix.SetTranslation3D(headStartPoint);

	TransformVertexArray3D(headVerts, headLookAtMatrix);


	for(int tailVert = 0; tailVert < static_cast<int>(tailVerts.size()); ++tailVert)
	{
		verts.push_back(tailVerts[tailVert]);
	}

	for(int headVert = 0; headVert < static_cast<int>(headVerts.size()); ++headVert)
	{
		verts.push_back(headVerts[headVert]);
	}

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void AddVertsForArrow3D(std::vector<Vertex_PCU>& verts, Vec3 const& startPos, Vec3 const& fwdNormal, float tailLength, float headLength, float tailRadius, float headRadius, Rgba8 const& color, AABB2 const& UVs, int numSlices)
{
	std::vector<Vertex_PCU> tailVerts;
	std::vector<Vertex_PCU> headVerts;

	Vec3 headStartPoint = startPos + (tailLength * fwdNormal);
	Vec3 endPosition = startPos + (tailLength + headLength) * fwdNormal;

	AddVertsForCylinder3D(tailVerts, tailLength, tailRadius, color, UVs, numSlices);

	Mat44 tailLookAtMatrix = GetLookAtTransform(startPos, headStartPoint);

	Mat44 tailTranslationMatrix = Mat44::MakeTranslation3D(startPos);

	tailLookAtMatrix.SetTranslation3D(startPos);

	TransformVertexArray3D(tailVerts, tailLookAtMatrix);

	AddVertsForCone3D(headVerts, headLength, headRadius, color, UVs, numSlices);

	Mat44 headLookAtMatrix = GetLookAtTransform(headStartPoint, endPosition);

	Mat44 headTranslationMatrix = Mat44::MakeTranslation3D(headStartPoint);

	headLookAtMatrix.SetTranslation3D(headStartPoint);

	TransformVertexArray3D(headVerts, headLookAtMatrix);


	for(int tailVert = 0; tailVert < static_cast<int>(tailVerts.size()); ++tailVert)
	{
		verts.push_back(tailVerts[tailVert]);
	}

	for(int headVert = 0; headVert < static_cast<int>(headVerts.size()); ++headVert)
	{
		verts.push_back(headVerts[headVert]);
	}

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void AddVertsForArrow3D(std::vector<Vertex_PCU>& verts, Ray3 const& ray, Rgba8 const& color, AABB2 const& UVs, int numSlices)
{

	std::vector<Vertex_PCU> tailVerts;
	std::vector<Vertex_PCU> headVerts;

	float headLength = 0.3f;
	float tailLength = ray.m_maxLength - headLength;

	float tailRadius = 0.015f;
	float headRadius = 0.07f;

	Vec3 headStartPoint = ray.m_startPos + (tailLength * ray.m_fwdNormal);
	Vec3 endPosition = ray.m_startPos + ray.m_maxLength * ray.m_fwdNormal;

	AddVertsForCylinder3D(tailVerts, tailLength, tailRadius, color, UVs, numSlices);

	Mat44 tailLookAtMatrix = GetLookAtTransform(ray.m_startPos, headStartPoint);

	Mat44 tailTranslationMatrix = Mat44::MakeTranslation3D(ray.m_startPos);

	tailLookAtMatrix.SetTranslation3D(ray.m_startPos);

	TransformVertexArray3D(tailVerts, tailLookAtMatrix);

	float red = static_cast<float>(color.r) * 0.5f;
	float green = static_cast<float>(color.g) * 0.5f;
	float blue = static_cast<float>(color.b) * 0.5f;

	Rgba8 headShade = Rgba8(static_cast<uchar>(red), static_cast<uchar>(green), static_cast<uchar>(blue), color.a);

	AddVertsForCone3D(headVerts, headLength, headRadius, color, headShade, UVs, numSlices);

	Mat44 headLookAtMatrix = GetLookAtTransform(headStartPoint, endPosition);

	Mat44 headTranslationMatrix = Mat44::MakeTranslation3D(headStartPoint);

	headLookAtMatrix.SetTranslation3D(headStartPoint);

	TransformVertexArray3D(headVerts, headLookAtMatrix);

	for(int tailVert = 0; tailVert < static_cast<int>(tailVerts.size()); ++tailVert)
	{
		verts.push_back(tailVerts[tailVert]);
	}

	for(int headVert = 0; headVert < static_cast<int>(headVerts.size()); ++headVert)
	{
		verts.push_back(headVerts[headVert]);
	}

}

