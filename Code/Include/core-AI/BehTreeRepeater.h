/// @file   core-AI/BehTreeRepeater.h
/// @brief  this node will repeat executing decorated child node
#pragma once

#include "core-AI\BehTreeDecorator.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * This node will repeat executing decorated child node.
 */
class BehTreeRepeater : public BehTreeDecorator
{
   DECLARE_ALLOCATOR( BehTreeRepeater, AM_DEFAULT );
   DECLARE_CLASS();

private:
   bool                 m_breakOnFailure;

public:
   /**
    * Constructor.
    *
    * @param breakOnFailure
    */
   BehTreeRepeater( bool breakOnFailure = true );

   // ----------------------------------------------------------------------
   // BehTreeNode implementation
   // ----------------------------------------------------------------------
   Result execute( BehaviorTreeRunner& runner ) const;
};

///////////////////////////////////////////////////////////////////////////////
