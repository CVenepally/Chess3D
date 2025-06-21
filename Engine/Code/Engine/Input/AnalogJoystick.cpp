#include "Engine/Input/AnalogJoystick.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/EngineCommon.hpp"



AnalogJoystick::AnalogJoystick()
{
	
	float innerDeadzone = g_gameConfigBlackboard.GetValue("innerDeadzone", 0.4f);
	float outerDeadzone = g_gameConfigBlackboard.GetValue("outerDeadzone", 0.9f);
	SetDeadZoneThresholds(innerDeadzone, outerDeadzone);

}

AnalogJoystick::~AnalogJoystick()
{}

void AnalogJoystick::Reset()
{
	m_correctedPosition = Vec2(0.f, 0.f);
	m_rawPosition = Vec2(0.f, 0.f);
}




void AnalogJoystick::SetDeadZoneThresholds(float normalizedInnerDeadZoneThreshold, float normalizedOuterDeadZoneThreshold)
{
	
	m_innerDeadZoneFraction = normalizedInnerDeadZoneThreshold;
	m_outerDeadZoneFraction = normalizedOuterDeadZoneThreshold;

}




void AnalogJoystick::UpdatePosition(float rawNormalizedX, float rawNormalizedY)
{

	m_rawPosition = Vec2(rawNormalizedX, rawNormalizedY);
	float thetaDegrees = m_rawPosition.GetOrientationDegrees();
	float rawMagnitude = m_rawPosition.GetLength();
	float correctedMagnitude = RangeMapClamped(rawMagnitude, m_innerDeadZoneFraction, m_outerDeadZoneFraction, 0.f, 1.f);
	m_correctedPosition.SetPolarDegrees(thetaDegrees, correctedMagnitude);

}




float AnalogJoystick::GetMagnitude() const
{

	return m_correctedPosition.GetLength();

}




float AnalogJoystick::GetOrientationDegrees() const
{

	return m_correctedPosition.GetOrientationDegrees();

}




float AnalogJoystick::GetInnerDeadZoneFraction() const
{

	return m_innerDeadZoneFraction;

}




float AnalogJoystick::GetOuterDeadZoneFraction() const
{

	return m_outerDeadZoneFraction;

}




Vec2 AnalogJoystick::GetPosition() const
{

	return m_correctedPosition;

}




Vec2 AnalogJoystick::GetRawUncorrectedPosition() const
{

	return m_rawPosition;

}
