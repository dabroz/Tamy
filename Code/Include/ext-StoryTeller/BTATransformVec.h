/// @file   ext-StoryTeller\BTATransformVec.h
/// @brief  transforms a vector into a different reference space
#pragma once

#include "core-AI\BehTreeAction.h"
#include "core\RuntimeData.h"


///////////////////////////////////////////////////////////////////////////////

class BehTreeVarVector;

///////////////////////////////////////////////////////////////////////////////

enum BTAVecTranformSpace
{
   TS_Local_To_Camera2D,
   TS_Local_To_Camera3D,
};

///////////////////////////////////////////////////////////////////////////////

class BTATransformVec : public BehTreeAction
{
   DECLARE_ALLOCATOR( BTATransformVec, AM_DEFAULT );
   DECLARE_CLASS();

private:
   BehTreeVarVector*          m_inVec;
   BehTreeVarVector*          m_outVec;
   BTAVecTranformSpace        m_transformSpace;

public:
   /**
    * Constructor.
    */
   BTATransformVec();
   ~BTATransformVec();

   // -------------------------------------------------------------------------
   // BehTreeAction implementation
   // -------------------------------------------------------------------------
   Result execute( BehaviorTreeRunner& runner ) const;
};

///////////////////////////////////////////////////////////////////////////////

