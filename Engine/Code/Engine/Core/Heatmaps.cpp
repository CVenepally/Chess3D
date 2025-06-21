#include "Engine/Core/HeatMaps.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/FloatRange.hpp"


//------------------------------------------------------------------------------------------------------------------
TileHeatMap::TileHeatMap(IntVec2 const& dimensions, float initialValue)
	: m_dimensions(dimensions)
{

	int numValues = m_dimensions.x * m_dimensions.y;

	m_values.resize(numValues, initialValue);


}

//------------------------------------------------------------------------------------------------------------------
void TileHeatMap::SetValues(float valueToSet)
{
	for(int index = 0; index < static_cast<int>(m_values.size()); ++index)
	{
		m_values[index] = valueToSet;
	}
}


//------------------------------------------------------------------------------------------------------------------
void TileHeatMap::SetHeatValueForTileCoord(IntVec2 tileCoord, float value)
{

	int index = (tileCoord.y * m_dimensions.x) + tileCoord.x;
	
	if(index < static_cast<int>(m_values.size()))
	{
		m_values[index] = value;
	}

}


//------------------------------------------------------------------------------------------------------------------
float TileHeatMap::GetHeatValueForTileCoord(IntVec2 tileCoord)
{

	int index = (tileCoord.y * m_dimensions.x) + tileCoord.x;

	return m_values[index];
}


//------------------------------------------------------------------------------------------------------------------
void TileHeatMap::AddVertsForDebugDraw(std::vector<Vertex_PCU>& verts, AABB2 totalBounds, FloatRange floatRange, float specialValue, Rgba8 lowColor, Rgba8 highColor, Rgba8 specialColor)
{

	float tileWidth = (totalBounds.m_maxs.x - totalBounds.m_mins.x) / m_dimensions.x;
	float tileHeight = (totalBounds.m_maxs.y - totalBounds.m_mins.y) / m_dimensions.y;

	for(int y = 0; y < m_dimensions.y; ++y)
	{
		for(int x = 0; x < m_dimensions.x; ++x)
		{

			int tileIndex = (y * m_dimensions.x) + x;
			
			if(tileIndex >= m_dimensions.x * m_dimensions.y)
			{
				continue;
			}

			Vec2 tileMin = Vec2(static_cast<float>(x), static_cast<float>(y));
			Vec2 tileMax = Vec2(x + tileWidth, y + tileHeight);
			AABB2 tileBound = AABB2(tileMin, tileMax);
			Rgba8 tileColor;

			if(m_values[tileIndex] == specialValue)
			{
				
				tileColor = specialColor;

				AddVertsForAABB2D(verts, tileBound, tileColor);
				continue;
			}


			float fraction = RangeMapClamped(m_values[tileIndex], floatRange.m_min, floatRange.m_max, 0.f, 1.f);

			tileColor = tileColor.ColorLerp(lowColor, highColor, fraction);
			
			AddVertsForAABB2D(verts, tileBound, tileColor);

		}
	}
}

//------------------------------------------------------------------------------------------------------------------
void TileHeatMap::AddVertsForDebugDraw(std::vector<Vertex_PCU>& verts, AABB2 totalBounds, Rgba8 lowColor, Rgba8 highColor)
{

	float tileWidth = (totalBounds.m_maxs.x - totalBounds.m_mins.x) / m_dimensions.x;
	float tileHeight = (totalBounds.m_maxs.y - totalBounds.m_mins.y) / m_dimensions.y;

	for(int y = 0; y < m_dimensions.y; ++y)
	{
		for(int x = 0; x < m_dimensions.x; ++x)
		{

			int tileIndex = (y * m_dimensions.x) + x;

			if(tileIndex >= m_dimensions.x * m_dimensions.y)
			{
				continue;
			}

			Vec2 tileMin = Vec2(static_cast<float>(x), static_cast<float>(y));
			Vec2 tileMax = Vec2(x + tileWidth, y + tileHeight);
			AABB2 tileBound = AABB2(tileMin, tileMax);

			Rgba8 tileColor;

			if(m_values[tileIndex] == 0.f)
			{
				tileColor = lowColor;
			}

			if(m_values[tileIndex] == 1.f)
			{
				tileColor = highColor;
			}


			AddVertsForAABB2D(verts, tileBound, tileColor);

		}
	}
}