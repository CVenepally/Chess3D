#include "Engine/Math/Capsule2.hpp"
#include "Engine/Core/EngineCommon.hpp"


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Capsule2::Capsule2()
{

}




//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Capsule2::~Capsule2()
{


}



//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Capsule2::Capsule2(Vec2 start, Vec2 end, float radius)
	:	m_start(start)
	,	m_end(end)
	,	m_radius(radius)
{


}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Capsule2::Capsule2(LineSegment2 lineSegment, float radius)
	:	m_start(lineSegment.m_start)
	,	m_end(lineSegment.m_end)
	,	m_radius(radius)
{

	// MP1TODO: Add after all of MP1 A5 is finished

}



//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Capsule2::Translate(Vec2 translation)
{

	// MP1TODO: Add after all of MP1 A5 is finished
	UNUSED(translation)

}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Capsule2::SetCenter(Vec2 newCenter)
{

	// MP1TODO: Add after all of MP1 A5 is finished
	UNUSED(newCenter)

}



//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Capsule2::RotateAboutCenter(float rotationDeltaDegrees)
{

	// MP1TODO: Add after all of MP1 A5 is finished
	UNUSED(rotationDeltaDegrees);


}
