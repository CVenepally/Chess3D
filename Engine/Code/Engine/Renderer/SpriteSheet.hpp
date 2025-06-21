#pragma once

#include "Engine/Renderer/SpriteDefinition.hpp"

#include <vector>
//------------------------------------------------------------------------------------------------------------------
class Texture;

struct IntVec2;

//------------------------------------------------------------------------------------------------------------------
class SpriteSheet
{

public:

	SpriteSheet() = default;
	explicit SpriteSheet(Texture& texture, IntVec2 const& simpleGridLayout);
	

	Texture&					GetTexture() const;
	int							GetNumSprites() const;
	SpriteDefinition const&		GetSpriteDef(int spriteIndex) const;
	void						GetSpriteUVs(Vec2& out_uvAtMins, Vec2& out_uvAtMaxs, int spriteIndex) const;
	AABB2						GetSpriteUVs(int spriteIndex) const;
	AABB2						GetSpriteUVs(IntVec2 spriteCoords, int spriteSheetWidth) const;


protected:

	Texture&							m_texture;
	std::vector<SpriteDefinition>		m_spriteDefs;

};