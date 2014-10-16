/// @file   core-AI\BehTreeSequence.h
/// @brief  a composite node that executes its children in a sequence
#pragma once

#include "core-AI\BehTreeComposite.h"
#include "core\RuntimeData.h"
#include "core\types.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * A composite node that executes its children in a sequence.
 */
class BehTreeSequence : public BehTreeComposite
{
   DECLARE_ALLOCATOR( BehTreeSequence, AM_DEFAULT );
   DECLARE_CLASS();

private:
   // static data
   bool                       m_neverFail;      // overrides the output of the sequence, always 
                                                // signaling that the section has evaluated positive.
                                                // This mechanism is useful if we have a sequence of subsections - 
                                                // all subsections should be executed, no matter if any of them fails.

   // runtime data
   TRuntimeVar< uint >        m_activeNodeIdx;

public:
   /**
    * Constructor.
    */
   BehTreeSequence();

   // ----------------------------------------------------------------------
   // BehTreeNode implementation
   // ----------------------------------------------------------------------
   void createLayout( BehaviorTreeRunner& runner ) const;
   void initialize( BehaviorTreeRunner& runner ) const;
   void deinitialize( BehaviorTreeRunner& runner ) const;
   Result execute( BehaviorTreeRunner& runner ) const;
};

///////////////////////////////////////////////////////////////////////////////
