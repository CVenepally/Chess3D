#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Math/MathUtils.hpp"

#include <thread>

static Clock* s_systemClock = new Clock();


//------------------------------------------------------------------------------------------------------------------
Clock::Clock(std::string const& name, int fpsCap)
	: m_name(name)
{
	if(s_systemClock)
	{
		m_parentClock = s_systemClock;
		m_parentClock->AddChild(this);
	}

	if(fpsCap == 0)
	{
		m_minDeltaSeconds = 0.;
	}
	else
	{
		m_minDeltaSeconds = 1. / static_cast<double>(fpsCap);
	}

}

//------------------------------------------------------------------------------------------------------------------
Clock::~Clock()
{
	if(m_parentClock)
	{
		m_parentClock->m_children.erase(std::remove(m_parentClock->m_children.begin(), m_parentClock->m_children.end(), this), m_parentClock->m_children.end());
	}

	for(int clocks = 0; clocks < static_cast<int>(m_children.size()); ++clocks)
	{
		if(m_children[clocks])
		{
			m_children[clocks]->m_parentClock = nullptr;
		}
	}
}

//------------------------------------------------------------------------------------------------------------------
Clock::Clock(Clock& parent, std::string const& name)
	: m_parentClock(&parent)
	, m_name(name)
{
	m_parentClock->AddChild(this);
}

//------------------------------------------------------------------------------------------------------------------
void Clock::Reset()
{

	m_totalSeconds = 0.;
	m_deltaSeconds = 0.;
	m_frameCount = 0;
	
	m_lastUpdateTimeInSeconds = GetCurrentTimeSeconds();

}

//------------------------------------------------------------------------------------------------------------------
bool Clock::IsPaused() const
{
	return m_isPaused;
}

//------------------------------------------------------------------------------------------------------------------
void Clock::Pause()
{
	m_isPaused = true;
}

//------------------------------------------------------------------------------------------------------------------
void Clock::Unpause()
{

	m_isPaused = false;

}

//------------------------------------------------------------------------------------------------------------------
void Clock::TogglePause()
{

	m_isPaused = !m_isPaused;

}

//------------------------------------------------------------------------------------------------------------------
void Clock::StepSingleFrame()
{
	m_stepSingleFrame = true;
	m_isPaused = false;
}

//------------------------------------------------------------------------------------------------------------------
void Clock::SetTimeScale(double timeScale)
{

	m_timeScale = timeScale;

}

//------------------------------------------------------------------------------------------------------------------
double Clock::GetTimeScale()
{
	return m_timeScale;
}

//------------------------------------------------------------------------------------------------------------------
float Clock::GetDeltaSeconds() const
{
 	return static_cast<float>(m_deltaSeconds);
}

//------------------------------------------------------------------------------------------------------------------
double Clock::GetTotalSeconds() const
{
	return m_totalSeconds;
}

//------------------------------------------------------------------------------------------------------------------
int Clock::GetFrameCount() const
{
	return m_frameCount;
}

//------------------------------------------------------------------------------------------------------------------
Clock& Clock::GetSystemClock()
{

	return *s_systemClock;

}

//------------------------------------------------------------------------------------------------------------------
void Clock::TickSystemClock()
{

	s_systemClock->Tick();

}

//------------------------------------------------------------------------------------------------------------------
void Clock::Tick()
{

	double deltaSeconds = GetCurrentTimeSeconds() - m_lastUpdateTimeInSeconds;
	double currentTime = GetCurrentTimeSeconds();

	while(deltaSeconds < m_minDeltaSeconds)
	{
		std::this_thread::yield();
		currentTime = GetCurrentTimeSeconds();
		deltaSeconds = currentTime - m_lastUpdateTimeInSeconds;
	}

	Advance(deltaSeconds);

}

//------------------------------------------------------------------------------------------------------------------
void Clock::Advance(double deltaTimeSeconds)
{

	if(m_isPaused)
	{
		m_deltaSeconds = 0.;
	}

	if(!m_isPaused)
	{
		m_deltaSeconds = deltaTimeSeconds * m_timeScale;
		m_totalSeconds += m_deltaSeconds;

		for(int clocks = 0; clocks < static_cast<int>(m_children.size()); ++clocks)
		{
			if(m_children[clocks])
			{
				m_children[clocks]->Advance(m_deltaSeconds);
			}
		}
	}
	
	if(m_stepSingleFrame)
	{
		m_isPaused = true;
		m_stepSingleFrame = false;
	}

	m_lastUpdateTimeInSeconds = GetCurrentTimeSeconds();

}

//------------------------------------------------------------------------------------------------------------------
void Clock::AddChild(Clock * childClock)
{

	m_children.push_back(childClock);

}

//------------------------------------------------------------------------------------------------------------------
void Clock::RemoveChild(Clock * childClock)
{

	for(int clocks = 0; clocks < static_cast<int>(m_children.size()); ++clocks)
	{
		if(m_children[clocks] == childClock)
		{
			m_children[clocks] = nullptr;
		}
	}

}
