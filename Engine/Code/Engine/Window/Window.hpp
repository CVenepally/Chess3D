#pragma once

#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Renderer/Camera.hpp"
#include <string>

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
class InputSystem;


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
struct WindowConfig
{
	float				m_aspectRatio = (16.f / 9.f);
	InputSystem*		m_theInputSystem = nullptr;
	std::string			m_windowTitle = "Unnamed Game";
};



//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
class Window
{
public:

	static Window*		s_mainWindow;

	Window(WindowConfig const& config);
	~Window();

	void	Startup();
	void	BeginFrame();
	void	EndFrame();
	void	Shutdown();
	void*	GetDisplayContext() const;
	void*	GetHWND() const;

	bool	IsWindowActive();

	IntVec2	GetClientDimensions() const;

	WindowConfig const&		GetConfig() const;
	Vec2					GetNormalizedMouseUV() const;

private:
	
	void CreateOSWindow();
	void RunMessagePump();

private:

	WindowConfig		m_config;
	IntVec2				m_clientDimensions;
	void*				m_displayDeviceContext = nullptr;
	void*				m_windowHandle		   = nullptr;

};