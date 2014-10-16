/// @file   ext-StoryTeller/BTAInputToWorldPos.h
/// @brief  this action handles user's input
#pragma once

#include "core-AI\BehTreeAction.h"
#include "core\RuntimeData.h"
#include "core-Renderer\RendererSelectionListener.h"
#include "core\Vector.h"


///////////////////////////////////////////////////////////////////////////////

class StoryNodeInstance;
class PlayerSceneQuery;
class BehTreeVarVector;
class BehTreeVarReflectionObject;
struct StoryBehTreeContext;

///////////////////////////////////////////////////////////////////////////////

/**
 * This action handles user's input.
 */
class BTAInputToWorldPos : public BehTreeAction
{
   DECLARE_ALLOCATOR( BTAInputToWorldPos, AM_DEFAULT );
   DECLARE_CLASS();

   friend class PlayerSceneQuery;

private:
   // static data
   BehTreeVarVector*                         m_worldPos;
   BehTreeVarReflectionObject*               m_selectedObject;

   // runtime data
   TRuntimeVar< PlayerSceneQuery* >          m_posQuery;

public:
   /**
    * Constructor.
    */
   BTAInputToWorldPos();
   ~BTAInputToWorldPos();

   // -------------------------------------------------------------------------
   // BehTreeAction implementation
   // -------------------------------------------------------------------------
   void createLayout( BehaviorTreeRunner& runner ) const;
   void initialize( BehaviorTreeRunner& runner ) const;
   void deinitialize( BehaviorTreeRunner& runner ) const;
   Result execute( BehaviorTreeRunner& runner ) const;
};

///////////////////////////////////////////////////////////////////////////////

/**
 * Scene query used by the BTAInputToWorldPos action.
 */
class PlayerSceneQuery : public ReflectionObject, public RendererSelectionListener
{
   DECLARE_ALLOCATOR( PlayerSceneQuery, AM_DEFAULT );
   DECLARE_CLASS();

public:
   enum State
   {
      READY,
      WAITING,
      RESULT_AVAILABLE
   };

public:
   State                   m_state;

   StoryNodeInstance*      m_selectedNodeInstance;
   Vector                  m_clickedWorldPos;

public:
   /**
    * Constructor.
    */
   PlayerSceneQuery();

   /**
    * Issues a query.
    *
    * @param context.
    */
   void issue( StoryBehTreeContext& context );

   /**
    * Commits query results to the specified runtime context.
    *
    * @param runner
    * @param hostAction
    */
   void commitResults( BehaviorTreeRunner& runner, const BTAInputToWorldPos& hostAction );

   // -------------------------------------------------------------------------
   // RendererSelectionListener implementation
   // -------------------------------------------------------------------------
   void onEntitiesSelected( const List< Entity* >& entities );
   void onWorldPositionClicked( const Vector& worldPos );
};

///////////////////////////////////////////////////////////////////////////////
