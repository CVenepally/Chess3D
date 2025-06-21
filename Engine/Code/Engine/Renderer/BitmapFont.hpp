#pragma once

#include "Engine/Core/Rgba8.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"

#include <vector>
#include <string>
//------------------------------------------------------------------------------------------------------------------

class Texture;

struct Vec2;
struct Vertex_PCU;
struct AABB2;


//------------------------------------------------------------------------------------------------------------------

enum TextBoxMode
{
	SHRINK_TO_FIT,
	OVERRUN
};

//------------------------------------------------------------------------------------------------------------------
class BitmapFont
{
	friend class Renderer;

private:

	BitmapFont(char const* fontFilePathWithNoExtension, Texture& fontTexture);


public:

	Texture& GetTexture();
	
	void AddVertsForText2D(std::vector<Vertex_PCU>& vertexArray, Vec2 const& textMins, float cellHeight, std::string const& text, Rgba8 const& tint = Rgba8::WHITE, float cellAspectScale = 1.f);
	void AddVertsForTextInBox2D(std::vector<Vertex_PCU>& vertexArray, std::string const& text, AABB2 const& box, float cellHeight, Rgba8 const& tint = Rgba8::WHITE, float cellAspect = 1.0f, 
							 Vec2 const& alignment = Vec2(0.5f, 0.5f), TextBoxMode mode = TextBoxMode::SHRINK_TO_FIT, int maxGlyphsToDraw = INT_MAX);

	float GetTextWidth(float cellHeight, std::string const& text, float cellAspectSCale = 1.f);
	float GetTextWidthForMultiLine(float cellHeight, std::string const& text, float cellAspectScale = 1.f);

	void GetWidthAndHeightForMultiLineText(Vec2& out_widthAndHeight, float cellHeight, std::string const& text, float cellAspectScale = 1.f);

	void AddVertsForText3DAtOriginXForward(std::vector<Vertex_PCU>& verts, float cellHeight, std::string const& text, Rgba8 const& tint = Rgba8::WHITE, float cellAspect = 1.f, Vec2 const& alignment = Vec2(0.5f, 0.5f), int maxGlyphsToDraw = 999);

	std::string const& GetImageFilePath() const;

protected:

	float GetGlyphAspect(int glyphUnicode) const;


protected:

	std::string		m_filePathNameWithNoExtension;
	SpriteSheet		m_fontGlyphsSpriteSheet;
	float			m_defultAspect = 1.0f;
};