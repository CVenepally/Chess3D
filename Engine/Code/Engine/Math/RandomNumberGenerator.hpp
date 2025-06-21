#pragma once

struct FloatRange;


class RandomNumberGenerator
{

public:

	int RollIntLessThan(int maxIntNotInclusive) const;
	int RollIntInRangeOf(int minIntInclusive, int maxIntInclusive) const;

	float RollFloatZeroToOne() const;
	float RollFloatInRangeOf(float minFloatInclusive, float maxFloatInclusive) const;
	float RollFloatInRangeOf(FloatRange range) const;

	float RollRandomAngle() const;
	float RollNewAngleInRange(float minAngle, float maxAngle) const;
	float RollNewAngleInRange(FloatRange range) const;

};