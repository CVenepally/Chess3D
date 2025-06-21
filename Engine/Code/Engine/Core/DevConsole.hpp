#pragma once

#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Renderer/Camera.hpp"

#include <string>
#include <vector>

#if defined ERROR
#undef ERROR
#endif

//------------------------------------------------------------------------------------------------------------------
struct AABB2;

class Camera;
class Timer;
class BitmapFont;
class Renderer;

//------------------------------------------------------------------------------------------------------------------
struct DevConsoleLine
{
	std::string m_text;
	double		m_timestamp;
	int			m_frameNumber;
	Rgba8		m_color = Rgba8::WHITE;
};

//------------------------------------------------------------------------------------------------------------------
struct DevConsoleConfig
{
	Renderer*	m_renderer = nullptr;
	Camera		m_devConsoleCamera;
	std::string m_fontFileNamePath;
	int			m_maxCommandHistory = 128;
	float		m_fontSize;
	float		m_fontAspect;
	float		m_numLinesToRender;
};

//------------------------------------------------------------------------------------------------------------------
enum DevConsoleMode
{
	HIDDEN,
	OPEN_FULL
};



//------------------------------------------------------------------------------------------------------------------
class DevConsole
{
public:
	DevConsole(DevConsoleConfig const& config);
	~DevConsole();

	void Startup();
	void Shutdown();
	void BeginFrame();
	void EndFrame();

	void Execute(std::string const& consoleCommandText);
	void AddLine(Rgba8 const& color, std::string const& text);
	void Render(AABB2 const& bounds, Renderer* rendererOverride = nullptr) const;

	DevConsoleMode GetMode() const;
	void SetMode(DevConsoleMode mode);
	void ToggleMode(DevConsoleMode mode);
	bool IsOpen();

	bool SetInputString(uchar charToAppend);

	static const Rgba8 INTRO_TEXT;
	static const Rgba8 INTRO_SUBTEXT;
	static const Rgba8 ERROR;
	static const Rgba8 WARNING;
	static const Rgba8 INFO_MAJOR;
	static const Rgba8 INFO_MINOR;
	static const Rgba8 INPUT;
	static const Rgba8 INVALID_INPUT;
	static const Rgba8 TEXT_HIGHLIGHT;

	static bool OnKeyPressedEvent(EventArgs& args);
	static bool OnCharPressedEvent(EventArgs& args);
	static bool OnClearEvent(EventArgs& args);
	static bool OnHelpEvent(EventArgs& args);

protected:
	void Render_OpenFull(AABB2 const& bounds, Renderer& renderer, BitmapFont& font, float fontAspect = 1.f) const;

protected:
	DevConsoleConfig				m_config;
	DevConsoleMode					m_mode = HIDDEN;
	std::vector<DevConsoleLine>		m_lines;
	int								m_frameNumber = 0;
	std::string						m_inputText;
	Timer*							m_insertionPointBlinkTimer = nullptr;
	int								m_insertionPointPosition = 0;
	bool							m_insertionPointVisible = true;
	std::vector<std::string>		m_commandHistory;
	int								m_historyIndex = -1;

};