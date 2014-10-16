/// @file   TamyEditor/MaterialEditor.h
/// @brief  Editor allowing to edit material resources
#pragma once

#include "ResourceEditor.h"
#include "core\GraphBuilderListener.h"
#include "core\List.h"
#include "core\ReflectionObjectChangeListener.h"
#include "core-Renderer\MaterialNode.h"
#include "core-Renderer\Shader.h"


///////////////////////////////////////////////////////////////////////////////

class Material;
class MaterialProfile;
class MaterialLayout;
class Model;
class GraphView;
class TamySceneWidget;
class QVBoxLayout;
class QSplitter;
class RenderingPipeline;
class PreviewViewer;
class TextEditWidget;
class QTabWidget;
class QListWidget;
class FilesystemTree;
class QTreeWidgetItem;
class QMenu;
class MaterialRoutine;
class QMimeData;
class MNInputStruct;
class MNOutputStruct;
class QPropertiesView;

///////////////////////////////////////////////////////////////////////////////

class MaterialEditor :  public ResourceEditor
                      , public GraphBuilderListener< MaterialNode >
                      , public GraphBuilderNodeListener< MaterialNode >
{
   Q_OBJECT

private:
   Material&                                 m_material;
   MaterialLayout*                           m_materialLayout[LAST_SHADER_ROUTINE];

   Model*                                    m_previewScene;
   TamySceneWidget*                          m_sceneWidget;

   RenderingPipeline*                        m_renderingPipeline;

   GraphView*                                m_graphView[LAST_SHADER_ROUTINE];

   int                                       m_activeRoutineIdx;
   
   TextEditWidget*                           m_codePreview;
   FilesystemTree*                           m_librariesBrowser;
   MaterialRoutine*                          m_droppedRoutineLib;

   QPropertiesView*                          m_materialProperties;


   // input and output structures
   MNInputStruct*                            m_routinesInputs[LAST_SHADER_ROUTINE];
   MNOutputStruct*                           m_routinesOutputs[LAST_SHADER_ROUTINE];
   MaterialNode*                             m_synchronizationRootNode; // this field will keep track of the entry node the sockets
                                                                        // updates started from ( those updates will propagate to other nodes
                                                                        // in other routines ). It's to prevent the subsequently updated entry nodes
                                                                        // from calling back on the original one, thus causing an infinite update loop

public:
   /**
    * Constructor.
    *
    * @param material      material to be edited
    */
   MaterialEditor( Material& material );
   ~MaterialEditor();

   // -------------------------------------------------------------------------
   // ResourceEditor implementation
   // -------------------------------------------------------------------------
   void onInitialize();
   void onDeinitialize( bool saveProgress );

   // -------------------------------------------------------------------------
   // GraphBuilderListener< MaterialNode > implementation
   // -------------------------------------------------------------------------
   void onNodeCreated( MaterialNode* node );
   void onNodeRemoved( MaterialNode* node );
   void afterNodeRemoved( MaterialNode* node );
   void onConnectionCreated( const MaterialNode* startNode, const char* outputName, const MaterialNode* endNode, const char* inputName );
   void afterConnectionRemoved( const MaterialNode* endNode, const char* inputName );

   // -------------------------------------------------------------------------
   // GraphBuilderNodeListener< MaterialNode > implementation
   // -------------------------------------------------------------------------
   void onInputAdded( MaterialNode* node, GBNodeInput< MaterialNode >* input );
   void onOutputAdded( MaterialNode* node, GBNodeOutput< MaterialNode >* output );
   void onInputRemoved( MaterialNode* node, GBNodeInput< MaterialNode >* input );
   void onOutputRemoved( MaterialNode* node, GBNodeOutput< MaterialNode >* output );

public slots:
   void save();
   void onRoutineTabSelected( int tabIdx );
   void exportToLibrary();
   void showLibrariesBrowserPopupMenu( QTreeWidgetItem* selectedItem, QMenu& menu );
   void onLibraryDropped( const QMimeData* data );
   void clearCurrentRoutine();
   void buildMaterial();

private:
   void setupLeftSide( QSplitter* graphSplitter );
   void setupRightSide( QSplitter* graphSplitter );
   void setupMainPreviewWindow( QSplitter* previewsAreaSplitter );
   void setupBrowsers( QSplitter* previewsAreaSplitter );
   void setupTestScene();

   void activateProfile( MaterialProfile* profile );
   void deactivateProfile( MaterialProfile* profile );
   void createProfileEntryPoints();
};

///////////////////////////////////////////////////////////////////////////////
