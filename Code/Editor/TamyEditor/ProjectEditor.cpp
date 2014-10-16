#include "ProjectEditor.h"
#include "Project.h"
#include "tamyeditor.h"
#include "ToolsWindow.h"

// tools
#include "progressdialog.h"
#include <QtWidgets\QFileDialog>
#include <QtWidgets\QLayout>

// game deployment
#include "GameDeploymentInfo.h"
#include "GameRunner.h"
#include "GameDeploymentUtil.h"

// property editors
#include "ResourceDropArea.h"
#include "FSNodeMimeData.h"


///////////////////////////////////////////////////////////////////////////////

ProjectEditor::ProjectEditor( Project& project )
   : m_project( project )
{
}

///////////////////////////////////////////////////////////////////////////////

void ProjectEditor::onInitialize()
{
   m_ui.setupUi( this );

   // setup extra widgets
   {
      m_mainSceneResourceEd = new ResourceDropArea( m_ui.mainSceneFrame );
      m_ui.mainSceneFrame->layout()->addWidget( m_mainSceneResourceEd );

      m_renderingPipelineFrameEd = new ResourceDropArea( m_ui.renderingPipelineFrame );
      m_ui.renderingPipelineFrame->layout()->addWidget( m_renderingPipelineFrameEd );
   }

   connect( m_ui.deploymentPathButton, SIGNAL( clicked() ), this, SLOT( onSelectPath() ) );
   connect( m_ui.deployButton, SIGNAL( clicked() ), this, SLOT( onDeploy() ) );
}

///////////////////////////////////////////////////////////////////////////////

void ProjectEditor::onDeinitialize( bool saveProgress )
{
   if ( saveProgress )
   {
      m_project.saveResource();
   }
}

///////////////////////////////////////////////////////////////////////////////

void ProjectEditor::onSelectPath()
{
   const std::string& fsRootDir = TSingleton< ResourcesManager >::getInstance().getFilesystem().getCurrRoot();

   QString dirName = QFileDialog::getExistingDirectory( this, "Select target deployment directory", fsRootDir.c_str(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks );
   m_ui.deploymentPathText->setText( dirName );
}

///////////////////////////////////////////////////////////////////////////////

void ProjectEditor::onDeploy()
{
   // prepare the deployment info structure
   GameDeploymentInfo deploymentInfo;
   {
      // set the target directory
      deploymentInfo.m_targetDir = m_ui.deploymentPathText->text().toStdString();

      // set the deployment platform ( by default it's always Windows / DX9 )
      deploymentInfo.m_platform = GDP_WINDOWS_DX9;
      if ( m_ui.windowsDXRadio->isChecked() )
      {
         deploymentInfo.m_platform = GDP_WINDOWS_DX9;
      }
      else
      {
         deploymentInfo.m_platform = GDP_WINDOWS_OPENGL;
      }

      // set the key resource paths
      deploymentInfo.m_worldModelPath = m_mainSceneResourceEd->getFilePath();
      deploymentInfo.m_renderingPipelinePath = m_renderingPipelineFrameEd->getFilePath();

      // collect directories to deploy
      m_project.collectDirectories( deploymentInfo.m_projectDirectories );

      // do we want to run the game after it's been successfully deployed?
      deploymentInfo.m_runAfterDeployment = m_ui.runAfterDeploymentCheckBox->isChecked();
   }

   // deploy the game
   GameRunner* gameRunner = NULL;
   if ( deploymentInfo.isOk() )
   {
      // create a progress dialog that will track the deployment process
      TamyEditor& tamyEd = TamyEditor::getInstance();
      ToolsWindow* toolsWindow = tamyEd.getToolsWindow();
      IProgressObserver& progressDialog = toolsWindow->addProgressObserver( "Game deployment" );
        
      // deploy the game
      gameRunner = GameDeploymentUtil::deployGame( deploymentInfo, progressDialog );
   }

   // color code the specified widget to indicate if the deployment went well or not
   {
      m_ui.deployButton->setAutoFillBackground( true );
      QPalette palette = m_ui.deployButton->palette();

      const QColor opSuccessfulColor( 135,255, 56 );
      const QColor opFailedColor( 255, 92, 43 );
      palette.setColor( QPalette::Button, ( gameRunner != NULL ) ? opSuccessfulColor : opFailedColor );

      m_ui.deployButton->setPalette( palette );
   }

   // if the deployment was successful, and the user chose to - run the game
   if ( gameRunner && deploymentInfo.m_runAfterDeployment )
   {
      // create a game runner
      gameRunner->run();
   }

   // cleanup
   delete gameRunner;
}

///////////////////////////////////////////////////////////////////////////////
