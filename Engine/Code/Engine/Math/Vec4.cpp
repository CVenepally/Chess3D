#include "Engine/Math/Vec4.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
//#include "Engine/Core/EngineCommon.hpp"

#include <math.h>


const Vec4 Vec4::ZERO = Vec4(0.f, 0.f, 0.f, 0.f);
const Vec4 Vec4::ONE = Vec4(1.f, 1.f, 1.f, 1.f);

Vec4::Vec4( Vec4 const& copy )
	: x( copy.x )
	, y( copy.y )
	, z( copy.z )
	, w( copy.w )
{
}



Vec4::Vec4( float initialX, float initialY, float initialZ, float initialW )
	: x( initialX )
	, y( initialY )
	, z( initialZ )
	, w( initialW)
{
}



float Vec4::GetLength() const
{

	float length = sqrtf((x * x) + (y * y) + (z * z) + (w * w));

	return length;

}



float Vec4::GetLengthXY() const
{
	
	float length = sqrtf((x * x) + (y * y));

	return length;

}



float Vec4::GetLengthSquared() const
{
	
	float length = (x * x) + (y * y) + (z * z) + (w * w);
	
	return length;

}



float Vec4::GetLengthXYSquared() const
{

	float length = (x * x) + (y * y);
	
	return length;

}



float Vec4::GetAngleAboutZRadians() const
{

	return atan2f(y, x);

}



float Vec4::GetAngleAboutZDegrees() const
{

	return ATan2Degrees(y, x);

}



Vec4 const Vec4::GetRotatedAboutZRadians(float deltaRadians) const
{

	float length = GetLengthXY();
	float angle = atan2f(y, x);
	
	angle += deltaRadians;

	float rcos = length * cosf(angle);
	float rsin = length * sinf(angle);

	return Vec4(rcos, rsin, z, w);

}



Vec4 const Vec4::GetRotatedAboutZDegrees(float deltaDegrees) const
{

	float length = GetLengthXY();
	float angle = ATan2Degrees(y, x);
	
	angle += deltaDegrees;

	float rcos = length * CosDegrees(angle);
	float rsin = length * SinDegrees(angle);

	return Vec4(rcos, rsin, z, w);

}



Vec4 const Vec4::GetClamped(float maxLength) const
{
	
	float length = GetLength();
	
	if(length != 0)
	{
		if(length > maxLength)
		{
			float scale = maxLength / length;
			return Vec4(x * scale, y * scale, z * length, w * length);
		}
	}
	
	if(length == 0)
	{
		return Vec4(0.f, 0.f, 0.f, 0.f);
	}
	
	return Vec4(x, y, z, w);

}



Vec4 const Vec4::GetNormalized() const
{

	float length = GetLength();

	float scale = 1 / length;

	if(length > 0)
	{
		return Vec4(x * scale, y * scale, z * scale, w * scale);
	}
	return Vec4(0.f, 0.f, 0.f, 0.f);

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Vec4::SetFromText(char const* textVec4)
{
	Strings vec3Strings = SplitStringOnDelimiter(textVec4, ',');

	x = static_cast<float>(atof(vec3Strings[0].c_str()));
	y = static_cast<float>(atof(vec3Strings[1].c_str()));
	z = static_cast<float>(atof(vec3Strings[2].c_str()));
	w = static_cast<float>(atof(vec3Strings[3].c_str()));
}



Vec4 const Vec4::operator + ( Vec4 const& vecToAdd ) const
{

	return Vec4( x + vecToAdd.x, y + vecToAdd.y, z + vecToAdd.z, w + vecToAdd.w );

}



Vec4 const Vec4::operator-( Vec4 const& vecToSubtract ) const
{

	return Vec4( x - vecToSubtract.x, y - vecToSubtract.y, z - vecToSubtract.z, w - vecToSubtract.w);

}



Vec4 const Vec4::operator-() const
{

	return Vec4( -x, -y, -z, -w );

}



Vec4 const Vec4::operator*( float uniformScale ) const
{

	return Vec4( x * uniformScale,  y * uniformScale, z * uniformScale, w * uniformScale ) ;

}



Vec4 const Vec4::operator*( Vec4 const& vecToMultiply ) const
{

	return Vec4( x * vecToMultiply.x, y * vecToMultiply.y, z * vecToMultiply.z, w * vecToMultiply.w );

}



Vec4 const Vec4::operator/( float inverseScale ) const
{

	return Vec4( x/inverseScale, y/inverseScale, z/inverseScale, w/inverseScale);

}



void Vec4::operator+=( Vec4 const& vecToAdd )
{

	x += vecToAdd.x;
	y += vecToAdd.y;
	z += vecToAdd.z;
	w += vecToAdd.w;

}



void Vec4::operator-=( Vec4 const& vecToSubtract )
{

	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
	z -= vecToSubtract.z;
	w -= vecToSubtract.w;

}



void Vec4::operator*=( const float uniformScale )
{

	x *= uniformScale;
	y *= uniformScale;
	z *= uniformScale;
	w *= uniformScale;

}



void Vec4::operator/=( const float uniformDivisor )
{

	x /= uniformDivisor;
	y /= uniformDivisor;
	z /= uniformDivisor;
	w /= uniformDivisor;

}



void Vec4::operator=( Vec4 const& copyFrom )
{

	x = copyFrom.x;
	y = copyFrom.y;
	z = copyFrom.z;
	w = copyFrom.w;

}



Vec4 const operator*( float uniformScale, Vec4 const& vecToScale )
{

	return Vec4( vecToScale.x * uniformScale, vecToScale.y * uniformScale, vecToScale.z * uniformScale, vecToScale.w * uniformScale);

}



bool Vec4::operator==( Vec4 const& compare ) const
{

	return x == compare.x && y == compare.y && z == compare.z && w == compare.w;

}



bool Vec4::operator!=( Vec4 const& compare ) const
{

	return x != compare.x || y != compare.y || z != compare.z || w != compare.w;

}

