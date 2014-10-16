#include "core-TestFramework\TestFramework.h"
#include "core-TestFramework\MatrixWriter.h"
#include "core-Sound\SoundSceneManager.h"
#include "Sound3DMock.h"
#include "SoundMock.h"
#include "SoundListenerMock.h"
#include "SoundDeviceMock.h"
#include "core\Array.h"
#include "core\Matrix.h"
#include "core\Vector.h"


//////////////////////////////////////////////////////////////////////////////

TEST(SoundSceneManager, soundsAreHeardOnlyToCertainDistance)
{
   SoundDeviceMock soundDevice;
   SoundSceneManager soundScene;

   SoundMock barking;
   float dogSoundHearingRadius = 5;
   float listenerHearingRadius = 1;
   Sound3DMock dogSound("dogSound", barking, dogSoundHearingRadius);
   soundScene.addEmitter(dogSound);
   SoundListenerMock* listener = new SoundListenerMock();
   soundScene.setListener(listener);

   Array<Sound3D*> soundsToEnable;
   Array<Sound3D*> soundsToDisable;
   Array<Sound3D*>* activeSounds = NULL;

   activeSounds = &(soundScene.update(soundsToDisable, soundsToEnable));
   CPPUNIT_ASSERT_EQUAL(dogSound.getGlobalMtx(), listener->getGlobalMtx());
   CPPUNIT_ASSERT_EQUAL((unsigned int)1, soundsToEnable.size());
   CPPUNIT_ASSERT_EQUAL((unsigned int)0, soundsToDisable.size());

   // assign a phony channel to the sound that's hearable
   soundsToEnable[0]->assignChannel(soundDevice);

   // no change in listener's position - no change in sounds hearability either
   soundsToEnable.clear();
   soundsToDisable.clear();
   activeSounds = &(soundScene.update(soundsToDisable, soundsToEnable));
   CPPUNIT_ASSERT_EQUAL(dogSound.getGlobalMtx(), listener->getGlobalMtx());
   CPPUNIT_ASSERT_EQUAL((unsigned int)0, soundsToEnable.size());
   CPPUNIT_ASSERT_EQUAL((unsigned int)0, soundsToDisable.size());

   // we're completely outside the sound hearing are - no sound is heard
   Vector pos;
   soundsToEnable.clear();
   soundsToDisable.clear();
   pos.set( dogSoundHearingRadius + 1, 0, 0 );
   listener->accessLocalMtx().setTranslation( pos );
   listener->updateTransforms();

   activeSounds = &(soundScene.update(soundsToDisable, soundsToEnable));
   CPPUNIT_ASSERT_EQUAL((unsigned int)0, soundsToEnable.size());
   CPPUNIT_ASSERT_EQUAL((unsigned int)1, soundsToDisable.size());

   // deassign the channel from the sound that can no longer be heard
   soundsToDisable[0]->deassignChannel(soundDevice);

   soundsToEnable.clear();
   soundsToDisable.clear();
   pos.set( dogSoundHearingRadius - 1, 0, 0 );
   listener->accessLocalMtx().setTranslation( pos );
   listener->updateTransforms();

   activeSounds = &(soundScene.update(soundsToDisable, soundsToEnable));
   CPPUNIT_ASSERT_EQUAL((unsigned int)1, soundsToEnable.size());
   CPPUNIT_ASSERT_EQUAL((unsigned int)0, soundsToDisable.size());
}

//////////////////////////////////////////////////////////////////////////////
