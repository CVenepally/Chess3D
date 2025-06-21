#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/Texture.hpp"

//------------------------------------------------------------------------------------------------------------------
SpriteSheet::SpriteSheet(Texture& texture, IntVec2 const& simpleGridLayout)
	: m_texture(texture)
{

	m_spriteDefs.reserve(simpleGridLayout.x * simpleGridLayout.y);

	int numRows = simpleGridLayout.x;
	int numCols = simpleGridLayout.y;

	IntVec2 textureDimension = m_texture.GetDimensions();

	float uNudge = 1.f / 128.f * 1.f / static_cast<float>(textureDimension.x);
	float vNudge = 1.f / 128.f * 1.f / static_cast<float>(textureDimension.y);

	float maxV = static_cast<float>(numCols);
	float minV = maxV - 1.f;

	for(int spriteIndex = 0; spriteIndex < (numCols * numRows); ++spriteIndex)
	{

		float minU = static_cast<float>(spriteIndex % numRows);
		float maxU = minU + 1.f;

		Vec2 spriteMinUVs = Vec2(minU / static_cast<float>(numRows) + uNudge, minV / static_cast<float>(numCols) + vNudge);
		Vec2 spriteMaxUVs = Vec2(maxU / static_cast<float>(numRows) - uNudge, maxV / static_cast<float>(numCols) - vNudge);

		SpriteDefinition spriteDef = SpriteDefinition(*this, spriteIndex, spriteMinUVs, spriteMaxUVs);
		m_spriteDefs.push_back(spriteDef);

		if(minU == numRows - 1)
		{
			maxV -= 1.f;
			minV = maxV - 1.f;
		}

	}

}


//------------------------------------------------------------------------------------------------------------------
Texture& SpriteSheet::GetTexture() const
{
	return m_texture;
}


//------------------------------------------------------------------------------------------------------------------
int SpriteSheet::GetNumSprites() const
{
	return static_cast<int>(m_spriteDefs.size());
}


//------------------------------------------------------------------------------------------------------------------
SpriteDefinition const& SpriteSheet::GetSpriteDef(int spriteIndex) const
{

	return m_spriteDefs[spriteIndex];

}


//------------------------------------------------------------------------------------------------------------------
void SpriteSheet::GetSpriteUVs(Vec2& out_uvAtMins, Vec2& out_uvAtMaxs, int spriteIndex) const
{

	m_spriteDefs[spriteIndex].GetUVs(out_uvAtMins, out_uvAtMaxs);

}


//------------------------------------------------------------------------------------------------------------------
AABB2 SpriteSheet::GetSpriteUVs(int spriteIndex) const
{
	return m_spriteDefs[spriteIndex].GetUVs();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
AABB2 SpriteSheet::GetSpriteUVs(IntVec2 spriteCoords, int spriteSheetWidth) const
{
	int spriteIndex = spriteCoords.x + (spriteCoords.y * spriteSheetWidth);

	return m_spriteDefs[spriteIndex].GetUVs();
}
