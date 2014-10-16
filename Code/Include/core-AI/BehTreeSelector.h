/// @file   core-AI\BehTreeSelector.h
/// @brief  a decorator that executes its child only when the condition evaluates true
#pragma once

#include "core-AI\BehTreeComposite.h"
#include "core\RuntimeData.h"


///////////////////////////////////////////////////////////////////////////////

class BehTreeCondition;

///////////////////////////////////////////////////////////////////////////////

class BehTreeSelector : public BehTreeComposite
{
   DECLARE_ALLOCATOR( BehTreeSelector, AM_DEFAULT );
   DECLARE_CLASS();

private:
   BehTreeCondition*             m_condition;

   TRuntimeVar< int >            m_activeIdx;

public:
   /**
    * Constructor.
    */
   BehTreeSelector();
   ~BehTreeSelector();

   /**
    * Sets the condition the decorator should run.
    *
    * @param condition.
    */
   void setCondition( BehTreeCondition* condition );

   // -------------------------------------------------------------------------
   // BehTreeAction implementation
   // -------------------------------------------------------------------------
   void createLayout( BehaviorTreeRunner& runner ) const;
   void initialize( BehaviorTreeRunner& runner ) const;
   void deinitialize( BehaviorTreeRunner& runner ) const;
   Result execute( BehaviorTreeRunner& runner ) const;
};

///////////////////////////////////////////////////////////////////////////////
