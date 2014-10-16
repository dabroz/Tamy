/// @file   TamyEditor/BlendTreeEditor.h
/// @brief  a blend tree resource editor
#pragma once

#include "ResourceEditor.h"
#include "core\TimeDependent.h"
#include "core\ReflectionObjectChangeListener.h"
#include "core\Transform.h"


///////////////////////////////////////////////////////////////////////////////

class BlendTree;
class BlendTreePlayer;
class BlendTreeWidget;
class BlendTreeStateMachine;
class TamySceneWidget;
class QPropertiesView;
class QTreeWidgetItem;
class BlendTreeWidgetItem;
class QVBoxLayout;
class Model;
class Entity;
class Camera;
class OrbitingCameraController;
class DirectionalLight;
class SkeletonComponent;
class GeometryComponent;
class BlendTreeVariablesViewer;
class BlendTreeEventsViewer;
class BlendTreeEditorBottomFrame;
class AnimationEventsEditor;
class AnimationPlayer;
class QSplitter;
class TimetrackDebugger;

///////////////////////////////////////////////////////////////////////////////

/**
 * A blend tree resource editor.
 */
class BlendTreeEditor : public ResourceEditor, public TimeDependent, public ReflectionObjectChangeListener
{
   DECLARE_ALLOCATOR( BlendTreeEditor, AM_ALIGNED_16 );

   Q_OBJECT

private:
   BlendTree&                    m_blendTree;

   BlendTreeWidget*              m_treeWidget;
   
   TamySceneWidget*              m_sceneWidget;
   BlendTreeEditorBottomFrame*   m_bottomFrame;

   QPropertiesView*              m_treeProperties;
   QPropertiesView*              m_nodeProperties;
   QVBoxLayout*                  m_stateGraphEditorLayout;
   BlendTreeVariablesViewer*     m_variablesViewer;
   BlendTreeEventsViewer*        m_eventsViewer;
   TimetrackDebugger*            m_timetrackDebugger;

   // current selection
   QTreeWidgetItem*              m_selectedItem;
   BlendTreeStateMachine*        m_selectedStateMachine;

   // a scene along with the components that make the animated entity
   Model*                        m_scene;
   Entity*                       m_animatedEntity;
   SkeletonComponent*            m_skeletonComponent;
   BlendTreePlayer*              m_blendTreePlayer;
   AnimationPlayer*              m_attachedAnimPlayer;

   // lighting
   Camera*                       m_camera;
   OrbitingCameraController*     m_cameraController;
   Entity*                       m_frontLight;

public:
   /**
    * Constructor.
    *
    * @param blendTree
    */
   BlendTreeEditor( BlendTree& blendTree );
   ~BlendTreeEditor();

   /**
    * Makes the editor use an arbitrary animation player - which will make the displayed
    * model animate to a different animation source.
    *
    * If you set NULL, the editor will go back to playing the behavior tree.
    *
    * @param animPlayer
    */
   void setAnimationPlayer( AnimationPlayer* animPlayer );

   /**
    * Returns a reference to the edited blend tree.
    */
   inline BlendTree& getBlendTree() const { return m_blendTree; }

   // -------------------------------------------------------------------------
   // TimeDependent implementation
   // -------------------------------------------------------------------------
   void update( float timeElapsed );

   // -------------------------------------------------------------------------
   // ReflectionObjectChangeListener implementation
   // -------------------------------------------------------------------------
   void onObservedPropertyChanged( ReflectionProperty& property );
   void onObservedObjectDeleted( ReflectionObject* deletedObject );

public slots:
   void onSave();
   void onScenePlayed( bool isPlaying );
   void editItem( QTreeWidgetItem* item, int column );

protected:
   // -------------------------------------------------------------------------
   // ResourceEditor implementation
   // -------------------------------------------------------------------------
   void onInitialize();
   void onDeinitialize( bool saveProgress );

private:
   void buildLeftRightMainSplitter( QSplitter* leftRightMainSplitter );
   void initScene();
   BlendTreeStateMachine* getNearestStateMachine( BlendTreeWidgetItem* btItem ) const;
};

///////////////////////////////////////////////////////////////////////////////
