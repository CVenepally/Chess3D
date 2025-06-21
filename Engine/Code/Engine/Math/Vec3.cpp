#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/StringUtils.hpp"

#include <math.h>


const Vec3 Vec3::ZERO =		Vec3(0.f, 0.f, 0.f);
const Vec3 Vec3::ONE =		Vec3(1.f, 1.f, 1.f);
const Vec3 Vec3::RIGHT =	Vec3(0.f, -1.f, 0.f);
const Vec3 Vec3::LEFT =		Vec3(0.f, 1.f, 0.f);
const Vec3 Vec3::UP =		Vec3(0.f, 0.f, 1.f);
const Vec3 Vec3::DOWN =		Vec3(0.f, 0.f, -1.f);
const Vec3 Vec3::FORWARD =	Vec3(1.f, 0.f, 0.f);
const Vec3 Vec3::BACKWARD =	Vec3(-1.f, 0.f, 0.f);


Vec3::Vec3( Vec3 const& copy )
	: x( copy.x )
	, y( copy.y )
	, z( copy.z )
{
}



Vec3::Vec3( float initialX, float initialY, float initialZ )
	: x( initialX )
	, y( initialY )
	, z( initialZ )
{
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Vec3::Vec3(int initialX, int initialY, int initialZ)
	: x(static_cast<float>(initialX))
	, y(static_cast<float>(initialY))
	, z(static_cast<float>(initialZ))
{

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Vec3::Vec3(Vec2 vec2, float z)
	: x(vec2.x)
	, y(vec2.y)
	, z(z)
{

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Vec3::Vec3(IntVec2 intVec2, int z)
	: x(static_cast<float>(intVec2.x))
	, y(static_cast<float>(intVec2.y))
	, z(static_cast<float>(z))
{}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Vec3 const Vec3::MakeFromPolarRadians(float pitchRadians, float yawRadians, float length)
{
	float x = length * cosf(pitchRadians) * cosf(yawRadians);
	float y = length * cosf(pitchRadians) * sinf(yawRadians);
	float z = -length * sinf(pitchRadians);

	return Vec3(x, y, z);

}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Vec3 const Vec3::MakeFromPolarDegrees(float pitchDegrees, float yawDegrees, float length)
{
	float x = length * CosDegrees(pitchDegrees) * CosDegrees(yawDegrees);
	float y = length * CosDegrees(pitchDegrees) * SinDegrees(yawDegrees);
	float z = length * SinDegrees(pitchDegrees);

	return Vec3(x, y, z);

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
float Vec3::GetLength() const
{

	float length = sqrtf((x * x) + (y * y) + (z * z));

	return length;

}



float Vec3::GetLengthXY() const
{
	
	float length = sqrtf((x * x) + (y * y));

	return length;

}



float Vec3::GetLengthSquared() const
{
	
	float length = (x * x) + (y * y) + (z * z);
	
	return length;

}



float Vec3::GetLengthXYSquared() const
{

	float length = (x * x) + (y * y);
	
	return length;

}



float Vec3::GetAngleAboutZRadians() const
{

	return atan2f(y, x);

}



float Vec3::GetAngleAboutZDegrees() const
{

	return ATan2Degrees(y, x);

}



Vec3 const Vec3::GetRotatedAboutZRadians(float deltaRadians) const
{

	float length = GetLengthXY();
	float angle = atan2f(y, x);
	
	angle += deltaRadians;

	float rcos = length * cosf(angle);
	float rsin = length * sinf(angle);

	return Vec3(rcos, rsin, z);

}



Vec3 const Vec3::GetRotatedAboutZDegrees(float deltaDegrees) const
{

	float length = GetLengthXY();
	float angle = ATan2Degrees(y, x);
	
	angle += deltaDegrees;

	float rcos = length * CosDegrees(angle);
	float rsin = length * SinDegrees(angle);

	return Vec3(rcos, rsin, z);

}



Vec3 const Vec3::GetClamped(float maxLength) const
{
	
	float length = GetLength();
	
	if(length != 0)
	{
		if(length > maxLength)
		{
			float scale = maxLength / length;
			return Vec3(x * scale, y * scale, z * length);
		}
	}
	
	if(length == 0)
	{
		return Vec3(0.f, 0.f, 0.f);
	}
	
	return Vec3(x, y, z);

}



Vec3 const Vec3::GetNormalized() const
{

	float length = GetLength();
	
	float scale = 1 / length;

	if(length > 0)
	{
		return Vec3(x * scale, y * scale, z * scale);
	}

	return Vec3(0.f, 0.f, 0.f);

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Vec3 Vec3::GetXY() const
{
	return Vec3(x, y, 0.f);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Vec2 Vec3::GetXY2D() const
{
	return Vec2(x, y);
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Vec3::SetLength(float newLength)
{

	float oldLength = GetLength();

	float scaleDownValue = 1.f / oldLength;

	if(oldLength > 0)
	{
		x *= scaleDownValue;
		y *= scaleDownValue;
		z *= scaleDownValue;

		x *= newLength;
		y *= newLength;
		z *= newLength;
	}

	if(oldLength == 0)
	{

		x += newLength;
		y += newLength;
		z += newLength;

	}

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Vec3::Normalize()
{

	float length = GetLength();

	float scale = 1 / length;

	if(length > 0)
	{
		x *= scale;
		y *= scale;
		z *= scale;
	}
	else
	{
		x = 0.f;
		y = 0.f;
		z = 0.f;
	}
}


//------------------------------------------------------------------------------------------------------------------
void Vec3::SetFromText(char const* textVec3)
{

	Strings vec3Strings = SplitStringOnDelimiter(textVec3, ',');

	x = static_cast<float>(atof(vec3Strings[0].c_str()));
	y = static_cast<float>(atof(vec3Strings[1].c_str()));
	z = static_cast<float>(atof(vec3Strings[2].c_str()));

}


//------------------------------------------------------------------------------------------------------------------
Vec3 const Vec3::operator + ( Vec3 const& vecToAdd ) const
{

	return Vec3( x + vecToAdd.x, y + vecToAdd.y, z + vecToAdd.z );

}



Vec3 const Vec3::operator-( Vec3 const& vecToSubtract ) const
{

	return Vec3( x - vecToSubtract.x, y - vecToSubtract.y, z - vecToSubtract.z );

}



Vec3 const Vec3::operator-() const
{

	return Vec3( -x, -y, -z );

}



Vec3 const Vec3::operator*( float uniformScale ) const
{

	return Vec3( x * uniformScale,  y * uniformScale, z * uniformScale ) ;

}



Vec3 const Vec3::operator*( Vec3 const& vecToMultiply ) const
{

	return Vec3( x * vecToMultiply.x, y * vecToMultiply.y, z * vecToMultiply.z );

}



Vec3 const Vec3::operator/( float inverseScale ) const
{

	return Vec3( x/inverseScale, y/inverseScale, z/inverseScale );

}



void Vec3::operator+=( Vec3 const& vecToAdd )
{

	x += vecToAdd.x;
	y += vecToAdd.y;
	z += vecToAdd.z;

}



void Vec3::operator-=( Vec3 const& vecToSubtract )
{

	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
	z -= vecToSubtract.z;

}



void Vec3::operator*=( const float uniformScale )
{

	x *= uniformScale;
	y *= uniformScale;
	z *= uniformScale;

}



void Vec3::operator/=( const float uniformDivisor )
{

	x /= uniformDivisor;
	y /= uniformDivisor;
	z /= uniformDivisor;

}



void Vec3::operator=( Vec3 const& copyFrom )
{

	x = copyFrom.x;
	y = copyFrom.y;
	z = copyFrom.z;

}



Vec3 const operator*( float uniformScale, Vec3 const& vecToScale )
{

	return Vec3( vecToScale.x * uniformScale, vecToScale.y * uniformScale, vecToScale.z * uniformScale);

}



bool Vec3::operator==( Vec3 const& compare ) const
{

	return x == compare.x && y == compare.y && z == compare.z;

}



bool Vec3::operator!=( Vec3 const& compare ) const
{

	return x != compare.x || y != compare.y || z != compare.z;

}

