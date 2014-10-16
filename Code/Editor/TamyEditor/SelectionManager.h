/// @file   TamyEditor\SelectionManager.h
/// @brief  Model view that manages the scene entities selection
#pragma once

#include "core-MVC\ModelView.h"
#include "tamyeditor.h"
#include "core\MemoryRouter.h"
#include "core\List.h"


///////////////////////////////////////////////////////////////////////////////

class SceneNode;
class EntitiesGroup;
class SelectionManagerListener;
class GizmoAxis;
class DebugEntitiesManager;

///////////////////////////////////////////////////////////////////////////////

/**
 * Component that manages the scene entities selection.
 */
class SelectionManager : public ModelView
{
public:
   typedef List< SelectionManagerListener* >             Listeners;

private:
   Listeners                                             m_listeners;

   bool                                                  m_appendedSelectionEnabled;
   List< SceneNode* >                                    m_selectedNodes; 
   EntitiesGroup&                                        m_selectedEntities;

public:
   /**
    * Constructor.
    *
    * @param selectedEntities
    */
   SelectionManager( EntitiesGroup& selectedEntities );
   ~SelectionManager();

   /**
    * Enables/disables appended selection mode.
    */
   inline void enableAppendedSelectionMode( bool enable ) { m_appendedSelectionEnabled = enable; }

   /**
    * Checks the state of the appended selection mode.
    */
   inline bool isAppendedSelectionModeEnabled() const {
      return m_appendedSelectionEnabled;
   }

   // -------------------------------------------------------------------------
   // Selected entities management
   // -------------------------------------------------------------------------

   /**
    * Call this when you want to handle node selection that comes from a viewport.
    *
    * @param nodes
    */
   void selectNodesFromViewport( const List< SceneNode* >& nodes );

   /**
    * This method allows to select a single node. Previous selection will be automatically
    * discarded.
    *
    * @param node
    */
   void selectNode( SceneNode& node );

   /**
    * Removes the selected nodes properties from the view.
    */
   void resetSelection();

   /**
    * Tells if any node is currently selected.
    */
   inline bool isSomethingSelected() const { return !m_selectedNodes.empty(); }

   /**
    * Returns a list of selected nodes.
    */
   inline const List< SceneNode* >& getSelectedNodes() const { return m_selectedNodes; }

   /**
    * Returns an node that was hand picked during the last selection operation.
    */
   inline SceneNode* getLastPickedNode() const { return m_selectedNodes.empty() ? NULL : m_selectedNodes.back(); }

   // -------------------------------------------------------------------------
   // Listeners management
   // -------------------------------------------------------------------------
   /**
    * Attaches a new listener.
    *
    * @param listener
    */
   void attach( SelectionManagerListener& listener );

   /**
    * Detaches a listener.
    *
    * @param listener
    */
   void detach( SelectionManagerListener& listener );

   // -------------------------------------------------------------------------
   // ModelView implementation
   // -------------------------------------------------------------------------
   void onAttachedToModel( Model& model );
   void onNodeAdded( SceneNode* node );
   void onNodeRemoved( SceneNode* node );
   void onNodeChanged( SceneNode* node );
   void resetContents( Model& scene );

private:
   void notifySelectionChanged();
};

///////////////////////////////////////////////////////////////////////////////
