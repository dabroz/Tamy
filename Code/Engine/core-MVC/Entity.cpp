#include "core-MVC\Entity.h"
#include "core-MVC\Model.h"
#include "core-MVC\ModelView.h"
#include "core-MVC\Prefab.h"
#include "core-MVC\EntityUtils.h"
#include "core-MVC\EntityListener.h"
#include "core\Sphere.h"
#include "core\AxisAlignedBox.h"
#include "core\Assert.h"
#include "core\ListUtils.h"
#include "core\LocalList.h"
#include <algorithm>


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( Entity );
   PARENT( SceneNode );
   PROPERTY( Entity::Children, m_serializedChildren );
   PROPERTY_EDIT( "Prefab", Prefab*, m_prefab );
   PROPERTY_EDIT( "Local transformation", Matrix, m_localMtx )
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

Entity::Entity( const char* name )
   : SceneNode( name )
   , m_prefab( NULL )
   , m_localVolume( NULL )
   , m_localMtx( Matrix::IDENTITY )
   , m_globalVolume( NULL )
   , m_parentWorldMtx( &Matrix::IDENTITY )
{
   m_globalMtx.setIdentity();
   buildBoundingVolume();
}

///////////////////////////////////////////////////////////////////////////////

Entity::Entity( const Entity& rhs )
   : SceneNode( rhs )
   , m_prefab( rhs.m_prefab )
   , m_serializedChildren( rhs.m_serializedChildren )
   , m_localVolume( new AxisAlignedBox( *rhs.m_localVolume ) )
   , m_localMtx( rhs.m_localMtx )
   , m_globalVolume( new AxisAlignedBox( *rhs.m_globalVolume ) )
   , m_parentWorldMtx( &Matrix::IDENTITY )
{
   if ( m_prefab )
   {
      m_prefab->addReference();
   }
   m_globalMtx = rhs.m_globalMtx;

   // clone the children and attach them
   uint count = rhs.m_children.size();
   for ( uint i = 0; i < count; ++i )
   {
      SceneNode* child = rhs.m_children[i];
      if ( !child->wasInstantiatedFromPrefab() )
      {
         SceneNode* childClone = child->clone();
         addChild( childClone );
      }
   }

   // do not copy bounding volumes - each cloned component will register them itself when it's attached etc.
}

///////////////////////////////////////////////////////////////////////////////

Entity::~Entity()
{
   clear();

   if ( m_prefab )
   {
      m_prefab->removeReference();
   }

   delete m_localVolume;
   m_localVolume = NULL;

   delete m_globalVolume;
   m_globalVolume = NULL;

   m_prefab = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void Entity::clear()
{
   uint count = m_children.size();

   // inform each child that its sibling is being detached
   for ( uint i = 0; i < count; ++i )
   {
      SceneNode* child = m_children[i];

      for ( uint j = 0; j < count; ++j )
      {
         SceneNode* sibling = m_children[j];
         if ( sibling != child )
         {
            child->onSiblingDetached( sibling );
         }
      }
   }

   // release the children ( this entity's children, not the one's pointed at by the m_children ptr )
   for ( uint i = 0; i < count; ++i )
   {
      SceneNode* child = m_children[i];
      if ( child )
      {
         // send proper notifications
         onChildDetached( child );
         child->onDetached( this );

         // inform the listeners
         for ( List< EntityListener* >::iterator it = m_listeners.begin(); !it.isEnd(); ++it )
         {
            EntityListener* listener = *it;
            listener->onChildDetached( this, child );
         }

         // decrease reference counter
         child->removeReference();
      }
   }
   m_children.clear();
   m_serializedChildren.clear();
}

///////////////////////////////////////////////////////////////////////////////

void Entity::addChild( SceneNode* addedChild )
{
   if ( !addedChild )
   {
      return;
   }

   // re-parent the node
   if ( addedChild->isAttached() )
   {
      addedChild->addReference();
      Entity* formerParent = static_cast< Entity* >( addedChild->getParent() );
      formerParent->removeChild( addedChild );
   }

   // send proper notifications
   {
      onChildAttached( addedChild );
      addedChild->onAttached( this );

      // inform the immediate children about a new sibling,
      // and the new child about its siblings
      uint count = m_children.size();
      for ( uint i = 0; i < count; ++i )
      {
         SceneNode* child = m_children[i];
         child->onSiblingAttached( addedChild );

         addedChild->onSiblingAttached( child );
      }

      // inform the listeners
      for ( List< EntityListener* >::iterator it = m_listeners.begin(); !it.isEnd(); ++it )
      {
         EntityListener* listener = *it;
         listener->onChildAttached( this, addedChild );
      }
   }

   // register it as a child
   m_children.push_back( addedChild );
}

///////////////////////////////////////////////////////////////////////////////

void Entity::removeChild( SceneNode* removedChild )
{
   if ( !removedChild )
   {
      return;
   }

   bool nodeRemoved = false;

   // remove the entity from the managed entities list
   int count = (int)m_children.size();
   for ( int i = count - 1; i >= 0; --i )
   {
      SceneNode* child = m_children[i];
      if ( child == removedChild )
      {
         // send notifications
         onChildDetached( removedChild );
         removedChild->onDetached( this );

         m_children.remove( i );
         nodeRemoved = true;
      }
      else
      {
         child->onSiblingDetached( removedChild );
         removedChild->onSiblingDetached( child );
      }
   }

   // inform the listeners
   if ( nodeRemoved )
   {
      // inform the listeners
      for ( List< EntityListener* >::iterator it = m_listeners.begin(); !it.isEnd(); ++it )
      {
         EntityListener* listener = *it;
         listener->onChildDetached( this, removedChild );
      }
   }

   ASSERT_MSG( nodeRemoved, "This node was not among the children of this entity" );

   // remove a reference to this node
   removedChild->removeReference();
}

///////////////////////////////////////////////////////////////////////////////

void Entity::setPrefab( Prefab* prefab )
{
   if ( prefab == m_prefab )
   {
      // nothing has changed
      return;
   }

   NOTIFY_PROPERTY_CHANGE( m_prefab );

   m_prefab = prefab;
}

///////////////////////////////////////////////////////////////////////////////

void Entity::makeUnique( bool withEmbeddedPrefabs )
{
   if ( getHostModel() == NULL )
   {
      return;
   }

   LocalStack< Entity* > entitiesToUnprefabify;

   // remove references to prefabs
   LocalList< SceneNode* > nodes;
   nodes.pushBack( this );

   while ( !nodes.empty() )
   {
      SceneNode* currNode = nodes.front();
      nodes.popFront();

      currNode->markAsInstantiatedFromPrefab( false );

      if ( !currNode->isA< Entity >() )
      {
         continue;
      }

      Entity* currEntity = static_cast< Entity* >( currNode );
      if ( !withEmbeddedPrefabs && currEntity != this && currEntity->m_prefab != NULL )
      {
         continue;
      }

      entitiesToUnprefabify.push( currEntity );

      uint childrenCount = currEntity->m_children.size();
      for ( uint i = 0; i < childrenCount; ++i )
      {
         SceneNode* childNode = currEntity->m_children[i];
         nodes.pushBack( childNode );
      }
   }

   // remove prefabs from the marked entities
   while ( !entitiesToUnprefabify.empty() )
   {
      Entity* entity = entitiesToUnprefabify.pop();
      entity->setPrefab( NULL );
   }
}

///////////////////////////////////////////////////////////////////////////////

void Entity::onEmbeddedEntityChanged( Prefab* prefab )
{
   if ( !isAttached() )
   {
      return;
   }

   ASSERT( m_prefab == prefab );

   deinstantiatePrefab();
   instantiatePrefab();
}

///////////////////////////////////////////////////////////////////////////////

void Entity::onPrePropertyChanged( ReflectionProperty& property )
{
   SceneNode::onPrePropertyChanged( property );

   if ( property.getName() == "m_prefab" )
   {
      if ( m_prefab )
      {
         // stop listening to the prefab
         m_prefab->detachListener( this );
         m_prefab->removeReference();
      }

      deinstantiatePrefab();
   }
}

///////////////////////////////////////////////////////////////////////////////

void Entity::onPropertyChanged( ReflectionProperty& property )
{
   SceneNode::onPropertyChanged( property );

   if ( property.getName() == "m_prefab" )
   {
      instantiatePrefab();

      if ( m_prefab )
      {
         m_prefab->addReference();

         if ( isAttached() )
         {
            // start listening to the prefab
            m_prefab->attachListener( this );
         }
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void Entity::instantiatePrefab()
{
   if ( !m_prefab || !isAttached() )
   {
      return;
   }

   List< SceneNode* > nodesToAdd;
   m_prefab->instantiate( nodesToAdd );

   for ( List< SceneNode* >::iterator it = nodesToAdd.begin(); !it.isEnd(); ++it )
   {
      SceneNode* node = *it;
      addChild( node );
   }
}

///////////////////////////////////////////////////////////////////////////////

void Entity::deinstantiatePrefab()
{
   // remove all children that come from a prefab
   int count = (int)m_children.size();
   for ( int i = count - 1; i >= 0; --i )
   {
      SceneNode* child = m_children[i];
      if ( child->wasInstantiatedFromPrefab() )
      {
         removeChild( child );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void Entity::onObjectPreSave()
{
   SceneNode::onObjectPreSave();

   m_serializedChildren.clear();

   // copy only the nodes that haven't been instantiated from a prefab
   uint count = m_children.size();
   m_serializedChildren.allocate( count );

   for ( uint i = 0; i < count; ++i )
   {
      SceneNode* child = m_children[i];
      if ( !child->wasInstantiatedFromPrefab() )
      {
         m_serializedChildren.push_back( child );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void Entity::onModelLoaded()
{
   if ( m_prefab )
   {
      m_prefab->addReference();
   }

   int count = m_serializedChildren.size();
   for ( int i = 0; i < count; ++i )
   {
      SceneNode* child = m_serializedChildren[i];
      if ( child )
      {
         // child was loaded correctly
         addChild( child );
      }
      else
      {
         // remove the empty child
         m_serializedChildren.remove( i );
         --count;
      }
   }

   for ( int i = 0; i < count; ++i )
   {
      SceneNode* child = m_serializedChildren[i];
      child->onModelLoaded();
   }
   m_serializedChildren.clear();
}

///////////////////////////////////////////////////////////////////////////////

void Entity::collectChildren( Stack< SceneNode* >& outChildren ) const
{
   uint count = m_children.size();
   for ( uint i = 0; i < count; ++i )
   {
      SceneNode* child = m_children[i];
      outChildren.push( child );
   }
}

///////////////////////////////////////////////////////////////////////////////

void Entity::collectChildren( Stack< const SceneNode* >& outChildren ) const
{
   uint count = m_children.size();
   for ( uint i = 0; i < count; ++i )
   {
      const SceneNode* child = m_children[i];
      outChildren.push( child );
   }
}

///////////////////////////////////////////////////////////////////////////////

void Entity::collectChildren( List< SceneNode* >& outChildren ) const
{
   uint count = m_children.size();
   for ( uint i = 0; i < count; ++i )
   {
      SceneNode* child = m_children[i];
      outChildren.pushBack( child );
   }
}

///////////////////////////////////////////////////////////////////////////////

void Entity::collectChildren( List< const SceneNode* >& outChildren ) const
{
   uint count = m_children.size();
   for ( uint i = 0; i < count; ++i )
   {
      const SceneNode* child = m_children[i];
      outChildren.pushBack( child );
   }
}

///////////////////////////////////////////////////////////////////////////////

void Entity::onAttachToModel( Model* model )
{
   SceneNode::onAttachToModel( model );

   // inform the children that they have been attached to a model
   uint count = m_children.size();
   for ( uint i = 0; i < count; ++i )
   {
      SceneNode* child = m_children[i];
      child->onAttachToModel( model );
   }
}

///////////////////////////////////////////////////////////////////////////////

void Entity::onDetachFromModel( Model* model )
{
   // inform the children that they are about to be detached from a model
   uint count = m_children.size();
   for ( uint i = 0; i < count; ++i )
   {
      SceneNode* child = m_children[i];
      child->onDetachFromModel( model );
   }

   SceneNode::onDetachFromModel( model );
}

///////////////////////////////////////////////////////////////////////////////

void Entity::onAttached( Entity* parent )
{
   SceneNode::onAttached( parent );

   // adjust the pointer to a parent's world matrix
   Entity* parentEntity = static_cast< Entity* >( parent );
   m_parentWorldMtx = &parentEntity->m_globalMtx;

   // instantiate a prefab
   instantiatePrefab();

   if ( m_prefab )
   {
      // start listening to the prefab
      m_prefab->attachListener( this );
   }
}

///////////////////////////////////////////////////////////////////////////////

void Entity::onDetached( Entity* parent )
{
   // uninstantiate the prefab
   deinstantiatePrefab();

   if ( m_prefab )
   {
      // start listening to the prefab
      m_prefab->detachListener( this );
   }

   // adjust the pointer to a parent's world matrix
   m_parentWorldMtx = &Matrix::IDENTITY;

   SceneNode::onDetached( parent );
}

///////////////////////////////////////////////////////////////////////////////

void Entity::pullStructure( ModelView* view )
{
   // pull children structure
   uint count = m_children.size();
   for ( uint i = 0; i < count; ++i )
   {
      SceneNode* child = m_children[i];

      view->onNodeAdded( child );
      child->pullStructure( view );
   }
}

///////////////////////////////////////////////////////////////////////////////

void Entity::setLocalMtx( const Matrix& localMtx ) 
{
   m_localMtx = localMtx;
}

///////////////////////////////////////////////////////////////////////////////

void Entity::setRightVec( const Vector& vec )
{
   m_localMtx.setSideVec<3>( vec );
}

///////////////////////////////////////////////////////////////////////////////

void Entity::setUpVec( const Vector& vec )
{
   m_localMtx.setUpVec<3>( vec );
}

///////////////////////////////////////////////////////////////////////////////

void Entity::setLookVec( const Vector& vec )
{
   m_localMtx.setForwardVec<3>( vec );
}

///////////////////////////////////////////////////////////////////////////////

void Entity::setPosition( const Vector& vec )
{
   m_localMtx.setPosition<3>( vec );
}

///////////////////////////////////////////////////////////////////////////////

void Entity::getRightVec( Vector& outRightVec ) const
{
   outRightVec = m_localMtx.sideVec();
}

///////////////////////////////////////////////////////////////////////////////

void Entity::getUpVec( Vector& outUpVec ) const
{
   outUpVec = m_localMtx.upVec();
}

///////////////////////////////////////////////////////////////////////////////

void Entity::getLookVec( Vector& outLookVec ) const
{
   outLookVec = m_localMtx.forwardVec();
}

///////////////////////////////////////////////////////////////////////////////

void Entity::getPosition( Vector& outPos ) const
{
   outPos = m_localMtx.position();
}

///////////////////////////////////////////////////////////////////////////////

void Entity::updateTransforms()
{
   // update this matrix
   m_globalMtx.setMul( m_localMtx, *m_parentWorldMtx );

   // update the volume
   ASSERT( m_localVolume && m_globalVolume );
   m_localVolume->transform( m_globalMtx, *m_globalVolume );

   // propagate the call down the hierarchy
   uint childrenCount = m_children.size();
   for ( uint i = 0; i < childrenCount; ++i )
   {
      SceneNode* child = m_children[i];
      child->updateTransforms();
   }
}

///////////////////////////////////////////////////////////////////////////////

void Entity::getGlobalVectors( Vector& outRightVec, Vector& outUpVec, Vector& outLookVec, Vector& outPos ) const
{
   m_globalMtx.getVectors( outRightVec, outUpVec, outLookVec, outPos );
}

///////////////////////////////////////////////////////////////////////////////

const AxisAlignedBox& Entity::getBoundingVolume() const
{
   return *m_globalVolume;
}

///////////////////////////////////////////////////////////////////////////////

void Entity::addBoundingVolume( AxisAlignedBox* volume )
{
   ASSERT( volume );

   List< AxisAlignedBox* >::iterator it = ListUtils::find( m_componentBoundingVolumes, volume );
   if ( it.isEnd() )
   {
      m_componentBoundingVolumes.pushBack( volume );

      // delete the volume in order to allow the 'buildBoundingVolume' method to build a new one
      delete m_localVolume;
      m_localVolume = NULL;

      buildBoundingVolume();
   }
}

///////////////////////////////////////////////////////////////////////////////

void Entity::removeBoundingVolume( AxisAlignedBox* volume )
{
   ASSERT( volume );

   List< AxisAlignedBox* >::iterator it = ListUtils::find( m_componentBoundingVolumes, volume );
   if ( !it.isEnd() )
   {
      it.markForRemoval();
      it.processElementsRemoval();

      // delete the volume in order to allow the 'buildBoundingVolume' method to build a new one
      delete m_localVolume;
      m_localVolume = NULL;
   }

   buildBoundingVolume();
}

///////////////////////////////////////////////////////////////////////////////

void Entity::buildBoundingVolume()
{
   if ( m_localVolume )
   {
      // volume already exists - don't build it
      return;
   }

   // calculate a bounding box that comprises of the bounding boxes that surround the component volumes
   AxisAlignedBox boundingBox, componentBoundingBox;
   for ( List< AxisAlignedBox* >::iterator it = m_componentBoundingVolumes.begin(); !it.isEnd(); ++it )
   {
      AxisAlignedBox* componentVolume = *it;
      boundingBox.add( *componentVolume, boundingBox );
   }

   m_localVolume = new AxisAlignedBox( boundingBox );

   delete m_globalVolume;
   m_globalVolume = new AxisAlignedBox(boundingBox );
}

///////////////////////////////////////////////////////////////////////////////

void Entity::attachEntityListener( EntityListener* listener )
{
   if ( listener )
   {
      ListUtils::pushBackUnique( m_listeners, listener );
   }
}

///////////////////////////////////////////////////////////////////////////////

void Entity::detachEntityListener( EntityListener* listener )
{
   List< EntityListener* >::iterator it = ListUtils::find( m_listeners, listener );
   if ( !it.isEnd() )
   {
      it.markForRemoval();
   }
}

///////////////////////////////////////////////////////////////////////////////

void Entity::pullStructure( EntityListener* listener )
{
   const uint childrenCount = m_children.size();
   for ( uint i = 0; i < childrenCount; ++i )
   {
      SceneNode* child = m_children[i];
      listener->onChildAttached( this, child );
   }
}

///////////////////////////////////////////////////////////////////////////////
