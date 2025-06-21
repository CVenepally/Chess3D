#pragma comment ( lib, "xinput")

#include <Windows.h>
#include <XInput.h>

#include "Engine/Math/MathUtils.hpp"
#include "Engine/Input/XboxController.hpp"




bool XboxController::IsConntected() const
{
	
	return m_isConnected;

}



bool XboxController::IsButtonDown(XboxButtonID buttonID) const
{

	KeyButtonState button = m_buttons[buttonID];

	return button.m_isPressed;

}



bool XboxController::WasButtonJustPressed(XboxButtonID buttonID) const
{

	KeyButtonState button = m_buttons[buttonID];

	return button.m_isPressed && !button.m_wasPressedLastFrame;

}



bool XboxController::WasButtonJustReleased(XboxButtonID buttonID) const
{

	KeyButtonState button = m_buttons[buttonID];

	return !button.m_isPressed && button.m_wasPressedLastFrame;

}



int XboxController::GetControllerID() const
{

	return m_controllerID;

}



float XboxController::GetLeftTrigger() const
{

	return m_leftTrigger;

}



float XboxController::GetRightTrigger() const
{

	return m_rightTrigger;

}



AnalogJoystick const& XboxController::GetLeftStick() const
{

	return m_leftStick;

}



AnalogJoystick const& XboxController::GetRightStick() const
{

	return m_rightStick;

}



void XboxController::Update()
{

	XINPUT_STATE xboxControllerState;
	memset(&xboxControllerState, 0, sizeof(xboxControllerState));
	DWORD errorStatus = XInputGetState(m_controllerID, &xboxControllerState);

	if(errorStatus != ERROR_SUCCESS)
	{
		Reset();
		m_isConnected = false;
		return;
	}

	m_isConnected = true;

	XINPUT_GAMEPAD const& state = xboxControllerState.Gamepad;
	
	UpdateJoystick(m_leftStick, state.sThumbLX, state.sThumbLY);
	UpdateJoystick(m_rightStick, state.sThumbRX, state.sThumbRY);

	UpdateTrigger(m_leftTrigger, state.bLeftTrigger);
	UpdateTrigger(m_rightTrigger, state.bRightTrigger);
	 
	UpdateButton(XBOX_BUTTON_A,                state.wButtons, XINPUT_GAMEPAD_A);
	UpdateButton(XBOX_BUTTON_B,                state.wButtons, XINPUT_GAMEPAD_B);
	UpdateButton(XBOX_BUTTON_X,                state.wButtons, XINPUT_GAMEPAD_X);
	UpdateButton(XBOX_BUTTON_Y,                state.wButtons, XINPUT_GAMEPAD_Y);
	UpdateButton(XBOX_BUTTON_BACK,             state.wButtons, XINPUT_GAMEPAD_BACK);
	UpdateButton(XBOX_BUTTON_START,            state.wButtons, XINPUT_GAMEPAD_START);
	UpdateButton(XBOX_BUTTON_LEFT_SHOULDER,    state.wButtons, XINPUT_GAMEPAD_LEFT_SHOULDER);
	UpdateButton(XBOX_BUTTON_RIGHT_SHOULDER,   state.wButtons, XINPUT_GAMEPAD_RIGHT_SHOULDER);
	UpdateButton(XBOX_BUTTON_LTHUMB,           state.wButtons, XINPUT_GAMEPAD_LEFT_THUMB);
	UpdateButton(XBOX_BUTTON_RTHUMB,           state.wButtons, XINPUT_GAMEPAD_RIGHT_THUMB);
	UpdateButton(XBOX_BUTTON_DPAD_RIGHT,       state.wButtons, XINPUT_GAMEPAD_DPAD_RIGHT);
	UpdateButton(XBOX_BUTTON_DPAD_LEFT,        state.wButtons, XINPUT_GAMEPAD_DPAD_LEFT);
	UpdateButton(XBOX_BUTTON_DPAD_UP,          state.wButtons, XINPUT_GAMEPAD_DPAD_UP);
	UpdateButton(XBOX_BUTTON_DPAD_DOWN,        state.wButtons, XINPUT_GAMEPAD_DPAD_DOWN);

}



void XboxController::Reset()
{

	for(int buttonIndex = 0; buttonIndex < XBOX_BUTTON_NUM; ++buttonIndex)
	{
		KeyButtonState button = m_buttons[buttonIndex];

		button.m_isPressed = false;
		button.m_wasPressedLastFrame = false;
	}

	m_leftTrigger = 0.f;
	m_rightTrigger = 0.f;
	
	m_leftStick.Reset();
	m_rightStick.Reset();

}



void XboxController::UpdateJoystick(AnalogJoystick& out_joystick, short rawX, short rawY)
{

	float rawNormalizedX = RangeMapClamped(static_cast<float> (rawX), XBOX_JOYSTICK_MIN_AXIS_VALUE, XBOX_JOYSTICK_MAX_AXIS_VALUE, -1.f, 1.f);
	float rawNormalizedY = RangeMapClamped(static_cast<float> (rawY), XBOX_JOYSTICK_MIN_AXIS_VALUE, XBOX_JOYSTICK_MAX_AXIS_VALUE, -1.f, 1.f);

	out_joystick.UpdatePosition(rawNormalizedX, rawNormalizedY);

}



void XboxController::UpdateTrigger(float& out_triggerValue, unsigned char value)
{

	out_triggerValue = RangeMapClamped(static_cast<float>(value), 5.f, 250.f, 0.f, 1.f);

}



void XboxController::UpdateButton(XboxButtonID buttonID, unsigned short buttonFlags, unsigned short buttonFlag)
{

	KeyButtonState& button = m_buttons[buttonID];

	button.m_wasPressedLastFrame = button.m_isPressed;
	button.m_isPressed = (buttonFlags&buttonFlag) == buttonFlag;

}
