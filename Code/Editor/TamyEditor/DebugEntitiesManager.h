/// @file   TamyEditor/DebugEntitiesManager.h
/// @brief  manager of the debug entities
#pragma once

#include "core\GenericFactory.h"
#include "core-MVC\Entity.h"
#include "core-MVC\ModelView.h"
#include "core\TimeDependent.h"
#include <vector>


///////////////////////////////////////////////////////////////////////////////

class EditorDebugRenderer;
class DebugGeometry;
class DebugGrid;
class Model;

///////////////////////////////////////////////////////////////////////////////

enum DebugFeature
{
   DF_Grid,
   DF_BoundingBoxes,
   DF_Shapes,

   MAX_DEBUG_FEATURES
};

///////////////////////////////////////////////////////////////////////////////

/**
 * Manager of the debug entities.
 */
class DebugEntitiesManager : public ModelView, public TimeDependent
{
private:
   Model&                                          m_debugScene;

   GenericFactory< SceneNode, DebugGeometry >*     m_representationsFactory;
   std::vector< SceneNode* >                       m_representedNodes;
   std::vector< DebugGeometry* >                   m_representations;

   bool                                            m_debugViewEnabled;
   bool*                                           m_debugFeatureEnabled;
   DebugGrid*                                      m_debugGrid;

public:
   /**
    * Constructor.
    *
    * @param debugScene
    */
   DebugEntitiesManager( Model& debugScene );
   ~DebugEntitiesManager();

   /**
    * Enables/disables the debug view.
    */
   void setEnabled( bool enable );

   /**
    * Returns the state of the specified debug feature.
    */
   bool getDebugDisplayState( DebugFeature flag ) const { return m_debugFeatureEnabled[flag]; }

   /**
    * Toggles the display of a particular type of debug entities.
    *
    * @return     new state of the feature
    */
   bool toggleDebugDisplay( DebugFeature flag );

   /**
    * Disables all debug rendering features
    */
   void disableAll();

   /**
    * Shows/hides debug grid.
    * 
    * @param enable
    */
   void showGrid( bool enable );

   /**
    * Returns a debug representation of a scene node.
    *
    * @param node
    */
   DebugGeometry* getRepresentation( SceneNode* node ) const;
       
   // -------------------------------------------------------------------------
   // TimeDependent implementation
   // -------------------------------------------------------------------------
   void update( float timeElapsed );

   // -------------------------------------------------------------------------
   // DebugEntitiesManager implementation
   // -------------------------------------------------------------------------
   void onAttachedToModel( Model& model );
   void onNodeAdded( SceneNode* node );
   void onNodeRemoved( SceneNode* node );
   void onNodeChanged( SceneNode* node );
   void resetContents( Model& model );

private:
   /**
    * Applies the selected debug feature ( toggling it on or off, depending on its state )
    * to the selected debug representation.
    *
    * @param representation
    * @param feature
    */
   void applyFeature( DebugGeometry* representation, DebugFeature feature );

   /**
    * Updates the visibility of all features.
    */
   void updateFeaturesVisibility();
};

///////////////////////////////////////////////////////////////////////////////
