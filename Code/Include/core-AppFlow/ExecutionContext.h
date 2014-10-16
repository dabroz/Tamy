#pragma once


///////////////////////////////////////////////////////////////////////////////

class Application;
struct Point;
class ApplicationData;
struct D3DXVECTOR2;

///////////////////////////////////////////////////////////////////////////////

/**
 * This interface provides the means for an Application instance
 * to communicate back with the ApplicationManager, without exposing
 * AppManager's methods (such as deinitialize and initialize) to the app.
 */
class ExecutionContext
{
public:
   virtual ~ExecutionContext() {}

   virtual ApplicationData& blackboard() = 0;

   /**
    * This way we can send signals to the app manager
    * to inform it that we want to switch over to a different app,
    * or that it finished whatever it was doing and can be closed
    */
   virtual void signal(const Application& app, int signalId) = 0;

   /**
    * This way we can send signals to other applications
    * to tell them to do something. The receiver app needs
    * to be hibernated or running in order to receive the signal 
    * (an app can also send signals to itself)
    */
   virtual void signal(const Application& app, 
                       const std::string& receiverApp, 
                       int signalId) = 0;
};

///////////////////////////////////////////////////////////////////////////////
