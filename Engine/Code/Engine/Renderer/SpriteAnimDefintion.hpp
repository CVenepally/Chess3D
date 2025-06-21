#pragma once

#include "Engine/Core/EngineCommon.hpp"

//------------------------------------------------------------------------------------------------------------------

class SpriteSheet;
class SpriteDefinition;


//------------------------------------------------------------------------------------------------------------------

enum SpriteAnimPlaybackType
{
	ONCE,
	LOOP, 
	PINGPONG
};

//------------------------------------------------------------------------------------------------------------------
class SpriteAnimDefinition
{
public:

 	SpriteAnimDefinition() = default;
	explicit SpriteAnimDefinition(SpriteSheet const& sheet, int startSpriteIndex, int endSpriteIndex, float framesPerSecond, SpriteAnimPlaybackType playBackType = SpriteAnimPlaybackType::LOOP);


	SpriteDefinition const& GetSpriteDefAtTime(float seconds) const;

	int GetStartIndex();
	int GetEndIndex();
	int GetNumberOfFramesForAnimation();
	float GetFPS();
	SpriteAnimPlaybackType GetPlaybackType() const;

	void SetStartIndex(int newStart);
	void SetEndIndex(int newEnd);
	void SetPlaybackType(SpriteAnimPlaybackType newPlaybackType);
	void SetFramesPerSecond(float newFramesPerSecond);

private:
	SpriteSheet const&			m_spriteSheet;
	int							m_startSpriteIndex	= -1;
	int							m_endSpriteIndex	= -1;
	float						m_framesPerSeconds	= 1.f;
	SpriteAnimPlaybackType		m_playBackType		= LOOP;
};