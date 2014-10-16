#include "core-MVC\TransformsManagementSystem.h"
#include "core-MVC\Model.h"
#include "core-MVC\Entity.h"
#include "core\ListUtils.h"


///////////////////////////////////////////////////////////////////////////////

INIT_SINGLETON( TransformsManagementSystem );

///////////////////////////////////////////////////////////////////////////////

TransformsManagementSystem::TransformsManagementSystem( const SingletonConstruct& )
{
}

///////////////////////////////////////////////////////////////////////////////

TransformsManagementSystem::~TransformsManagementSystem()
{
   for ( List< Model* >::iterator it = m_scenes.begin(); !it.isEnd(); ++it )
   {
      Model* scene = *it;
      scene->removeReference();
   }

   for ( List< Entity* >::iterator it = m_entities.begin(); !it.isEnd(); ++it )
   {
      Entity* entity = *it;
      entity->removeReference();
   }
}

///////////////////////////////////////////////////////////////////////////////

void TransformsManagementSystem::addScene( Model* scene )
{
   if ( !scene )
   {
      return;
   }

   List< Model* >::iterator it = ListUtils::find( m_scenes, scene );
   if ( it.isEnd() )
   {
      m_scenes.pushBack( scene );
      scene->addReference();
   }
}

///////////////////////////////////////////////////////////////////////////////

void TransformsManagementSystem::removeScene( Model* scene )
{
   if ( !scene )
   {
      return;
   }

   List< Model* >::iterator it = ListUtils::find( m_scenes, scene );
   if ( !it.isEnd() )
   {
      scene->removeReference();
      it.markForRemoval();
   }
}

///////////////////////////////////////////////////////////////////////////////

void TransformsManagementSystem::addEntity( Entity* entity )
{
   if ( !entity )
   {
      return;
   }

   List< Entity* >::iterator it = ListUtils::find( m_entities, entity );
   if ( it.isEnd() )
   {
      m_entities.pushBack( entity );
      entity->addReference();
   }

}

///////////////////////////////////////////////////////////////////////////////

void TransformsManagementSystem::removeEntity( Entity* entity )
{
   if ( !entity )
   {
      return;
   }

   List< Entity* >::iterator it = ListUtils::find( m_entities, entity );
   if ( !it.isEnd() )
   {
      entity->removeReference();
      it.markForRemoval();
   }
}

///////////////////////////////////////////////////////////////////////////////

void TransformsManagementSystem::tick()
{
   for ( List< Model* >::iterator it = m_scenes.begin(); !it.isEnd(); ++it )
   {
      Model* scene = *it;
      scene->getRoot()->updateTransforms();
   }

   for ( List< Entity* >::iterator it = m_entities.begin(); !it.isEnd(); ++it )
   {
      Entity* entity = *it;
      entity->updateTransforms();
   }
}

///////////////////////////////////////////////////////////////////////////////
