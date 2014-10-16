#include "ext-StoryTeller\SCWaitForAMoment.h"
#include "ext-StoryTeller\StoryPlayer.h"
#include "core\TimeController.h"
#include "core\RuntimeData.h"

///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( SCWaitForAMoment );
   PARENT( StoryCondition );
   PROPERTY_EDIT( "Duration [s]", float, m_numSeconds )
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

SCWaitForAMoment::SCWaitForAMoment( float numSeconds )
   : m_numSeconds( numSeconds )
{
}

///////////////////////////////////////////////////////////////////////////////

void SCWaitForAMoment::createLayout( StoryPlayer& player ) const
{
   RuntimeDataBuffer& data = player.data();
   data.registerVar( m_timeElapsed );
}

///////////////////////////////////////////////////////////////////////////////

void SCWaitForAMoment::initialize( StoryPlayer& player ) const
{
   RuntimeDataBuffer& data = player.data();
   data[m_timeElapsed] = 0.0f;
}

///////////////////////////////////////////////////////////////////////////////

void SCWaitForAMoment::deinitialize( StoryPlayer& player ) const
{
}

///////////////////////////////////////////////////////////////////////////////

bool SCWaitForAMoment::evaluate( StoryPlayer& player ) const
{
   TimeController& timeController = TSingleton< TimeController >::getInstance();
   RuntimeDataBuffer& data = player.data();

   float timeElapsed = data[m_timeElapsed] + timeController.getTimeElapsed();
   data[m_timeElapsed] = timeElapsed;

   return timeElapsed >= m_numSeconds;
}

///////////////////////////////////////////////////////////////////////////////
