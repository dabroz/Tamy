#include "ext-StoryTeller\BTAClosestActorInDirection.h"
#include "ext-StoryTeller\StoryBehTreeContext.h"
#include "ext-StoryTeller\StoryNodeInstance.h"
#include "ext-StoryTeller\StoryPlayer.h"
#include "ext-StoryTeller\StoryActor.h"
#include "core-AI\BehaviorTreeRunner.h"
#include "core-AI\BehTreeVariable.h"
#include "core-MVC\EntityUtils.h"

// utils
#include "core\BestResultsList.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( BTAClosestActorInDirection );
   PARENT( BehTreeAction );
   PROPERTY_EDIT( "Facing [deg, local space]", float, m_facingLocalSpace );
   PROPERTY_EDIT( "Query cone half-width [deg]", float, m_queryConeHalfWidth );
   PROPERTY_EDIT( "Actor tag", std::string, m_actorTag );
   PROPERTY_EDIT( "Max range", float, m_maxRange );
   PROPERTY_EDIT_REFERENCE( "Result [out]", BehTreeVarReflectionObject*, m_actorFound );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

BTAClosestActorInDirection::BTAClosestActorInDirection()
   : m_facingLocalSpace( 0.f )
   , m_queryConeHalfWidth( 30.f )
   , m_maxRange( 20.f )
   , m_actorFound( NULL )
{
   registerReferences();
}

///////////////////////////////////////////////////////////////////////////////

BTAClosestActorInDirection::~BTAClosestActorInDirection()
{
}

///////////////////////////////////////////////////////////////////////////////

BehTreeNode::Result BTAClosestActorInDirection::execute( BehaviorTreeRunner& runner ) const
{
   StoryBehTreeContext* context = ( StoryBehTreeContext* ) runner.getContext();

   List< StoryNodeInstance* > instances;
   context->m_player.collectInstances( m_actorTag, instances );
   if ( instances.empty() )
   {
      return FAILED;
   }

   // get the position and the desired query direction ( in world space )
   const Vector& parentActorPos = context->m_ownerInstance->getGlobalMtx().position();
   Vector queryDir;
   {
      Quaternion currFacing;
      context->m_ownerInstance->getGlobalMtx().getRotation( currFacing );

      Quaternion queryDirLS;
      queryDirLS.setAxisAngle( Vector_OZ, FastFloat::fromFloat( m_facingLocalSpace ) );

      Quaternion queryDirWS;
      queryDirWS.setMul( queryDirLS, currFacing );

      queryDirWS.transform( Vector_OX, queryDir );
   }


   // run through all collected nodes and find the ones located inside the query cone
   BestResultsList< float, StoryNodeInstance* > bestCandidates;
   bestCandidates.init( 2 );
   const FastFloat queryConeHalfWidth = FastFloat::fromFloat( DEG2RAD( m_queryConeHalfWidth ) );
   const float maxRangeSq = m_maxRange*m_maxRange;
   Vector dirToOtherActor;
   for ( List< StoryNodeInstance* >::iterator it = instances.begin(); !it.isEnd(); ++it )
   {
      StoryNodeInstance* node = *it;
      if ( node == context->m_ownerInstance )
      {
         // don't analyze oneself
         continue;
      }

      const Vector& otherActorPos = node->getGlobalMtx().position();
      dirToOtherActor.setSub( otherActorPos, parentActorPos );

      const float distToActor = dirToOtherActor.lengthSq().getFloat();
      if ( distToActor > maxRangeSq )
      {
         // that actor's too far away
         continue;
      }
      dirToOtherActor.normalize();

      const FastFloat angularDiff = dirToOtherActor.dot( queryDir );
      if ( angularDiff >= queryConeHalfWidth )
      {
         // the actor lies inside our cone of interest.

         // We're doing the 'greater than' comparison, 'cause the dot product will return 1
         // if the angle == 0, and a number <1 for every greater angle, so the nodes
         // that would fall outside our cone of interest would present a value that's lower
         // than our threshold value
         bestCandidates.insert( distToActor, node );
      }
   }

   // get the selected actor, if any
   if ( bestCandidates.empty() )
   {
      // no actors were found
      return FAILED;
   }

   // expose the found actor
   StoryNodeInstance* actorInstance = bestCandidates.front();
   m_actorFound->setRuntime( &runner, actorInstance );

   return FINISHED;

}

///////////////////////////////////////////////////////////////////////////////
