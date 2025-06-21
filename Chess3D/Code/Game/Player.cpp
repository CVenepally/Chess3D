#include "Game/Player.hpp"
#include "Game/Game.hpp" 
#include "Game/GameCommon.hpp"
#include "Engine/Core/EngineCommon.hpp"

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Player::Player(Game* owner)
	:m_owner(owner)
{
	m_playerCamera.m_viewportBounds.m_mins = Vec2::ZERO;
	m_playerCamera.m_viewportBounds.m_maxs = Vec2(static_cast<float>(g_theWindow->GetClientDimensions().x), static_cast<float>(g_theWindow->GetClientDimensions().y));
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Player::~Player()
{}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Player::Update()
{
	float deltaSeconds = Clock::GetSystemClock().GetDeltaSeconds();
	KeyboardControls(deltaSeconds);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Player::Render() const
{}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Vec3 Player::GetForwardNormal() const
{
	Mat44 orientationMat = m_orientation.GetAsMatrix_IFwd_JLeft_KUp();

	return orientationMat.GetIBasis3D().GetNormalized();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Player::KeyboardControls(float deltaSeconds)
{

	XboxController const& xboxController = g_inputSystem->GetController(0);
	AnalogJoystick const& rightStick = xboxController.GetRightStick();
	AnalogJoystick const& leftStick = xboxController.GetLeftStick();

	m_orientation.m_yawDegrees += -(g_inputSystem->GetCursorClientDelta().x * 0.125f);
	m_orientation.m_pitchDegrees += g_inputSystem->GetCursorClientDelta().y * 0.125f;

	m_orientation.m_yawDegrees -= (rightStick.GetPosition().x * 0.125f);
	m_orientation.m_pitchDegrees -= rightStick.GetPosition().y * 0.125f;

	m_orientation.m_pitchDegrees = GetClamped(m_orientation.m_pitchDegrees, -85.f, 85.f);
	m_orientation.m_rollDegrees = GetClamped(m_orientation.m_rollDegrees, -45.f, 45.f);

	Vec3 fwdVector;
	Vec3 leftVector;
	Vec3 upVector;

	m_orientation.GetAsVectors_IFwd_JLeft_KUp(fwdVector, leftVector, upVector);

	float moveSpeed = g_gameConfigBlackboard.GetValue("playerMoveSpeed", 2.f);

	if(g_inputSystem->IsKeyDown(KEYCODE_LSHIFT) || xboxController.IsButtonDown(XBOX_BUTTON_A))
	{
		moveSpeed *= 10.f;
	}
 
  	if(g_inputSystem->IsKeyDown('W') || leftStick.GetPosition().y > 0.1f)
  	{
  		m_position +=  fwdVector * moveSpeed * deltaSeconds;
  	}
  
  	if(g_inputSystem->IsKeyDown('S') || leftStick.GetPosition().y < -0.1f)
  	{
  		m_position -= fwdVector * moveSpeed * deltaSeconds;
  	}
  
  	if(g_inputSystem->IsKeyDown('A') || leftStick.GetPosition().x < -0.1f)
  	{
  		m_position += leftVector * moveSpeed * deltaSeconds;
  	}
  
  	if(g_inputSystem->IsKeyDown('D') || leftStick.GetPosition().x > 0.1f)
  	{
  		m_position -= leftVector * moveSpeed * deltaSeconds;
  	}

	if(g_inputSystem->IsKeyDown('E') || xboxController.IsButtonDown(XBOX_BUTTON_RIGHT_SHOULDER))
	{
		m_position.z += moveSpeed * deltaSeconds;
	}

	if(g_inputSystem->IsKeyDown('Q') || xboxController.IsButtonDown(XBOX_BUTTON_LEFT_SHOULDER))
	{
		m_position.z -=  moveSpeed * deltaSeconds;
	}
}

