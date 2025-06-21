#include "RandomNumberGenerator.hpp"
#include "Engine/Math/FloatRange.hpp"

#include <stdlib.h>




int RandomNumberGenerator::RollIntLessThan(int maxIntNotInclusive) const
{

    int randInt = rand() % maxIntNotInclusive;

    return randInt;

}



int RandomNumberGenerator::RollIntInRangeOf(int minIntInclusive, int maxIntInclusive) const
{

    int randInt = minIntInclusive + (rand() % ((maxIntInclusive - minIntInclusive) + 1));

    return randInt;

}



float RandomNumberGenerator::RollFloatZeroToOne() const
{
    float randFloat = float(rand()) / float(RAND_MAX);

    return randFloat;
}



float RandomNumberGenerator::RollFloatInRangeOf(float minFloatInclusive, float maxFloatInclusive) const
{

    float randFloat = minFloatInclusive + ((float(rand()) / float(RAND_MAX)) * (maxFloatInclusive - minFloatInclusive));

    return randFloat;

}

float RandomNumberGenerator::RollFloatInRangeOf(FloatRange range) const
{

    return RollFloatInRangeOf(range.m_min, range.m_max);

}

float RandomNumberGenerator::RollRandomAngle() const
{
    return RollFloatInRangeOf(0.f, 360.f);
}

float RandomNumberGenerator::RollNewAngleInRange(float minAngle, float maxAngle) const
{
    return RollFloatInRangeOf(minAngle, maxAngle);
}

float RandomNumberGenerator::RollNewAngleInRange(FloatRange range) const
{
    return RollFloatInRangeOf(range.m_min, range.m_max);
}
