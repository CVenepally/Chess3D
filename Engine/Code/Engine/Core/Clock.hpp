#pragma once
#include <vector>
#include <string>
//------------------------------------------------------------------------------------------------------------------
class Clock
{
public:

	Clock(std::string const& name = "System", int fpsCap = 240);

	explicit Clock(Clock& parent, std::string const& name = "Game");

	~Clock();
	Clock(const Clock& copy) = delete;


	void	Reset();

	bool	IsPaused() const;
	void	Pause();
	void	Unpause();
	void	TogglePause();

	void	StepSingleFrame();

	void	SetTimeScale(double timeScale);
	double	GetTimeScale();

	float	GetDeltaSeconds() const;
	double	GetTotalSeconds() const;
	int		GetFrameCount() const;

public:

	static Clock&	GetSystemClock();
	static void		TickSystemClock();

protected:
	void Tick();
	void Advance(double deltaTimeSeconds);
	void AddChild(Clock* childClock);	
	void RemoveChild(Clock* childClock);

protected:

	std::string m_name;

	Clock* m_parentClock = nullptr;
	std::vector<Clock*> m_children;

	double	m_lastUpdateTimeInSeconds	= 0.;
	double	m_totalSeconds				= 0.;
	double	m_deltaSeconds				= 0.;
	double  m_minDeltaSeconds			= 0.;
	int		m_frameCount				= 0;

	double	m_timeScale					= 1.;
	
	bool	m_isPaused					= false;
	bool	m_stepSingleFrame			= false;
	float   m_maxDeltaSeconds			= 0.1f;
};