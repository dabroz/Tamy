#include "ext-StoryTeller\BTATransformVec.h"
#include "ext-StoryTeller\StoryBehTreeContext.h"
#include "ext-StoryTeller\StoryPlayer.h"
#include "core-AI\BehaviorTreeRunner.h"
#include "core-AI\BehTreeVariable.h"
#include "core-Renderer\Renderer.h"
#include "core-Renderer\Camera.h"
#include "core\Matrix.h"
#include "core\Plane.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_ENUM( BTAVecTranformSpace );
   ENUM_VAL( TS_Local_To_Camera2D );
   ENUM_VAL( TS_Local_To_Camera3D );
END_ENUM();

///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( BTATransformVec );
   PARENT( BehTreeAction );
   PROPERTY_EDIT_REFERENCE( "Input vec", BehTreeVarVector*, m_inVec );
   PROPERTY_EDIT_REFERENCE( "Output vec", BehTreeVarVector*, m_outVec );
   PROPERTY_EDIT( "Transform space", BTAVecTranformSpace, m_transformSpace );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

BTATransformVec::BTATransformVec()
   : m_inVec( NULL )
   , m_outVec( NULL )
   , m_transformSpace( TS_Local_To_Camera2D )
{
   registerReferences();
}

///////////////////////////////////////////////////////////////////////////////

BTATransformVec::~BTATransformVec()
{
}

///////////////////////////////////////////////////////////////////////////////

BehTreeNode::Result BTATransformVec::execute( BehaviorTreeRunner& runner ) const
{     
   if ( !m_inVec|| !m_outVec )
   {
      return FAILED;
   }

   RuntimeDataBuffer& data = runner.data();

   StoryBehTreeContext* context = (StoryBehTreeContext*)runner.getContext();
   Camera& camera = context->m_player.renderer()->getActiveCamera();

   switch( m_transformSpace )
   {
   case TS_Local_To_Camera2D:
      {
         Matrix camWorld = camera.getGlobalMtx();
         camWorld.setPosition<4>( Vector_OW );
         const Vector& input = m_inVec->getRuntime( &runner );

         Quaternion localToWorldQuat;
         {
            const Vector& camForwardVec = camWorld.forwardVec();

            Plane transformPlane; transformPlane.set( Vector_OZ, Float_0 );
            FastFloat forwardDist = transformPlane.dotNormal( camForwardVec ); forwardDist.neg();

            Vector camForwardVec2D;
            camForwardVec2D.setMulAdd( Vector_OZ, forwardDist, camForwardVec );

            localToWorldQuat.setFromShortestRotation( Vector_OX, camForwardVec2D );
         }


         Vector output;
         localToWorldQuat.transform( input, output );
         m_outVec->setRuntime( &runner, output );

         break;
      }

   case TS_Local_To_Camera3D:
      {
         Matrix camWorld = camera.getGlobalMtx();
         camWorld.setPosition<4>( Vector_OW );
         const Vector& input = m_inVec->getRuntime( &runner );

         Vector output;
         camWorld.transform( input, output );
         m_outVec->setRuntime( &runner, output );

         break;
      }

   }

   return FINISHED;
}

///////////////////////////////////////////////////////////////////////////////
