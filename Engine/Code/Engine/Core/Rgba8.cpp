
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vec4.hpp"

//------------------------------------------------------------------------------------------------------------------
// Global Constants

const Rgba8 Rgba8::BLACK		= Rgba8(0, 0, 0, 255);
const Rgba8 Rgba8::WHITE		= Rgba8(255, 255, 255, 255);
const Rgba8 Rgba8::RED			= Rgba8(255, 0, 0, 255);
const Rgba8 Rgba8::GREEN		= Rgba8(0, 255, 0, 255);
const Rgba8 Rgba8::BLUE			= Rgba8(0, 0, 255, 255);
const Rgba8 Rgba8::CYAN			= Rgba8(0, 255, 255, 255);
const Rgba8 Rgba8::YELLOW		= Rgba8(255, 255, 0, 255);
const Rgba8 Rgba8::ORANGE		= Rgba8(255, 165, 0, 255);
const Rgba8 Rgba8::ORANGERED	= Rgba8(238, 89, 44, 255);
const Rgba8 Rgba8::MAGENTA		= Rgba8(255, 0, 220, 255);
const Rgba8 Rgba8::GREY			= Rgba8(128, 128, 128, 255);


//------------------------------------------------------------------------------------------------------------------
Rgba8::Rgba8(uchar r, uchar g, uchar b, uchar a )
	: r(r)
	, g(g)
	, b(b)
	, a(a)

{}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Rgba8 Rgba8::StaticColorLerp(Rgba8 startColor, Rgba8 endColor, float fraction)
{

	Rgba8 returnColor; 

	float red = Lerp(NormalizeByte(startColor.r), NormalizeByte(endColor.r), fraction);
	float green = Lerp(NormalizeByte(startColor.g), NormalizeByte(endColor.g), fraction);
	float blue = Lerp(NormalizeByte(startColor.b), NormalizeByte(endColor.b), fraction);
	float alpha = Lerp(NormalizeByte(startColor.a), NormalizeByte(endColor.a), fraction);

	returnColor.r = DenormalizeByte(red);
	returnColor.g = DenormalizeByte(green);
	returnColor.b = DenormalizeByte(blue);
	returnColor.a = DenormalizeByte(alpha);


	return returnColor;
}


//------------------------------------------------------------------------------------------------------------------
void Rgba8::SetFromText(char const* textRgba8)
{

	Strings rgbaStrings = SplitStringOnDelimiter(textRgba8, ',');

	float redf =	static_cast<float>(atof(rgbaStrings[0].c_str()));
	float greenf =	static_cast<float>(atof(rgbaStrings[1].c_str()));
	float bluef =	static_cast<float>(atof(rgbaStrings[2].c_str()));

	 r	= static_cast<uchar>(GetClamped(redf, 0.f, 255.f));
	 g	= static_cast<uchar>(GetClamped(greenf, 0.f, 255.f));
	 b	= static_cast<uchar>(GetClamped(bluef, 0.f, 255.f));

	if(rgbaStrings.size() == 4)
	{
		float alphaf = static_cast<float>(atof(rgbaStrings[3].c_str()));

		a = static_cast<uchar>(GetClamped(alphaf, 0.f, 255.f));
	}

}


//------------------------------------------------------------------------------------------------------------------
Rgba8 Rgba8::ColorLerp(Rgba8 startColor, Rgba8 endColor, float fraction)
{

	float red = Lerp(NormalizeByte(startColor.r), NormalizeByte(endColor.r), fraction);
	float green = Lerp(NormalizeByte(startColor.g), NormalizeByte(endColor.g), fraction);
	float blue = Lerp(NormalizeByte(startColor.b), NormalizeByte(endColor.b), fraction);
	float alpha = Lerp(NormalizeByte(startColor.a), NormalizeByte(endColor.a), fraction);

	r = DenormalizeByte(red);
	g = DenormalizeByte(green);
	b = DenormalizeByte(blue);
	a = DenormalizeByte(alpha);

	return Rgba8(r, g, b, a);

}

//------------------------------------------------------------------------------------------------------------------
void Rgba8::GetAsFloat(float* colorAsFloat) const
{
	float red =	static_cast<float>(r);
	float green = static_cast<float>(g);
	float blue = static_cast<float>(b);
	float alpha = static_cast<float>(a);

	colorAsFloat[0] = RangeMapClamped(red, 0.f, 255.f, 0.f, 1.f);
	colorAsFloat[1] = RangeMapClamped(green, 0.f, 255.f, 0.f, 1.f);
	colorAsFloat[2] = RangeMapClamped(blue, 0.f, 255.f, 0.f, 1.f);
	colorAsFloat[3] = RangeMapClamped(alpha, 0.f, 255.f, 0.f, 1.f);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Vec4 Rgba8::GetAsVec4() const
{
	float ucharToFloatScale = 1.f / 255.f;

	float red	= r * ucharToFloatScale;
	float green = g * ucharToFloatScale;
	float blue	= b * ucharToFloatScale;
	float alpha = a * ucharToFloatScale;

	return Vec4(red, green, blue, alpha);
}
