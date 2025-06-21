#pragma once

#include "Engine/Core/EngineCommon.hpp"
#include <map>
#include <string>

//------------------------------------------------------------------------------------------------------------------
struct Vec2;
struct IntVec2;
struct IntRange;
struct FloatRange;
struct Rgba8;

//------------------------------------------------------------------------------------------------------------------
class NamedStrings
{

public:
	
	void			PopulateFromXMLElementAttributes(XmlElement const& element);
	void			SetValue(std::string const& keyName, std::string const& newValue);

	std::string		GetValue(std::string const& keyName, std::string const& defaultValue) const;
	std::string		GetValue(std::string const& keyName, char const* defaultValue) const;
	bool			GetValue(std::string const& keyName, bool const& defaultValue) const;
	int				GetValue(std::string const& keyName, int const& defaultValue) const;
	float			GetValue(std::string const& keyName, float const& defaultValue) const;
	Vec2			GetValue(std::string const& keyName, Vec2 const& defaultValue) const;
	IntVec2			GetValue(std::string const& keyName, IntVec2 const& defaultValue) const;
	Rgba8			GetValue(std::string const& keyName, Rgba8 const& defaultValue) const;
	IntRange		GetValue(std::string const& keyName, IntRange const& defaultValue) const;
	FloatRange		GetValue(std::string const& keyName, FloatRange const& defaultValue) const;

	bool			IsEmpty() const;

private:

	std::map<std::string, std::string> m_keyValuePairs;

};