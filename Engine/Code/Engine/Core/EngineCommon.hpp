#pragma once

#include "ThirdParty/tinyXML2/tinyxml2.h"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/FileUtils.hpp"

//------------------------------------------------------------------------------------------------------------------
#define UNUSED(x) (void)(x);

//------------------------------------------------------------------------------------------------------------------
#define DX_SAFE_RELEASE(dxObject)	\
{									\
	if((dxObject) != nullptr)		\
	{								\
		(dxObject)->Release();		\
		(dxObject) = nullptr;		\
	}								\
}	

//------------------------------------------------------------------------------------------------------------------
typedef tinyxml2::XMLDocument	XmlDocument;
typedef tinyxml2::XMLAttribute	XmlAttribute;
typedef tinyxml2::XMLElement	XmlElement;
typedef tinyxml2::XMLError		XmlResult;

//------------------------------------------------------------------------------------------------------------------
class NamedStrings;
class DevConsole;
class EventSystem;
class InputSystem;
class Window;

//------------------------------------------------------------------------------------------------------------------
extern NamedStrings g_gameConfigBlackboard;
extern DevConsole*	g_devConsole;
extern EventSystem*	g_eventSystem;
extern InputSystem*	g_inputSystem;
extern Window*		g_theWindow;

//------------------------------------------------------------------------------------------------------------------
typedef unsigned char uchar;
typedef NamedStrings EventArgs;