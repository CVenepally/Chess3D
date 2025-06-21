#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/DevConsole.hpp"

//------------------------------------------------------------------------------------------------------------------
EventSystem* g_eventSystem = nullptr;

//------------------------------------------------------------------------------------------------------------------
EventSystem::EventSystem(EventSystemConfig const& config)
	: m_config(config)
{}


//------------------------------------------------------------------------------------------------------------------
EventSystem::~EventSystem()
{}


//------------------------------------------------------------------------------------------------------------------
void EventSystem::Startup()
{}


//------------------------------------------------------------------------------------------------------------------
void EventSystem::Shutdown()
{}


//------------------------------------------------------------------------------------------------------------------
void EventSystem::BeginFrame()
{}


//------------------------------------------------------------------------------------------------------------------
void EventSystem::EndFrame()
{}


//------------------------------------------------------------------------------------------------------------------
void EventSystem::SubscribeEventCallbackFunction(std::string const& eventName, EventCallbackFunction* functionPtr)
{

	if(!functionPtr)
	{
		ERROR_AND_DIE("Invalid Function Pointer when subscribing to an event");
	}

	auto result = m_subscriptionListByEventName.find(eventName);

	if(result == m_subscriptionListByEventName.end())
	{
		SubscriptionList newSubscriptionList;
		
		m_subscriptionListByEventName[eventName] = newSubscriptionList;

		m_eventList.push_back(eventName);
	}

	EventSubscription newEventSubscription(functionPtr);

	m_subscriptionListByEventName[eventName].push_back(newEventSubscription);

}


//------------------------------------------------------------------------------------------------------------------
void EventSystem::UnsubscribeEventCallbackFunction(std::string const& eventName, EventCallbackFunction* functionPtr)
{

	if(!functionPtr)
	{
		ERROR_AND_DIE("Invalid Function Pointer when unsubscribing to an event");
	}

	auto result = m_subscriptionListByEventName.find(eventName);

	if(result == m_subscriptionListByEventName.end())
	{
		return;
	}

	SubscriptionList& eventSubscriptionList = m_subscriptionListByEventName[eventName];

	for(int function = 0; function < static_cast<int>(eventSubscriptionList.size()); ++function)
	{
		if(eventSubscriptionList[function].m_functionPtr == functionPtr)
		{
			m_subscriptionListByEventName[eventName].erase(eventSubscriptionList.begin() + function);
			return;
		}
	}

}

//------------------------------------------------------------------------------------------------------------------
void EventSystem::FireEvent(std::string const& eventName, EventArgs& args)
{

	auto result = m_subscriptionListByEventName.find(eventName);

	if(result == m_subscriptionListByEventName.end())
	{
		g_devConsole->AddLine(DevConsole::ERROR, Stringf("Event '%s' does not exist.", eventName.c_str()));
		return;
	}
	
	SubscriptionList& eventSubscriptionList = m_subscriptionListByEventName[eventName];

	for(int function = 0; function < static_cast<int>(eventSubscriptionList.size()); ++function)
	{
		bool fireResult = eventSubscriptionList[function].m_functionPtr(args);

		if(fireResult)
		{
			return;
		}
	}

}


//------------------------------------------------------------------------------------------------------------------
void EventSystem::FireEvent(std::string const& eventName)
{

	EventArgs args;

	UNUSED(args);

	auto result = m_subscriptionListByEventName.find(eventName);

	if(result == m_subscriptionListByEventName.end())
	{
		DebuggerPrintf("Event %s does not exist.", eventName.c_str());
		return;
	}

	SubscriptionList& eventSubscriptionList = m_subscriptionListByEventName[eventName];

	for(int function = 0; function < static_cast<int>(eventSubscriptionList.size()); ++function)
	{
		bool fireResult = eventSubscriptionList[function].m_functionPtr(args);

		if(fireResult)
		{
			return;
		}
	}

}

//------------------------------------------------------------------------------------------------------------------
void SubscribeEventCallbackFunction(std::string const& eventName, EventCallbackFunction* functionPtr)
{
	g_eventSystem->SubscribeEventCallbackFunction(eventName, functionPtr);
}

//------------------------------------------------------------------------------------------------------------------
void UnsubscribeEventCallbackFunction(std::string const& eventName, EventCallbackFunction* functionPtr)
{

	g_eventSystem->UnsubscribeEventCallbackFunction(eventName, functionPtr);

}

//------------------------------------------------------------------------------------------------------------------
void FireEvent(std::string const& eventName, EventArgs& args)
{

	g_eventSystem->FireEvent(eventName, args);

}

//------------------------------------------------------------------------------------------------------------------
void FireEvent(std::string const& eventName)
{

	g_eventSystem->FireEvent(eventName);

}
