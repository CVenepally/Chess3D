#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <windows.h>

#include "Engine/Window/Window.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/DebugRender.hpp"


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Global Variables
Window* Window::s_mainWindow = nullptr;
Window* g_theWindow = nullptr;

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
LRESULT CALLBACK WindowsMessageHandlingProcedure(HWND windowHandle, UINT wmMessageCode, WPARAM wParam, LPARAM lParam)
{

	InputSystem* input = nullptr;
	if(Window::s_mainWindow)
	{
		WindowConfig const& config = Window::s_mainWindow->GetConfig();
		input = config.m_theInputSystem;
	}

	switch(wmMessageCode)
	{
		
		case WM_CLOSE:
		{

			FireEvent("Quit");

			return 0; 
		}

		case WM_CHAR:
		{
			EventArgs args;
			args.SetValue("CharTyped", Stringf("%d", static_cast<uchar>(wParam)));
			FireEvent("CharTyped", args);
			return 0;
		}
		
		case WM_KEYDOWN:
		{
			
			EventArgs args;
			args.SetValue("KeyCode", Stringf("%d", static_cast<uchar>(wParam)));
			FireEvent("KeyPressed", args);

			return 0;
		}

		case WM_KEYUP:
		{

			EventArgs args;
			args.SetValue("KeyCode", Stringf("%d", static_cast<uchar>(wParam)));
			FireEvent("KeyReleased", args);
	
			return 0;
		}

		case WM_LBUTTONDOWN:
		{
			if(input)
			{
				input->HandleKeyPressed(KEYCODE_LEFT_MOUSE);
			}
			return 0;
		}

		case WM_RBUTTONDOWN:
		{
			if(input)
			{
				input->HandleKeyPressed(KEYCODE_RIGHT_MOUSE);
			}
			return 0;

		}
		case WM_LBUTTONUP:
		{
			if(input)
			{
				input->HandleKeyReleased(KEYCODE_LEFT_MOUSE);
			}
			return 0;

		}
		case WM_RBUTTONUP:
		{
			if(input)
			{
				input->HandleKeyReleased(KEYCODE_RIGHT_MOUSE);
			}
			return 0;

		}

		case WM_MOUSEWHEEL:
		{
			if(input)
			{
				input->SetWheelDelta(static_cast<float>(GET_WHEEL_DELTA_WPARAM(wParam)));
			}
		}
	}

	// Send back to Windows any unhandled/unconsumed messages we want other apps to see (e.g. play/pause in music apps, etc.)
	return DefWindowProc(windowHandle, wmMessageCode, wParam, lParam);
}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Window::Window(WindowConfig const& config)
	:m_config(config)
{
	s_mainWindow = this;
}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void* Window::GetDisplayContext() const
{
		
	return m_displayDeviceContext;

}

//------------------------------------------------------------------------------------------------------------------
void* Window::GetHWND() const
{
	return m_windowHandle;
}

//------------------------------------------------------------------------------------------------------------------
bool Window::IsWindowActive()
{
	return GetActiveWindow() == static_cast<HWND>(GetHWND());
}

//------------------------------------------------------------------------------------------------------------------
IntVec2 Window::GetClientDimensions() const
{

	return m_clientDimensions;

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Window::~Window()
{

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Window::Startup()
{
	CreateOSWindow();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Window::BeginFrame()
{
	RunMessagePump();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Window::EndFrame()
{

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Window::Shutdown()
{

}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
WindowConfig const& Window::GetConfig() const
{
	return m_config;
}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Vec2 Window::GetNormalizedMouseUV() const
{

	HWND windowHandle = static_cast<HWND>(m_windowHandle);
	POINT cursorCoords;
	RECT clientRect;
	::GetCursorPos(&cursorCoords);
	::ScreenToClient(windowHandle, &cursorCoords);
	::GetClientRect(windowHandle, &clientRect);

	float cursorX = static_cast<float>(cursorCoords.x) / static_cast<float>(clientRect.right);
	float cursorY = static_cast<float>(cursorCoords.y) / static_cast<float>(clientRect.bottom);

	return Vec2(cursorX, 1.f - cursorY);

}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Window::CreateOSWindow()
{
	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

	HMODULE applicationInstanceHandle = ::GetModuleHandle(NULL);
	float clientAspect = m_config.m_aspectRatio;


	// Define a window style/class
	WNDCLASSEX windowClassDescription;
	memset(&windowClassDescription, 0, sizeof(windowClassDescription));
	windowClassDescription.cbSize = sizeof(windowClassDescription);
	windowClassDescription.style = CS_OWNDC; // Redraw on move, request own Display Context
	windowClassDescription.lpfnWndProc = static_cast<WNDPROC>(WindowsMessageHandlingProcedure); // Register our Windows message-handling function
	windowClassDescription.hInstance = GetModuleHandle(NULL);
	windowClassDescription.hIcon = NULL;
	windowClassDescription.hCursor = NULL;
	windowClassDescription.lpszClassName = TEXT("Simple Window Class");
	RegisterClassEx(&windowClassDescription);

	const DWORD windowStyleFlags = WS_CAPTION | WS_BORDER | WS_SYSMENU | WS_OVERLAPPED;
	const DWORD windowStyleExFlags = WS_EX_APPWINDOW;

	// Get desktop rect, dimensions, aspect
	RECT desktopRect;
	HWND desktopWindowHandle = GetDesktopWindow();
	GetClientRect(desktopWindowHandle, &desktopRect);
	float desktopWidth = (float)(desktopRect.right - desktopRect.left);
	float desktopHeight = (float)(desktopRect.bottom - desktopRect.top);
	float desktopAspect = desktopWidth / desktopHeight;

	// Calculate maximum client size (as some % of desktop size)
	constexpr float maxClientFractionOfDesktop = 0.90f;
	float clientWidth = desktopWidth * maxClientFractionOfDesktop;
	float clientHeight = desktopHeight * maxClientFractionOfDesktop;

	if(clientAspect > desktopAspect)
	{
		// Client window has a wider aspect than desktop; shrink client height to match its width
		clientHeight = clientWidth / clientAspect;
	}
	else
	{
		// Client window has a taller aspect than desktop; shrink client width to match its height
		clientWidth = clientHeight * clientAspect;
	}

	// Calculate client rect bounds by centering the client area
	float clientMarginX = 0.5f * (desktopWidth - clientWidth);
	float clientMarginY = 0.5f * (desktopHeight - clientHeight);
	RECT clientRect;
	clientRect.left = (int)clientMarginX;
	clientRect.right = clientRect.left + (int)clientWidth;
	clientRect.top = (int)clientMarginY;
	clientRect.bottom = clientRect.top + (int)clientHeight;

	// Calculate the outer dimensions of the physical window, including frame et. al.
	RECT windowRect = clientRect;

	m_clientDimensions.x = static_cast<int>(windowRect.right - windowRect.left);
	m_clientDimensions.y = static_cast<int>(windowRect.bottom - windowRect.top);

	AdjustWindowRectEx(&windowRect, windowStyleFlags, FALSE, windowStyleExFlags);

	// storing client dimensions


	WCHAR windowTitle[1024];
	MultiByteToWideChar(GetACP(), 0, m_config.m_windowTitle.c_str(), -1, windowTitle, sizeof(windowTitle) / sizeof(windowTitle[0]));
	m_windowHandle = CreateWindowEx(
		windowStyleExFlags,
		windowClassDescription.lpszClassName,
		windowTitle,
		windowStyleFlags,
		windowRect.left,
		windowRect.top,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		NULL,
		NULL,
		(HINSTANCE)applicationInstanceHandle,
		NULL);

	ShowWindow(static_cast<HWND>(m_windowHandle), SW_SHOW);
	SetForegroundWindow(static_cast<HWND>(m_windowHandle));
	SetFocus(static_cast<HWND>(m_windowHandle));

	m_displayDeviceContext = GetDC(static_cast<HWND>(m_windowHandle));

	HCURSOR cursor = LoadCursor(NULL, IDC_ARROW);
	SetCursor(cursor);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Window::RunMessagePump()
{
	MSG queuedMessage;
	for(;; )
	{
		const BOOL wasMessagePresent = PeekMessage(&queuedMessage, NULL, 0, 0, PM_REMOVE);
		if(!wasMessagePresent)
		{
			break;
		}

		TranslateMessage(&queuedMessage);
		DispatchMessage(&queuedMessage); // This tells Windows to call our "WindowsMessageHandlingProcedure" (a.k.a. "WinProc") function
	}
}
