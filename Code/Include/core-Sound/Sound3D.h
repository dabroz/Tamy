/// @file   core-Sound/Sound3D.h
/// @brief  a scene node that emits sounds
#pragma once

#include "core-MVC\Entity.h"


///////////////////////////////////////////////////////////////////////////////

class Sound;
class SoundChannel;
class SoundDevice;
class SoundListener;

///////////////////////////////////////////////////////////////////////////////

/**
 * This class is an instance of a sound in the 3D scene.
 */
class Sound3D : public Entity
{
   DECLARE_ALLOCATOR( Sound3D, AM_DEFAULT );

protected:
   FastFloat            m_soundHearingRadius;
   SoundChannel*        m_channel;

private:
   Sound&               m_sound;
   FastFloat            m_soundHearingRadiusSq;
   bool                 m_looped;

public:
   Sound3D( const char* name, Sound& sound, float soundHearingRadius );
   virtual ~Sound3D();

   virtual void update(SoundListener& listener) = 0;

   Sound& getSound() {return m_sound;}

   void assignChannel(SoundDevice& device);
   void deassignChannel(SoundDevice& device);
   bool hasChannelAssigned() const {return m_channel != NULL;}

   inline const FastFloat& getHearingRadius() const { return m_soundHearingRadius; }
   inline const FastFloat& getHearingRadiusSq() const { return m_soundHearingRadiusSq; }

   /**
    * This method allows to define whether the played sound should 
    * be looped or not.
    *
    * @param looped     'true' indicates that we want to play the sound
    *                   int a looped manner.
    */
   void setLooped(bool looped);

   /**
    * The method allows to check if the sound is played in the looped mode.
    *
    * @return           'true' if the sound is played looped, 'false' otherwise
    */
   bool isLooped() const;

protected:
   virtual void onChannelAssigned(SoundChannel& channel) = 0;

   // <sound.todo> should this be an entity or a component?
   // <sound.todo> SceneNode cloning
};

///////////////////////////////////////////////////////////////////////////////
