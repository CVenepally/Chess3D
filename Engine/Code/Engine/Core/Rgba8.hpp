#pragma once

struct Vec4;

typedef unsigned char uchar; 

struct Rgba8
{

public: 

	uchar r = 255;
	uchar g = 255;
	uchar b = 255;
	uchar a = 255;

	static const Rgba8 BLACK;
	static const Rgba8 WHITE;
	static const Rgba8 RED;
	static const Rgba8 GREEN;
	static const Rgba8 BLUE;
	static const Rgba8 CYAN;
	static const Rgba8 YELLOW;
	static const Rgba8 ORANGE;
	static const Rgba8 ORANGERED;
	static const Rgba8 MAGENTA;
	static const Rgba8 GREY;


public:

	Rgba8() {}
	~Rgba8() {}
	explicit Rgba8(uchar r, uchar g, uchar b, uchar a = 255);

	static Rgba8 StaticColorLerp(Rgba8 startColor, Rgba8 endColor, float fraction);

	void SetFromText(char const* textRgba8);
 	Rgba8 ColorLerp(Rgba8 startColor, Rgba8 endColor, float fraction);
	void GetAsFloat(float* colorAsFloat) const;
	Vec4 GetAsVec4() const;

};
