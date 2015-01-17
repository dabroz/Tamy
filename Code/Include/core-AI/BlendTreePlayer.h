/// @file   core-AI/BlendTreePlayer.h
/// @brief  this animation player will play a blend tree on the entity it's attached to
#pragma once

#include "core-AI\AnimationPlayer.h"
#include "core-AI\BlendTreeListener.h"
#include "core\Matrix.h"
#include "core\Transform.h"
#include "core\ReflectionObjectChangeListener.h"
#include "core\List.h"


///////////////////////////////////////////////////////////////////////////////

class BlendTree;
class BlendTreeEvent;
class SkeletonComponent;
class Skeleton;
class Entity;
class RuntimeDataBuffer;
struct Transform;
class BlendTreeSyncProfile;
class BlendTreePlayerListener;
class SkeletonMapper;
class SkeletonMapperRuntime;

///////////////////////////////////////////////////////////////////////////////

/**
 * This animation player will play a blend tree on the entity it's attached to.
 */
class BlendTreePlayer : public AnimationPlayer, public ReflectionObjectChangeListener, public BlendTreeListener
{
   DECLARE_ALLOCATOR( BlendTreePlayer, AM_DEFAULT );
   DECLARE_CLASS();

public:
   // static data
   BlendTree*                                m_blendTree;
   const SkeletonMapper*                     m_skeletonMapper;

   // runtime data
   List< BlendTreePlayerListener* >          m_listeners;

   Transform                                 m_accumulatedMotion;
   SkeletonComponent*                        m_posesSink;
   Skeleton*                                 m_skeleton;
   uint                                      m_bonesCount;
   BlendTreeSyncProfile*                     m_syncData;

private:
   RuntimeDataBuffer*                        m_runtimeData;
   SkeletonMapperRuntime*                    m_skeletonMapperRuntime;

   // ------------------------------------------
   // Events

   // Array that records triggered events. It's a double buffer - it stores twice as many
   // entires as there are events, and the start index is switched every frame 
   // to point at the record of events triggered the last frame
   Array< bool >                             m_triggeredEvents;

   uint                                      m_eventsCount;
   uint                                      m_eventsReadFrameOffset;
   uint                                      m_eventsWriteFrameOffset;
   // ------------------------------------------

public:
   /**
    * Constructor.
    *
    * @param name
    */
   BlendTreePlayer( const char* name = "BlendTreePlayer" );

   /**
    * Copy constructor.
    */
   BlendTreePlayer( const BlendTreePlayer& rhs );
   ~BlendTreePlayer();

   /**
    * Sets a new blend tree instance.
    *
    * @param blendTree
    */
   void setBlendTree( BlendTree& blendTree );

   /**
    * Assigns a skeleton mapper to be used translating the poses the blend produces to those
    * used by the model this component is attached to.
    *
    * @param skeletonMapper
    */
   void setSkeletonMapper( const SkeletonMapper* skeletonMapper );

   /**
    * Returns a blend tree instance this component is using.
    */
   inline BlendTree* getBlendTree() const { return m_blendTree; }

   /**
    * Returns the runtime data buffer used during simulation.
    */
   inline RuntimeDataBuffer& data() { return *m_runtimeData; }

   /**
    * Returns the skeleton this player operates on.
    */
   inline Skeleton* getSkeleton() const { return m_skeleton; }

   /**
    * Returns the number of bones in the skeleton.
    */
   inline uint getBoneCount() const { return m_bonesCount; }

   /**
    * Returns the latest synchronization information the tree should use to synchronize itself.
    */
   inline const BlendTreeSyncProfile& getSyncData() const { return *m_syncData; }

   /**
    * Puts the node on the list of nodes that need to be force-synchronized
    * once the tree sync profile for this frame has been generated.
    *
    * @param nodeToSync
    */
   void forceSynchronization( const BlendTreeNode* nodeToSync );

   // -------------------------------------------------------------------------
   // Listeners management
   // -------------------------------------------------------------------------
   /**
    * Attaches a new listener.
    *
    * @param listener
    */
   void attachListener( BlendTreePlayerListener* listener );

   /**
    * Detaches a listener.
    *
    * @param listener
    */
   void detachListener( BlendTreePlayerListener* listener );

   /**
    * Pulls the runtime state of the tree, feeding it to the listener.
    *
    * @param listener
    */
   void pullStructure( BlendTreePlayerListener* listener );

   // -------------------------------------------------------------------------
   // Events
   // -------------------------------------------------------------------------
   /**
    * Checks if the specified event was triggered last frame.
    *
    * @param btEvent
    */
   bool wasEventTriggered( BlendTreeEvent* btEvent ) const;

   /**
    * Triggers the specified event. The event will become active in the next frame,
    * together with all the other events triggered this frame.
    *
    * @param btEvent
    */
   void triggerEvent( BlendTreeEvent* btEvent );

   // -------------------------------------------------------------------------
   // AnimationPlayer implementation
   // -------------------------------------------------------------------------
   bool isEnabled() const override;
   void onFrameStart() override;
   void samplePoses( float deltaTime ) override;
   void onFrameEnd() override;
   void onStarted() override;
   void onFinished() override;

   // -------------------------------------------------------------------------
   // ReflectionObjectChangeListener implementation
   // -------------------------------------------------------------------------
   void onObservedPrePropertyChanged( ReflectionProperty& property ) override;
   void onObservedPropertyChanged( ReflectionProperty& property ) override;
   void onObservedObjectDeleted( ReflectionObject* deletedObject ) override;

   // -------------------------------------------------------------------------
   // BlendTreeListener implementation
   // -------------------------------------------------------------------------
   void onNodeAdded( BlendTreeNode* parentNode, int insertionIdx, BlendTreeNode* node ) override;
   void onNodeRemoved( BlendTreeNode* parentNode, BlendTreeNode* node ) override;
   void onNodeChanged( BlendTreeNode* node ) override;
   void onTransitionAdded( BlendTreeStateMachine* parentStateMachine, BlendTreeStateTransition* transition ) override;
   void onTransitionRemoved( BlendTreeStateMachine* parentStateMachine, BlendTreeStateTransition* transition ) override;
   void onEventAdded( BlendTreeEvent* event ) override;
   void onEventRemoved( BlendTreeEvent* event ) override;
   void onVariableAdded( BlendTreeVariable* btVariable ) override;
   void onVariableRemoved( BlendTreeVariable* btVariable ) override;

protected:
   // -------------------------------------------------------------------------
   // SceneNode implementation
   // -------------------------------------------------------------------------
   void onSiblingAttached( SceneNode* node ) override;
   void onSiblingDetached( SceneNode* node ) override;
   void onAttachToModel( Model* model ) override;
   void onDetachFromModel( Model* model ) override;

   // -------------------------------------------------------------------------
   // ReflectionObject implementation
   // -------------------------------------------------------------------------
   void onPrePropertyChanged( ReflectionProperty& property ) override;
   void onPropertyChanged( ReflectionProperty& property ) override;

private:
   void initializeBlendTreeRuntimeContext();
   void deinitializeBlendTreeRuntimeContext();
   void initializeSkeletonMapper();
   void deinitializeSkeletonMapper();
   void initializePosesSinkRuntimeContext();
   void deinitializePosesSinkRuntimeContext();
   void initializeEventsArray();
   void cacheTransforms();
   void restoreTransforms();
};

///////////////////////////////////////////////////////////////////////////////

