/// @file   win-AppFlow\IWindowMessagesProcessor.h
/// @brief  classes for window messages processing
#pragma once

#include <windows.h>
#include <vector>
#include <map>


///////////////////////////////////////////////////////////////////////////////

/**
 * A window will notify about 'system events' (change of size, input etc),
 * happening in it via this interface.
 */
class IWindowMessagesProcessor
{
public:
   virtual ~IWindowMessagesProcessor(void) {}

   /**
    * @return 'true' if the message was processed, 'false' otherwise
    */
   virtual bool wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) = 0;
};

///////////////////////////////////////////////////////////////////////////////

class CompositeWindowMessagesProcessor : public IWindowMessagesProcessor
{
private:
   std::vector<IWindowMessagesProcessor*> m_processors;

public:
   ~CompositeWindowMessagesProcessor()
   {
      m_processors.clear();
   }

   void add(IWindowMessagesProcessor& processor)
   {
      m_processors.push_back(&processor);
   }

   bool wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
   {
      unsigned int count = m_processors.size();

      bool wasMsgProcessed = false;
      for (unsigned int i = 0 ; i < count; ++i)
      {
         wasMsgProcessed |= m_processors[i]->wndProc(hWnd, 
                                                     message, 
                                                     wParam, 
                                                     lParam);
      }

      return wasMsgProcessed;
   }
};

///////////////////////////////////////////////////////////////////////////////
