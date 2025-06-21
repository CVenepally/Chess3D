#include "Engine/Math/FloatRange.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------

// Global variables
const FloatRange FloatRange::ZERO = FloatRange();
const FloatRange FloatRange::ONE = FloatRange(1.f, 1.f);
const FloatRange FloatRange::ZERO_TO_ONE = FloatRange(0.f, 1.f);



//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
FloatRange::FloatRange()
	:	m_min(0.f)
	,	m_max(0.f)
{

}



//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
FloatRange::~FloatRange()
{


}



//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
FloatRange::FloatRange(float min, float max)
	:	m_min(min)
	,	m_max(max)
{

}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool FloatRange::IsOnRange(float floatToCheck)
{

	if(floatToCheck < m_min || floatToCheck > m_max)
	{
		return false;
	}

	return true;
}



//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool FloatRange::IsOverlappingWith(FloatRange floatRange)
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
void FloatRange::SetFromText(char const* floatRangeText)
{
	Strings rangeText = SplitStringOnDelimiter(floatRangeText, '~');

	m_min = static_cast<float>(atof(rangeText[0].c_str()));
	m_max = static_cast<float>(atof(rangeText[1].c_str()));
}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
FloatRange FloatRange::GetOverlapRange(FloatRange overlappingFloatRange)
{

	FloatRange overlapRange;

	if(m_min > overlappingFloatRange.m_min)
	{
		overlapRange.m_min = m_min;
	}
	else
	{
		overlapRange.m_min = overlappingFloatRange.m_min;
	}

	if(m_max < overlappingFloatRange.m_max)
	{
		overlapRange.m_max = m_max;
	}
	else
	{
		overlapRange.m_max = overlappingFloatRange.m_max;
	}

	return overlapRange;

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
float FloatRange::GetRandomFloat() const
{

	RandomNumberGenerator rng;

	return rng.RollFloatInRangeOf(m_min, m_max);
}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool FloatRange::operator==(FloatRange const& compare) const
{

	return compare.m_min == m_min && compare.m_max == m_max;

}



//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool FloatRange::operator!=(FloatRange const& compare) const
{

	return compare.m_min != m_min && compare.m_max != m_max;

}



//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void FloatRange::operator=(FloatRange const& copyFrom)
{

	m_min = copyFrom.m_min;
	m_max = copyFrom.m_max;

}