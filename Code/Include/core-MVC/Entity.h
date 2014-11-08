/// @file   core-MVC\Entity.h
/// @brief  object populating a model
#ifndef _ENTITY_H
#define _ENTITY_H

/**
 * Entities management:
 * ---------------------
 * Entities can be organized in hierarchies. Furthermore, they can
 * be added to a model (a single entity may be added to one model
 * at a time).
 *
 * Entities should be designed to be self contained and self initializing.
 * A model can have multiple components attached to it, and 
 * entities gain access to those components - thus they may initialize
 * themselves based on them.
 * 
 * Serialization:
 * ---------------
 * A model can be serialized. What's important is that components added
 * to a model are not taken into consideration during the serialization.
 * This allows to serialize a model that was i.e. using one instance 
 * of a renderer, and deserialize it using another - all the entities
 * will transparently connect to the other one.
 *
 */

#include "core-MVC\SceneNode.h"
#include "core-MVC\Model.h"
#include "core-MVC\PrefabListener.h"
#include "core\Array.h"
#include "core\List.h"
#include "core\Matrix.h"
#include "core\AxisAlignedBox.h"


///////////////////////////////////////////////////////////////////////////////

class Prefab;

///////////////////////////////////////////////////////////////////////////////

/**
 * Models consist of various entities that interact with each other
 * and create the model state.
 *
 * An entity has various properties. You can put the properties definitions
 * inside a 'registerProperties' method implementation. 
 */
class Entity : public SceneNode, public PrefabListener
{
   DECLARE_ALLOCATOR( Entity, AM_ALIGNED_16 );
   DECLARE_CLASS()

public:
   typedef Array< SceneNode* >         Children;

   // ------------------------
   // static data
   // ------------------------
   Prefab*                             m_prefab;

   // local coordinate system
   Matrix                              m_localMtx;

   // ------------------------
   // runtime data
   // ------------------------

   Matrix*                             m_parentWorldMtx;
   Matrix                              m_globalMtx;

   List< AxisAlignedBox* >             m_componentBoundingVolumes;
   AxisAlignedBox*                     m_localVolume;
   AxisAlignedBox*                     m_globalVolume;

public:
   // ------------------------
   // Entity's children
   // ------------------------
   Children                            m_children;             // ( RUNTIME ) all children attached to the entity

private:
   Children                            m_serializedChildren;   // ( STATIC )  children serialized with the entity

public:
   /**
    * Constructor.
    *
    * @param name
    */
   Entity( const char* name = "Entity" );

   /**
    * Copy constructor.
    */
   Entity( const Entity& rhs );
   virtual ~Entity();

   // -------------------------------------------------------------------------
   // Prefabs support.
   // -------------------------------------------------------------------------

   /**
    * Makes this entity instantiate part of its contents from a prefab.
    *
    * @param prefab
    */
   void setPrefab( Prefab* prefab );

   /**
    * Clones the contents of embedded prefab, turning this entity into its independent instance.
    *
    * If you set 'entireHierarchy' value to 'true', prefabs embedded in this entity's prefab
    * will also be inlined, otherwise the operation will affect the referenced prefab only.
    *
    * CAUTION: works ONLY on entities attached to a model. Those that are not attached don't
    *          have anything from the prefab yet, so there's nothing to inline.
    *
    * @param entireHierarchy
    */
   void makeUnique( bool entireHierarchy = false );

   // -------------------------------------------------------------------------
   // PrefabListener implementation
   // -------------------------------------------------------------------------
   void onEmbeddedEntityChanged( Prefab* prefab );

   // -------------------------------------------------------------------------
   // Transforms management implementation
   // -------------------------------------------------------------------------
   /**
    * Returns parent's world matrix.
    */
   inline const Matrix& getParentWorldMtx() const;

   /**
    * This is the matrix that describes the node's position in relation
    * to the position of its parent.
    * It the node doesn't have a parent, this one will be equal
    * to the global matrix
    */
   const Matrix& getLocalMtx() const { return m_localMtx; }

   /**
    * Assigns the node a new local matrix.
    */
   void setLocalMtx( const Matrix& localMtx );

   /**
    * The method allows to access the matrix of the node directly,
    * skipping the setters.
    * Manipulating the matrix in this way is sometimes necessary
    * as various libs manipulate pointers to matrices. 
    * Not to worry - the global matrix will always remain in sync
    */
   Matrix& accessLocalMtx() { return m_localMtx; }

   /*
    * A group of accessors to the local coordinate system vectors
    */
   void setRightVec( const Vector& vec );
   void setUpVec( const Vector& vec );
   void setLookVec( const Vector& vec );
   void setPosition( const Vector& vec );
   void getRightVec( Vector& outRightVec ) const;
   void getUpVec( Vector& outUpVec ) const;
   void getLookVec( Vector& outLookVec ) const;
   void getPosition( Vector& outPos ) const;

   /*
    * Returns the global coordinate system vectors.
    *
    * @param outRightVec
    * @param outUpVec
    * @param outLookVec
    * @Param outPos
    */
   void getGlobalVectors( Vector& outRightVec, Vector& outUpVec, Vector& outLookVec, Vector& outPos ) const;

   // -------------------------------------------------------------------------
   // Bounding volumes management
   // -------------------------------------------------------------------------
   /**
    * Adds a new component bounding volume to the entity.
    * CAUTION: Volumes are managed externally and the entity.
    *
    * @param volume
    */
   void addBoundingVolume( AxisAlignedBox* volume );

   /**
    * Adds a new component bounding volume to the entity.
    * CAUTION: Volumes are managed externally and the entity.
    *
    * @param volume
    */
   void removeBoundingVolume( AxisAlignedBox* volume );

   /**
    * Returns the bounding volume that bounds the node's contents. 
    * The bounding volume is located in the world space.
    *
    * @return  node's contents bounding volume
    */
   const AxisAlignedBox& getBoundingVolume() const;

   // -------------------------------------------------------------------------
   // Hierarchy management
   // -------------------------------------------------------------------------
   /**
    * Adds a new child to the entity's hierarchy.
    *
    * @param child     new child
    */
   void addChild( SceneNode* child );

   /**
    * This method removes a child from the entity.
    *
    * @param entity     entity we want to remove
    */
   void removeChild( SceneNode* child );

   /**
    * Removes all attached children.
    */
   void clear();

   // -------------------------------------------------------------------------
   // SceneNode implementation
   // -------------------------------------------------------------------------
   void onModelLoaded() override;
   void collectChildren( Stack< SceneNode* >& outChildren ) const override;
   void collectChildren( Stack< const SceneNode* >& outChildren ) const override;
   void collectChildren( List< SceneNode* >& outChildren ) const override;
   void collectChildren( List< const SceneNode* >& outChildren ) const override;
   void updateTransforms() override;
   void onAttachToModel( Model* model ) override;
   void onDetachFromModel( Model* model ) override;
   void pullStructure( ModelView* view ) override;
   void onAttached( Entity* parent ) override;
   void onDetached( Entity* parent ) override;
   inline const Matrix& getGlobalMtx() const override;

   // -------------------------------------------------------------------------
   // Object implementation
   // -------------------------------------------------------------------------
   void onObjectPreSave() override;
   void onPrePropertyChanged( ReflectionProperty& property ) override;
   void onPropertyChanged( ReflectionProperty& property ) override;

protected:
   // -------------------------------------------------------------------------
   // Extra notifications
   // -------------------------------------------------------------------------
   /**
    * Method called when a child node is attached
    * to this entity.
    *
    * @param child      child entity that was attached
    */
   virtual void onChildAttached( SceneNode* child ) {}

   /**
    * Method called when a child node is about to be detached
    * from this entity
    *
    * @param child      child entity about to be detached
    */
   virtual void onChildDetached( SceneNode* child ) {}

private:
   void buildBoundingVolume();
   void instantiatePrefab();
   void deinstantiatePrefab();
};

///////////////////////////////////////////////////////////////////////////////

#include "core-MVC\Entity.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _ENTITY_H
