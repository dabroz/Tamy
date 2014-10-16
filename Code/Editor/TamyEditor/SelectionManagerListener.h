/// @file   TamyEditor/SelectionManagerListener.h
/// @brief  selection manager listener interface
#pragma once

#include "core\List.h"


///////////////////////////////////////////////////////////////////////////////

class SelectionManager;

///////////////////////////////////////////////////////////////////////////////

/**
 * An instance will be informed what entities are currently selected through
 * this interface.
 */
class SelectionManagerListener
{
public:
   virtual ~SelectionManagerListener() {}

   /**
    * Called when entities are selected.
    *
    * @param selectionMgr
    */
   virtual void onSelectionChanged( SelectionManager* selectionMgr ) = 0;
};


///////////////////////////////////////////////////////////////////////////////
