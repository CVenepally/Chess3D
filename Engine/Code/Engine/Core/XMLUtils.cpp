#include "Engine/Core/XMLUtils.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/IntRange.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/StringUtils.hpp"


//------------------------------------------------------------------------------------------------------------------
int ParseXmlAttribute(XmlElement const& element, char const* attribureName, int defaultValue)
{
	char const* value = element.Attribute(attribureName);

	if(!value)
	{
		return defaultValue;
	}

	return atoi(value);

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
float ParseXmlAttribute(XmlElement const& element, char const* attribureName, float defaultValue)
{
	char const* value = element.Attribute(attribureName);

	if(!value)
	{
		return defaultValue;
	}

	return static_cast<float>(atof(value));
}


//------------------------------------------------------------------------------------------------------------------
char ParseXmlAttribute(XmlElement const& element, char const* attribureName, char defaultValue)
{
	char const* value = element.Attribute(attribureName);

	if(!value)
	{
		return defaultValue;
	}

	return *value;
}



//------------------------------------------------------------------------------------------------------------------
bool ParseXmlAttribute(XmlElement const& element, char const* attribureName, bool defaultValue)
{

	char const* value = element.Attribute(attribureName);

	if(!value)
	{
		return defaultValue;
	}
	
	if(strcmp(value, "true") == 0)
	{
		return true;
	}

	if(strcmp(value, "false") == 0)
	{
		return false;
	}

	return defaultValue;

}


//------------------------------------------------------------------------------------------------------------------
Rgba8 ParseXmlAttribute(XmlElement const& element, char const* attribureName, Rgba8 defaultValue)
{
	char const* value = element.Attribute(attribureName);

	if(!value)
	{
		return defaultValue;
	}

	Rgba8 result = Rgba8();
	
	result.SetFromText(value);

	return result;
}


//------------------------------------------------------------------------------------------------------------------
Vec2 ParseXmlAttribute(XmlElement const& element, char const* attribureName, Vec2 defaultValue)
{
	char const* value = element.Attribute(attribureName);

	if(!value)
	{
		return defaultValue;
	}

	Vec2 result = Vec2();
	result.SetFromText(value);

	return result;
}


//------------------------------------------------------------------------------------------------------------------
Vec3 ParseXmlAttribute(XmlElement const& element, char const* attribureName, Vec3 defaultValue)
{
	char const* value = element.Attribute(attribureName);

	if(!value)
	{
		return defaultValue;
	}

	Vec3 result = Vec3();

	result.SetFromText(value);

	return result;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Vec4 ParseXmlAttribute(XmlElement const& element, char const* attribureName, Vec4 defaultValue)
{
	char const* value = element.Attribute(attribureName);

	if(!value)
	{
		return defaultValue;
	}

	Vec4 result = Vec4();

	result.SetFromText(value);

	return result;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
FloatRange ParseXmlAttribute(XmlElement const& element, char const* attribureName, FloatRange defaultValue)
{
	char const* value = element.Attribute(attribureName);

	if(!value)
	{
		return defaultValue;
	}

	FloatRange result = FloatRange();

	result.SetFromText(value);

	return result;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
IntRange ParseXmlAttribute(XmlElement const& element, char const* attribureName, IntRange defaultValue)
{
	char const* value = element.Attribute(attribureName);

	if(!value)
	{
		return defaultValue;
	}

	IntRange result = IntRange();

	result.SetFromText(value);

	return result;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
EulerAngles ParseXmlAttribute(XmlElement const& element, char const* attribureName, EulerAngles defaultValue)
{
	char const* value = element.Attribute(attribureName);

	if(!value)
	{
		return defaultValue;
	}

	EulerAngles result = EulerAngles();

	result.SetFromText(value);

	return result;
}


//------------------------------------------------------------------------------------------------------------------
IntVec2 ParseXmlAttribute(XmlElement const& element, char const* attribureName, IntVec2 defaultValue)
{
	char const* value = element.Attribute(attribureName);

	if(!value)
	{
		return defaultValue;
	}

	IntVec2 result = IntVec2();

	result.SetFromText(value);

	return result;
}


//------------------------------------------------------------------------------------------------------------------
std::string ParseXmlAttribute(XmlElement const& element, char const* attribureName, std::string defaultValue)
{
	char const* value = element.Attribute(attribureName);

	if(!value)
	{
		return defaultValue;
	}

	std::string result = Stringf(value);
	return result;

}


//------------------------------------------------------------------------------------------------------------------
std::string ParseXmlAttribute(XmlElement const& element, char const* attribureName, char const* defaultValue)
{
	char const* value = element.Attribute(attribureName);

	if(!value)
	{
		return Stringf(defaultValue);
	}

	std::string result = Stringf(value);
	return result;
}


//------------------------------------------------------------------------------------------------------------------
Strings ParseXmlAttribute(XmlElement const& element, char const* attribureName, Strings defaultValue, char delimiterSplitOn)
{

	char const* value = element.Attribute(attribureName);

	if(!value)
	{
		return defaultValue;
	}

	Strings result = SplitStringOnDelimiter(value, delimiterSplitOn);

	return result;

}
