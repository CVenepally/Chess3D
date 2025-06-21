#include <Windows.h>
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Window/Window.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/NamedStrings.hpp"

InputSystem* g_inputSystem = nullptr;


uchar const KEYCODE_F1               =VK_F1;
uchar const KEYCODE_F2               =VK_F2;
uchar const KEYCODE_F3               =VK_F3;
uchar const KEYCODE_F4               =VK_F4;
uchar const KEYCODE_F5               =VK_F5;
uchar const KEYCODE_F6               =VK_F6;
uchar const KEYCODE_F7               =VK_F7;
uchar const KEYCODE_F8               =VK_F8;
uchar const KEYCODE_F9               =VK_F9;
uchar const KEYCODE_F10              =VK_F10;
uchar const KEYCODE_F11              =VK_F11;
uchar const KEYCODE_F12              =VK_F12;
uchar const KEYCODE_ESC              =VK_ESCAPE;
uchar const KEYCODE_SPACE            =VK_SPACE;
uchar const KEYCODE_ENTER            =VK_RETURN;
uchar const KEYCODE_LSHIFT           =VK_SHIFT;
uchar const KEYCODE_UP_ARROW         =VK_UP;
uchar const KEYCODE_DOWN_ARROW       =VK_DOWN;
uchar const KEYCODE_LEFT_ARROW       =VK_LEFT;
uchar const KEYCODE_RIGHT_ARROW      =VK_RIGHT;
uchar const KEYCODE_LEFT_MOUSE       =VK_LBUTTON;
uchar const KEYCODE_RIGHT_MOUSE		 =VK_RBUTTON;
uchar const KEYCODE_BACKSPACE		 =VK_BACK;
uchar const KEYCODE_DELETE			 =VK_DELETE;
uchar const KEYCODE_INSERT			 =VK_INSERT;
uchar const KEYCODE_HOME			 =VK_HOME;
uchar const KEYCODE_END				 =VK_END;
uchar const KEYCODE_TILDE		     =0xC0;  // ` and ~
uchar const KEYCODE_LEFT_BRACKET     =0xDB;
uchar const KEYCODE_RIGHT_BRACKET	 =0xDD;


InputSystem::InputSystem(InputConfig const& config)
	:m_config(config)
{}




InputSystem::~InputSystem()
{}




void InputSystem::Startup()
{
	for(int controllerIndex = 0; controllerIndex < NUM_XBOX_CONTROLLERS; ++controllerIndex)
	{
		m_controllers[controllerIndex].m_controllerID = controllerIndex;
	}

	g_eventSystem->SubscribeEventCallbackFunction("KeyPressed", InputSystem::OnKeyPressedEvent);
	g_eventSystem->SubscribeEventCallbackFunction("KeyReleased", InputSystem::OnKeyReleasedEvent);

}


void InputSystem::Shutdown()
{
	g_eventSystem->UnsubscribeEventCallbackFunction("KeyPressed", InputSystem::OnKeyPressedEvent);
	g_eventSystem->UnsubscribeEventCallbackFunction("KeyReleased", InputSystem::OnKeyReleasedEvent);
}




void InputSystem::BeginFrame()
{

	for(int controllerIndex = 0; controllerIndex < NUM_XBOX_CONTROLLERS; ++controllerIndex)
	{
		m_controllers[controllerIndex].Update();
	}

	POINT windowsCursorClientPosition;
	BOOL cursorResult = GetCursorPos(&windowsCursorClientPosition); // do smth if it fails

	if(!cursorResult)
	{
		ERROR_AND_DIE("Unable to get cursor position InputSystem::BeginFrame");
	}

	m_cursorState.m_cursorClientDelta = IntVec2::ZERO;

	if(m_cursorState.m_cursorMode == CursorMode::FPS)
	{
		IntVec2 lastFrameCursorClientPosition = GetCursorClientPosition();
		m_cursorState.m_cursorClientDelta.x = windowsCursorClientPosition.x - lastFrameCursorClientPosition.x;
		m_cursorState.m_cursorClientDelta.y = windowsCursorClientPosition.y - lastFrameCursorClientPosition.y;
		
		HWND clientHWND = static_cast<HWND>(g_theWindow->GetHWND());

		POINT clientWindowCenterInScreenSpace;
		clientWindowCenterInScreenSpace.x = static_cast<LONG>(g_theWindow->GetClientDimensions().x * 0.5f);
		clientWindowCenterInScreenSpace.y = static_cast<LONG>(g_theWindow->GetClientDimensions().y * 0.5f);

		BOOL result = ClientToScreen(clientHWND, &clientWindowCenterInScreenSpace);
		if(!result)
		{
			ERROR_AND_DIE("Unable to covert client center to screen space InputSystem::BeginFrame");
		}

		result = SetCursorPos(clientWindowCenterInScreenSpace.x, clientWindowCenterInScreenSpace.y);
		if(!result)
		{
			ERROR_AND_DIE("Unable to set cursor pos to center of the client InputSystem::BeginFrame");
		}

		result = GetCursorPos(&windowsCursorClientPosition); // do smth if it fails

		if(!result)
		{
			ERROR_AND_DIE("Unable to get cursor position InputSystem::BeginFrame");
		}

		m_cursorState.m_cursorClientPosition = IntVec2(windowsCursorClientPosition.x, windowsCursorClientPosition.y);
	}

	CURSORINFO pci;
	pci.cbSize = sizeof(CURSORINFO);

	BOOL result;
	result = GetCursorInfo(&pci);

	if(pci.flags & CURSOR_SHOWING)
	{
		if(m_cursorState.m_cursorMode == CursorMode::FPS)
		{
			while(ShowCursor(FALSE) >= 0);
		}
	}
	else
	{
		if(m_cursorState.m_cursorMode == CursorMode::POINTER)
		{
			while(ShowCursor(TRUE) < 0);
		}
	}

	m_cursorState.m_cursorClientPosition = IntVec2(windowsCursorClientPosition.x, windowsCursorClientPosition.y);

	SetWheelDelta(0.f);

}

void InputSystem::EndFrame()
{

	for(int keyIndex = 0; keyIndex < NUM_KEYCODES; ++keyIndex)
	{
		m_keyStates[keyIndex].m_wasPressedLastFrame = m_keyStates[keyIndex].m_isPressed;
	}

}




bool InputSystem::WasKeyJustPressed(unsigned char keyCode)
{

	KeyButtonState button = m_keyStates[keyCode];
	return button.m_isPressed && !button.m_wasPressedLastFrame;

}




bool InputSystem::WasKeyJustReleased(unsigned char keyCode)
{

	KeyButtonState button = m_keyStates[keyCode];
	return button.m_wasPressedLastFrame && !button.m_isPressed;

}


bool InputSystem::IsKeyDown(unsigned char keyCode)
{

	KeyButtonState button = m_keyStates[keyCode];
	return button.m_isPressed;

}




void InputSystem::HandleKeyPressed(unsigned char keyCode)
{
	m_keyStates[keyCode].m_isPressed = true;
}




void InputSystem::HandleKeyReleased(unsigned char keyCode)
{

	m_keyStates[keyCode].m_isPressed = false;

}

//------------------------------------------------------------------------------------------------------------------
void InputSystem::SetCursorMode(CursorMode cursorMode)
{
	m_cursorState.m_cursorMode = cursorMode;
}

//------------------------------------------------------------------------------------------------------------------
Vec2 InputSystem::GetCursorClientDelta() const
{
	return Vec2(m_cursorState.m_cursorClientDelta);
}

//------------------------------------------------------------------------------------------------------------------
IntVec2 InputSystem::GetCursorClientPosition() const
{
	return m_cursorState.m_cursorClientPosition;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void InputSystem::SetWheelDelta(float wheelDelta)
{
	m_cursorState.m_wheelDelta = wheelDelta;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
float InputSystem::GetWheelDelta()
{
	return m_cursorState.m_wheelDelta;
}

//------------------------------------------------------------------------------------------------------------------
Vec2 InputSystem::GetCursorNormalizedPosition() const
{
	Vec2 normalizedPos = Vec2(GetCursorClientPosition());
	
	normalizedPos.Normalize();

	return Vec2(normalizedPos.x, -normalizedPos.y);
}

//------------------------------------------------------------------------------------------------------------------
XboxController const& InputSystem::GetController(int controllerID)
{

	return m_controllers[controllerID];

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool InputSystem::OnKeyPressedEvent(EventArgs& args)
{
	if(!g_inputSystem)
	{
		return false;
	}

	unsigned char keyCode = static_cast<uchar>(args.GetValue("KeyCode", -1));
	g_inputSystem->HandleKeyPressed(keyCode);

	return true;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool InputSystem::OnKeyReleasedEvent(EventArgs& args)
{

	if(!g_inputSystem)
	{
		return false;
	}

	unsigned char keyCode = static_cast<uchar>(args.GetValue("KeyCode", -1));
	g_inputSystem->HandleKeyReleased(keyCode);

	return true;

}