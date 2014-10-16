/// @file   core-AI/BehTreeParallel.h
/// @brief  a composite node that executes its children in one go
#pragma once

#include "core-AI\BehTreeComposite.h"
#include "core\RuntimeData.h"
#include "core\types.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * A composite node that executes its children in once go.
 *
 * If any of the children return failure, parallel node also returns failure.
 */
class BehTreeParallel : public BehTreeComposite
{
   DECLARE_ALLOCATOR( BehTreeParallel, AM_DEFAULT );
   DECLARE_CLASS();

private:
   TRuntimeVar< bool* >          m_finishedNodes;

public:
   // ----------------------------------------------------------------------
   // BehTreeNode implementation
   // ----------------------------------------------------------------------
   void createLayout( BehaviorTreeRunner& runner ) const;
   void initialize( BehaviorTreeRunner& runner ) const;
   void deinitialize( BehaviorTreeRunner& runner ) const;
   Result execute( BehaviorTreeRunner& runner ) const;
};

///////////////////////////////////////////////////////////////////////////////
