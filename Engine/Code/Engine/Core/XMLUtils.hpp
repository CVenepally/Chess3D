#pragma once
#include "Engine/Core/EngineCommon.hpp"

#include <string>
#include <vector>
//------------------------------------------------------------------------------------------------------------------
typedef std::vector<std::string> Strings;

struct Vec2;
struct Vec3;
struct Vec4;
struct FloatRange;
struct IntRange;
struct IntVec2;
struct Rgba8;
struct EulerAngles;


//------------------------------------------------------------------------------------------------------------------

int ParseXmlAttribute(XmlElement const& element, char const* attribureName, int defaultValue);
float ParseXmlAttribute(XmlElement const& element, char const* attribureName, float defaultValue);
char ParseXmlAttribute(XmlElement const& element, char const* attribureName, char defaultValue);
bool ParseXmlAttribute(XmlElement const& element, char const* attribureName, bool defaultValue);
Rgba8 ParseXmlAttribute(XmlElement const& element, char const* attribureName, Rgba8 defaultValue);
Vec2 ParseXmlAttribute(XmlElement const& element, char const* attribureName, Vec2 defaultValue);
Vec3 ParseXmlAttribute(XmlElement const& element, char const* attribureName, Vec3 defaultValue);
Vec4 ParseXmlAttribute(XmlElement const& element, char const* attribureName, Vec4 defaultValue);
FloatRange ParseXmlAttribute(XmlElement const& element, char const* attribureName, FloatRange defaultValue);
IntRange ParseXmlAttribute(XmlElement const& element, char const* attribureName, IntRange defaultValue);
EulerAngles ParseXmlAttribute(XmlElement const& element, char const* attribureName, EulerAngles defaultValue);
IntVec2 ParseXmlAttribute(XmlElement const& element, char const* attribureName, IntVec2 defaultValue);
std::string ParseXmlAttribute(XmlElement const& element, char const* attribureName, std::string defaultValue);
std::string ParseXmlAttribute(XmlElement const& element, char const* attribureName, char const* defaultValue);
Strings ParseXmlAttribute(XmlElement const& element, char const* attribureName, Strings defaultValue, char delimiterSplitOn = ',');
