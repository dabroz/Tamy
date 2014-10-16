/// @file   TamyEditor/ProjectEditor.h
/// @brief  Project resource editor that will allow to edit project settings and deploy it.
#pragma once

#include "ResourceEditor.h"
#include <QtWidgets\QFrame>
#include <QtGui\QCloseEvent>
#include "ui_gamedeploymentframe.h"
#include <core/FilePath.h>
#include <vector>

///////////////////////////////////////////////////////////////////////////////

class Project;
class ResourceDropArea;

///////////////////////////////////////////////////////////////////////////////

/**
 * Project resource editor that will allow to edit project settings and deploy it.
 */
class ProjectEditor : public ResourceEditor
{
   Q_OBJECT

private:
   Ui_GameDeploymentFrame           m_ui;
   Project&                         m_project;

   ResourceDropArea*                m_mainSceneResourceEd;
   ResourceDropArea*                m_renderingPipelineFrameEd;

public:
   /**
    * Constructor.
    *
    * @param project
    */
   ProjectEditor( Project& project );

protected:
   // -------------------------------------------------------------------------
   // ResourceEditor implementation
   // -------------------------------------------------------------------------
   void onInitialize();
   void onDeinitialize( bool saveProgress );

public slots:
   void onSelectPath();
   void onDeploy();
};

///////////////////////////////////////////////////////////////////////////////
