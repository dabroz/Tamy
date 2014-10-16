#include "core-MVC\SceneNode.h"
#include "core-MVC\Model.h"
#include "core-MVC\ModelView.h"
#include "core-MVC\Entity.h"
#include "core\ReflectionProperty.h"
#include "core\Assert.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_ABSTRACT_OBJECT( SceneNode );
   PARENT( ReflectionObject );
   PROPERTY_EDIT( "Name", std::string, m_name );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

SceneNode::SceneNode( const char* name )
   : m_name( name )
   , m_parentNode( NULL )
   , m_hostModel ( NULL )
   , m_instantiatedFromPrefab( false )
{
}

///////////////////////////////////////////////////////////////////////////////

SceneNode::SceneNode( const SceneNode& rhs )
   : ReflectionObject( rhs )
   , m_name( rhs.m_name )
   , m_parentNode( NULL )
   , m_hostModel ( NULL )
   , m_instantiatedFromPrefab( false )
{
}

///////////////////////////////////////////////////////////////////////////////

SceneNode::~SceneNode()
{
   ASSERT_MSG( !m_parentNode, "This scene node is still attached to its parent" );
   ASSERT_MSG( !m_hostModel, "This scene node is still a part of a Model" );
}

///////////////////////////////////////////////////////////////////////////////

void SceneNode::setSceneNodeName( const char* name )
{
   m_name = name;

   // notify listeners
   {
      ReflectionProperty* property = getProperty( "m_name" );
      notifyPropertyChange( *property );
      delete property;
   }
}

///////////////////////////////////////////////////////////////////////////////

void SceneNode::remove()
{
   if ( m_parentNode )
   {
      m_parentNode->removeChild( this );
   }
   else
   {
      m_parentNode->removeReference();
   }
}

///////////////////////////////////////////////////////////////////////////////

void SceneNode::onAttachToModel( Model* model )
{
   ASSERT_MSG( !m_hostModel, "This entity is already attached to a model");

   m_hostModel = model;

   // notify listeners
   if ( m_hostModel != NULL )
   {
      m_hostModel->notifyNodeAdded( this );
   }
}

///////////////////////////////////////////////////////////////////////////////

void SceneNode::onDetachFromModel( Model* model )
{
   ASSERT_MSG( model == m_hostModel, "The model this entity is being detached from is not its host" );

   // notify listeners
   if ( m_hostModel != NULL )
   {
      m_hostModel->notifyNodeRemoved( this );
   }

   m_hostModel = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void SceneNode::onAttached( Entity* parent )
{
   if ( m_parentNode == parent )
   {
      // This is the parent of the node - no need to reattach it.
      
      // This situation takes place when an entity that references a prefab is loaded
      return;
   }

   ASSERT_MSG( !m_parentNode, "This node already has a parent" );
   m_parentNode = parent;

   // if the parent is a part of the model, make sure the children attached to it become a part of it as well
   Model* parentsHostModel = m_parentNode->getHostModel();
   if ( parentsHostModel )
   {
      onAttachToModel( parentsHostModel );
   }
}

///////////////////////////////////////////////////////////////////////////////

void SceneNode::onDetached( Entity* parent )
{
   if ( !m_parentNode )
   {
      // the node wasn't attached in the first place;
      return;
   }

   ASSERT_MSG( m_parentNode == parent, "This node is not parented by the specified parent" );

   // automatically detach from the parent's model
   Model* parentsHostModel = m_parentNode->getHostModel();
   if ( parentsHostModel )
   {
      onDetachFromModel( parentsHostModel );
   }

   ASSERT_MSG( m_parentNode == parent, "Trying to detach from a node that is node our parent" );
   m_parentNode = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void SceneNode::onPropertyChanged( ReflectionProperty& property )
{
   ReflectionObject::onPropertyChanged( property );

   if ( m_hostModel )
   {
      m_hostModel->notifyNodeChanged( this );
   }
}

///////////////////////////////////////////////////////////////////////////////

SceneNode* SceneNode::clone() const
{
   const SerializableReflectionType& type = getVirtualRTTI();
   SceneNode* selfClone = type.instantiate( *this );

   return selfClone;
}

///////////////////////////////////////////////////////////////////////////////

void SceneNode::pullStructure( ModelView* view )
{
   // no structure spanning underneath
}

///////////////////////////////////////////////////////////////////////////////

const Matrix& SceneNode::getGlobalMtx() const
{
   return Matrix::IDENTITY;
}

///////////////////////////////////////////////////////////////////////////////
