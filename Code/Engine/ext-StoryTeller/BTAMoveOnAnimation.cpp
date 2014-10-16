#include "ext-StoryTeller\BTAMoveOnAnimation.h"
#include "ext-StoryTeller\StoryBehTreeContext.h"
#include "ext-StoryTeller\StoryNodeInstance.h"
#include "ext-StoryTeller\StoryPlayer.h"
#include "core-Physics\PhysicsCharacterController.h"
#include "core-AI\BehaviorTreeRunner.h"
#include "core-AI\BehTreeVariable.h"
#include "core-AI\BlendTree.h"
#include "core-AI\BlendTreePlayer.h"
#include "core-AI\BlendTreeVariable.h"
#include "core-Renderer\Renderer.h"
#include "core-MVC\EntityUtils.h"
#include "core\TimeController.h"
#include "core\Math.h"
#include "core\Log.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( BTAMoveOnAnimation );
   PARENT( BehTreeAction );
   PROPERTY_EDIT_REFERENCE( "Input velocity", BehTreeVarVector*, m_velocity );
   PROPERTY_EDIT( "Linear velocity var", std::string, m_linearSpeedVarName );
   PROPERTY_EDIT( "Character fwd vec", Vector, m_characterFwd );
   PROPERTY_EDIT( "Direction var", std::string, m_directionVarName );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

BTAMoveOnAnimation::BTAMoveOnAnimation()
   : m_linearSpeedVarName( "linearVelocity" )
   , m_directionVarName( "direction" )
   , m_characterFwd( Vector_OX )
   , m_velocity( NULL )
{
   registerReferences();
}

///////////////////////////////////////////////////////////////////////////////

BTAMoveOnAnimation::~BTAMoveOnAnimation()
{
}

///////////////////////////////////////////////////////////////////////////////

void BTAMoveOnAnimation::createLayout( BehaviorTreeRunner& runner ) const
{
   RuntimeDataBuffer& data = runner.data();
   data.registerVar( m_blendTreePlayer );
   data.registerVar( m_linearSpeedVar );
   data.registerVar( m_directionVar );
}

///////////////////////////////////////////////////////////////////////////////

void BTAMoveOnAnimation::initialize( BehaviorTreeRunner& runner ) const
{
   RuntimeDataBuffer& data = runner.data();

   StoryBehTreeContext* context = ( StoryBehTreeContext* ) runner.getContext();
   StoryNodeInstance* controlledNodeInstance = context->m_ownerInstance;

   BlendTreePlayer* blendTreePlayer = EntityUtils::getFirstChild< BlendTreePlayer >( controlledNodeInstance );
   data[m_blendTreePlayer] = blendTreePlayer;

   data[m_linearSpeedVar] = NULL;
   data[m_directionVar] = NULL;
   if ( blendTreePlayer )
   {
      BlendTree* blendTree = blendTreePlayer->getBlendTree();
      if ( blendTree )
      {
         const BTVarFloat* var = blendTree->getVariable< BTVarFloat >( m_linearSpeedVarName );
         data[m_linearSpeedVar] = var;
         if ( !var )
         {
            LOG( "BTAMoveWithVelocity: The character doesn't have a BlendTreePlayer component or the tree has no '%s' variable defined", m_linearSpeedVarName.c_str() );
         }

         var = blendTree->getVariable< BTVarFloat >( m_directionVarName );
         data[m_directionVar] = var;
         if ( !var )
         {
            LOG( "BTAMoveWithVelocity: The character doesn't have a BlendTreePlayer component or the tree has no '%s' variable defined", m_directionVarName.c_str() );
         }
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void BTAMoveOnAnimation::deinitialize( BehaviorTreeRunner& runner ) const
{
   RuntimeDataBuffer& data = runner.data();
   data[m_blendTreePlayer] = NULL;
   data[m_blendTreePlayer] = NULL;
   data[m_linearSpeedVar] = NULL;
}

///////////////////////////////////////////////////////////////////////////////

BehTreeNode::Result BTAMoveOnAnimation::execute( BehaviorTreeRunner& runner ) const
{
   RuntimeDataBuffer& data = runner.data();

   BlendTreePlayer* blendTreePlayer = data[m_blendTreePlayer];

   if ( !m_velocity )
   {
      LOG( "BTAMoveOnAnimation: Assign a velocity variable" );
      return FAILED;
   }

   if ( !blendTreePlayer )
   {
      LOG( "BTAMoveOnAnimation: The character doesn't have a BlendTreePlayer component" );
      return FAILED;
   }

   StoryBehTreeContext* context = ( StoryBehTreeContext* ) runner.getContext();

   const Matrix& characterWorldMtx = context->m_ownerInstance->getGlobalMtx();
   Quaternion modelToWorld, worldToModel;
   characterWorldMtx.getRotation( modelToWorld );
   worldToModel.setConjugated( modelToWorld );

   const Vector& velocityWorldSpace = m_velocity->getRuntime( &runner );

   Vector velocityModelSpace;
   worldToModel.transform( velocityWorldSpace, velocityModelSpace );

   const BTVarFloat* linearSpeedVar = data[m_linearSpeedVar];
   if ( linearSpeedVar )
   {
      linearSpeedVar->setRuntime( blendTreePlayer, velocityModelSpace.length().getFloat() );
   }

   const BTVarFloat* directionVar = data[m_directionVar];
   if ( directionVar )
   {
      FastFloat angle;
      VectorUtil::calcAngle( m_characterFwd, velocityModelSpace, Vector_OZ, angle );

      directionVar->setRuntime( blendTreePlayer, RAD2DEG( angle.getFloat() ) );
   }

   return FINISHED;
}

///////////////////////////////////////////////////////////////////////////////

