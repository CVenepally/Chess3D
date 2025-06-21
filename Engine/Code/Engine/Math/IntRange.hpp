#pragma once

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------

struct IntRange
{
public:

	int m_min;
	int m_max;

	static const IntRange ZERO;
	static const IntRange ONE;
	static const IntRange ZERO_TO_ONE;


public:

	IntRange();
	~IntRange();

	explicit IntRange(int min, int max);

	bool		IsOnRange(int intToCheck);
	bool		IsOverlappingWith(IntRange flaotRange);

	int			GetRandomInt() const;

	int			GetRange();

	void		SetFromText(char const* intRangeText);
	bool		operator==(IntRange const& compare) const;
	bool		operator!=(IntRange const& compare) const;
	void		operator=(IntRange const& copyFrom);


};