#pragma once

struct Vec2;


struct IntVec2
{

	static const IntVec2 ZERO;
	static const IntVec2 ONE;
	static const IntVec2 RIGHT;
	static const IntVec2 LEFT;
	static const IntVec2 UP;
	static const IntVec2 DOWN;

public:

	int x = 0;
	int y = 0;


public:

	IntVec2() {}
	~IntVec2() {}

	IntVec2(const IntVec2& copyFrom);

	explicit IntVec2(int initialX, int initialY);
	explicit IntVec2(int initialXY);

	float GetLength() const;

	int GetTaxicabLength() const;
	int GetLengthSquared() const;

	float GetOrientationDegrees() const;
	float GetOrientationRadians() const;

	IntVec2 const GetRotated90Degrees() const;
	IntVec2 const GetRotatedMinus90Degrees() const;

	Vec2 const GetAsVec2();

	void SetFromText(char const* textIntVec2);

	void Rotate90Degrees();
	void RotateMinus90Degrees();

	IntVec2 const	operator+(IntVec2 const& vecToAdd) const;
	IntVec2 const	operator-(IntVec2 const& vecToSubtract) const;
	
	bool operator==(IntVec2 const& vecToCompare) const;
	bool operator!=(IntVec2 const& vecToCompare) const;
	void operator=(const IntVec2& copyFrom);



};