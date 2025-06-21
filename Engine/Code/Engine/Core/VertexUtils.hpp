#pragma once

#include <vector> 
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Rgba8.hpp"

struct Vec2;
struct Vec3;
struct Vertex_PCU;
struct Vertex_PCUTBN;
struct Capsule2;
struct OBB2;
struct AABB3;
struct OBB3;
struct LineSegment2;
struct LineSegment3;
struct Disc2;
struct Sphere;
struct Cylinder3D;
struct Ray3;
struct Triangle2;
struct Mat44;

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void TransformVertexArrayXY3D(int numVerts, Vertex_PCU* verts, float uniformScaleXY, float rotationDegreesAboutZ, Vec2 const& translationXY);
void TransformVertexArrayXY3D(std::vector<Vertex_PCU>& verts, float uniformScaleXY, float rotationDegreesAboutZ, Vec2 const& translationXY);

void TransformVertexArray3D(std::vector<Vertex_PCU>& verts, Mat44 const& transform);
void TransformVertexArray3DForPartialVector(std::vector<Vertex_PCU>& verts, Mat44 const& transform, int startPos, int endPos);

AABB2 GetVertexBounds(std::vector<Vertex_PCU>& verts);

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void AddVertsForCapsule2D(std::vector<Vertex_PCU>& verts, Capsule2 const& capsule, Rgba8 const& color);
void AddVertsForCapsule2D(std::vector<Vertex_PCU>& verts, Vec2 const& boneStart, Vec2 const& boneEnd, float radius, Rgba8 const& color);

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void AddVertsForDisc2D(std::vector<Vertex_PCU>& verts, Vec2 const& center, float radius, Rgba8 const& color);
void AddVertsForDisc2D(std::vector<Vertex_PCU>& verts, Disc2 disc, Rgba8 const& color);

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void AddVertsForAABB2D(std::vector<Vertex_PCU>& verts, AABB2 const& bounds, Rgba8 const& color);
void AddVertsForAABB2D(std::vector<Vertex_PCU>& verts, AABB2 const& bounds, Rgba8 const& color, Vec2 const& minUVs, Vec2 const& maxUVs);
void AddVertsForAABB2D(std::vector<Vertex_PCU>& verts, AABB2 const& bounds, Rgba8 const& color, AABB2 UVs);

void AddVertsForOBB2D(std::vector<Vertex_PCU>& verts, OBB2 const& box, Rgba8 const& color);

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void AddVertsForLineSegment2D(std::vector<Vertex_PCU>& verts, Vec2 const& start, Vec2 const& end, float thickness, Rgba8 const& color);
void AddVertsForLineSegment2D(std::vector<Vertex_PCU>& verts, LineSegment2 const& lineSegment, float thickness, Rgba8 const& color);
void AddVertsForLineSegment2D(std::vector<Vertex_PCU>& verts, Vec2 const& start, Vec2 const& end, float thickness, Rgba8 const& startColor, Rgba8 const& endColor);

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void AddVertsForTriangle2D(std::vector<Vertex_PCU>& verts, Vec2 const& ccOne, Vec2 const& ccTwo, Vec2 const& ccThree, Rgba8 const& color);
void AddVertsForTriangle2D(std::vector<Vertex_PCU>& verts, Vec2 const& ccOne, Vec2 const& ccTwo, Vec2 const& ccThree, Rgba8 const& color, AABB2 UVs);
void AddVertsForTriangle2D(std::vector<Vertex_PCU>& verts, Triangle2 const& triangle, Rgba8 const& color);

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void AddVertsForRay2(std::vector<Vertex_PCU>& verts, Vec2 tailPos, Vec2 tipPos, float arrowSize, float lineThickness, Rgba8 const& color);

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void AddVertsForRing(std::vector<Vertex_PCU>& verts, Vec2 const& center, float radius, float thickness, Rgba8 const& color);

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void AddVertsForQuad3D(std::vector<Vertex_PCU>& verts, Vec3 const& bottomLeft, Vec3 const& bottomRight, Vec3 const& topRight, Vec3 const& topLeft, Rgba8 const& color = Rgba8::WHITE, AABB2 const& UVs = AABB2::ZERO_TO_ONE);
void AddVertsForQuad3D(std::vector<Vertex_PCUTBN>& verts, std::vector<unsigned int>& indices, Vec3 const& bottomLeft, Vec3 const& bottomRight, Vec3 const& topRight, Vec3 const& topLeft, Rgba8 const& color = Rgba8::WHITE, AABB2 const& UVs = AABB2::ZERO_TO_ONE);

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void AddVertsForRoundedQuad3D_VPCU(std::vector<Vertex_PCU>& verts, Vec3 bottomLeft, Vec3 bottomRight, Vec3 topRight, Vec3 topLeft, Rgba8 const& color = Rgba8::WHITE, AABB2 const& UVs = AABB2::ZERO_TO_ONE);
void AddVertsForRoundedQuad3D(std::vector<Vertex_PCUTBN>& verts, Vec3 const& bottomLeft, Vec3 const& bottomRight, Vec3 const& topRight, Vec3 const& topLeft, Rgba8 const& color = Rgba8::WHITE, AABB2 const& UVs = AABB2::ZERO_TO_ONE);
void AddVertsForRoundedQuad3D(std::vector<Vertex_PCUTBN>& verts, std::vector<unsigned int>& indices, Vec3 const& bottomLeft, Vec3 const& bottomRight, Vec3 const& topRight, Vec3 const& topLeft, Rgba8 const& color = Rgba8::WHITE, AABB2 const& UVs = AABB2::ZERO_TO_ONE);

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void AddVertsForSphere3D(std::vector<Vertex_PCU>& verts, Vec3 const& center, float radius, Rgba8 const& color = Rgba8::WHITE, AABB2 const& UVs = AABB2::ZERO_TO_ONE, int numSlices = 16, int numStacks = 8);
void AddVertsForSphere3D(std::vector<Vertex_PCU>& verts, Sphere const& sphere, Rgba8 const& color = Rgba8::WHITE, AABB2 const& UVs = AABB2::ZERO_TO_ONE, int numSlices = 16, int numStacks = 8);
void AddVertsForIndexedSphere3D(std::vector<Vertex_PCUTBN>& verts, std::vector<unsigned int>& indexes, Sphere const& sphere, Rgba8 const& color = Rgba8::WHITE, AABB2 const& UVs = AABB2::ZERO_TO_ONE, int numSlices = 16, int numStacks = 8, bool debugTangents = false);

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void AddVertsForAABB3D(std::vector<Vertex_PCU>& verts, AABB3 const& bounds, Rgba8 const& color = Rgba8::WHITE, AABB2 const& UVs = AABB2::ZERO_TO_ONE);
void AddVertsForAABB3D(std::vector<Vertex_PCUTBN>& verts, std::vector<unsigned int>& indices, AABB3 const& bounds, Rgba8 const& color = Rgba8::WHITE, AABB2 const& UVs = AABB2::ZERO_TO_ONE);

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void AddVertsForOBB3D(std::vector<Vertex_PCU>& verts, OBB3 const& bounds, Rgba8 const& color = Rgba8::WHITE, AABB2 const& UVs = AABB2::ZERO_TO_ONE);
void AddVertsForIndexedOBB3D(std::vector<Vertex_PCUTBN>& verts, std::vector<unsigned int>& indices, OBB3 const& bounds, Rgba8 const& color = Rgba8::WHITE, AABB2 const& UVs = AABB2::ZERO_TO_ONE);

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void AddVertsForCylinder3D(std::vector<Vertex_PCU>& verts, float height, float radius, Rgba8 const& color = Rgba8::WHITE, AABB2 const& UVs = AABB2::ZERO_TO_ONE, int numSlices = 8);
void AddVertsForZCylinder3D(std::vector<Vertex_PCU>& verts, Cylinder3D const& cylinder, Rgba8 const& color = Rgba8::WHITE, AABB2 const& UVs = AABB2::ZERO_TO_ONE, int numSlices = 8);
void AddVertsForIndexedZCylinder3D(std::vector<Vertex_PCUTBN>& verts, std::vector<unsigned int>& indexes, Cylinder3D const& cylinder, Rgba8 const& color = Rgba8::WHITE, AABB2 const& UVs = AABB2::ZERO_TO_ONE, int numSlices = 8);

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void AddVertsForCone3D(std::vector<Vertex_PCU>& verts, float height, float radius, Rgba8 const& color = Rgba8::WHITE, AABB2 const& UVs = AABB2::ZERO_TO_ONE, int numSlices = 8);
void AddVertsForCone3D(std::vector<Vertex_PCU>& verts, float height, float radius, Rgba8 const& topColor = Rgba8::WHITE, Rgba8 const& baseColor = Rgba8::WHITE, AABB2 const& UVs = AABB2::ZERO_TO_ONE, int numSlices = 8);
void AddVertsForIndexedCone3D(std::vector<Vertex_PCUTBN>& verts, std::vector<unsigned int>& indexes, float height, float radius, Rgba8 const& color = Rgba8::WHITE, AABB2 const& UVs = AABB2::ZERO_TO_ONE, int numSlices = 8);
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void AddVertsForArrow3D(std::vector<Vertex_PCU>& verts, Vec3 const& startPosition, Vec3 const& fwdNormal, float length, float radius, Rgba8 const& color = Rgba8::WHITE, AABB2 const& UVs = AABB2::ZERO_TO_ONE, int numSlices = 32);
void AddVertsForArrow3D(std::vector<Vertex_PCU>& verts, Vec3 const& startPosition, Vec3 const& fwdNormal, float tailLength, float headLength, float tailRadius, float headRadius, Rgba8 const& color = Rgba8::WHITE, AABB2 const& UVs = AABB2::ZERO_TO_ONE, int numSlices = 32);
void AddVertsForArrow3D(std::vector<Vertex_PCU>& verts, Ray3 const& ray, Rgba8 const& color = Rgba8::WHITE, AABB2 const& UVs = AABB2::ZERO_TO_ONE, int numSlices = 32);

