#pragma once

#include "Engine/Renderer/Camera.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/EulerAngles.hpp"
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
class Game;

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
class Player
{
public:

	Player(Game* owner);
	~Player();

	void			Update();
	void			Render() const ;

	Vec3			GetForwardNormal() const;

	void			KeyboardControls(float deltaSeconds);

public:

	Game*			m_owner			= nullptr;
	Vec3			m_position		= Vec3::ZERO;
	EulerAngles		m_orientation	= EulerAngles(0.f, 0.f, 0.f);
	Camera			m_playerCamera;

};