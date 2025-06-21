#pragma once
//------------------------------------------------------------------------------------------------------------------
class Clock;

//------------------------------------------------------------------------------------------------------------------
class Timer
{
public:

	Timer();
	explicit Timer(double period, const Clock* clock = nullptr);
	~Timer();

	void Start();
	void Stop();
	void Restart();

	double GetElapsedTime() const;
	float GetElapsedFraction() const;

	bool IsStopped() const;
	bool HasPeriodElapsed() const;

	bool DecrementPeriodIfElapsed();

	const Clock* m_clock = nullptr;
	double m_startTime = -1.;
	double m_period = 0.;

};