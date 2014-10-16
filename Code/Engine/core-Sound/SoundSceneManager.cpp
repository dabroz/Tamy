#include "core-Sound\SoundSceneManager.h"
#include "core-MVC\Entity.h"
#include "core-Sound\SoundListener.h"
#include "core-Sound\Sound3D.h"
#include "core\Vector.h"


///////////////////////////////////////////////////////////////////////////////

SoundSceneManager::SoundSceneManager()
: m_activeListener(NULL)
{
}

///////////////////////////////////////////////////////////////////////////////

SoundSceneManager::~SoundSceneManager()
{
   delete m_activeListener;
   m_activeListener = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void SoundSceneManager::setListener(SoundListener* listener)
{
   delete m_activeListener;
   m_activeListener = listener;
}

///////////////////////////////////////////////////////////////////////////////

bool SoundSceneManager::hasActiveListener() const
{
   return (m_activeListener != NULL);
}

///////////////////////////////////////////////////////////////////////////////

SoundListener& SoundSceneManager::getActiveListener()
{
   if (m_activeListener == NULL)
   {
      ASSERT_MSG( false, "No active listener set" );
   }
   return *m_activeListener;
}

///////////////////////////////////////////////////////////////////////////////

void SoundSceneManager::addEmitter(Sound3D& sound)
{
   m_soundsArr.push_back(&sound);
}

///////////////////////////////////////////////////////////////////////////////

void SoundSceneManager::removeEmitter(Sound3D& sound)
{
   unsigned int idx = m_soundsArr.find(&sound);
   if (idx != EOA)
   {
      m_soundsArr.remove(idx);
   }
}

///////////////////////////////////////////////////////////////////////////////

Array< Sound3D* >& SoundSceneManager::update( Array< Sound3D* >& soundsToDisable, Array< Sound3D* >& soundsToEnable )
{
   if ( m_activeListener == NULL ) 
   {
      return m_soundsArr;
   }

   m_activeSoundsArr.clear();

   const Matrix& listenerMtx = m_activeListener->getGlobalMtx();
   Vector listenerPos = listenerMtx.position();
   Matrix soundMtx;
   Vector distVec;

   Sound3D* sound = NULL;
   DWORD soundsCount = m_soundsArr.size();
   for (DWORD i = 0; i < soundsCount; ++i)
   {
      soundMtx = m_soundsArr[i]->getGlobalMtx();
      distVec.setSub( listenerPos, soundMtx.position() );

      sound = m_soundsArr[i];

      bool hearable = ( distVec.lengthSq() <= m_soundsArr[i]->getHearingRadiusSq() );
      bool active = sound->hasChannelAssigned();

      if ( ( hearable == true ) && ( active == false ) )
      {
         soundsToEnable.push_back(sound);
      }
      else if ( ( hearable == false ) && ( active == true ) )
      {
         soundsToDisable.push_back( sound );
      }

      if ( hearable )
      {
         m_activeSoundsArr.push_back( sound );
      }
   }

   return m_activeSoundsArr;
}

///////////////////////////////////////////////////////////////////////////////
