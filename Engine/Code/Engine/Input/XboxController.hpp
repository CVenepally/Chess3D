#pragma once
#include "Engine/Input/AnalogJoystick.hpp"
#include "Engine/Input/KeyButtonState.hpp"


enum XboxButtonID
{

	XBOX_BUTTON_INVALID = -1,

	XBOX_BUTTON_DPAD_UP,
	XBOX_BUTTON_DPAD_DOWN,
	XBOX_BUTTON_DPAD_LEFT,
	XBOX_BUTTON_DPAD_RIGHT,
	XBOX_BUTTON_START,
	XBOX_BUTTON_BACK,
	XBOX_BUTTON_LTHUMB,
	XBOX_BUTTON_RTHUMB,
	XBOX_BUTTON_LEFT_SHOULDER,
	XBOX_BUTTON_RIGHT_SHOULDER,
	XBOX_BUTTON_A,
	XBOX_BUTTON_B,
	XBOX_BUTTON_X,
	XBOX_BUTTON_Y,

	XBOX_BUTTON_NUM

};


constexpr short XBOX_JOYSTICK_MIN_AXIS_VALUE = -32768;
constexpr short XBOX_JOYSTICK_MAX_AXIS_VALUE = 32767;



class XboxController
{

	friend class InputSystem;


public:

	bool        IsConntected() const;
	bool        IsButtonDown(XboxButtonID buttonID) const;
	bool        WasButtonJustPressed(XboxButtonID buttonID) const;
	bool        WasButtonJustReleased(XboxButtonID buttonID) const;

	int         GetControllerID() const;

	float       GetLeftTrigger() const;
	float       GetRightTrigger() const;
	
	AnalogJoystick const&      GetLeftStick() const;
	AnalogJoystick const&      GetRightStick() const;


private:

	void        Update();
	void        Reset();
	void        UpdateJoystick(AnalogJoystick& out_joystick, short rawX, short rawY);
	void        UpdateTrigger(float& out_triggerValue, unsigned char value);
	void        UpdateButton(XboxButtonID buttonID, unsigned short buttonFlags, unsigned short buttonFlag);


private:

	bool              m_isConnected        = false;
	
	int               m_controllerID       = -1;
	
	float             m_leftTrigger        = 0.f;
	float             m_rightTrigger       = 0.f;

	KeyButtonState    m_buttons[XBOX_BUTTON_NUM];

	AnalogJoystick    m_leftStick;
	AnalogJoystick    m_rightStick;

};