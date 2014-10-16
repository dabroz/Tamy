/// @file   core-Sound/SoundRenderer.h
/// @brief  renders a sound scene into existence, playing in on a designated sound device
#pragma once

#include "core\MemoryRouter.h"
#include "core\Array.h"


///////////////////////////////////////////////////////////////////////////////

class SoundSceneManager;
class SoundDevice;
class Sound3D;

///////////////////////////////////////////////////////////////////////////////

/**
 * Just as the graphical renderer converts the scene nodes into 
 * a visual representation, the SoundRenderer converts the applicable
 * scene nodes into an audio representation
 */
class SoundRenderer
{
   DECLARE_ALLOCATOR( SoundRenderer, AM_DEFAULT );

private:
   SoundDevice& m_soundDevice;
   Array<SoundSceneManager*> m_soundScenes;
   Array<Sound3D*> m_soundsToDisable;
   Array<Sound3D*> m_soundsToEnable;

public:
   SoundRenderer(SoundDevice& soundDevice);
   ~SoundRenderer();

   /**
    * This method adds a new sound scene to the 3D sound rendering pipeline
    */
   void addSoundScene(SoundSceneManager& scene);

   /**
    * This method removes a sound scene from the 3D sound rendering pipeline
    */
   void removeSoundScene(SoundSceneManager& scene);

   /**
    * The method renders the contents of an audio scene on the sound device
    */
   void update(float timeElapsed);
};

///////////////////////////////////////////////////////////////////////////////
