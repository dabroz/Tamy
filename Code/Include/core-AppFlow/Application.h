#pragma once

#include "core\Assert.h"
#include <string>
#include "core\TimeController.h"
#include "core-AppFlow\ExecutionContext.h"


///////////////////////////////////////////////////////////////////////////////

class UserInputController;

///////////////////////////////////////////////////////////////////////////////

/**
 * An implementation of this class represents a single 'app'
 * that can be run by the flow framework.
 * An application can be thought of as a standalone program that 
 * does something, receives an input and uses system resources
 *
 */
class Application
{
public:
   enum Signal
   {
      ON_EXIT = 0x7fff
   };

private:
   std::string             m_name;
   ExecutionContext*       m_context;
   TimeController*         m_timeController;

public:
   virtual ~Application();

   const std::string& getName() const { return m_name; }
   bool operator==(const Application& rhs) const {return m_name == rhs.m_name;}
   bool operator!=(const Application& rhs) const {return !(*this == rhs);}
   bool operator<(const Application& rhs) const {return m_name < rhs.m_name;}

   /**
    * Method used by the ApplicationManager to inform the application
    * what's managing it and whom should it communicate its state to
    */
   void setContext(ExecutionContext& context) {m_context = &context;}

   /**
    * This method will be the first one to be called when
    * the application is scheduled to be ran
    */
   virtual void initialize() = 0;

   /**
    * This method is an application destructor - the class can't
    * do anything after this call is made - and it's bound to clean up after
    * itself in it
    */
   virtual void deinitialize() = 0;

   /**
    * This method will be called as soon as an application 
    * enters the hibernation mode.
    * In this mode, the app can keep all its resources, however it is supposed
    * to release common resources, such as the renderer etc.
    */
   virtual void hibernate() = 0;

   /**
   * This method will be called as soon as an application 
   * exits the hibernation mode.
   * When this call happens, the application is free to reclaim
   * common resources, such as the renderer etc.
   */
   virtual void dehibernate() = 0;

   /**
    * This is the method that updates the internal state of the application
    */
   void update(float timeElapsed);

   /**
    * When this method is called, it means that some other active application
    * is sending us a signal.
    */
   virtual void notify(const std::string& senderApp, int signalCode) = 0;

protected:
   Application(const std::string& name);

   ExecutionContext& context() 
   {
      if (m_context == NULL)
      {
         ASSERT_MSG( false, "Application not embedded in an execution context" );
      }

      return *m_context;
   }

   const ExecutionContext& context() const
   {
      if (m_context == NULL)
      {
         ASSERT_MSG( false, "Application not embedded in an execution context" );
      }

      return *m_context;
   }

   /**
    * If you want the application to run any code, you need to add it to
    * the time controller of that application. 
    * This method gives you the access to the controller
    */
   TimeController& timeController() {return *m_timeController;}
};

///////////////////////////////////////////////////////////////////////////////
