#include "core-MVC\TransformsManagementSystem.h"
#include "core-MVC\Model.h"
#include "core-MVC\Entity.h"
#include "core-MVC\Transformable.h"
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

void TransformsManagementSystem::addTransformable( Transformable* obj )
{
   if ( !obj )
   {
      return;
   }

   List< Transformable* >::iterator it = ListUtils::find( m_transformables, obj );
   if ( it.isEnd() )
   {
      m_transformables.pushBack( obj );
   }

}

///////////////////////////////////////////////////////////////////////////////

void TransformsManagementSystem::removeTransformable( Transformable* obj )
{
   if ( !obj )
   {
      return;
   }

   List< Transformable* >::iterator it = ListUtils::find( m_transformables, obj );
   if ( !it.isEnd() )
   {
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

   for ( List< Transformable* >::iterator it = m_transformables.begin(); !it.isEnd(); ++it )
   {
      Transformable* transformable = *it;
      transformable->updateTransforms();
   }

}

///////////////////////////////////////////////////////////////////////////////
