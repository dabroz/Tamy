/// @file   core-MVC/Prefab.h
/// @brief  an entity turned to a resource that can be referenced many times over in one level
#pragma once

#include "core\Resource.h"
#include "core\List.h"


///////////////////////////////////////////////////////////////////////////////

class Entity;
class SceneNode;
class PrefabListener;

///////////////////////////////////////////////////////////////////////////////

/**
 * An entity turned to a resource that can be referenced many times over in one level.
 */
class Prefab : public Resource
{
   DECLARE_ALLOCATOR( Prefab, AM_DEFAULT );
   DECLARE_RESOURCE();

private:
   // static data
   Entity*                       m_embeddedEntity;

   // runtime data
   List< PrefabListener* >       m_listeners;

public:
   /**
    * Constructor.
    *
    * @param filePath
    */
   Prefab( const FilePath& filePath = FilePath() );
   ~Prefab();

   /**
    * Embeds an entity in the prefab. Only one entity can be embedded at a time.
    * The existing one will be removed automatically.
    *
    * @param entity
    */
   void setEntity( Entity* entity );

   /**
    * Returns a pointer to the embedded entity.
    * CAUTION: please mind, that entity is not added to any scene.
    */
   inline const Entity* getEntity() const {
      return m_embeddedEntity;
   }

   // -------------------------------------------------------------------------
   // Listeners management
   // -------------------------------------------------------------------------

   /**
    * Attaches a new listener.
    *
    * @param listener
    */
   void attachListener( PrefabListener* listener );

   /**
    * Detaches a listener.
    *
    * @param listener
    */
   void detachListener( PrefabListener* listener );

   // -------------------------------------------------------------------------
   // Resource implementation
   // -------------------------------------------------------------------------
   void replaceContents( Resource& rhs );

protected:
   // -------------------------------------------------------------------------
   // Object implementation
   // -------------------------------------------------------------------------
   void onObjectLoaded();

   // -------------------------------------------------------------------------
   // Instantiation
   // -------------------------------------------------------------------------
   /**
    * Instantiates the prefab.
    *
    * @param outNodesToAdd
    */
   friend class Entity;
   void instantiate( List< SceneNode* >& outNodesToAdd ) const;
};

///////////////////////////////////////////////////////////////////////////////
