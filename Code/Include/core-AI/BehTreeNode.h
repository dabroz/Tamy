/// @file   core-AI/BehTreeNode.h
/// @brief  base behavior tree node
#pragma once

#include "core\ReflectionObject.h"
#include <string>


///////////////////////////////////////////////////////////////////////////////

class BehaviorTreeRunner;
class BehaviorTreeListener;
class BehaviorTree;

///////////////////////////////////////////////////////////////////////////////

/**
 * Base behavior tree node.
 */
class BehTreeNode : public ReflectionObject
{
   DECLARE_ALLOCATOR( BehTreeNode, AM_DEFAULT );
   DECLARE_CLASS();

public:
   enum Result
   {
      IN_PROGRESS,
      FINISHED,
      FAILED,
   };

protected:
   BehaviorTree*              m_tree;
   std::string                m_name;

public:
   virtual ~BehTreeNode() {}

   /**
    * Sets the instance of the tree that owns this node.
    *
    * @param tree
    */
   void setHostTree( BehaviorTree* tree );

   /**
    * Gives the node a new name
    *
    * @param nodeName
    */
   void setName( const char* nodeName );

   /**
    * Returns the node's name.
    */
   inline const std::string& getName() const { return m_name; }

   /**
    * Initializes the runtime variables layout for this node.
    *
    * @param runner
    */
   virtual void createLayout( BehaviorTreeRunner& runner ) const = 0;

   /**
    * Initializes the tree from a context of the specified runner.
    *
    * @param runner
    */
   virtual void initialize( BehaviorTreeRunner& runner ) const = 0;

   /**
    * Deinitializes the tree from a context of the specified runner.
    *
    * @param runner
    */
   virtual void deinitialize( BehaviorTreeRunner& runner ) const = 0;

   /**
    * Executes the node's functionality.
    *
    * @param runner
    */
   virtual Result execute( BehaviorTreeRunner& runner ) const = 0;

   /**
    * Called in order to push the information about the tree structure to the specified listener.
    *
    * @param listener
    */
   virtual void pullStructure( BehaviorTreeListener* listener ) = 0;

   // -------------------------------------------------------------------------
   // ReflectionObject implementation
   // -------------------------------------------------------------------------
   void onPropertyChanged( ReflectionProperty& property );

protected:
   /**
    * Constructor.
    */
   BehTreeNode();

   /**
    * Called when a new host tree instance is set on the node.
    *
    * @param tree
    */
   virtual void onHostTreeSet( BehaviorTree* tree ) = 0;
};

///////////////////////////////////////////////////////////////////////////////
