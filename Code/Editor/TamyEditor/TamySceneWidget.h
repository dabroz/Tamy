/// @file   TamyEditor\TamySceneWidget.h
/// @brief  a widget for displaying scenes
#pragma once

#include "Defines.h"
#include <QtWidgets\QFrame>
#include <windows.h>
#include "core.h"
#include "core-Renderer\Camera.h"
#include "core\TimeDependent.h"
#include "Gizmo.h"
#include <d3d9.h>
#include <set>
#include "TamySceneWidgetInputManager.h"


///////////////////////////////////////////////////////////////////////////////

class TamyEditor;
class Renderer;
class Camera;
class Model;
class EntitiesGroup;
class RenderingPipeline;
class ResourcesManager;
class TimeController;
class SceneRendererInputController;
class TimeControllerTrack;
class EditorDebugRenderer;
class DebugEntitiesManager;
class SelectionManager;
class SceneObjectsManipulator;
class QToolBar;
class GizmoOperation;
class AnimationWorld;
class QAction;
class QSettings;
class TamySceneWidgetInputManager;
class PhysicsWorld;
class SceneNodesTransformsCache;
class TamyRenderingInterface;
class MimeDataEncoder;
class QMimeData;
class QToolButton;

///////////////////////////////////////////////////////////////////////////////

enum TamySceneWidgetInputPolicy
{
   IP_RELATIVE_ON_DEMAND,
   IP_ALWAYS_RELATIVE,
};

///////////////////////////////////////////////////////////////////////////////

class TamySceneWidget : public QFrame, public TimeDependent
{
   Q_OBJECT

private:
   // window definition
   QWidget*                                           m_renderWindow;
   HWND                                               m_hWnd;
   TimeController*                                    m_localTimeController;
   
   // input tracking
   TamySceneWidgetInputManager*                       m_userInputController;
   TamySceneWidgetInputPolicy                         m_inputPolicy;
   bool                                               m_relativeInputEnabled;

   // required services
   ResourcesManager*                                  m_resMgr;
   Renderer*                                          m_renderer;
   Camera*                                            m_camera;
   PhysicsWorld*                                      m_physicsWorld;
   SceneNodesTransformsCache*                         m_transformsCache;

   Model*                                             m_scene;
   EntitiesGroup*                                     m_selectionGroup;

   class DeferredRenderingMechanism*                   m_sceneRenderer;

   // debug renderer
   Model*                                             m_debugScene;
   DebugEntitiesManager*                              m_debugEntitiesManager;
   QToolButton*                                       m_debugViewMenuButton;

   // animations world
   AnimationWorld*                                    m_animationWorld;
   QAction*                                           m_playAnimationsAction;

   // a gizmo that will be displayed when a scene object that can be manipulated gets selected
   Gizmo::Mode                                        m_gizmoMode;
   Gizmo*                                             m_gizmo;
   Model*                                             m_gizmoScene;
   GizmoOperation*                                    m_gizmoOperations[2][4];

   SelectionManager*                                  m_selectionManager;

   SceneObjectsManipulator*                           m_sceneObjectsManipulator;

   QAction*                                           m_snapToGridAction;
   bool                                               m_snapToGridEnabled;
   float                                              m_snapTranslationStep;
   float                                              m_snapRotationStep;

   QToolBar*                                          m_toolbar;

   // drag&drop support
   MimeDataEncoder*                                   m_dataEncoder;

public:
   /**
    * Constructor.
    *
    * @param parent                       parent widget
    * @param settingsGroupName
    */
   TamySceneWidget( QWidget* parent, const char* settingsGroupName = "DefaultRenderer" );
   ~TamySceneWidget();

   /**
    * Enables/disables objects manipulator.
    *
    * @param enable
    */
   void enableObjectsManipulator( bool enable );

   /**
    * Enables/disables the ability to select objects.
    */
   void enableObjectsSelection( bool enable );

   /**
    *  Enables/disables the debug view.
    */
   void enableDebugView( bool enable );

   /**
    * Shows/hides the toolbar.
    *
    * @param show
    */
   void showToolbar( bool show );

   /**
    * Changes mouse movement mode.
    * In the relative mode, the cursor is hidden and the user can move it as far in any direction as he wants.
    * In the absolute mode, the cursor is visible and its movement is limited byu the screen boundaries.
    *
    * @param enable
    */
   void setRelativeMouseMovement( bool enable );

   /**
    * Defines how the mode the input will work in when the widget gains focus.
    *
    * @param policy
    */
   void setInputPolicy( TamySceneWidgetInputPolicy policy );

   /**
   * Sets a new data encoder.
   *
   * @param dataEncoder
   */
   void setDataEncoder( MimeDataEncoder* dataEncoder );

   // -------------------------------------------------------------------------
   // Scene management
   // -------------------------------------------------------------------------

   /**
   * Allows to set up a rendered scene.
   *
   * @param scene
   */
   void setScene( Model* scene );

   /**
   * Plays/stops playing the scene.
   *
   * @param play
   */
   void playScene( bool play );

   // -------------------------------------------------------------------------
   // Objects manipulation gizmo management
   // -------------------------------------------------------------------------
   /**
    * Returns currently set gizmo manipulation mode.
    */
   GizmoOperation* getGizmoOperation( GizmoAxis* axis ) const;

   /**
    * Returns active gizmo's global matrix.
    */
   Gizmo* getGizmo();

   /**
    * Checks if snap to grid is enabled.
    */
   inline bool isSnapToGridEnabled() const {
      return m_snapToGridEnabled;
   }

   /**
    * Establishes different gizmo snap step values.
    *
    * @param translation
    * @param rotation ( in DEG )
    */
   void setSnapSteps( float translation, float rotation );

   /**
    * Returns the translation step of gizmo.
    */
   inline float getTranslationSnapStep() const {
      return m_snapTranslationStep;
   }

   /**
   * Returns the rotation step of gizmo.
   */
   inline float getRotationSnapStep() const {
      return m_snapTranslationStep;
   }

   /**
    * Restores default gizmo snap step values.
    */
   void setDefaultSnapSteps();

   /**
   * The method translates the screen space coordinates
   * (i.e. ones your mouse moves in) to viewport space coordinates
   */
   void localToViewport( const Point& inScreenPt, Vector& outViewportPt ) const;

   // -------------------------------------------------------------------------
   // Accessors
   // -------------------------------------------------------------------------
   /**
    * Returns a UserInputController instance.
    */
   inline UserInputController& getUserInputController() const { return *m_userInputController; }

   /**
    * Returns the renderer instance.
    */
   inline Renderer& getRenderer() const { return *m_renderer; }

   /**
    * Returns the active camera instance.
    */
   inline Camera& getCamera() const { return *m_camera; }

   /**
    * Gives access to the debug entities manager that allows to toggle certain debugging features.
    */
   inline DebugEntitiesManager& getDebugEntitiesManager() const { return *m_debugEntitiesManager; }
   
   /**
    * Returns a selection manager instance.
    */
   inline SelectionManager& getSelectionMgr() { return *m_selectionManager; }

   /**
    * Returns the rendering pipeline mechanism that draws the contents of the main scene.
    */
   inline DeferredRenderingMechanism* getRenderingInterface() const {
      return m_sceneRenderer;
   }
   
   // -------------------------------------------------------------------------
   // UI serialization support
   // -------------------------------------------------------------------------
   /**
    * Saves UI settings.
    *
    * @param settings
    */
   void saveUI( QSettings& settings ) const;

   /**
    * Restores UI settings.
    *
    * @param settings
    */
   void loadUI( QSettings& settings );

   // ----------------------------------------------------------------------
   // SelectionManagerListener implementation
   // ----------------------------------------------------------------------
   void onSelectionChanged( SelectionManager* selectionMgr );

   // ----------------------------------------------------------------------
   // TimeDependent implementation
   // ----------------------------------------------------------------------
   void update( float timeElapsed );

public slots: 
   /**
    * Sets the objects manipulation gizmo in the translation mode.
    */
   void setNodeTranslateMode();

   /**
    * Sets the objects manipulation gizmo in the rotation mode.
    */
   void setNodeRotateMode();

   /**
    * Toggles snapping while one transforms an object manually ( using mouse ).
    */
   void toggleSnapToGrid();

   /**
    * Toggles animations playback.
    */
   void onPlayAnimations();

   /**
    * Called when the user selects a different gizmo reference system.
    */
   void onReferenceSystemChanged( const QString& referenceSystemId );

   /**
    * Called when the user selects a different camera rotation mode.
    */
   void onCameraRotationModeChanged( const QString& rotationModeId );

   /**
    * Called when the user selects a different render pass
    */
   void onRenderPassSelected( const QString& renderPassId );

signals:
   /**
    * This signal is emitted when the user toggles the replay of animations.
    *
    * @param isPlayed
    */
   void scenePlayed( bool isPlayed );

   /**
    * Emitted when an item is dropped on the widget.
    */
   void changed( const QMimeData *mimeData = 0 );

protected:
   // -------------------------------------------------------------------------
   // QWidget implementation
   // -------------------------------------------------------------------------
   void moveEvent( QMoveEvent* event );
   void resizeEvent( QResizeEvent* event );
   void keyPressEvent( QKeyEvent* event );
   void keyReleaseEvent( QKeyEvent* event );
   void mousePressEvent( QMouseEvent* event );
   void mouseReleaseEvent( QMouseEvent* event );
   void showEvent( QShowEvent* event );
   void hideEvent( QHideEvent* event );
   void focusInEvent( QFocusEvent* event );
   void focusOutEvent( QFocusEvent* event );
   void dragEnterEvent( QDragEnterEvent *event );
   void dragMoveEvent( QDragMoveEvent *event );
   void dropEvent( QDropEvent *event );

private:
   void initialize();
   void initializeSubsystems( QWidget* renderWindow );
   void initializeToolbar( QToolBar* toolbar );
   void initializeRenderingPipeline();
   void deinitializeRenderingPipeline();
   void updateSnapToGrid();
};

///////////////////////////////////////////////////////////////////////////////
