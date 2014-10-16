#include "core-Sound\Sound3D.h"
#include "core-Sound\Sound.h"
#include "core-Sound\SoundDevice.h"
#include "core-Sound\SoundChannel.h"
#include "core-Sound\SoundListener.h"
#include "core\Assert.h"
#include <math.h>


///////////////////////////////////////////////////////////////////////////////

Sound3D::Sound3D(const char* name, Sound& sound, float soundHearingRadius)
   : Entity( name )
   , m_channel( NULL )
   , m_sound( sound )
   , m_looped( true )
{
   m_soundHearingRadius.setFromFloat( soundHearingRadius );
   m_soundHearingRadiusSq.setMul( m_soundHearingRadius, m_soundHearingRadius );
}

///////////////////////////////////////////////////////////////////////////////

Sound3D::~Sound3D()
{
}

///////////////////////////////////////////////////////////////////////////////

void Sound3D::assignChannel(SoundDevice& device)
{
   if (m_channel != NULL)
   {
      ASSERT_MSG( false, "The 3d sound already has a channel assigned");
      return;
   }

   m_channel = &(device.activateSound(m_sound));
   onChannelAssigned(*m_channel);
   m_channel->setLooped(m_looped);
   m_channel->play();
}

///////////////////////////////////////////////////////////////////////////////

void Sound3D::deassignChannel(SoundDevice& device)
{
   if (m_channel == NULL)
   {
      ASSERT_MSG( false, "No channel assigned");
      return;
   }
   device.deactivateSound( m_channel );
   m_channel = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void Sound3D::setLooped(bool looped)
{
    m_looped = looped;
}

///////////////////////////////////////////////////////////////////////////////

bool Sound3D::isLooped() const
{
    return m_looped;
}

///////////////////////////////////////////////////////////////////////////////
