/// @file   TamyEditor/DebugMenuActions.h
/// @brief  actions plugged into Tamy scene widget's debug menu
#pragma once

#include <QtWidgets\QAction>


///////////////////////////////////////////////////////////////////////////////

class QueryRenderingPass;
class DebugEntitiesManager;
enum DebugFeature;

///////////////////////////////////////////////////////////////////////////////

class DebugEntitiesAction : public QAction
{
   Q_OBJECT

private:
   DebugEntitiesManager&      m_debugEntitiesManager;
   DebugFeature               m_toggledFeature;

public:
   /**
    * Constructor.
    *
    * @param label
    * @param toggledFeature      toggled debug feature
    * @param debugEntitiesManager
    * @param parentWidget
    */
   DebugEntitiesAction( const char* label, DebugFeature toggledFeature, DebugEntitiesManager& debugEntitiesManager, QWidget* parentWidget );

public slots:
   void onTriggered();
};

///////////////////////////////////////////////////////////////////////////////
