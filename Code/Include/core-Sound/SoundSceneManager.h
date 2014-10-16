/// @file   core-Sound\SoundSceneManager.h
/// @brief  a sound scene
#pragma once

#include "core\MemoryRouter.h"
#include "core\Array.h"
#include <windows.h>


///////////////////////////////////////////////////////////////////////////////

class Sound3D;
class SoundListener;

///////////////////////////////////////////////////////////////////////////////

/**
 * This scene manager manages the audio representation of a 3D scene.
 */
class SoundSceneManager
{
   DECLARE_ALLOCATOR( SoundSceneManager, AM_DEFAULT );

private:
   SoundListener* m_activeListener;

   Array<Sound3D*> m_activeSoundsArr;
   Array<Sound3D*> m_soundsArr;

public:
   SoundSceneManager();
   ~SoundSceneManager();

   /**
    * The method returns true if there's an active listener set for the scene.
    */
   bool hasActiveListener() const;

   /**
    * This method returns an instance of a sound listener currently activated
    * in this scene.
    *
    * @return                 currently active sound listener
    */
   SoundListener& getActiveListener();

   /**
    * Sets a new listener for the scene.
    *
    * @param                  new sound listener
    */
   void setListener(SoundListener* listener);

   /**
    * Adds a new sound emitter to the sound scene.
    *
    * @param sound            new sound emiter
    */
   void addEmitter(Sound3D& sound);

   /**
    * Removes an emitter from the sound scene.
    *
    * @param sound            sound to remove
    */
   void removeEmitter(Sound3D& sound);

   /**
    * The method checks which sounds that could be heard before
    * aren't now, and which of those that weren't are.
    *
    * @param soundsToDisable  upon method return this array will contain
    *                         sounds that should be disabled
    * @param soundsToEnable   upon method return this array will contain
    *                         sounds that should be enabled
    * @return                 an array of currently hearable sounds
    */
   Array<Sound3D*>& update(Array<Sound3D*>& soundsToDisable, 
                           Array<Sound3D*>& soundsToEnable);
};

///////////////////////////////////////////////////////////////////////////////
