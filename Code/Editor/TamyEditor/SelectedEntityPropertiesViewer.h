/// @file   TamyEditor/SelectedEntityPropertiesViewer.h
/// @brief  a helper widget that displays the properties of the most recently selected entity
#pragma once

#include <QtWidgets\QTabWidget>
#include "SelectionManagerListener.h"


///////////////////////////////////////////////////////////////////////////////

class QPropertiesView;
class SceneNode;

///////////////////////////////////////////////////////////////////////////////

/**
 * A helper widget that displays the properties of the most recently selected entity.
 */
class SelectedEntityPropertiesViewer : public QTabWidget, public SelectionManagerListener
{
   Q_OBJECT

private:
   SceneNode*                 m_displayedNode;

public:
   /**
    * Constructor.
    *
    * @param parent
    */
   SelectedEntityPropertiesViewer( QWidget* parent );

   /**
    * Initializes the properties viewer.
    */
   void initializeViewer();

   /**
    * Deinitializes the properties viewer.
    */
   void deinitializeViewer();

   // -------------------------------------------------------------------------
   // SelectionManager implementation
   // -------------------------------------------------------------------------
   void onSelectionChanged( SelectionManager* selectionMgr );
};

///////////////////////////////////////////////////////////////////////////////
