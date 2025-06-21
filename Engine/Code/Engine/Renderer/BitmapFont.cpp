#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/AABB2.hpp"
//------------------------------------------------------------------------------------------------------------------
BitmapFont::BitmapFont(char const* fontFilePathWithNoExtension, Texture& fontTexture)
	: m_filePathNameWithNoExtension(fontFilePathWithNoExtension)
	, m_fontGlyphsSpriteSheet(fontTexture, IntVec2(16, 16))
{





}

//------------------------------------------------------------------------------------------------------------------
Texture& BitmapFont::GetTexture()
{

	return m_fontGlyphsSpriteSheet.GetTexture();

}


//------------------------------------------------------------------------------------------------------------------
void BitmapFont::AddVertsForText2D(std::vector<Vertex_PCU>& vertexArray, Vec2 const& textMins, float cellHeight, std::string const& text, Rgba8 const& tint, float cellAspectScale)
{

	float fontAspect = m_defultAspect * cellAspectScale;

	float cellWidth = cellHeight * fontAspect;


	for(int index = 0; index < static_cast<int>(text.size()); ++index)
	{
		int textSpriteIndex = text[index];

		SpriteDefinition def = m_fontGlyphsSpriteSheet.GetSpriteDef(textSpriteIndex);

		AABB2 uvs = def.GetUVs();

		Vec2 mins = Vec2(textMins.x + (index * cellWidth), textMins.y);
		Vec2 maxs = Vec2(textMins.x + ((index + 1) * cellWidth), textMins.y + cellHeight);

		AddVertsForAABB2D(vertexArray, AABB2(mins, maxs), tint, uvs.m_mins, uvs.m_maxs);
	
	}
}


//------------------------------------------------------------------------------------------------------------------
void BitmapFont::AddVertsForTextInBox2D(std::vector<Vertex_PCU>& vertexArray, std::string const& text, AABB2 const& box, float cellHeight, Rgba8 const& tint, float cellAspect, Vec2 const& alignment, TextBoxMode mode, int maxGlyphsToDraw)
{


	float boxHeight = box.m_maxs.y - box.m_mins.y;
	float boxWidth = box.m_maxs.x - box.m_mins.x;

	int numGlyphsDrawn = 0;

	if(mode == SHRINK_TO_FIT)
	{
		float maxTextWidth = GetTextWidthForMultiLine(cellHeight, text, cellAspect);
		int numLines = static_cast<int>(SplitStringOnDelimiter(text, '\n').size());
		float textHeight = numLines * cellHeight;

		float scaleX = boxWidth / maxTextWidth;
		float scaleY = boxHeight / textHeight;

		float uniformScale = scaleX < scaleY ? scaleX : scaleY;
	
		uniformScale = GetClampedZeroToOne(uniformScale);

		float minCellHeight = 8.0f;
		
		cellHeight = cellHeight * uniformScale > minCellHeight ? cellHeight * uniformScale : minCellHeight;

		if(maxTextWidth > boxWidth)
		{
			cellHeight *= boxWidth / maxTextWidth;
		}
	}

	std::vector<std::string> multiLineTexts = SplitStringOnDelimiter(text, '\n');

	float textHeight = static_cast<int>(multiLineTexts.size()) * cellHeight;
	float paddingY = boxHeight - textHeight;

	float textBoxMinsY = box.m_mins.y + (paddingY * alignment.y);
	float textBoxMaxsY = textBoxMinsY + textHeight;

	AABB2 textBox(Vec2(box.m_mins.x, textBoxMinsY), Vec2(box.m_maxs.x, textBoxMaxsY));

	for(int lineIndex = 0; lineIndex < static_cast<int>(multiLineTexts.size()); ++lineIndex)
	{
		std::string lineText = multiLineTexts[lineIndex];

		float textWidth = GetTextWidth(cellHeight, lineText, cellAspect);
		float paddingX = boxWidth - textWidth;

		float textMinsX = (paddingX * alignment.x) + textBox.m_mins.x;

		float textMaxY = textBox.m_maxs.y - (lineIndex * cellHeight);

		Vec2 textPos = Vec2(textMinsX, textMaxY);

		float fontAspect = m_defultAspect * cellAspect;

		float cellWidth = cellHeight * fontAspect;

		for(int index = 0; index < static_cast<int>(lineText.size()); ++index)
		{

			int textSpriteIndex = lineText[index];

			SpriteDefinition def = m_fontGlyphsSpriteSheet.GetSpriteDef(textSpriteIndex);

			AABB2 uvs = def.GetUVs();

			Vec2 mins = Vec2(textPos.x + (index * cellWidth), textPos.y - cellHeight);
			Vec2 maxs = Vec2(textPos.x + ((index + 1) * cellWidth), textPos.y);

			if(numGlyphsDrawn <= maxGlyphsToDraw)
			{
				AddVertsForAABB2D(vertexArray, AABB2(mins, maxs), tint, uvs.m_mins, uvs.m_maxs);
				numGlyphsDrawn += 1;
			}
		}
	}

}


//------------------------------------------------------------------------------------------------------------------
float BitmapFont::GetTextWidth(float cellHeight, std::string const& text, float cellAspectScale)
{

	float fontAspect = m_defultAspect * cellAspectScale;

	float cellWidth = cellHeight * fontAspect;

	return cellWidth * text.size();

}


//------------------------------------------------------------------------------------------------------------------
float BitmapFont::GetTextWidthForMultiLine(float cellHeight, std::string const& text, float cellAspectScale)
{

	float maxWidth = 0.f;

	std::vector<std::string> multiLineTexts = SplitStringOnDelimiter(text, '\n');

	for(int i = 0; i < static_cast<int>(multiLineTexts.size()); ++i)
	{
		std::string lineText = multiLineTexts[i];

		float width = GetTextWidth(cellHeight, lineText, cellAspectScale);

		if(width > maxWidth)
		{
			maxWidth = width;
		}

	}

	return maxWidth;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void BitmapFont::GetWidthAndHeightForMultiLineText(Vec2& out_widthAndHeight, float cellHeight, std::string const& text, float cellAspectScale)
{
	out_widthAndHeight.x = GetTextWidthForMultiLine(cellHeight, text, cellAspectScale);

	std::vector<std::string> multiLineTexts = SplitStringOnDelimiter(text, '\n');
	
	out_widthAndHeight.y = static_cast<float>(multiLineTexts.size()) * cellHeight;

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void BitmapFont::AddVertsForText3DAtOriginXForward(std::vector<Vertex_PCU>& verts, float cellHeight, std::string const& text, Rgba8 const& tint, float cellAspect, Vec2 const& alignment, int maxGlyphsToDraw)
{

	UNUSED(maxGlyphsToDraw);

	AddVertsForText2D(verts, Vec2(0.f, 0.f), cellHeight, text, tint, cellAspect);

	AABB2 vertBounds = GetVertexBounds(verts);

	float height = vertBounds.m_maxs.y - vertBounds.m_mins.y;
	float width = vertBounds.m_maxs.x - vertBounds.m_mins.x;

	Vec3 iBasis = Vec3(0.f, 1.f, 0.f);
	Vec3 jBasis = Vec3(0.f, 0.f, 1.f);
	Vec3 kBasis = Vec3(1.f, 0.f, 0.f);

	Vec3 translation3D = Vec3(-alignment.x * width, -alignment.y * height, 0.f);

	Mat44 transformationMatrix = Mat44();
	transformationMatrix.SetIJK3D(iBasis, jBasis, kBasis);

	transformationMatrix.AppendTranslation3D(translation3D);

	TransformVertexArray3D(verts, transformationMatrix);
}

//------------------------------------------------------------------------------------------------------------------
std::string const& BitmapFont::GetImageFilePath() const
{

	return m_filePathNameWithNoExtension;

}

//------------------------------------------------------------------------------------------------------------------
float BitmapFont::GetGlyphAspect(int glyphUnicode) const
{
	UNUSED(glyphUnicode);

	return m_defultAspect;
}
