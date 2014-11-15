#include "core-AI\BlendTreePlayer.h"
#include "core-AI\BlendTree.h"
#include "core-AI\BlendTreeStateMachine.h"
#include "core-AI\BlendTreeEvent.h"
#include "core-AI\BlendTreeVariable.h"
#include "core-AI\BlendTreeStateTransition.h"
#include "core-AI\BlendTreeSyncProfile.h"
#include "core-AI\SkeletonComponent.h"
#include "core-AI\Skeleton.h"
#include "core-AI\BlendTreePlayerListener.h"
#include "core-MVC\EntityUtils.h"
#include "core-MVC\Entity.h"
#include "core\RuntimeData.h"
#include "core\ReflectionProperty.h"
#include "core\Transform.h"
#include "core\ListUtils.h"
#include "core\Log.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( BlendTreePlayer );
   PARENT( AnimationPlayer );
   PROPERTY_EDIT( "Blend tree", BlendTree*, m_blendTree );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

BlendTreePlayer::BlendTreePlayer( const char* name )
   : AnimationPlayer( name )
   , m_blendTree( NULL )
   , m_posesSink( NULL )
   , m_runtimeData( NULL )
   , m_skeleton( NULL )
   , m_bonesCount( 0 )
   , m_eventsCount( 0 )
   , m_eventsReadFrameOffset( 0 )
   , m_eventsWriteFrameOffset( 0 )
   , m_syncData( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////

BlendTreePlayer::BlendTreePlayer( const BlendTreePlayer& rhs )
   : AnimationPlayer( rhs )
   , m_blendTree( rhs.m_blendTree )
   , m_posesSink( NULL )
   , m_runtimeData( NULL )
   , m_skeleton( NULL )
   , m_bonesCount( 0 )
   , m_eventsCount( 0 )
   , m_eventsReadFrameOffset( 0 )
   , m_eventsWriteFrameOffset( 0 )
   , m_syncData( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////

BlendTreePlayer::~BlendTreePlayer()
{
   if ( m_blendTree && m_runtimeData )
   {
      m_blendTree->deinitializeLayout( this );
   }
   delete m_runtimeData;
   m_runtimeData = NULL;

   delete m_syncData;
   m_syncData = NULL;

   m_bonesCount = 0;
   m_skeleton = NULL;
   m_blendTree = NULL;
   m_posesSink = NULL;

   m_listeners.clear();
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreePlayer::setBlendTree( BlendTree& blendTree )
{
   ReflectionProperty* property = getProperty( "m_blendTree" );
   notifyPrePropertyChange( *property );

   // set the new animation source
   m_blendTree = &blendTree;

   // notify listeners about it
   notifyPropertyChange( *property );
   delete property;
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreePlayer::onPrePropertyChanged( ReflectionProperty& property )
{
   AnimationPlayer::onPrePropertyChanged( property );

   if ( property.getName() == "m_blendTree" )
   {
      if ( m_blendTree )
      {
         m_blendTree->detachBlendTreeListener( this );
         m_blendTree->detachListener( *this );

         if ( m_runtimeData )
         {
            m_blendTree->deinitializeLayout( this );
         }
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreePlayer::onPropertyChanged( ReflectionProperty& property )
{
   AnimationPlayer::onPropertyChanged( property );

   if ( property.getName() == "m_blendTree" )
   {
      if ( m_blendTree )
      {
         m_blendTree->attachBlendTreeListener( this );
         m_blendTree->attachListener( *this );

         if ( m_runtimeData )
         {
            // initialize tree's runtime data layout
            m_blendTree->initializeLayout( this );
         }
      }

      initializeEventsArray();
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreePlayer::onObservedPrePropertyChanged( ReflectionProperty& property )
{
   // changes to the SkeletonComponent this component observes
   if ( property.getName() == "m_skeleton" )
   {
      deinitializePosesSinkRuntimeContext();
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreePlayer::onObservedPropertyChanged( ReflectionProperty& property )
{
   // changes to the SkeletonComponent this component observes
   if ( property.getName() == "m_skeleton" )
   {
      initializePosesSinkRuntimeContext();
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreePlayer::onObservedObjectDeleted( ReflectionObject* deletedObject )
{
   if ( deletedObject == m_blendTree )
   {
      NOTIFY_PROPERTY_CHANGE( m_blendTree );
      m_blendTree = NULL;
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreePlayer::initializeBlendTreeRuntimeContext()
{
   ASSERT( isAttached() );
   ASSERT( getHostModel() != NULL );
   ASSERT( m_runtimeData == NULL );

   if ( m_blendTree )
   {
      // initialize the sync data
      m_syncData = new BlendTreeSyncProfile();

      // create a memory pool the blend tree will place its runtime data in
      m_runtimeData = new RuntimeDataBuffer();

      // initialize tree's runtime data layout
      m_blendTree->initializeLayout( this );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreePlayer::deinitializeBlendTreeRuntimeContext()
{
   ASSERT( isAttached() );
   ASSERT( getHostModel() != NULL );

   if ( m_blendTree )
   {
      // deinitialize tree's runtime data layout
      m_blendTree->deinitializeLayout( this );
   }

   delete m_runtimeData;
   m_runtimeData = NULL;

   delete m_syncData;
   m_syncData = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreePlayer::initializePosesSinkRuntimeContext()
{
   if ( !m_posesSink || !m_posesSink->m_skeleton )
   {
      return;
   }

   if ( m_posesSink->m_skeleton == m_skeleton )
   {
      // this is to prevent double initialization when components are being attached to an entity.
      // OnSiblingAttached will be called twice then - first, when the SkeletonComponent is attached,
      // and it informs all other components ( including the player ) about its presence,
      // and the second time - when the BlendTreePlayer component is attached, it will try recognizing
      // what other components are already attached, calling onSiblingAdded for each one.
      return;
   }

   m_skeleton = m_posesSink->m_skeleton;
   m_bonesCount = m_skeleton->getBoneCount();
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreePlayer::deinitializePosesSinkRuntimeContext()
{
   m_skeleton = NULL;
   m_bonesCount = 0;
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreePlayer::initializeEventsArray()
{
   if ( m_blendTree )
   {
      m_eventsCount = m_blendTree->m_events.size();
      m_eventsReadFrameOffset = 0;
      m_eventsWriteFrameOffset = m_eventsCount;
      m_triggeredEvents.resize( m_eventsCount * 2, false );
   }
   else
   {
      m_eventsReadFrameOffset = 0;
      m_eventsWriteFrameOffset = 0;
      m_triggeredEvents.clear();
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreePlayer::onSiblingAttached( SceneNode* node )
{
   AnimationPlayer::onSiblingAttached( node );

   if ( node->isA< SkeletonComponent >() )
   {
      m_posesSink = static_cast< SkeletonComponent* >( node );
      m_posesSink->attachListener( *this );

      initializePosesSinkRuntimeContext();
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreePlayer::onSiblingDetached( SceneNode* node )
{
   AnimationPlayer::onSiblingDetached( node );

   if ( node == m_posesSink )
   {
      deinitializePosesSinkRuntimeContext();

      m_posesSink->detachListener( *this );
      m_posesSink = NULL;
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreePlayer::onAttachToModel( Model* model )
{
   AnimationPlayer::onAttachToModel( model );

   if ( m_blendTree )
   {
      m_blendTree->attachBlendTreeListener( this );
      m_blendTree->attachListener( *this );
   }

   initializeEventsArray();
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreePlayer::onDetachFromModel( Model* model )
{
   AnimationPlayer::onDetachFromModel( model );

   if ( m_blendTree )
   {
      m_blendTree->detachBlendTreeListener( this );
      m_blendTree->detachListener( *this );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreePlayer::onNodeAdded( BlendTreeNode* parentNode, int insertionIdx, BlendTreeNode* node )
{
   if ( m_runtimeData )
   {
      node->initializeLayout( this );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreePlayer::onNodeRemoved( BlendTreeNode* parentNode, BlendTreeNode* node )
{
   if ( m_runtimeData )
   {
      node->deinitializeLayout( this );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreePlayer::onNodeChanged( BlendTreeNode* node )
{
   // nothing to do here
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreePlayer::onTransitionAdded( BlendTreeStateMachine* parentStateMachine, BlendTreeStateTransition* transition )
{
   if ( m_runtimeData )
   {
      transition->initializeLayout( this );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreePlayer::onTransitionRemoved( BlendTreeStateMachine* parentStateMachine, BlendTreeStateTransition* transition )
{
   if ( m_runtimeData )
   {
      transition->deinitializeLayout( this );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreePlayer::onEventAdded( BlendTreeEvent* event )
{
   initializeEventsArray();
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreePlayer::onEventRemoved( BlendTreeEvent* event )
{
   initializeEventsArray();
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreePlayer::onVariableAdded( BlendTreeVariable* btVariable )
{
   // register layout for the new variable
   if ( m_runtimeData )
   {
      btVariable->initializeLayout( this );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreePlayer::onVariableRemoved( BlendTreeVariable* btVariable )
{
   // nothing to do here - we can leave it's layout behind, no need to re-hash the entire
   // memory pool
}

///////////////////////////////////////////////////////////////////////////////

bool BlendTreePlayer::isEnabled() const
{
   return m_posesSink && m_blendTree && m_blendTree->m_skeleton;
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreePlayer::onStarted()
{
   cacheTransforms();

   // rebuild the layout
   initializeBlendTreeRuntimeContext();

   // notify listeners - do it before we activate the tree
   for ( List< BlendTreePlayerListener* >::iterator it = m_listeners.begin(); !it.isEnd(); ++it )
   {
      BlendTreePlayerListener* listener = *it;
      listener->onSimulationStarted( this );
   }

   // activate the tree
   BlendTreeStateMachine& root = m_blendTree->getRoot();
   m_syncData->reset();
   root.activateNode( this );
   root.generateTreeSyncProfile( this, *m_syncData );
   root.synchronizeNodeToTree( this, *m_syncData );
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreePlayer::onFinished()
{
   // deactivate the tree
   BlendTreeStateMachine& rootStateMachine = m_blendTree->getRoot();
   rootStateMachine.deactivateNode( this );

   // delete the runtime layout
   deinitializeBlendTreeRuntimeContext();

   restoreTransforms();

   // notify listeners
   for ( List< BlendTreePlayerListener* >::iterator it = m_listeners.begin(); !it.isEnd(); ++it )
   {
      BlendTreePlayerListener* listener = *it;
      listener->onSimulationFinished( this );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreePlayer::onFrameStart()
{
   // shift the active events frame
   uint tmp = m_eventsReadFrameOffset;
   m_eventsReadFrameOffset = m_eventsWriteFrameOffset;
   m_eventsWriteFrameOffset = tmp;

   // reset event settings
   memset( m_triggeredEvents + m_eventsWriteFrameOffset, false, sizeof( bool ) * m_eventsCount );
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreePlayer::samplePoses( float timeElapsed )
{
   BlendTreeStateMachine& root = m_blendTree->getRoot();

   // update tree logic
   root.updateLogic( this );

   // generate synchronization info
   m_syncData->reset();
   root.generateTreeSyncProfile( this, *m_syncData );

   // synchronize nodes to the tree
   root.synchronizeNodeToTree( this, *m_syncData );

   // sample the pose
   root.samplePose( this, timeElapsed );
   Transform* finalPose = root.getGeneratedPose( this );

   // set the pose on the skeleton component
   Matrix tmpMtx;
   for ( uint i = 0; i < m_bonesCount; ++i )
   {
      finalPose[i].toMatrix( tmpMtx );

      m_posesSink->m_boneLocalMtx[i].setMul( tmpMtx, m_skeleton->m_boneLocalMatrices[i] );
   }

   // get the accumulated motion
   m_accumulatedMotion = root.getAccumulatedMotion( this );
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreePlayer::onFrameEnd()
{
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreePlayer::cacheTransforms()
{
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreePlayer::restoreTransforms()
{
   if ( m_posesSink )
   {
      m_posesSink->resetToTPose();
   }
}

///////////////////////////////////////////////////////////////////////////////

bool BlendTreePlayer::wasEventTriggered( BlendTreeEvent* btEvent ) const
{
   uint eventIdx = btEvent->getIndex();
   return m_triggeredEvents[m_eventsReadFrameOffset + eventIdx];
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreePlayer::triggerEvent( BlendTreeEvent* btEvent )
{
   uint eventIdx = btEvent->getIndex();
   m_triggeredEvents[m_eventsWriteFrameOffset + eventIdx] = true;

   // notify listeners
   for ( List< BlendTreePlayerListener* >::iterator it = m_listeners.begin(); !it.isEnd(); ++it )
   {
      BlendTreePlayerListener* listener = *it;
      listener->onEventTriggered( this, btEvent );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreePlayer::attachListener( BlendTreePlayerListener* listener )
{
   List< BlendTreePlayerListener* >::iterator it = ListUtils::find( m_listeners, listener );
   if ( it.isEnd() )
   {
      m_listeners.pushBack( listener );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreePlayer::detachListener( BlendTreePlayerListener* listener )
{
   List< BlendTreePlayerListener* >::iterator it = ListUtils::find( m_listeners, listener );
   if ( !it.isEnd() )
   {
      it.markForRemoval();
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreePlayer::pullStructure( BlendTreePlayerListener* listener )
{
   BlendTreeStateMachine& root = m_blendTree->getRoot();
   root.pullRuntimeStructure( this, listener );
}

///////////////////////////////////////////////////////////////////////////////
