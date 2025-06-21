#include "Engine/Math/IntRange.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------

// Global variables
const IntRange ZERO = IntRange();
const IntRange ONE = IntRange(1, 1);
const IntRange ZERO_TO_ONE = IntRange(0, 1);



//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
IntRange::IntRange()
	: m_min(0)
	, m_max(0)
{

}



//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
IntRange::~IntRange()
{


}



//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
IntRange::IntRange(int min, int max)
	: m_min(min)
	, m_max(max)
{

}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool IntRange::IsOnRange(int floatToCheck)
{

	if(floatToCheck < m_min || floatToCheck > m_max)
	{
		return false;
	}

	return true;
}



//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool IntRange::IsOverlappingWith(IntRange floatRange)
{

	if(floatRange.m_min < m_min && floatRange.m_max < m_min)
	{
		return false;
	}

	if(floatRange.m_min > m_max && floatRange.m_max > m_max)
	{
		return false;
	}

	return true;

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
int IntRange::GetRandomInt() const
{
	RandomNumberGenerator rng;

	return rng.RollIntInRangeOf(m_min, m_max);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
int IntRange::GetRange()
{
	return m_max - m_min;
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void IntRange::SetFromText(char const* intRangeText)
{
	Strings rangeText = SplitStringOnDelimiter(intRangeText, '~');

	m_min = static_cast<int>(atoi(rangeText[0].c_str()));
	m_max = static_cast<int>(atoi(rangeText[1].c_str()));
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool IntRange::operator==(IntRange const& compare) const
{
	return compare.m_min == m_min && compare.m_max == m_max;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool IntRange::operator!=(IntRange const& compare) const
{

	return compare.m_min != m_min && compare.m_max != m_max;

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void IntRange::operator=(IntRange const& copyFrom)
{

	m_min = copyFrom.m_min;
	m_max = copyFrom.m_max;

}