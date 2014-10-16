/// @file   core-MVC/PrefabListener.h
/// @brief  a prefab resource listener
#pragma once


///////////////////////////////////////////////////////////////////////////////

class Prefab;

///////////////////////////////////////////////////////////////////////////////

/**
 * A prefab resource listener.
 */
class PrefabListener
{
public:
   virtual ~PrefabListener() {}

   /**
    * Called when the entity embedded in a prefab changes.
    *
    * @param prefab
    */
   virtual void onEmbeddedEntityChanged( Prefab* prefab ) = 0;
};

///////////////////////////////////////////////////////////////////////////////
