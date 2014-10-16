/// @file   core-MVC/EntityUtils.h
/// @brief  various utilities that operate on hierarchies of entities
#ifndef _ENTITY_UTILS_H
#define _ENTITY_UTILS_H

#include "core\List.h"
#include "core\Array.h"
#include <string>


///////////////////////////////////////////////////////////////////////////////

class Entity;
class SceneNode;
class Model;

///////////////////////////////////////////////////////////////////////////////

/**
 * Various utilities that operate on hierarchies of entities.
 */
class EntityUtils
{
public:
   /**
    * Looks for a SceneNode with the specified name and returns
    * the first one that meets that criteria, or NULL if none is found.
    *
    * CAUTION: Even though the method returns a pointer, it doesn't relinquish the
    * the lifetime control over the entity - it's just for the sake of being able
    * to return NULL.
    *
    * @param node          node we start the search from
    * @param name          name of the node we're looking for
    */
   static SceneNode* findNodeByName( SceneNode* node, const std::string& name );

   /**
    * Recursively collects all entities of the specified type that span underneath the specified node ( itself being included in the search ).
    *
    * @param node        node we start the search from
    * @param outNodes
    */
   template< typename NodeType >
   static void collectNodesByType( SceneNode* node, List< NodeType* >& outNodes );

   /**
    * Recursively collects all entities of the specified type that span underneath the specified node ( itself being included in the search ).
    *
    * @param node        node we start the search from
    * @param outNodes
    */
   template< typename NodeType >
   static void collectNodesByType( SceneNode* node, Array< NodeType* >& outNodes );

   /**
   * Recursively collects all entities of the specified type that span underneath the specified node ( itself being included in the search ) ( const version )
   *
   * @param node        node we start the search from
   * @param outNodes
   */
   template< typename NodeType >
   static void collectNodesByType( const SceneNode* node, List< const NodeType* >& outNodes );

   /**
   * Recursively collects all entities of the specified type that span underneath the specified node ( itself being included in the search ) ( const version )
   *
   * @param node        node we start the search from
   * @param outNodes
   */
   template< typename NodeType >
   static void collectNodesByType( const SceneNode* node, Array< const NodeType* >& outNodes );

   /**
    * Looks for the first child node that matches the specified type.
    *
    * @param NodeType
    * @param node        node we start the search from
    */
   template< typename NodeType >
   static NodeType* getFirstChild( SceneNode* node );

   /**
    * Looks for the first child node that matches the specified type ( const version )
    *
    * @param NodeType
    * @param node        node we start the search from
    */
   template< typename NodeType >
   static const NodeType* getFirstChild( const SceneNode* node );

   /**
    * Looks for the first parent node that matches the specified type.
    *
    * @param NodeType
    * @param node        node we start the search from
    */
   template< typename NodeType >
   static NodeType* getFirstParent( SceneNode* node );

   /**
    * Searches for the topmost parent of this node that came from a prefab.
    *
    * @param node
    */
   static Entity* getFirstPrefabbedParent( SceneNode* node );

   /**
    * Searches for the topmost parent of this node that came from a prefab.
    *
    * @param node
    */
   static Entity* getTopmostPrefabbedParent( SceneNode* node );

   /**
    * Collects all nodes for which this entity is a parent at some stage ( recursive collection ).
    *
    * @param entity        entity we start the search from
    * @param outChildren
    */
   static void collectChildren( Entity* entity, List< SceneNode* >& outChildren );

};

///////////////////////////////////////////////////////////////////////////////

#include "core-MVC\EntityUtils.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _ENTITY_UTILS_H
