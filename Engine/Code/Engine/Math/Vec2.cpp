#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/intVec2.hpp"
#include "Engine/Core/StringUtils.hpp"

#include <math.h>
//------------------------------------------------------------------------------------------------------------------
// Global Constants

const Vec2 Vec2::ZERO		= Vec2(0.f, 0.f);
const Vec2 Vec2::ONE		= Vec2(1.f, 1.f);
const Vec2 Vec2::RIGHT		= Vec2(1.f, 0.f);
const Vec2 Vec2::LEFT		= Vec2(-1.f, 0.f);
const Vec2 Vec2::UP			= Vec2(0.f, 1.f);
const Vec2 Vec2::DOWN		= Vec2( 0.f, -1.f );

//------------------------------------------------------------------------------------------------------------------
Vec2::Vec2( Vec2 const& copy )
	: x( copy.x )
	, y( copy.y )
{
}


//------------------------------------------------------------------------------------------------------------------
Vec2::Vec2( float initialX, float initialY )
	: x( initialX )
	, y( initialY )
{
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Vec2::Vec2(float initialXY)
	: x(initialXY)
	, y(initialXY)
{}

//------------------------------------------------------------------------------------------------------------------
Vec2::Vec2(IntVec2 intVec)
{

	x = static_cast<float>(intVec.x);
	y = static_cast<float>(intVec.y);

}


//------------------------------------------------------------------------------------------------------------------
Vec2 const Vec2::MakeFromPolarRadians(float orientationRadians, float length)
{

	float rcos = length * cosf(orientationRadians);
	float rsin = length * sinf(orientationRadians);

	return Vec2(rcos, rsin);

}


//------------------------------------------------------------------------------------------------------------------
Vec2 const Vec2::MakeFromPolarDegrees(float orientationDegrees, float length)
{

	float rcos = length * CosDegrees(orientationDegrees);
	float rsin = length * SinDegrees(orientationDegrees);

	return Vec2(rcos, rsin);

}


//------------------------------------------------------------------------------------------------------------------
float Vec2::GetLength() const
{

	float length = sqrtf((x * x) + (y * y));

	return length;

}


//------------------------------------------------------------------------------------------------------------------
float Vec2::GetLengthSquared() const
{

	float length = (x * x) + (y * y);

	return length;

}


//------------------------------------------------------------------------------------------------------------------
float Vec2::GetOrientationRadians() const
{

	return atan2f(y, x);

}


//------------------------------------------------------------------------------------------------------------------
float Vec2::GetOrientationDegrees() const
{

	return ATan2Degrees(y, x);

}


//------------------------------------------------------------------------------------------------------------------
Vec2 const Vec2::GetRotated90Degrees() const
{

	return Vec2(-y, x);

}


//------------------------------------------------------------------------------------------------------------------
Vec2 const Vec2::GetRotatedMinus90Degrees() const
{

	return Vec2(y, -x);

}


//------------------------------------------------------------------------------------------------------------------
Vec2 const Vec2::GetRotatedRadians(float deltaRadians) const
{

	float length = GetLength();
	float angle = atan2f(y, x);

	angle += deltaRadians;
	
	float rcos = length * cosf(angle);
	float rsin = length * sinf(angle);
	
	return Vec2(rcos, rsin);

}


//------------------------------------------------------------------------------------------------------------------
Vec2 const Vec2::GetRotatedDegrees(float deltaDegrees) const
{

	float length = GetLength();
	float angle = ATan2Degrees(y, x);

	angle += deltaDegrees;
	
	float rcos = length * CosDegrees(angle);
	float rsin = length * SinDegrees(angle);
	
	return Vec2(rcos, rsin);

}


//------------------------------------------------------------------------------------------------------------------
Vec2 const Vec2::GetClamped(float maxLength) const 
{

	float length = GetLength();

	if(length != 0)
	{
		if(length > maxLength)
		{
			float scale = maxLength / length;
			return Vec2(x * scale, y * scale);
		}
	}
	else
	{
		return Vec2(0.f, 0.f);
	}

	return Vec2(x, y);


}


//------------------------------------------------------------------------------------------------------------------
Vec2 const Vec2::GetNormalized() const
{

	float length = GetLength();
	if(length > 0)
	{
		return Vec2(x / length, y / length);
	}

	return Vec2(0.f, 0.f);
	
}


//------------------------------------------------------------------------------------------------------------------
Vec2 const Vec2::GetReflected(Vec2 const& normalOfSurfaceToReflectOff) const
{
	float currentVectorNormalLength = DotProduct2D(Vec2(x, y), normalOfSurfaceToReflectOff);

	Vec2 currentVectorNormal = normalOfSurfaceToReflectOff * currentVectorNormalLength;

	Vec2 vectorParallelToSurface = Vec2(x, y) - currentVectorNormal;

	Vec2 reflectedVector = vectorParallelToSurface - currentVectorNormal;

	return reflectedVector;

}

//------------------------------------------------------------------------------------------------------------------
void Vec2::SetFromText(char const* vec2Text)
{

	Strings vec2Strings = SplitStringOnDelimiter(vec2Text, ',');

	x = static_cast<float>(atof(vec2Strings[0].c_str())); 
	y = static_cast<float>(atof(vec2Strings[1].c_str()));	
}

//------------------------------------------------------------------------------------------------------------------
void Vec2::SetOrientationRadians(float newOrientationRadians) // changes the angle of the current vector to the new angle(radians)
{

	float r = GetLength();

	x = r * cosf(newOrientationRadians);
	y = r * sinf(newOrientationRadians);

}


//------------------------------------------------------------------------------------------------------------------
void Vec2::SetOrientationDegrees(float newOrientationDegrees) // changes the angle of the current vector to the new one (degrees)
{
	
	float angle = ATan2Degrees(y, x);
	angle = newOrientationDegrees;

	float r = GetLength();
	x = r * CosDegrees(newOrientationDegrees);
	y = r * SinDegrees(newOrientationDegrees);

}


//------------------------------------------------------------------------------------------------------------------
void Vec2::SetPolarRadians(float newOrientationRadians, float newLength)
{
	
	x = newLength * cosf(newOrientationRadians);
	y = newLength * sinf(newOrientationRadians);

}


//------------------------------------------------------------------------------------------------------------------
void Vec2::SetPolarDegrees(float newOrientationDegrees, float newLength)
{

	x = newLength * CosDegrees(newOrientationDegrees);
	y = newLength * SinDegrees(newOrientationDegrees);

}


//------------------------------------------------------------------------------------------------------------------
void Vec2::Rotate90Degrees()
{
	
	float tempX = x;
	x = -y;
	y = tempX;

}


//------------------------------------------------------------------------------------------------------------------
void Vec2::RotateMinus90Degrees()
{
	
	float tempY = y;
	y = -x;
	x = tempY;

}


//------------------------------------------------------------------------------------------------------------------
void Vec2::RotateRadians(float deltaRadians)
{
	
	float length = GetLength();
	float angle = atan2f(y, x);
	
	angle += deltaRadians;

	x = length * cosf(angle);
	y = length * sinf(angle);

}


//------------------------------------------------------------------------------------------------------------------
void Vec2::RotateDegrees(float deltaDegree)
{

	float length = GetLength();
	float angle = ATan2Degrees(y, x);
	
	angle += deltaDegree;

	x = length * CosDegrees(angle);
	y = length * SinDegrees(angle);

}


//------------------------------------------------------------------------------------------------------------------
void Vec2::SetLength(float newLength)
{

	float oldLength = GetLength();

	float scaleDownValue = 1.f / oldLength;

	if(oldLength > 0)
	{
		x *= scaleDownValue;
		y *= scaleDownValue;

		x *= newLength;
		y *= newLength;
	}

	if(oldLength == 0)
	{

		x += newLength;
		y += newLength;
	
	}

}


//------------------------------------------------------------------------------------------------------------------
void Vec2::ClampLength(float maxLength)
{

	float length = GetLength();
	
	if(length != 0)
	{
		if(length > maxLength)
		{
			float scale = maxLength / length;

			x *= scale;
			y *= scale;
		}
	}

	if(length == 0)
	{
		x = 0.f;
		y = 0.f;
	}

}

//------------------------------------------------------------------------------------------------------------------
void Vec2::Normalize()
{

	float length = GetLength();

	if(length > 0)
	{
		x /= length;
		y /= length;
	}

	if(length == 0)
	{
		x = 0.f;
		y = 0.f;
	}

}

//------------------------------------------------------------------------------------------------------------------
float Vec2::NormalizeAndGetPreviousLength()
{

	float length = GetLength();

	if(length > 0)
	{
		x /= length;
		y /= length;
	}

	if(length == 0)
	{
		x = 0.f;
		y = 0.f;
	
	}

	return length;

}


//------------------------------------------------------------------------------------------------------------------
void Vec2::Reflect(Vec2 const& normalOfSurfaceToReflectOff)
{

	float currentVectorNormalLength = DotProduct2D(Vec2(x, y), normalOfSurfaceToReflectOff);

	Vec2 currentVectorNormal = normalOfSurfaceToReflectOff * currentVectorNormalLength;

	Vec2 vectorParallelToSurface = Vec2(x, y) - currentVectorNormal;

	Vec2 reflectedVector = vectorParallelToSurface - currentVectorNormal;

	x = reflectedVector.x;
	y = reflectedVector.y;

}


//------------------------------------------------------------------------------------------------------------------
Vec2 const Vec2::operator + ( Vec2 const& vecToAdd ) const
{

	return Vec2( x + vecToAdd.x, y + vecToAdd.y );

}


//------------------------------------------------------------------------------------------------------------------
Vec2 const Vec2::operator + (int const& numToAdd) const
{

	return Vec2(x + numToAdd, y + numToAdd);

}


//------------------------------------------------------------------------------------------------------------------
Vec2 const Vec2::operator-( Vec2 const& vecToSubtract ) const
{

	return Vec2( x - vecToSubtract.x, y - vecToSubtract.y );

}


//------------------------------------------------------------------------------------------------------------------
Vec2 const Vec2::operator-() const
{

	return Vec2( -x, -y );

}


//------------------------------------------------------------------------------------------------------------------
Vec2 const Vec2::operator*( float uniformScale ) const
{

	return Vec2( x * uniformScale,  y * uniformScale ) ;

}


//------------------------------------------------------------------------------------------------------------------
Vec2 const Vec2::operator*( Vec2 const& vecToMultiply ) const
{

	return Vec2( x * vecToMultiply.x, y * vecToMultiply.y );

}


//------------------------------------------------------------------------------------------------------------------
Vec2 const Vec2::operator/( float inverseScale ) const
{

	return Vec2( x/inverseScale, y/inverseScale );

}


//------------------------------------------------------------------------------------------------------------------
void Vec2::operator+=( Vec2 const& vecToAdd )
{

	x += vecToAdd.x;
	y += vecToAdd.y;

}


//------------------------------------------------------------------------------------------------------------------
void Vec2::operator-=( Vec2 const& vecToSubtract )
{

	x -= vecToSubtract.x;
	y -= vecToSubtract.y;

}


//------------------------------------------------------------------------------------------------------------------
void Vec2::operator*=( const float uniformScale )
{

	x *= uniformScale;
	y *= uniformScale;

}


//------------------------------------------------------------------------------------------------------------------
void Vec2::operator/=( const float uniformDivisor )
{

	x /= uniformDivisor;
	y /= uniformDivisor;

}


//------------------------------------------------------------------------------------------------------------------
void Vec2::operator=( Vec2 const& copyFrom )
{

	x = copyFrom.x;
	y = copyFrom.y;

}


//------------------------------------------------------------------------------------------------------------------
Vec2 const Vec2::GetAbsoluteVec() const
{
	return Vec2(fabsf(x), fabsf(y));
}



//------------------------------------------------------------------------------------------------------------------
Vec2 const operator*( float uniformScale, Vec2 const& vecToScale )
{

	return Vec2( vecToScale.x * uniformScale, vecToScale.y * uniformScale );

}


//------------------------------------------------------------------------------------------------------------------
bool Vec2::operator==( Vec2 const& compare ) const
{

	return x == compare.x && y == compare.y;

}


//------------------------------------------------------------------------------------------------------------------
bool Vec2::operator!=( Vec2 const& compare ) const
{

	return x != compare.x || y != compare.y;

}

