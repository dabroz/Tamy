#include "core-MVC\Prefab.h"
#include "core-MVC\Entity.h"
#include "core-MVC\PrefabListener.h"
#include "core-MVC\EntityUtils.h"
#include "core\ListUtils.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_RESOURCE( Prefab, tpf, AM_BINARY );
   PROPERTY( Entity*, m_embeddedEntity );
END_RESOURCE();

///////////////////////////////////////////////////////////////////////////////

Prefab::Prefab( const FilePath& filePath )
   : Resource( filePath )
   , m_embeddedEntity( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////

Prefab::~Prefab()
{
   m_listeners.clear();

   if ( m_embeddedEntity )
   {
      m_embeddedEntity->removeReference();
      m_embeddedEntity = NULL;
   }
}

///////////////////////////////////////////////////////////////////////////////

void Prefab::replaceContents( Resource& rhs )
{
   Prefab& rhsPrefab = static_cast< Prefab& >( rhs );

   m_embeddedEntity = rhsPrefab.m_embeddedEntity;
   if ( m_embeddedEntity )
   {
      m_embeddedEntity->addReference();
   }
}

///////////////////////////////////////////////////////////////////////////////

void Prefab::setEntity( Entity* entity )
{
   ASSERT( entity != NULL );

   if ( m_embeddedEntity )
   {
      m_embeddedEntity->removeReference();
   }

   const SerializableReflectionType& type = entity->getVirtualRTTI();
   m_embeddedEntity = type.instantiate( *entity );

   // notify listeners
   for ( List< PrefabListener* >::iterator it = m_listeners.begin(); !it.isEnd(); ++it )
   {
      PrefabListener* listener = *it;
      listener->onEmbeddedEntityChanged( this );
   }
}

///////////////////////////////////////////////////////////////////////////////

void Prefab::attachListener( PrefabListener* listener )
{
   List< PrefabListener* >::iterator it = ListUtils::find( m_listeners, listener );
   if ( it.isEnd() )
   {
      m_listeners.pushBack( listener );
   }
}

///////////////////////////////////////////////////////////////////////////////

void Prefab::detachListener( PrefabListener* listener )
{
   List< PrefabListener* >::iterator it = ListUtils::find( m_listeners, listener );
   if ( !it.isEnd() )
   {
      it.markForRemoval();
   }
}

///////////////////////////////////////////////////////////////////////////////

void Prefab::onObjectLoaded()
{
   Resource::onObjectLoaded();

   if ( m_embeddedEntity )
   {
      m_embeddedEntity->onModelLoaded();
   }
}

///////////////////////////////////////////////////////////////////////////////

void Prefab::instantiate( List< SceneNode* >& outNodesToAdd ) const
{
   if ( !m_embeddedEntity )
   {
      // there's nothing to instantiate
      return;
   }

   // create a clone of the embedded entity's children and attach them to the specified PrefabEntity
   uint count = m_embeddedEntity->m_children.size();
   for ( uint i = 0; i < count; ++i )
   {
      const SceneNode* child = m_embeddedEntity->m_children[i];

      const SerializableReflectionType& type = child->getVirtualRTTI();
      SceneNode* childClone = type.instantiate( *child );

      // mark the entire hierarchy spanning underneath as instantiated from a prefab
      {
         LocalStack< SceneNode* > nodes;
         nodes.push( childClone );

         while ( !nodes.empty() )
         {
            SceneNode* currNode = nodes.pop();
            currNode->markAsInstantiatedFromPrefab( true );

            currNode->collectChildren( nodes );
         }
      }

      outNodesToAdd.pushBack( childClone );
   }
}

///////////////////////////////////////////////////////////////////////////////
