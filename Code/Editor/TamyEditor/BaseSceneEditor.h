/// @file   TamyEditor\BaseSceneEditor.h
/// @brief  a base class for scene editors
#pragma once

#include "ResourceEditor.h"
#include <QtWidgets\QAction>
#include "core\List.h"
#include "core-MVC\ModelView.h"
#include "SelectionManagerListener.h"


///////////////////////////////////////////////////////////////////////////////

class Model;
class Prefab;
class Entity;
class SceneNode;
class QVBoxLayout;
class SceneTreeWidget;
class TamySceneWidget;
class SelectedEntityPropertiesViewer;
class QKeyEvent;
class Camera;
class DirectionalLight;
class EntitiesClipboard;
struct Vector;
class QMimeData;
class Resource;
class QToolBar;
class GL2DProceduralLevel;

///////////////////////////////////////////////////////////////////////////////

/**
 * A base class for scene editors
 */
class BaseSceneEditor : public ResourceEditor, public ModelView, public SelectionManagerListener
{
   Q_OBJECT

protected:
   QToolBar*                                 m_toolBar;
   TamySceneWidget*                          m_sceneWidget;

private:
   Resource&                                 m_editedResource;
   Entity*                                   m_root;
   Model*                                    m_scene;

   QVBoxLayout*                              m_mainLayout;
   SceneTreeWidget*                          m_sceneTreeViewer;
   SelectedEntityPropertiesViewer*           m_customPropertiesEditor;

   QAction*                                  m_actionSave;
   QAction*                                  m_actionBuildGameLevelGeometry;

   Prefab*                                   m_droppedPrefab;

   GL2DProceduralLevel*                      m_activeProceduralLevel;

   // clipboard
   EntitiesClipboard*                        m_clipboard;

public:
   /**
    * Constructor.
    *
    * @param editedResource
    */
   BaseSceneEditor( Resource& editedResource );
   ~BaseSceneEditor(); 

protected:
   /**
    * Initializes the base class.
    * Needs to be called from the derived class's constructor.
    *
    * @param root
    * @param scene
    */
   void initializeBase( Entity* root, Model* scene );

   // -------------------------------------------------------------------------
   // ResourceEditor implementation
   // -------------------------------------------------------------------------
   void onInitialize();
   void onDeinitialize( bool saveProgress );
   void enable( bool flag );

   // -------------------------------------------------------------------------
   // QWidget implementation
   // -------------------------------------------------------------------------
   void keyPressEvent( QKeyEvent* event );
   void mousePressEvent( QMouseEvent* event );

   // -------------------------------------------------------------------------
   // ModelView implementation
   // -------------------------------------------------------------------------
   void onNodeAdded( SceneNode* node );
   void onNodeRemoved( SceneNode* node );
   void resetContents( Model& model );

   // -------------------------------------------------------------------------
   // SelectionManagerListener implementation
   // -------------------------------------------------------------------------
   void onSelectionChanged( SelectionManager* selectionMgr );

protected:

   /**
    * This template method allows the custom editor to do stuff to the resource
    * prior to it being saved.
    */
   virtual void onPreSaveResource() {}

private:
   /**
   * This method deletes all currently selected entities.
   */
   void deleteSelectedEntities();

   /**
   * This method copies all currently selected entities.
   */
   void copySelectedEntities();

   /**
   * This method pastes copied entities.
   */
   void pasteSelectedEntities();

   /**
   * Calculates a position where an entity should be placed when it's pasted
   * or dragged&dropped.
   *
   * @param outPos
   * @return  'true' if a valid position was found, 'false' otherwise
   */
   bool getEntityPlacementPoint( Vector& outPos ) const;

   /**
    * Toggles the procedural edition mode on/off.
    *
    * @param component
    */
   void setProceduralEditionMode( GL2DProceduralLevel* component );

   /**
    * Updates snap step settings of the scene gizmo.
    */
   void updateGizmoSnapStep();

public slots:
   void saveResource();
   void buildCollisionGeometry();
   void nodeSelected( SceneNode* );
   void selectionCleaned();
   void onScenePlayed( bool isPlayed );
   void onPrefabDropped( const QMimeData* mimeData );
};

///////////////////////////////////////////////////////////////////////////////
