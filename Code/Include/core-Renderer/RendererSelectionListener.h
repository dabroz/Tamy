/// @file   core-Renderer/RendererSelectionListener.h
/// @brief  selection manager listener interface
#pragma once

#include "core\List.h"


///////////////////////////////////////////////////////////////////////////////

class Entity;
struct Vector;

///////////////////////////////////////////////////////////////////////////////

/**
 * An instance will be informed what entities are currently selected through
 * this interface.
 */
class RendererSelectionListener
{
public:
   virtual ~RendererSelectionListener() {}

   /**
    * Called when an entity is selected.
    *
    * @param entities
    */
   virtual void onEntitiesSelected( const List< Entity* >& entities ) {}

   /**
    * Called in return to a world position query.
    *
    * @param worldPos
    */
   virtual void onWorldPositionClicked( const Vector& worldPos ) {}
};


///////////////////////////////////////////////////////////////////////////////
