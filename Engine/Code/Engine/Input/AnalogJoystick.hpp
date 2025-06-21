#pragma once
#include "Engine/Math/Vec2.hpp"





class AnalogJoystick
{
	
public:

	AnalogJoystick();
	~AnalogJoystick();

	void     Reset();
	void     SetDeadZoneThresholds(float normalizedInnerDeadZoneThreshold, float normalizedOuterDeadZoneThreshold);
	void     UpdatePosition(float rawNormalizedX, float rawNormalizedY);

	float    GetMagnitude() const;
	float    GetOrientationDegrees() const;
	float    GetInnerDeadZoneFraction() const;
	float    GetOuterDeadZoneFraction() const;
		     
	Vec2     GetPosition() const;
	Vec2     GetRawUncorrectedPosition() const;

protected:

	float    m_innerDeadZoneFraction = 0.f;
	float    m_outerDeadZoneFraction = 1.0f;

	Vec2     m_correctedPosition;
	Vec2     m_rawPosition;

};