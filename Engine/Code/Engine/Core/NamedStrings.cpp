#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/IntRange.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"

//------------------------------------------------------------------------------------------------------------------
void NamedStrings::PopulateFromXMLElementAttributes(XmlElement const& element)
{

	XmlAttribute const* attribute = element.FirstAttribute();

	while(attribute)
	{
		SetValue(attribute->Name(), attribute->Value());
		attribute = attribute->Next();
	}

}


//------------------------------------------------------------------------------------------------------------------
void NamedStrings::SetValue(std::string const& keyName, std::string const& newValue)
{

	m_keyValuePairs[keyName] = newValue;

}

//------------------------------------------------------------------------------------------------------------------
std::string NamedStrings::GetValue(std::string const& keyName, std::string const& defaultValue) const
{

	auto result = m_keyValuePairs.find(keyName); 

	if(result == m_keyValuePairs.end())
	{
		return defaultValue;
	}

	return result->second;

}

//------------------------------------------------------------------------------------------------------------------
std::string NamedStrings::GetValue(std::string const& keyName, char const* defaultValue) const
{
	
	auto result = m_keyValuePairs.find(keyName);

	if(result == m_keyValuePairs.end())
	{
		return Stringf(defaultValue);
	}

	return result->second;

}


//------------------------------------------------------------------------------------------------------------------
bool NamedStrings::GetValue(std::string const& keyName, bool const& defaultValue) const
{
	
	auto result = m_keyValuePairs.find(keyName);

	if(strcmp(result->second.c_str(), "true") == 0)
	{
		return true;
	}

	if(strcmp(result->second.c_str(), "false") == 0)
	{
		return false;
	}

	return defaultValue;


}


//------------------------------------------------------------------------------------------------------------------
int NamedStrings::GetValue(std::string const& keyName, int const& defaultValue) const
{

	auto result = m_keyValuePairs.find(keyName);

	if(result == m_keyValuePairs.end())
	{
		return defaultValue;
	}
		
	return atoi(result->second.c_str()); 
}


//------------------------------------------------------------------------------------------------------------------
float NamedStrings::GetValue(std::string const& keyName, float const& defaultValue) const
{
	auto result = m_keyValuePairs.find(keyName);

	if(result == m_keyValuePairs.end())
	{
		return defaultValue;
	}

	return static_cast<float>(atof(result->second.c_str()));
}


//------------------------------------------------------------------------------------------------------------------
Vec2 NamedStrings::GetValue(std::string const& keyName, Vec2 const& defaultValue) const
{
	auto result = m_keyValuePairs.find(keyName);

	if(result == m_keyValuePairs.end())
	{
		return defaultValue;
	}

	Vec2 resultVec = Vec2();

	resultVec.SetFromText(result->second.c_str());

	return resultVec;

}


//------------------------------------------------------------------------------------------------------------------
IntVec2 NamedStrings::GetValue(std::string const& keyName, IntVec2 const& defaultValue) const
{
	auto result = m_keyValuePairs.find(keyName);

	if(result == m_keyValuePairs.end())
	{
		return defaultValue;
	}

	IntVec2 resultVec = IntVec2();

	resultVec.SetFromText(result->second.c_str());

	return resultVec;
}


//------------------------------------------------------------------------------------------------------------------
Rgba8 NamedStrings::GetValue(std::string const& keyName, Rgba8 const& defaultValue) const
{
	auto result = m_keyValuePairs.find(keyName);

	if(result == m_keyValuePairs.end())
	{
		return defaultValue;
	}

	Rgba8 resultCol = Rgba8();

	resultCol.SetFromText(result->second.c_str());

	return resultCol;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
IntRange NamedStrings::GetValue(std::string const& keyName, IntRange const& defaultValue) const
{
	auto result = m_keyValuePairs.find(keyName);

	if(result == m_keyValuePairs.end())
	{
		return defaultValue;
	}

	IntRange resultCol = IntRange();

	resultCol.SetFromText(result->second.c_str());

	return resultCol;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
FloatRange NamedStrings::GetValue(std::string const& keyName, FloatRange const& defaultValue) const
{
	auto result = m_keyValuePairs.find(keyName);

	if(result == m_keyValuePairs.end())
	{
		return defaultValue;
	}

	FloatRange resultCol = FloatRange();

	resultCol.SetFromText(result->second.c_str());

	return resultCol;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool NamedStrings::IsEmpty() const
{
	return static_cast<int>(m_keyValuePairs.size()) < 1;
}
