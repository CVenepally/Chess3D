#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/Timer.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Window/Window.hpp"
//------------------------------------------------------------------------------------------------------------------
DevConsole* g_devConsole = nullptr;

//------------------------------------------------------------------------------------------------------------------
const Rgba8 DevConsole::INTRO_TEXT		= Rgba8(255, 120, 0, 255);
const Rgba8 DevConsole::INTRO_SUBTEXT	= Rgba8(255, 165, 0, 255);
const Rgba8 DevConsole::ERROR			= Rgba8(255, 0, 0, 255);
const Rgba8 DevConsole::WARNING			= Rgba8(255, 255, 0, 255);
const Rgba8 DevConsole::INFO_MAJOR		= Rgba8(0, 255, 255, 255);
const Rgba8 DevConsole::INFO_MINOR		= Rgba8(0, 139, 255, 255);
const Rgba8 DevConsole::INPUT			= Rgba8(255, 255, 255, 255);
const Rgba8 DevConsole::INVALID_INPUT	= Rgba8(128, 128, 128, 255);
const Rgba8 DevConsole::TEXT_HIGHLIGHT	= Rgba8(0, 255, 150, 255);

//------------------------------------------------------------------------------------------------------------------
DevConsole::DevConsole(DevConsoleConfig const& config)
	: m_config(config)
{
	m_commandHistory.reserve(m_config.m_maxCommandHistory);
}

//------------------------------------------------------------------------------------------------------------------
DevConsole::~DevConsole()
{}

//------------------------------------------------------------------------------------------------------------------
void DevConsole::Startup()
{

	m_insertionPointBlinkTimer = new Timer(0.6);
	m_insertionPointBlinkTimer->Start();

	g_eventSystem->SubscribeEventCallbackFunction("CharTyped", DevConsole::OnCharPressedEvent);
	g_eventSystem->SubscribeEventCallbackFunction("KeyPressed", DevConsole::OnKeyPressedEvent);
	g_eventSystem->SubscribeEventCallbackFunction("Clear", DevConsole::OnClearEvent);
	g_eventSystem->SubscribeEventCallbackFunction("Help", DevConsole::OnHelpEvent);

}

//------------------------------------------------------------------------------------------------------------------
void DevConsole::Shutdown()
{

	g_eventSystem->UnsubscribeEventCallbackFunction("CharTyped", DevConsole::OnCharPressedEvent);
	g_eventSystem->UnsubscribeEventCallbackFunction("KeyPressed", DevConsole::OnKeyPressedEvent);
	g_eventSystem->UnsubscribeEventCallbackFunction("Clear", DevConsole::OnClearEvent);
	g_eventSystem->UnsubscribeEventCallbackFunction("Help", DevConsole::OnHelpEvent);

}

//------------------------------------------------------------------------------------------------------------------
void DevConsole::BeginFrame()
{
	m_config.m_devConsoleCamera.m_mode = Camera::eMode_Orthographic;
	
	m_config.m_devConsoleCamera.m_viewportBounds.m_mins = Vec2::ZERO;
	m_config.m_devConsoleCamera.m_viewportBounds.m_maxs = Vec2(static_cast<float>(g_theWindow->GetClientDimensions().x), static_cast<float>(g_theWindow->GetClientDimensions().y));

	m_config.m_devConsoleCamera.SetOrthographicView(Vec2(0.f, 0.f), Vec2(1600.f, 800.f));

	while(m_insertionPointBlinkTimer->DecrementPeriodIfElapsed())
	{
		m_insertionPointVisible = !m_insertionPointVisible;
	}
}

//------------------------------------------------------------------------------------------------------------------
void DevConsole::EndFrame()
{}

//------------------------------------------------------------------------------------------------------------------
void DevConsole::Execute(std::string const& consoleCommandText)
{

	std::vector<std::string> lines = SplitStringOnDelimiter(consoleCommandText, '\n');
	
	for(int line = 0; line < static_cast<int>(lines.size()); ++line)
	{
		
		std::vector<std::string> words = SplitStringOnDelimiter(lines[line], ' ');

		EventArgs args;

		for(int word = 1; word < static_cast<int>(words.size()); ++word)
		{

			std::vector<std::string> keyValuePair = SplitStringOnDelimiter(words[word], '=');
			args.SetValue(keyValuePair[0], keyValuePair[1]);

		}

		FireEvent(words[0], args);

	}

}

//------------------------------------------------------------------------------------------------------------------
void DevConsole::AddLine(Rgba8 const& color, std::string const& text)
{

	DevConsoleLine line;

	line.m_text = text;
	line.m_color = color;
	line.m_timestamp = GetCurrentTimeSeconds();
	line.m_frameNumber = m_frameNumber;

	m_lines.push_back(line);
}


//------------------------------------------------------------------------------------------------------------------
void DevConsole::Render(AABB2 const& bounds, Renderer* rendererOverride) const
{
	
	switch(m_mode)
	{
		case HIDDEN:
		{
			break;
		}
		case OPEN_FULL:
		{
			BitmapFont* font = rendererOverride->CreateOrGetBitmapFont(m_config.m_fontFileNamePath.c_str());
			Render_OpenFull(bounds, *rendererOverride, *font, m_config.m_fontAspect);
			break;
		}
		default:
			break;
	}

}


//------------------------------------------------------------------------------------------------------------------
DevConsoleMode DevConsole::GetMode() const
{
	return m_mode;
}


//------------------------------------------------------------------------------------------------------------------
void DevConsole::SetMode(DevConsoleMode mode)
{
	m_mode = mode;
}


//------------------------------------------------------------------------------------------------------------------
void DevConsole::ToggleMode(DevConsoleMode mode)
{
	
	if(m_mode == HIDDEN)
	{
		m_mode = mode;
		return;
	}

	if(m_mode == mode)
	{
		m_mode = HIDDEN;
		m_inputText = "";
		m_insertionPointPosition = 0;
		return;
	}
	
}

//------------------------------------------------------------------------------------------------------------------
bool DevConsole::IsOpen()
{
	return m_mode != HIDDEN;
}

//------------------------------------------------------------------------------------------------------------------
bool DevConsole::SetInputString(uchar charToAppend)
{

	if(charToAppend >= 32 && charToAppend <= 126 && (charToAppend != '`' && charToAppend != '~'))
	{
		std::string appendString(1, charToAppend);
		m_inputText.insert(m_insertionPointPosition, appendString);
		m_insertionPointPosition += 1;
		return true;
	}
	
	return false;
}

//------------------------------------------------------------------------------------------------------------------
bool DevConsole::OnKeyPressedEvent(EventArgs& args)
{
	if(!g_devConsole)
	{
		return false;
	}
	
	if(g_devConsole->IsOpen())
	{

		g_devConsole->m_insertionPointVisible = true;
		g_devConsole->m_insertionPointBlinkTimer->Stop();
		g_devConsole->m_insertionPointBlinkTimer->Start();

		if(args.GetValue("KeyCode", 0) == KEYCODE_TILDE)
		{
			g_devConsole->ToggleMode(DevConsoleMode::OPEN_FULL);
			return true;
		}

		// insertion point positions
		if(args.GetValue("KeyCode", 0) == KEYCODE_LEFT_ARROW)
		{
			if(g_devConsole->m_insertionPointPosition > 0)
			{
				g_devConsole->m_insertionPointPosition -= 1;
				return true;
			}
			return false;
		}

		if(args.GetValue("KeyCode", 0) == KEYCODE_RIGHT_ARROW)
		{
			if(g_devConsole->m_insertionPointPosition < static_cast<int>(g_devConsole->m_inputText.length()))
			{
				g_devConsole->m_insertionPointPosition += 1;
				return true;
			}
			return false;
		}
		
		if(args.GetValue("KeyCode", 0) == KEYCODE_HOME)
		{
			g_devConsole->m_insertionPointPosition = 0;
			return true;
		}

		if(args.GetValue("KeyCode", 0) == KEYCODE_END)
		{
			g_devConsole->m_insertionPointPosition = static_cast<int>(g_devConsole->m_inputText.length());
			return true;
		}

		// clearing
		if(args.GetValue("KeyCode", 0) == KEYCODE_BACKSPACE)
		{
			if(g_devConsole->m_insertionPointPosition > 0 && static_cast<int>(g_devConsole->m_inputText.length()) > 0)
			{
				g_devConsole->m_inputText.erase(g_devConsole->m_inputText.begin() + g_devConsole->m_insertionPointPosition - 1);
				g_devConsole->m_insertionPointPosition -= 1;
				return true;
			}
			return false;
		}

		if(args.GetValue("KeyCode", 0) == KEYCODE_DELETE)
		{
			if(g_devConsole->m_insertionPointPosition < static_cast<int>(g_devConsole->m_inputText.length()) && static_cast<int>(g_devConsole->m_inputText.length()) > 0)
			{
				g_devConsole->m_inputText.erase(g_devConsole->m_inputText.begin() + g_devConsole->m_insertionPointPosition);
				return true;
			}
			return false;
		}

		if(args.GetValue("KeyCode", 0) == KEYCODE_ESC)
		{
			if(g_devConsole->m_inputText.length() == 0)
			{
				g_devConsole->SetMode(HIDDEN);
				return true;
			}

			g_devConsole->m_inputText.clear();
			g_devConsole->m_insertionPointPosition = 0;
			return true;
		}

		// up and down arrow
		if(args.GetValue("KeyCode", 0) == KEYCODE_UP_ARROW)
		{

			if(g_devConsole->m_historyIndex <= 0)
			{
				g_devConsole->m_historyIndex = static_cast<int>(g_devConsole->m_commandHistory.size()) - 1;
			}
			else if(g_devConsole->m_historyIndex != -1)
			{
				g_devConsole->m_historyIndex -= 1;
			}

			g_devConsole->m_inputText = g_devConsole->m_commandHistory[g_devConsole->m_historyIndex];

			g_devConsole->m_insertionPointPosition = static_cast<int>(g_devConsole->m_inputText.length());

			return true;
		}

		if(args.GetValue("KeyCode", 0) == KEYCODE_DOWN_ARROW)
		{
			

			if(g_devConsole->m_historyIndex < 0 || g_devConsole->m_historyIndex >= static_cast<int>(g_devConsole->m_commandHistory.size() - 1))
			{
				g_devConsole->m_historyIndex = 0;
			}
			else
			{
				g_devConsole->m_historyIndex += 1;
			}

			g_devConsole->m_inputText = g_devConsole->m_commandHistory[g_devConsole->m_historyIndex];

			g_devConsole->m_insertionPointPosition = static_cast<int>(g_devConsole->m_inputText.length());

			return true;
		}

		// execution
		if(args.GetValue("KeyCode", 0) == KEYCODE_ENTER)
		{

			if(g_devConsole->m_inputText.length() == 0)
			{
				g_devConsole->SetMode(HIDDEN);
				return true;
			}

			Rgba8 textColor = DevConsole::INFO_MAJOR;

			auto result = g_eventSystem->m_subscriptionListByEventName.find(g_devConsole->m_inputText);

			if(result == g_eventSystem->m_subscriptionListByEventName.end())
			{
				textColor = DevConsole::INVALID_INPUT;
			}

			g_devConsole->AddLine(textColor, g_devConsole->m_inputText);
			
			if(static_cast<int>(g_devConsole->m_commandHistory.size()) == g_devConsole->m_config.m_maxCommandHistory)
			{
				g_devConsole->m_commandHistory.erase(g_devConsole->m_commandHistory.begin());
				g_devConsole->m_commandHistory.push_back(g_devConsole->m_inputText);
			}
			else
			{
				g_devConsole->m_commandHistory.push_back(g_devConsole->m_inputText);
			}

			g_devConsole->Execute(g_devConsole->m_inputText);
			
			g_devConsole->m_inputText.clear();
			g_devConsole->m_insertionPointPosition = 0;

			return true;
		}


		if(args.GetValue("KeyCode", 0) == ' ')
		{
			return true;
		}

		return true;
	}

	return false;
}

//------------------------------------------------------------------------------------------------------------------
bool DevConsole::OnCharPressedEvent(EventArgs& args)
{

	if(!g_devConsole)
	{
		return false;
	}

	if(g_devConsole->IsOpen())
	{
		unsigned char charToAppend = static_cast<uchar>(args.GetValue("CharTyped", 0));
		return g_devConsole->SetInputString(charToAppend);
	}

	return false;
}

//------------------------------------------------------------------------------------------------------------------
bool DevConsole::OnClearEvent(EventArgs& args)
{
	UNUSED(args);

	if(!g_devConsole)
	{
		return false;
	}

	g_devConsole->m_lines.clear();
	return true;
}

//------------------------------------------------------------------------------------------------------------------
bool DevConsole::OnHelpEvent(EventArgs& args)
{

	UNUSED(args);

	if(!g_devConsole)
	{
		return false;
	}

	if(g_devConsole->IsOpen())
	{
		g_devConsole->AddLine(INFO_MAJOR, "Registered Commands");
		for(int i = 0; i < static_cast<int>(g_eventSystem->m_eventList.size()); ++i)
		{
			g_devConsole->AddLine(INFO_MINOR, g_eventSystem->m_eventList[i]);
		}
		return true;
	}

	return false;
}

//------------------------------------------------------------------------------------------------------------------
void DevConsole::Render_OpenFull(AABB2 const& bounds, Renderer& renderer, BitmapFont& font, float fontAspect) const
{

	renderer.BeginCamera(m_config.m_devConsoleCamera);

	float numLinesToRender = 38.f;

	std::vector<Vertex_PCU> overlayVerts;
	AddVertsForAABB2D(overlayVerts, bounds, Rgba8(0, 0, 0, 150));
	
	renderer.SetRasterizerMode(RasterizerMode::SOLID_CULL_NONE);
	renderer.SetBlendMode(BlendMode::ALPHA);
	renderer.BindShader(nullptr);
	renderer.BindTexture(nullptr);
	renderer.DrawVertexArray(overlayVerts);

	// input box
	std::vector<Vertex_PCU> inputOverlayVerts;

	Vec2 inputOverlayMins = Vec2(bounds.m_mins.x, bounds.m_mins.y);
	Vec2 inputOverlayMaxs = Vec2(bounds.m_maxs.x, bounds.m_mins.y + m_config.m_fontSize);

	AddVertsForAABB2D(inputOverlayVerts, AABB2(inputOverlayMins, inputOverlayMaxs), Rgba8(0, 0, 0, 175));

	renderer.BindTexture(nullptr);
	renderer.DrawVertexArray(inputOverlayVerts);

	// input render
	std::vector<Vertex_PCU> inputVerts;

	Vec2 inputMins = Vec2(bounds.m_mins.x, bounds.m_mins.y);
	Vec2 inputMaxs = Vec2(bounds.m_maxs.x, bounds.m_mins.y + m_config.m_fontSize);
	
	font.AddVertsForTextInBox2D(inputVerts, m_inputText, AABB2(inputMins, inputMaxs), m_config.m_fontSize, DevConsole::INPUT, fontAspect, Vec2(0.f, 0.f), SHRINK_TO_FIT);
	renderer.BindTexture(&font.GetTexture());
	renderer.DrawVertexArray(inputVerts);

	// insertion Point
	std::vector<Vertex_PCU> insertionPointVerts;

	float charWidth = fontAspect * m_config.m_fontSize;

	Vec2 insertionPointMins = Vec2(bounds.m_mins.x + (m_insertionPointPosition * charWidth), bounds.m_mins.y);
	Vec2 insertionPointMaxs = Vec2(insertionPointMins.x + 1.f , bounds.m_mins.y + m_config.m_fontSize);

	AddVertsForAABB2D(insertionPointVerts, AABB2(insertionPointMins, insertionPointMaxs), Rgba8::WHITE);
	
	if(m_insertionPointVisible)
	{
		renderer.BindTexture(nullptr);
		renderer.DrawVertexArray(insertionPointVerts);
	}

	// console text render
	std::vector<Vertex_PCU> textVerts;
	int linesToRender = static_cast<int>(GetClamped(static_cast<float>(numLinesToRender), 0.f, static_cast<float>(m_lines.size())));

	for(int i = 0; i < linesToRender; ++i)
	{
		DevConsoleLine const& currentLine = m_lines[static_cast<int>(m_lines.size()) - 1 - i];

		Vec2 mins = Vec2(bounds.m_mins.x, bounds.m_mins.y + (i * m_config.m_fontSize) + m_config.m_fontSize);
		Vec2 maxs = Vec2(bounds.m_maxs.x, mins.y + (bounds.m_maxs.y / numLinesToRender));

		font.AddVertsForTextInBox2D(textVerts, currentLine.m_text, AABB2(mins, maxs), m_config.m_fontSize, currentLine.m_color, fontAspect, Vec2(0.f, 0.f), SHRINK_TO_FIT);
	}

	renderer.BindTexture(&font.GetTexture());
	renderer.DrawVertexArray(textVerts);

	// command history and suggestions (have flags for both of them)

	renderer.EndCamera(m_config.m_devConsoleCamera);
}
