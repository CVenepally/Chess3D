#pragma once
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Core/Rgba8.hpp"
#include <vector>
//------------------------------------------------------------------------------------------------------------------

struct AABB2;
struct Vertex_PCU;
struct FloatRange;
struct Rgba8;



//------------------------------------------------------------------------------------------------------------------
class TileHeatMap
{
public:

	TileHeatMap(IntVec2 const& dimensions, float initialValue = 0.f);

	void	SetValues(float valueToSet);
	void	SetHeatValueForTileCoord(IntVec2 tileCoord, float value);
	
	float	GetHeatValueForTileCoord(IntVec2 tileCoord);

	FloatRange GetRangeOfValuesExcludingSpecial(float specialValueToIgnore) const;

	void	AddVertsForDebugDraw(std::vector<Vertex_PCU>& verts, AABB2 totalBounds, FloatRange floatRange = FloatRange::ZERO_TO_ONE, float specialValue = 99999.f, Rgba8 lowColor = Rgba8::BLACK, Rgba8 highColor = Rgba8::WHITE, Rgba8 specialColor = Rgba8::BLUE);
	void	AddVertsForDebugDraw(std::vector<Vertex_PCU>& verts, AABB2 totalBounds, Rgba8 lowColor, Rgba8 highColor = Rgba8::WHITE);

private:

	IntVec2				m_dimensions;
	std::vector<float>	m_values;
};