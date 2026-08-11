#pragma once

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Renderer/Camera.hpp"

//------------------------------------------------------------------------------------------------------------------
class App
{

public:

	App();
	~App();
	
	void Startup();
	void Shutdown();
	void RunFrame();

	void SetCursorVisibility();
	void RunMainFrame();
	void RequestQuit();
	void ResetGame();
	
	void LoadConfigFile(char const* configFilePath);
	bool isQuitting() const;

	static bool RequestQuitEvent(EventArgs& args);
	static bool OnChessListen(EventArgs& args);
	static bool OnChessConnect(EventArgs& args);
	static bool OnChessDisconnect(EventArgs& args);
	static bool OnChessServerInfo(EventArgs& args);
private:

	void BeginFrame();
	void Update();
	void Render() const;
	void EndFrame();


private:

	bool m_isQuitting = false;

};