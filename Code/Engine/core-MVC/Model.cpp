#include "core-MVC\Model.h"
#include "core-MVC\SceneNode.h"
#include "core-MVC\Entity.h"
#include "core-MVC\ModelView.h"
#include "core\ListUtils.h"
#include "core\Assert.h"
#include "core\ThreadSystem.h"
#include "core\Thread.h"

///////////////////////////////////////////////////////////////////////////////

BEGIN_RESOURCE( Model, tsc, AM_BINARY );
   PROPERTY( Entity*, m_root );
END_RESOURCE();

///////////////////////////////////////////////////////////////////////////////

Model::Model( const FilePath& resourceName )
   : Resource( resourceName )
   , m_root( NULL )
{
   if ( !IS_BEING_SERIALIZED() )
   {
      m_root = new Entity( "sceneRoot" );
      m_root->onAttachToModel( this );
   }
}

///////////////////////////////////////////////////////////////////////////////

Model::~Model() 
{
   // inform the views that the model goes out of scope
   for( Views::iterator it = m_views.begin(); !it.isEnd(); ++it )
   {
      ModelView* view = *it;
      view->onDetachedFromModel( *this );
   }
   m_views.clear();

   m_root->clear();
   if ( !IS_BEING_SERIALIZED() )
   {
      m_root->onDetachFromModel( this );
   }
   m_root->removeReference();
   m_root = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void Model::addChild( SceneNode* child )
{
   m_root->addChild( child );
}

///////////////////////////////////////////////////////////////////////////////

void Model::clear()
{
   m_root->clear();
}

///////////////////////////////////////////////////////////////////////////////

void Model::attachListener( ModelView* view )
{
   Views::iterator it = ListUtils::find( m_views, view );
   if ( it.isEnd() )
   {
      m_views.pushBack( view );
      view->onAttachedToModel( *this );
   }
}

///////////////////////////////////////////////////////////////////////////////

void Model::detachListener( ModelView* view )
{
   Views::iterator it = ListUtils::find( m_views, view );
   if ( !it.isEnd() )
   {
      view->onDetachedFromModel( *this );
      it.markForRemoval();
   }
}

///////////////////////////////////////////////////////////////////////////////

void Model::notifyNodeAdded( SceneNode* node )
{
   for ( Views::iterator it = m_views.begin(); !it.isEnd(); ++it )
   {
      ModelView* view = *it;
      view->onNodeAdded( node );
   }
}

///////////////////////////////////////////////////////////////////////////////

void Model::notifyNodeRemoved( SceneNode* node )
{
   for ( Views::iterator it = m_views.begin(); !it.isEnd(); ++it )
   {
      ModelView* view = *it;
      view->onNodeRemoved( node );
   }
}

///////////////////////////////////////////////////////////////////////////////

void Model::notifyNodeChanged( SceneNode* node )
{
   for ( Views::iterator it = m_views.begin(); !it.isEnd(); ++it )
   {
      ModelView* view = *it;
      view->onNodeChanged( node );
   }
}

///////////////////////////////////////////////////////////////////////////////

void Model::pullStructure( ModelView* view )
{
   m_root->pullStructure( view );
}

///////////////////////////////////////////////////////////////////////////////

void Model::replaceContents( Resource& rhs )
{
   Model& rhsModel = static_cast< Model& >( rhs );

   m_root = rhsModel.m_root;
   
   // erase the pointer from the other entity to avoid it being operated on
   // when the other entity gets deleted in a sec
   rhsModel.m_root = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void Model::onResourceLoaded( ResourcesManager& mgr )
{
   Resource::onResourceLoaded( mgr );

   // inform all entities that they are loaded
   Model* currHostModel = m_root->getHostModel();
   if ( !currHostModel )
   {
      // first - rebuild the entire hierarchy of the tree ( runtime connections between particular nodes )
      m_root->onModelLoaded();

      // next - inform that the hierarchy is a part of a model
      m_root->onAttachToModel( this );
   }
   else
   {
      ASSERT_MSG( currHostModel == this, "Root node is attached to a different model" );
   }
}

///////////////////////////////////////////////////////////////////////////////
