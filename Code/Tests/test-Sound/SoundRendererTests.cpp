#include "core-TestFramework\TestFramework.h"
#include "core-TestFramework\MatrixWriter.h"
#include "core-Sound\SoundRenderer.h"
#include "core-Sound\Sound.h"
#include "SoundDeviceMock.h"
#include "SoundMock.h"
#include "Sound3DMock.h"
#include "SoundListenerMock.h"
#include "core-Sound\SoundSceneManager.h"
#include "core\Vector.h"
#include "core\Matrix.h"


//////////////////////////////////////////////////////////////////////////////

TEST(SoundRenderer, renderingSound)
{
   SoundDeviceMock soundDevice;
   SoundRenderer soundRenderer(soundDevice);
   
   SoundSceneManager soundScene;
   soundRenderer.addSoundScene(soundScene);

   SoundMock someSound;
   Sound3DMock barkingSound("barkingSound", someSound, 100); 
   soundScene.addEmitter(barkingSound);
   soundScene.setListener(new SoundListenerMock());

   CPPUNIT_ASSERT_EQUAL((unsigned int)0, soundDevice.getActiveChannelsCount());

   soundRenderer.update(0);

   CPPUNIT_ASSERT_EQUAL((unsigned int)1, soundDevice.getActiveChannelsCount());
   CPPUNIT_ASSERT_EQUAL(true, soundDevice.getChannel(0).isPlaying());
}

//////////////////////////////////////////////////////////////////////////////

TEST(SoundRenderer, soundPlayedContinuouslyAcrossRenderings)
{
   SoundDeviceMock soundDevice;
   SoundRenderer soundRenderer(soundDevice);

   SoundSceneManager soundScene;
   soundRenderer.addSoundScene(soundScene);

   SoundMock someSound;
   Sound3DMock barkingSound("barkingSound", someSound, 100);
   soundScene.addEmitter(barkingSound);
   soundScene.setListener(new SoundListenerMock());

   CPPUNIT_ASSERT_EQUAL((unsigned int)0, soundDevice.getActiveChannelsCount());

   soundRenderer.update(0);
   CPPUNIT_ASSERT_EQUAL((unsigned int)1, soundDevice.getActiveChannelsCount());
   SoundChannel* channel1 = &(soundDevice.getChannel(0));

   soundRenderer.update(0);
   CPPUNIT_ASSERT_EQUAL((unsigned int)1, soundDevice.getActiveChannelsCount());
   SoundChannel* channel2 = &(soundDevice.getChannel(0));

   CPPUNIT_ASSERT(channel1 == channel2);
}

//////////////////////////////////////////////////////////////////////////////

TEST(SoundRenderer, channelsAreReleasedIfSoundCantBeHeardAnymore)
{
   SoundDeviceMock soundDevice;
   SoundRenderer soundRenderer(soundDevice);

   SoundSceneManager soundScene;
   soundRenderer.addSoundScene(soundScene);

   SoundMock bark("bark");
   SoundMock step("step");
   Sound3DMock barkingSound("barkingSound", bark, 1);
   Sound3DMock walkingSound("walkingSound", step, 1);

   Vector testPos;
   testPos.set( -10, 0, 0 );
   barkingSound.accessLocalMtx().setTranslation( testPos );
   barkingSound.updateTransforms();

   testPos.set( 10, 0, 0 );
   walkingSound.accessLocalMtx().setTranslation( testPos );
   walkingSound.updateTransforms();

   SoundListenerMock* listener = new SoundListenerMock();
   soundScene.setListener(listener);

   soundScene.addEmitter(walkingSound);
   soundScene.addEmitter(barkingSound);

   CPPUNIT_ASSERT_EQUAL((unsigned int)0, soundDevice.getActiveChannelsCount());

   // first - the listener is near the man - he can hear his walking,
   // but he can't hear the dog barking
   testPos.set( 10, 0, 0 );
   listener->accessLocalMtx().setTranslation( testPos );
   listener->updateTransforms();
   soundRenderer.update(0);
   CPPUNIT_ASSERT_EQUAL((unsigned int)1, soundDevice.getActiveChannelsCount());
   CPPUNIT_ASSERT_EQUAL(std::string("step"), soundDevice.getChannel(0).getSound().getName());

   // next moment - the listener moves close to the dog - he can hear it barking,
   // but he can't hear the man walking
   testPos.set( -10, 0, 0 );
   listener->accessLocalMtx().setTranslation( testPos );
   listener->updateTransforms();
   soundRenderer.update(0);
   CPPUNIT_ASSERT_EQUAL((unsigned int)1, soundDevice.getActiveChannelsCount());
   CPPUNIT_ASSERT_EQUAL(std::string("bark"), soundDevice.getChannel(0).getSound().getName());
}

//////////////////////////////////////////////////////////////////////////////
