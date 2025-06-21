#include "Engine/Renderer/SpriteAnimDefintion.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"

//------------------------------------------------------------------------------------------------------------------
SpriteAnimDefinition::SpriteAnimDefinition(SpriteSheet const& sheet, int startSpriteIndex, int endSpriteIndex, float framesPerSecond, SpriteAnimPlaybackType playBackType)
	: m_spriteSheet(sheet)
	, m_startSpriteIndex(startSpriteIndex)
	, m_endSpriteIndex(endSpriteIndex)
	, m_framesPerSeconds(framesPerSecond)
	, m_playBackType(playBackType)
{


}


//------------------------------------------------------------------------------------------------------------------
SpriteDefinition const& SpriteAnimDefinition::GetSpriteDefAtTime(float seconds) const
{

	int frameNumber = RoundDownToInt(seconds * m_framesPerSeconds);

	int spriteIndex;

	switch(m_playBackType)
	{
		case ONCE:
		{
			spriteIndex = m_startSpriteIndex + frameNumber;

			spriteIndex = static_cast<int>(GetClamped(static_cast<float>(spriteIndex), static_cast<float>(m_startSpriteIndex), static_cast<float>(m_endSpriteIndex)));
			
			break;

		}

		case LOOP:
		{
			int numFrames = (m_endSpriteIndex - m_startSpriteIndex) + 1;

			spriteIndex = (m_startSpriteIndex + (frameNumber % numFrames));

			break;
		}

		case PINGPONG:
		{

			// calculate sequence index
			int numSpritesInAnimation = (m_endSpriteIndex - m_startSpriteIndex) + 1;

			int numSequences = numSpritesInAnimation + (numSpritesInAnimation - 2);

			int sequenceIndex = frameNumber % numSequences;


			// calculate sprite offset
			if(sequenceIndex <= numSequences * 0.5f)
			{
				int spriteOffset = sequenceIndex;
				spriteIndex = m_startSpriteIndex + spriteOffset;
			}
			else
			{
				int spriteOffset = numSequences - sequenceIndex;
				spriteIndex = m_startSpriteIndex + spriteOffset;
			}

			break;

		}

		default:
			ERROR_AND_DIE("Invalid animation mode");
	}

	return m_spriteSheet.GetSpriteDef(spriteIndex);

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
int SpriteAnimDefinition::GetStartIndex()
{
	return m_startSpriteIndex;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
int SpriteAnimDefinition::GetEndIndex()
{
	return m_endSpriteIndex;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
int SpriteAnimDefinition::GetNumberOfFramesForAnimation()
{
	return m_endSpriteIndex - m_startSpriteIndex + 1;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
float SpriteAnimDefinition::GetFPS()
{
	return m_framesPerSeconds;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
SpriteAnimPlaybackType SpriteAnimDefinition::GetPlaybackType() const
{
	return m_playBackType;
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SpriteAnimDefinition::SetStartIndex(int newStart)
{
	m_startSpriteIndex = newStart;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SpriteAnimDefinition::SetEndIndex(int newEnd)
{
	m_endSpriteIndex = newEnd;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SpriteAnimDefinition::SetPlaybackType(SpriteAnimPlaybackType newPlaybackType)
{
	m_playBackType = newPlaybackType;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SpriteAnimDefinition::SetFramesPerSecond(float newFramesPerSecond)
{
	m_framesPerSeconds = newFramesPerSecond;
}
