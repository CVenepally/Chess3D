#pragma once


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------

struct FloatRange
{
public:

	float m_min;
	float m_max;

	static const FloatRange ZERO;
	static const FloatRange ONE;
	static const FloatRange ZERO_TO_ONE;

public:

	FloatRange();
	~FloatRange();

	explicit FloatRange(float min, float max);

	bool		IsOnRange(float floatToCheck);
	bool		IsOverlappingWith(FloatRange flaotRange);

	void		SetFromText(char const* floatRangeText);

	FloatRange	GetOverlapRange(FloatRange overlappingFloatRange);

	float		GetRandomFloat() const;

	bool		operator==(FloatRange const& compare) const;
	bool		operator!=(FloatRange const& compare) const;
	void		operator=(FloatRange const& copyFrom);


};