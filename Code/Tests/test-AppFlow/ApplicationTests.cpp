#include "core-TestFramework\TestFramework.h"
#include "ApplicationManagerMock.h"
#include "ApplicationMock.h"
#include "core-AppFlow\Application.h"
#include "core-TestFramework\MatrixWriter.h"
#include <windows.h>


///////////////////////////////////////////////////////////////////////////////

TEST(ApplicationManager, noEntryApplicationDefined)
{
   ApplicationManagerMock manager;
   ApplicationMock app("app");

   manager.addApplication(app);

   CPPUNIT_ASSERT_EQUAL(false, app.isInitialized());

   CPPUNIT_ASSERT_EQUAL(false, manager.step());
   CPPUNIT_ASSERT_EQUAL(false, app.isInitialized());

};

///////////////////////////////////////////////////////////////////////////////

TEST(ApplicationManager, runningSingleApplication)
{
   ApplicationManagerMock manager;
   ApplicationMock app("app");

   manager.addApplication(app);
   manager.setEntryApplication("app");

   CPPUNIT_ASSERT_EQUAL(false, app.isInitialized());

   CPPUNIT_ASSERT_EQUAL(true, manager.step());
   CPPUNIT_ASSERT_EQUAL(true, app.isInitialized());

   app.sendSignal(Application::ON_EXIT);
   CPPUNIT_ASSERT_EQUAL(true, app.isInitialized());

   CPPUNIT_ASSERT_EQUAL(false, manager.step());
   CPPUNIT_ASSERT_EQUAL(false, app.isInitialized());
};

///////////////////////////////////////////////////////////////////////////////

TEST(ApplicationManager, appReceivesUpdated)
{
   ApplicationManagerMock manager;
   ApplicationMock app("app");

   manager.addApplication(app);
   manager.setEntryApplication("app");

   CPPUNIT_ASSERT_EQUAL(0.f, app.getTimeElapsed());

   manager.step(); // initialization step
   CPPUNIT_ASSERT_EQUAL(0.f, app.getTimeElapsed());

   manager.step();
   CPPUNIT_ASSERT_EQUAL(1.f, app.getTimeElapsed());

   app.sendSignal(Application::ON_EXIT);

   manager.step();
   CPPUNIT_ASSERT_EQUAL(0.f, app.getTimeElapsed());
};

///////////////////////////////////////////////////////////////////////////////

TEST(ApplicationManager, connectingTwoApplications)
{
   ApplicationManagerMock manager;
   ApplicationMock menu("menu");
   ApplicationMock game("game");

   manager.addApplication(game);
   manager.addApplication(menu);

   manager.setEntryApplication("menu");
   manager.connect("menu", "game", 0);
   manager.connect("game", "menu", Application::ON_EXIT);

   // no app is initialized yet
   CPPUNIT_ASSERT_EQUAL(false, menu.isInitialized());
   CPPUNIT_ASSERT_EQUAL(false, game.isInitialized());

   // first step and menu gets initialized
   CPPUNIT_ASSERT_EQUAL(true, manager.step());
   CPPUNIT_ASSERT_EQUAL(true, menu.isInitialized());
   CPPUNIT_ASSERT_EQUAL(false, menu.isHibernated());
   CPPUNIT_ASSERT_EQUAL(0.f, menu.getTimeElapsed());
   CPPUNIT_ASSERT_EQUAL(false, game.isInitialized());
   CPPUNIT_ASSERT_EQUAL(false, game.isHibernated());
   CPPUNIT_ASSERT_EQUAL(0.f, game.getTimeElapsed());

   // menu starts getting updated, game still uninitialized
   CPPUNIT_ASSERT_EQUAL(true, manager.step());
   CPPUNIT_ASSERT_EQUAL(true, menu.isInitialized());
   CPPUNIT_ASSERT_EQUAL(false, menu.isHibernated());
   CPPUNIT_ASSERT_EQUAL(1.f, menu.getTimeElapsed());
   CPPUNIT_ASSERT_EQUAL(false, game.isInitialized());
   CPPUNIT_ASSERT_EQUAL(false, game.isHibernated());

   CPPUNIT_ASSERT_EQUAL(0.f, game.getTimeElapsed());

   // switching over to game:
   //   - menu doesn't get deinitialized, but it stops receiving updates (enters hibernation)
   //   - game gets initialized
   menu.sendSignal(0);
   CPPUNIT_ASSERT_EQUAL(true, manager.step());
   CPPUNIT_ASSERT_EQUAL(true, menu.isInitialized());
   CPPUNIT_ASSERT_EQUAL(true, menu.isHibernated());
   CPPUNIT_ASSERT_EQUAL(0.f, menu.getTimeElapsed());
   CPPUNIT_ASSERT_EQUAL(false, game.isInitialized());
   CPPUNIT_ASSERT_EQUAL(false, game.isHibernated());
   CPPUNIT_ASSERT_EQUAL(0.f, game.getTimeElapsed());

   CPPUNIT_ASSERT_EQUAL(true, manager.step());
   CPPUNIT_ASSERT_EQUAL(true, menu.isInitialized());
   CPPUNIT_ASSERT_EQUAL(true, menu.isHibernated());
   CPPUNIT_ASSERT_EQUAL(0.f, menu.getTimeElapsed());
   CPPUNIT_ASSERT_EQUAL(true, game.isInitialized());
   CPPUNIT_ASSERT_EQUAL(false, game.isHibernated());
   CPPUNIT_ASSERT_EQUAL(0.f, game.getTimeElapsed());

   // ...playing the game...
   CPPUNIT_ASSERT_EQUAL(true, manager.step());
   CPPUNIT_ASSERT_EQUAL(true, menu.isInitialized());
   CPPUNIT_ASSERT_EQUAL(true, menu.isHibernated());
   CPPUNIT_ASSERT_EQUAL(0.f, menu.getTimeElapsed());
   CPPUNIT_ASSERT_EQUAL(true, game.isInitialized());
   CPPUNIT_ASSERT_EQUAL(false, game.isHibernated());
   CPPUNIT_ASSERT_EQUAL(1.f, game.getTimeElapsed());

   // finished the game - game gets deinitialized...
   game.sendSignal(Application::ON_EXIT);
   CPPUNIT_ASSERT_EQUAL(true, manager.step());
   CPPUNIT_ASSERT_EQUAL(true, menu.isInitialized());
   CPPUNIT_ASSERT_EQUAL(false, menu.isHibernated());
   CPPUNIT_ASSERT_EQUAL(0.f, menu.getTimeElapsed());
   CPPUNIT_ASSERT_EQUAL(false, game.isInitialized());
   CPPUNIT_ASSERT_EQUAL(false, game.isHibernated());
   CPPUNIT_ASSERT_EQUAL(0.f, game.getTimeElapsed());

   // .. and menu gains focus with the next update
   CPPUNIT_ASSERT_EQUAL(true, manager.step());
   CPPUNIT_ASSERT_EQUAL(true, menu.isInitialized());
   CPPUNIT_ASSERT_EQUAL(false, menu.isHibernated());
   CPPUNIT_ASSERT_EQUAL(1.f, menu.getTimeElapsed());
   CPPUNIT_ASSERT_EQUAL(false, game.isInitialized());
   CPPUNIT_ASSERT_EQUAL(false, game.isHibernated());
   CPPUNIT_ASSERT_EQUAL(0.f, game.getTimeElapsed());

   // close the menu
   menu.sendSignal(Application::ON_EXIT);
   CPPUNIT_ASSERT_EQUAL(false, manager.step());
   CPPUNIT_ASSERT_EQUAL(false, menu.isInitialized());
   CPPUNIT_ASSERT_EQUAL(false, menu.isHibernated());
   CPPUNIT_ASSERT_EQUAL(false, game.isInitialized());
   CPPUNIT_ASSERT_EQUAL(false, game.isHibernated());
};

///////////////////////////////////////////////////////////////////////////////

TEST(ApplicationManager, sendingApplicationSwitchSignalFromInsideTheStepLoop)
{
   ApplicationManagerMock manager;
   ApplicationMock menu("menu");
   ApplicationMock game("game");

   manager.addApplication(game);
   manager.addApplication(menu);

   manager.setEntryApplication("menu");
   manager.connect("menu", "game", 0);
   manager.connect("game", "menu", Application::ON_EXIT);

   // no app is initialized yet
   CPPUNIT_ASSERT_EQUAL(false, menu.isInitialized());
   CPPUNIT_ASSERT_EQUAL(false, game.isInitialized());

   // first step and menu gets initialized
   CPPUNIT_ASSERT_EQUAL(true, manager.step());
   CPPUNIT_ASSERT_EQUAL(true, menu.isInitialized());
   CPPUNIT_ASSERT_EQUAL(0.f, menu.getTimeElapsed());
   CPPUNIT_ASSERT_EQUAL(false, game.isInitialized());
   CPPUNIT_ASSERT_EQUAL(0.f, game.getTimeElapsed());

   // menu starts getting updated, game still uninitialized
   CPPUNIT_ASSERT_EQUAL(true, manager.step());
   CPPUNIT_ASSERT_EQUAL(true, menu.isInitialized());
   CPPUNIT_ASSERT_EQUAL(1.f, menu.getTimeElapsed());
   CPPUNIT_ASSERT_EQUAL(false, game.isInitialized());
   CPPUNIT_ASSERT_EQUAL(0.f, game.getTimeElapsed());

   // switching over to game - but this time from inside of the 'step' method loop.
   // In this case the signal gets cached and will be executed in the next
   // 'step' method loop's iteration
   menu.sendSignalFromStep(0);
   CPPUNIT_ASSERT_EQUAL(true, manager.step());
   CPPUNIT_ASSERT_EQUAL(true, menu.isInitialized());
   CPPUNIT_ASSERT_EQUAL(1.f, menu.getTimeElapsed());
   CPPUNIT_ASSERT_EQUAL(false, game.isInitialized());
   CPPUNIT_ASSERT_EQUAL(0.f, game.getTimeElapsed());

   CPPUNIT_ASSERT_EQUAL(true, manager.step());
   CPPUNIT_ASSERT_EQUAL(true, menu.isInitialized());
   CPPUNIT_ASSERT_EQUAL(0.f, menu.getTimeElapsed());
   CPPUNIT_ASSERT_EQUAL(false, game.isInitialized());
   CPPUNIT_ASSERT_EQUAL(0.f, game.getTimeElapsed());

   CPPUNIT_ASSERT_EQUAL(true, manager.step());
   CPPUNIT_ASSERT_EQUAL(true, menu.isInitialized());
   CPPUNIT_ASSERT_EQUAL(0.f, menu.getTimeElapsed());
   CPPUNIT_ASSERT_EQUAL(true, game.isInitialized());
   CPPUNIT_ASSERT_EQUAL(0.f, game.getTimeElapsed());

   CPPUNIT_ASSERT_EQUAL(true, manager.step());
   CPPUNIT_ASSERT_EQUAL(true, menu.isInitialized());
   CPPUNIT_ASSERT_EQUAL(0.f, menu.getTimeElapsed());
   CPPUNIT_ASSERT_EQUAL(true, game.isInitialized());
   CPPUNIT_ASSERT_EQUAL(1.f, game.getTimeElapsed());

   // finished the game - we wish to deinitialize the game...
   game.sendSignalFromStep(Application::ON_EXIT);
   CPPUNIT_ASSERT_EQUAL(true, manager.step());
   CPPUNIT_ASSERT_EQUAL(true, menu.isInitialized());
   CPPUNIT_ASSERT_EQUAL(0.f, menu.getTimeElapsed());
   CPPUNIT_ASSERT_EQUAL(true, game.isInitialized());
   CPPUNIT_ASSERT_EQUAL(1.f, game.getTimeElapsed());

   // ... game gets deinitialized...
   CPPUNIT_ASSERT_EQUAL(true, manager.step());
   CPPUNIT_ASSERT_EQUAL(true, menu.isInitialized());
   CPPUNIT_ASSERT_EQUAL(0.f, menu.getTimeElapsed());
   CPPUNIT_ASSERT_EQUAL(false, game.isInitialized());
   CPPUNIT_ASSERT_EQUAL(0.f, game.getTimeElapsed());

   // .. and menu gains focus with the next update
   CPPUNIT_ASSERT_EQUAL(true, manager.step());
   CPPUNIT_ASSERT_EQUAL(true, menu.isInitialized());
   CPPUNIT_ASSERT_EQUAL(1.f, menu.getTimeElapsed());
   CPPUNIT_ASSERT_EQUAL(false, game.isInitialized());
   CPPUNIT_ASSERT_EQUAL(0.f, menu.getTimeElapsed());
}

///////////////////////////////////////////////////////////////////////////////

TEST(ApplicationManager, reinitializingFinishedApp)
{
   ApplicationManagerMock manager;
   ApplicationMock menu("menu");
   ApplicationMock game("game");

   manager.addApplication(menu);
   manager.addApplication(game);

   manager.setEntryApplication("menu");
   manager.connect("menu", "game", 0);
   manager.connect("game", "menu", Application::ON_EXIT);

   // no app is initialized yet
   CPPUNIT_ASSERT_EQUAL(false, menu.isInitialized());
   CPPUNIT_ASSERT_EQUAL(false, game.isInitialized());

   // first step and menu gets initialized
   CPPUNIT_ASSERT_EQUAL(true, manager.step());
   CPPUNIT_ASSERT_EQUAL(true, menu.isInitialized());
   CPPUNIT_ASSERT_EQUAL(0.f, menu.getTimeElapsed());
   CPPUNIT_ASSERT_EQUAL(false, game.isInitialized());
   CPPUNIT_ASSERT_EQUAL(0.f, game.getTimeElapsed());

   // menu starts getting updated, game still uninitialized
   CPPUNIT_ASSERT_EQUAL(true, manager.step());
   CPPUNIT_ASSERT_EQUAL(true, menu.isInitialized());
   CPPUNIT_ASSERT_EQUAL(1.f, menu.getTimeElapsed());
   CPPUNIT_ASSERT_EQUAL(false, game.isInitialized());
   CPPUNIT_ASSERT_EQUAL(0.f, game.getTimeElapsed());

   // switching over to game:
   //   - menu doesn't get deinitialized, but it stops receiving updates (enters hibernation)
   //   - game gets initialized
   menu.sendSignal(0);
   CPPUNIT_ASSERT_EQUAL(true, manager.step());
   CPPUNIT_ASSERT_EQUAL(true, menu.isInitialized());
   CPPUNIT_ASSERT_EQUAL(0.f, menu.getTimeElapsed());
   CPPUNIT_ASSERT_EQUAL(false, game.isInitialized());
   CPPUNIT_ASSERT_EQUAL(0.f, game.getTimeElapsed());

   CPPUNIT_ASSERT_EQUAL(true, manager.step());
   CPPUNIT_ASSERT_EQUAL(true, menu.isInitialized());
   CPPUNIT_ASSERT_EQUAL(0.f, menu.getTimeElapsed());
   CPPUNIT_ASSERT_EQUAL(true, game.isInitialized());
   CPPUNIT_ASSERT_EQUAL(0.f, game.getTimeElapsed());

   CPPUNIT_ASSERT_EQUAL(true, manager.step());
   CPPUNIT_ASSERT_EQUAL(true, menu.isInitialized());
   CPPUNIT_ASSERT_EQUAL(0.f, menu.getTimeElapsed());
   CPPUNIT_ASSERT_EQUAL(true, game.isInitialized());
   CPPUNIT_ASSERT_EQUAL(1.f, game.getTimeElapsed());

   // finished the game - game gets deinitialized...
   game.sendSignal(Application::ON_EXIT);
   CPPUNIT_ASSERT_EQUAL(true, manager.step());
   CPPUNIT_ASSERT_EQUAL(true, menu.isInitialized());
   CPPUNIT_ASSERT_EQUAL(0.f, menu.getTimeElapsed());
   CPPUNIT_ASSERT_EQUAL(false, game.isInitialized());
   CPPUNIT_ASSERT_EQUAL(0.f, game.getTimeElapsed());

   // .. and menu gains focus with the next update
   CPPUNIT_ASSERT_EQUAL(true, manager.step());
   CPPUNIT_ASSERT_EQUAL(true, menu.isInitialized());
   CPPUNIT_ASSERT_EQUAL(1.f, menu.getTimeElapsed());
   CPPUNIT_ASSERT_EQUAL(false, game.isInitialized());


   // switching over to game again...
   menu.sendSignal(0);
   CPPUNIT_ASSERT_EQUAL(true, manager.step());
   CPPUNIT_ASSERT_EQUAL(true, menu.isInitialized());
   CPPUNIT_ASSERT_EQUAL(0.f, menu.getTimeElapsed());
   CPPUNIT_ASSERT_EQUAL(false, game.isInitialized());
   CPPUNIT_ASSERT_EQUAL(0.f, game.getTimeElapsed());

   CPPUNIT_ASSERT_EQUAL(true, manager.step());
   CPPUNIT_ASSERT_EQUAL(true, menu.isInitialized());
   CPPUNIT_ASSERT_EQUAL(0.f, menu.getTimeElapsed());
   CPPUNIT_ASSERT_EQUAL(true, game.isInitialized());
   CPPUNIT_ASSERT_EQUAL(0.f, game.getTimeElapsed());

   // ...playing the game...
   CPPUNIT_ASSERT_EQUAL(true, manager.step());
   CPPUNIT_ASSERT_EQUAL(true, menu.isInitialized());
   CPPUNIT_ASSERT_EQUAL(0.f, menu.getTimeElapsed());
   CPPUNIT_ASSERT_EQUAL(true, game.isInitialized());
   CPPUNIT_ASSERT_EQUAL(1.f, game.getTimeElapsed());

   // ... and finishing the game once more
   game.sendSignal(Application::ON_EXIT);
   CPPUNIT_ASSERT_EQUAL(true, manager.step());
   CPPUNIT_ASSERT_EQUAL(true, menu.isInitialized());
   CPPUNIT_ASSERT_EQUAL(0.f, menu.getTimeElapsed());
   CPPUNIT_ASSERT_EQUAL(false, game.isInitialized());
   CPPUNIT_ASSERT_EQUAL(0.f, game.getTimeElapsed());

   // close the menu
   menu.sendSignal(Application::ON_EXIT);
   CPPUNIT_ASSERT_EQUAL(false, manager.step());
   CPPUNIT_ASSERT_EQUAL(false, menu.isInitialized());
   CPPUNIT_ASSERT_EQUAL(0.f, menu.getTimeElapsed());
   CPPUNIT_ASSERT_EQUAL(false, game.isInitialized());
}

///////////////////////////////////////////////////////////////////////////////

TEST(ApplicationManager, signalingOtherApplication)
{
   ApplicationManagerMock manager;
   ApplicationMock menu("menu");
   ApplicationMock game("game");

   manager.addApplication(menu);
   manager.addApplication(game);

   manager.setEntryApplication("menu");
   manager.connect("menu", "game", 0);
   manager.connect("game", "menu", Application::ON_EXIT);


   // first step and menu gets initialized
   CPPUNIT_ASSERT_EQUAL(true, manager.step());
   CPPUNIT_ASSERT_EQUAL(true, menu.isInitialized());
   CPPUNIT_ASSERT_EQUAL(0.f, menu.getTimeElapsed());
   CPPUNIT_ASSERT_EQUAL(false, game.isInitialized());
   CPPUNIT_ASSERT_EQUAL(0.f, game.getTimeElapsed());


   // switching over to game:
   //   - menu doesn't get deinitialized, but it stops receiving updates (enters hibernation)
   //   - game gets initialized
   menu.sendSignal(0);
   CPPUNIT_ASSERT_EQUAL(true, manager.step());
   CPPUNIT_ASSERT_EQUAL(true, menu.isInitialized());
   CPPUNIT_ASSERT_EQUAL(0.f, menu.getTimeElapsed());
   CPPUNIT_ASSERT_EQUAL(false, game.isInitialized());
   CPPUNIT_ASSERT_EQUAL(0.f, game.getTimeElapsed());

   CPPUNIT_ASSERT_EQUAL(true, manager.step());
   CPPUNIT_ASSERT_EQUAL(true, menu.isInitialized());
   CPPUNIT_ASSERT_EQUAL(0.f, menu.getTimeElapsed());
   CPPUNIT_ASSERT_EQUAL(true, game.isInitialized());
   CPPUNIT_ASSERT_EQUAL(0.f, game.getTimeElapsed());

   // sending signal from a hibernated app to the active app
   menu.sendSignal("game", 5);
   CPPUNIT_ASSERT_EQUAL(true, manager.step());
   CPPUNIT_ASSERT_EQUAL(-1, menu.getReceivedSignal());
   CPPUNIT_ASSERT_EQUAL(5, game.getReceivedSignal());

   // sending signal from the active app to a hibernated app
   game.sendSignal("menu", 7);
   CPPUNIT_ASSERT_EQUAL(true, manager.step());
   CPPUNIT_ASSERT_EQUAL(7, menu.getReceivedSignal());
   CPPUNIT_ASSERT_EQUAL(-1, game.getReceivedSignal());
}

///////////////////////////////////////////////////////////////////////////////
