#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Vec2.hpp"

//------------------------------------------------------------------------------------------------------------------
SpriteDefinition::SpriteDefinition(SpriteSheet const& spriteSheet, int spriteIndex, Vec2 const& uvAtMins, Vec2 const& uvAtMaxs)
	: m_spriteSheet(spriteSheet)
	, m_spriteIndex(spriteIndex)
	, m_uvAtMins(uvAtMins)
	, m_uvAtMaxs(uvAtMaxs)
{}


//------------------------------------------------------------------------------------------------------------------
void SpriteDefinition::GetUVs(Vec2& out_uvAtMins, Vec2& out_uvAtMaxs) const
{
	out_uvAtMins = m_uvAtMins;
	out_uvAtMaxs = m_uvAtMaxs;
}


//------------------------------------------------------------------------------------------------------------------
AABB2 SpriteDefinition::GetUVs() const
{
	return AABB2(m_uvAtMins, m_uvAtMaxs);
}


//------------------------------------------------------------------------------------------------------------------
SpriteSheet const& SpriteDefinition::GetSpriteSheet() const
{
	return m_spriteSheet;
}


//------------------------------------------------------------------------------------------------------------------
Texture& SpriteDefinition::GetTexture() const
{
	return m_spriteSheet.GetTexture();
}


//------------------------------------------------------------------------------------------------------------------
float SpriteDefinition::GetAspect() const
{

	Vec2 bottomRightUV = Vec2(m_uvAtMaxs.x, m_uvAtMins.y);
	Vec2 topLeftUV = Vec2(m_uvAtMins.x, m_uvAtMaxs.y);

	Vec2 xVec = bottomRightUV - m_uvAtMins;
	Vec2 yVec = topLeftUV - m_uvAtMins;

	float width = xVec.GetLength();
	float length = yVec.GetLength();

	return width / length;

}
