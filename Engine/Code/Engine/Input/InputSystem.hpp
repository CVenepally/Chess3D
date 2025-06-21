#pragma once

#include "Engine/Input/XboxController.hpp"
#include "Engine/Input/KeyButtonState.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/IntVec2.hpp"


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern uchar const KEYCODE_F1;
extern uchar const KEYCODE_F2;
extern uchar const KEYCODE_F3;
extern uchar const KEYCODE_F4;
extern uchar const KEYCODE_F5;
extern uchar const KEYCODE_F6;
extern uchar const KEYCODE_F7;
extern uchar const KEYCODE_F8;
extern uchar const KEYCODE_F9;
extern uchar const KEYCODE_F10;
extern uchar const KEYCODE_F11;
extern uchar const KEYCODE_F12;
extern uchar const KEYCODE_ESC;
extern uchar const KEYCODE_SPACE;
extern uchar const KEYCODE_ENTER;
extern uchar const KEYCODE_LSHIFT;
extern uchar const KEYCODE_UP_ARROW;
extern uchar const KEYCODE_DOWN_ARROW;
extern uchar const KEYCODE_LEFT_ARROW;
extern uchar const KEYCODE_RIGHT_ARROW;
extern uchar const KEYCODE_LEFT_MOUSE;
extern uchar const KEYCODE_RIGHT_MOUSE;
extern uchar const KEYCODE_TILDE;
extern uchar const KEYCODE_LEFT_BRACKET;
extern uchar const KEYCODE_RIGHT_BRACKET;
extern uchar const KEYCODE_BACKSPACE;
extern uchar const KEYCODE_DELETE;
extern uchar const KEYCODE_INSERT;
extern uchar const KEYCODE_HOME;
extern uchar const KEYCODE_END;

constexpr int NUM_KEYCODES           = 256;
constexpr int NUM_XBOX_CONTROLLERS   = 4;


//------------------------------------------------------------------------------------------------------------------
enum class CursorMode
{
	POINTER,
	FPS,

	COUNT
};

//------------------------------------------------------------------------------------------------------------------
struct CursorState
{
	IntVec2 m_cursorClientDelta;
	IntVec2 m_cursorClientPosition;

	CursorMode m_cursorMode = CursorMode::FPS;

	float m_wheelDelta;
};

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
struct InputConfig
{

};



//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
class InputSystem
{

public:

	InputSystem(InputConfig const& config);
	~InputSystem();

	void         Startup();
	void         Shutdown();
	void         BeginFrame();
	void         EndFrame();
		         
	bool         WasKeyJustPressed( unsigned char keyCode );
	bool         WasKeyJustReleased( unsigned char keyCode );
	bool         IsKeyDown( unsigned char keyCode );
	void         HandleKeyPressed( unsigned char keyCode );
	void         HandleKeyReleased( unsigned char keyCode );

	void		 SetCursorMode(CursorMode cursorMode);
	Vec2		 GetCursorClientDelta() const;
	IntVec2      GetCursorClientPosition() const;

	void		 SetWheelDelta(float wheelDelta);
	float		 GetWheelDelta();

	Vec2         GetCursorNormalizedPosition() const;

	XboxController const& GetController( int controllerID );

	static bool OnKeyPressedEvent(EventArgs& args);
	static bool OnKeyReleasedEvent(EventArgs& args);

protected:

	KeyButtonState     m_keyStates[ NUM_KEYCODES ];
	XboxController     m_controllers[ NUM_XBOX_CONTROLLERS ];


private:

	InputConfig		   m_config;
	CursorState		   m_cursorState;
};