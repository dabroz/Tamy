/// @file   core-MVC/SceneNode.h
/// @brief  a base class for the scene system
#pragma once

#include "core\MemoryRouter.h"
#include "core\ReflectionObject.h"
#include "core\List.h"
#include "core\Stack.h"
#include "core\Matrix.h"
#include "core-MVC\Transformable.h"
#include <string>


///////////////////////////////////////////////////////////////////////////////

class ModelView;
class Model;
class Entity;

///////////////////////////////////////////////////////////////////////////////

/**
 * A base class for the scene system.
 */
class SceneNode : public ReflectionObject, public Transformable
{
   DECLARE_ALLOCATOR( SceneNode, AM_DEFAULT );
   DECLARE_CLASS()

protected:
   // static data
   std::string                   m_name;

   // runtime data
   Entity*                       m_parentNode;
   Model*                        m_hostModel;
   bool                          m_instantiatedFromPrefab;

public:
   /**
    * Constructor.
    *
    * @param name
    */
   SceneNode( const char* name = "" );

   /**
    * Copy constructor.
    */
   SceneNode( const SceneNode& rhs );
   virtual ~SceneNode();

   /**
    * Returns the name of the scene node.
    */
   const std::string& getSceneNodeName() const { return m_name; }

   /**
    * Gives the node a new name.
    *
    * @param name
    */
   void setSceneNodeName( const char* name );

   // -------------------------------------------------------------------------
   // Prefabs support
   // -------------------------------------------------------------------------
   /**
    * Marks the node as instantiated from a prefab.
    */
   inline void markAsInstantiatedFromPrefab( bool flag ) { m_instantiatedFromPrefab = flag; }

   /**
    * Tells if the node was instantiated from a prefab.
    */
   inline bool wasInstantiatedFromPrefab() const { return m_instantiatedFromPrefab; }

   // -------------------------------------------------------------------------
   // Hierarchy management
   // -------------------------------------------------------------------------

   /**
    * Removes this node from its parent.
    *
    * CAUTION: One reference to this node will be removed in the process,
    * so it that was the last reference, the node will be deleted.
    */
   virtual void remove();

   /**
    * Tells whether this entity is attached to another entity (thus has a parent)
    */
   inline bool isAttached() const { return m_parentNode != NULL; }

   /**
    * Returns the parent of this entity (if it's attached to one).
    */
   inline Entity* getParent() const { return m_parentNode; }

   /**
    * Returns the model that hosts this entity ( if it's hosted by one ).
    */
   inline Model* getHostModel() const { return m_hostModel; }

   /**
    * Clones the node along with the structure that spans underneath it.
    */
   SceneNode* clone() const;

   // -------------------------------------------------------------------------
   // Transformable implementation
   // -------------------------------------------------------------------------
   virtual void updateTransforms() override {}

   // -------------------------------------------------------------------------
   // Transforms management
   // -------------------------------------------------------------------------
   /**
    * This is the matrix that describes the node's absolute world position
    * (unlike the local matrix which describes the position relative to node's
    * parent).
    */
   virtual const Matrix& getGlobalMtx() const;

   // -------------------------------------------------------------------------
   // Structure inspection
   // -------------------------------------------------------------------------

   /**
    * Returns the children of this node ( and only this node - the method doesn't step into
    * its children should there be any ).
    *
    * @param outrChildren
    */
   virtual void collectChildren( Stack< SceneNode* >& outChildren ) const {}

   /**
   * Returns the children of this node ( and only this node - the method doesn't step into
   * its children should there be any ).
   *
   * @param outrChildren
   */
   virtual void collectChildren( Stack< const SceneNode* >& outChildren ) const {}

   /**
    * Returns the children of this node ( and only this node - the method doesn't step into
    * its children should there be any ).
    *
    * @param outChildren
    */
   virtual void collectChildren( List< SceneNode* >& outChildren ) const {}

   /**
   * Returns the children of this node ( and only this node - the method doesn't step into
   * its children should there be any ) ( const version )
   *
   * @param outChildren
   */
   virtual void collectChildren( List< const SceneNode* >& outChildren ) const {}

   /**
    * Pulls the hierarchy that spans under this node.
    *
    * @param view
    */
   virtual void pullStructure( ModelView* view );

   // -------------------------------------------------------------------------
   // Notifications
   // -------------------------------------------------------------------------
   /**
    * Called when a model this node belongs to was loaded.
    * It's used to rebuild connections between entities and, as a part
    * of an internal mechanism, SHOULD NOT BE INHERITED FROM.
    */
   virtual void onModelLoaded() {}

   /**
    * This method will inform a node that it's been added to a model,
    * which from now on is its host and should be informed about
    * any children added to this node.
    *
    * @param model   host model
    */
   virtual void onAttachToModel( Model* model );

   /**
    * This method will inform a node that it's being detached
    * from a host model.
    *
    * @param model   former host model.
    */
   virtual void onDetachFromModel( Model* model );

   /**
    * Method called to inform the node about its new parent when it's being attached to it.
    *
    * @param parent
    */
   virtual void onAttached( Entity* parent );

   /**
    * Method called to inform the node that it's being detached from its parent.
    *
    * NOTE: when a node is detached from its parent, it's also supposed to deinitialize
    * all dependencies on other sibling nodes you created in the onSiblingAttached method,
    * because in this case onSiblingDetached won't be called!
    *
    * @param parent
    */
   virtual void onDetached( Entity* parent );

   /**
    * Called when a sibling node is attached to this node's parent.
    *
    * NOTE: here's where you establish inter-node dependencies.
    *
    * @param node
    */
   virtual void onSiblingAttached( SceneNode* node ) {}

   /**
    * Called when a sibling node is detached from this node's parent.
    *
    * NOTE: here's where you tear down inter-node dependencies.
    * CAUTION: When a node is removed from an entity, this method won't be called - that would have
    * caused way to much method call clutter, so we simply assume that one can do the proper
    * deinitialization in the onDetached method.
    *
    * @param node
    */
   virtual void onSiblingDetached( SceneNode* node ) {}

protected:

   // -------------------------------------------------------------------------
   // Serializable implementation
   // -------------------------------------------------------------------------
   void onPropertyChanged( ReflectionProperty& property );
};


///////////////////////////////////////////////////////////////////////////////
