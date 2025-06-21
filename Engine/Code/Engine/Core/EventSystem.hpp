#pragma once

#include "Engine/Core/EngineCommon.hpp"

#include <vector>
#include <string>
#include <map>

//------------------------------------------------------------------------------------------------------------------

typedef bool (EventCallbackFunction)(EventArgs& args);

//------------------------------------------------------------------------------------------------------------------
struct EventSubscription
{
	EventSubscription(EventCallbackFunction* functionPtr)
		: m_functionPtr(functionPtr)
	{}

	EventCallbackFunction* m_functionPtr = nullptr;
};

//------------------------------------------------------------------------------------------------------------------
struct EventSystemConfig
{

};

//------------------------------------------------------------------------------------------------------------------
typedef std::vector<EventSubscription> SubscriptionList;

//------------------------------------------------------------------------------------------------------------------
class EventSystem
{

	friend class DevConsole;

public:

	EventSystem(EventSystemConfig const& config);
	~EventSystem();

	void Startup();
	void Shutdown();
	void BeginFrame();
	void EndFrame();


	void SubscribeEventCallbackFunction(std::string const& eventName, EventCallbackFunction* functionPtr);
	void UnsubscribeEventCallbackFunction(std::string const& eventName, EventCallbackFunction* functionPtr);
	void FireEvent(std::string const& eventName, EventArgs& args);
	void FireEvent(std::string const& eventName);


protected:

	EventSystemConfig							m_config;
	std::vector<std::string>					m_eventList;
	std::map<std::string, SubscriptionList>		m_subscriptionListByEventName;
};

void SubscribeEventCallbackFunction(std::string const& eventName, EventCallbackFunction* functionPtr);
void UnsubscribeEventCallbackFunction(std::string const& eventName, EventCallbackFunction* functionPtr);
void FireEvent(std::string const& eventName, EventArgs& args);
void FireEvent(std::string const& eventName);

