#pragma once

#include <string>
#include "core-AppFlow\ExecutionContext.h"
#include <map>
#include <list>


///////////////////////////////////////////////////////////////////////////////

class Application;
class TimeController;

///////////////////////////////////////////////////////////////////////////////

/**
 * This class is the framework that runs the application the game consists of.
 *
 * This class is the one you will instantiate in your 'main' function,
 * passing the application it should run.
 */
class ApplicationManager : public ExecutionContext
{
protected:
   enum ProcessingCode
   {
      APC_SYSTEM,
      APC_APPLICATION,
      APC_EXIT
   };

private:
   typedef std::list<std::pair<std::string, int> > SignalsQueue;

   enum ApplicationState
   {
      AS_UNINITIALIZED,
      AS_BEING_HIBERNATED,
      AS_HIBERNATED,
      AS_BEING_DEHIBERNATED,
      AS_SCHEDULED,
      AS_RUNNING,
      AS_FINISHED
   };

   struct ApplicationNode
   {
      Application& app;
      ApplicationState state;
      std::map<int, std::string> connections;
      SignalsQueue signalsQueue;

      ApplicationNode(Application& _app)
         : app(_app), state(AS_UNINITIALIZED)
      {}
   };

private:
   typedef std::map<std::string, ApplicationNode> AppsMap;
   AppsMap m_apps;

   TimeController* m_globalTimeController;
   ApplicationData* m_blackboard;

public:
   ApplicationManager();
   virtual ~ApplicationManager();

   /**
    * Adds a new application to be ran
    */
   void addApplication(Application& app);

   /**
    * This method selects which application should be ran first
    */
   void setEntryApplication(const std::string& appName);

   /**
    * This method returns an instance of a TimeController
    * global to the entire program
    */
   TimeController& timeController();

   /**
    * This method returns a shared instance of a blackboard
    * used to share data between various applications
    */
   ApplicationData& blackboard();
   void setBlackboard(ApplicationData* blackboard);

   /**
    * This method is used to connect two applications together.
    * Whenever the 'originator' sends the 'signal', it looses focus,
    * which in turn is given to the 'targetApp'
    */
   void connect(const std::string& originator, 
                const std::string& targetApp, 
                int signal);

   /**
    * Execution step.
    *
    * @return - if false is returned, it means that
    *           there are no more apps to run and we can close the program
    */
   bool step();

   // ---------------------- ExecutionContext implementation ------------------
   void signal(const Application& app, int signalId);
   void signal(const Application& app, const std::string& receiverApp, int signalId);

protected:

   /**
    * Method called before each step. Should return 'APC_SYSTEM'
    * if system has the resources and needs to process something.
    *
    * 'APC_APPLICATION' value is a sign that we have our processing window
    * 'APC_EXIT' means that we should exist immediately
    */
   virtual ProcessingCode onStep() = 0;

   /**
    * This method returns the amount of time elapsed from its last call.
    * Implementation specific
    */
   virtual float getTimeElapsed() = 0;

private:
   void dispatchAppSignals(ApplicationNode& currNode);
   void processUserInput(float timeElapsed);
};

///////////////////////////////////////////////////////////////////////////////
