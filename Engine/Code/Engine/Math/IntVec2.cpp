#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/StringUtils.hpp"

#include <math.h>


const IntVec2 IntVec2::ZERO   =	IntVec2(0, 0);
const IntVec2 IntVec2::ONE    =	IntVec2(1, 1);
const IntVec2 IntVec2::RIGHT  =	IntVec2(1, 0);
const IntVec2 IntVec2::LEFT   =	IntVec2(-1, 0);
const IntVec2 IntVec2::UP     =	IntVec2(0, 1);
const IntVec2 IntVec2::DOWN   =	IntVec2(0, -1);



IntVec2::IntVec2(const IntVec2& copyFrom)
	: x(copyFrom.x)
	, y(copyFrom.y)
{}



IntVec2::IntVec2(int initialX, int initialY)
	: x(initialX)
	, y(initialY)
{}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
IntVec2::IntVec2(int initialXY)
	:x(initialXY)
	,y(initialXY)
{}


float IntVec2::GetLength() const
{

	float x_float = static_cast<float>(x);
	float y_float = static_cast<float>(y);

	float length = static_cast<float>(sqrt((x_float * x_float) + (y_float * y_float)));

	return length;

}



int IntVec2::GetTaxicabLength() const
{

	return abs(x) + abs(y);

}



int IntVec2::GetLengthSquared() const
{

	int length = (x * x) + (y * y);
	return length;

}



float IntVec2::GetOrientationDegrees() const
{

	return ATan2Degrees(static_cast<float>(y), static_cast<float>(x));

}



float IntVec2::GetOrientationRadians() const
{

	return atan2f(static_cast<float>(y), static_cast<float>(x));

}



IntVec2 const IntVec2::GetRotated90Degrees() const
{

	return IntVec2(-y, x);

}



IntVec2 const IntVec2::GetRotatedMinus90Degrees() const
{

	return IntVec2(y, -x);

}

//------------------------------------------------------------------------------------------------------------------
void IntVec2::SetFromText(char const* textIntVec2)
{

	Strings intVec2Strings = SplitStringOnDelimiter(textIntVec2, ',');

	x = static_cast<int>(atoi(intVec2Strings[0].c_str()));	
	y = static_cast<int>(atoi(intVec2Strings[1].c_str()));	

}


//------------------------------------------------------------------------------------------------------------------
void IntVec2::Rotate90Degrees()
{

	int tempX = x;
	x = -y;
	y = tempX;

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Vec2 const IntVec2::GetAsVec2()
{
	return Vec2(static_cast<float>(x), static_cast<float>(y));
}

void IntVec2::RotateMinus90Degrees()
{

	int tempY = y;
	y = -x;
	x = tempY;

}

IntVec2 const IntVec2::operator+(IntVec2 const& vecToAdd) const
{

	return IntVec2(x + vecToAdd.x, y + vecToAdd.y);

}


IntVec2 const IntVec2::operator-(IntVec2 const& vecToSubtract) const
{

	return IntVec2(x - vecToSubtract.x, y - vecToSubtract.y);

}


bool IntVec2::operator==(IntVec2 const& vecToCompare) const
{

	return x == vecToCompare.x && y == vecToCompare.y;

}

bool IntVec2::operator!=(IntVec2 const& vecToCompare) const
{

	return x != vecToCompare.x || y != vecToCompare.y;

}

void IntVec2::operator=(IntVec2 const& copyFrom)
{

	x = copyFrom.x;
	y = copyFrom.y;

}
