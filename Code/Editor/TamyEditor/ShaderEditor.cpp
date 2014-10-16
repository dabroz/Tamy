#include "ShaderEditor.h"
#include "Defines.h"
#include "core-Renderer\Shader.h"

// qt widgets
#include <QtWidgets\QToolBar>
#include <QtWidgets\QVBoxLayout>
#include <QtWidgets\QHBoxLayout>
#include <QtWidgets\QAction>
#include <QtWidgets\QLineEdit> 
#include <QtWidgets\QLabel>
#include <QtWidgets\QPushButton>
#include <QtWidgets\QFileDialog>

// tamy widgets
#include "TextEditWidget.h"
#include "ToolsWindow.h"
#include "tamyeditor.h"

// file handling
#include "core\StreamBuffer.h"
#include "core\Filesystem.h"

// syntax tools
#include "ShaderSyntaxHighlighter.h"

// compiler
#include "core-Renderer\ShaderCompiler.h"


///////////////////////////////////////////////////////////////////////////////

static const char* g_routineId[LAST_SHADER_ROUTINE] = 
{
   "VERTEX",
   "TESS_CONTROL",
   "TESS_EVALUATION",
   "GEOMETRY",
   "FRAGMENT"
}; 

///////////////////////////////////////////////////////////////////////////////

ShaderEditor::ShaderEditor( Shader& shader )
   : m_shader( shader )
{
}

///////////////////////////////////////////////////////////////////////////////

ShaderEditor::~ShaderEditor()
{
}

///////////////////////////////////////////////////////////////////////////////

void ShaderEditor::onInitialize()
{
   QVBoxLayout* mainFrameLayout = new QVBoxLayout( this );
   setLayout( mainFrameLayout );

   // create a toolbar
   QToolBar* toolbar = new QToolBar( this );
   {
      mainFrameLayout->addWidget( toolbar );

      // file management
      {
         QAction* actionOpenFile = new QAction( QIcon( tr( ":/TamyEditor/Resources/Icons/Editor/import.png" ) ), tr( "Open" ), toolbar );
         toolbar->addAction( actionOpenFile );
         connect( actionOpenFile, SIGNAL( triggered() ), this, SLOT( openFile() ) );

         QAction* actionSave = new QAction( QIcon( tr( ":/TamyEditor/Resources/Icons/Editor/saveFile.png" ) ), tr( "Save" ), toolbar );
         actionSave->setShortcut( QKeySequence( tr( "Ctrl+S" ) ) );
         toolbar->addAction( actionSave );
         connect( actionSave, SIGNAL( triggered() ), this, SLOT( save() ) );

         toolbar->addSeparator();
      }

      // script edition commands
      {
         QAction* actionUndo = new QAction( QIcon( tr( ":/TamyEditor/Resources/Icons/Editor/undo.png" ) ), tr( "Undo" ), toolbar );
         actionUndo->setShortcut( QKeySequence( tr( "Ctrl+Z" ) ) );
         toolbar->addAction( actionUndo );
         connect( actionUndo, SIGNAL( triggered() ), this, SLOT( undo() ) );

         QAction* actionRedo = new QAction( QIcon( tr( ":/TamyEditor/Resources/Icons/Editor/redo.png" ) ), tr( "Redo" ), toolbar );
         actionRedo->setShortcut( QKeySequence( tr( "Ctrl+Y" ) ) );
         toolbar->addAction( actionRedo );
         connect( actionRedo, SIGNAL( triggered() ), this, SLOT( redo() ) );

         toolbar->addSeparator();
      }

      // building commands
      {
         QAction* actionCompile = new QAction( QIcon( tr( ":/TamyEditor/Resources/Icons/Editor/play.png" ) ), tr( "Compile" ), toolbar );
         actionCompile->setShortcut( QKeySequence( tr( "F5" ) ) );
         toolbar->addAction( actionCompile );
         connect( actionCompile, SIGNAL( triggered() ), this, SLOT( compile() ) );
      }
   }

   // create a tab widget for languages
   m_languageTabs = new QTabWidget( this );
   {
      mainFrameLayout->addWidget( m_languageTabs, 1 );
      m_languageTabs->setTabsClosable( true );
      connect( m_languageTabs, SIGNAL( tabCloseRequested( int ) ), this, SLOT( closeFile( int ) ) );

      // create a tab widget where we'll store all the files
      m_editorTab = new ShaderLanguageTab( this, m_shader );
      m_languageTabs->addTab( m_editorTab, "GLSL" );
   }

   // create a tab widget that will show file paths assignments to particular shader stages
   m_shaderPathEditor = new PathEditorWidget( this, m_shader.getScriptPath() );
   mainFrameLayout->addWidget( m_shaderPathEditor, 0, Qt::AlignBottom );

   show();
}

///////////////////////////////////////////////////////////////////////////////

void ShaderEditor::onDeinitialize( bool saveProgress )
{
   if ( saveProgress )
   {
      saveAllFiles();

      m_shader.saveResource();
   }
}

///////////////////////////////////////////////////////////////////////////////

void ShaderEditor::openFile( const FilePath& path )
{
   if ( path.empty() )
   {
      return;
   }

   // check if the file isn't already open
   const int tabsCount = m_languageTabs->count();
   for ( int i = 0; i < tabsCount; ++i )
   {
      QString tabFilePath = m_languageTabs->tabText( i );
      if ( tabFilePath == path.c_str() )
      {
         // the file's already open - just move the focus to it
         m_languageTabs->setCurrentIndex( i );
         return;
      }
   }


   // open an editor in a new tab
   ExternalFileEditorTab* scriptEditor = new ExternalFileEditorTab( m_languageTabs, path );
   int tabIdx = m_languageTabs->addTab( scriptEditor, path.c_str() );

   // make it the active tab
   m_languageTabs->setCurrentIndex( tabIdx );
}

///////////////////////////////////////////////////////////////////////////////

void ShaderEditor::openFile()
{
   // show the file picker dialog
   std::string selectedPath = selectFileDialog( "" );
   if ( !selectedPath.empty() )
   {
      openFile( FilePath( selectedPath ) );
   }
}

///////////////////////////////////////////////////////////////////////////////

void ShaderEditor::save()
{
   saveAllFiles();

   // save the resource
   m_shader.build();
   m_shader.saveResource();
}

///////////////////////////////////////////////////////////////////////////////

void ShaderEditor::undo()
{
   ShaderEditorTab* tab = static_cast< ShaderEditorTab* >( m_languageTabs->currentWidget() );
   tab->undo();
}

///////////////////////////////////////////////////////////////////////////////

void ShaderEditor::redo()
{
   ShaderEditorTab* tab = static_cast< ShaderEditorTab* >( m_languageTabs->currentWidget() );
   tab->redo();
}

///////////////////////////////////////////////////////////////////////////////

void ShaderEditor::compile()
{
   // the compiler reads the actual files from the HD, so in order to do this, we'll check all files in,
   // compile the shader, and then restore the files previous contents on the HD
   std::string originalCode;
   loadOriginalScriptContents( originalCode );
   saveAllFiles();

   std::string errStr;
   ShaderCompiler compiler;

   // compile the code first
   HWND tmpCompileWindowHandler = (HWND)winId();
   bool result = compiler.compileShader( m_shader, tmpCompileWindowHandler );
   ToolsWindow* toolsWindow = TamyEditor::getInstance().getToolsWindow();
   if ( !result )
   {
      QString label;
      label.sprintf( "Shader %s compilation results", m_shader.getFilePath().c_str() );
      toolsWindow->displayMessage( label.toStdString().c_str(), compiler.getLastError().c_str() );
   }
   else
   {
      QString label;
      label.sprintf( "Shader %s compilation successful", m_shader.getFilePath().c_str() );
      toolsWindow->displayMessage( label.toStdString().c_str(), "" );

   }

   restoreOriginalScriptContents( originalCode );
}

///////////////////////////////////////////////////////////////////////////////

void ShaderEditor::loadOriginalScriptContents( std::string& outContents ) const
{
   Filesystem& fs = TSingleton< Filesystem >::getInstance();

   // the file doesn't exist, so create it
   File* file = fs.open( m_shader.getScriptPath(), std::ios_base::in );
   if ( !file )
   {
      return;
   }
      
   StreamBuffer<char> fileContentsBuf( *file );
   outContents = fileContentsBuf.getBuffer();
   delete file;
}

///////////////////////////////////////////////////////////////////////////////

void ShaderEditor::restoreOriginalScriptContents( const std::string& contents ) const
{
   Filesystem& fs = TSingleton< Filesystem >::getInstance();

   // save the contents of every open tab
   File* file = fs.open( m_shader.getScriptPath(), std::ios_base::out );
   ASSERT_MSG( file != NULL, "Couldn't open a file" );

   // save the contents of the file
   file->write( ( byte* ) contents.c_str(), contents.length() );
   delete file;
}

///////////////////////////////////////////////////////////////////////////////

void ShaderEditor::saveAllFiles() const
{
   Filesystem& fs = TSingleton< Filesystem >::getInstance();

   // save the contents of every open tab
   const int tabsCount = m_languageTabs->count();
   for ( int i = 0; i < tabsCount; ++i )
   {
      ShaderEditorTab* tab = static_cast< ShaderEditorTab* >( m_languageTabs->widget( i ) );
      tab->save();
   }
}

///////////////////////////////////////////////////////////////////////////////

std::string ShaderEditor::selectFileDialog( const std::string& currPath )
{
   Filesystem& fs = TSingleton< Filesystem >::getInstance();

   std::string startDir;
   if ( currPath.empty() )
   {
      startDir = m_shader.getFilePath().c_str();
   }
   else
   {
      FilePath selPath( currPath );
      FilePath currDir;
      selPath.extractDir( currDir );

      startDir = currDir.c_str();
   }

   startDir = fs.toAbsolutePath( startDir );
   QString fileName = QFileDialog::getOpenFileName( this, "Open file", startDir.c_str() );
   if ( fileName.isEmpty() )
   {
      // no file selected
      return "";
   }

   std::string relPath = fs.toRelativePath( fileName.toStdString() );
   return relPath;
}

///////////////////////////////////////////////////////////////////////////////

void ShaderEditor::closeFile( int tabIdx )
{
   // we can only close editor tabs
   QWidget* embeddedWidget = m_languageTabs->widget( tabIdx );

   delete embeddedWidget;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

PathEditorWidget::PathEditorWidget( ShaderEditor* parent, const FilePath& path )
   : QFrame( parent )
   , m_shaderEditor( parent )
{
   QHBoxLayout* layout = new QHBoxLayout( this );
   setLayout( layout );

   layout->addWidget( new QLabel( "Path:", this ), 0, Qt::AlignLeft );

   m_pathEditBox = new QLineEdit( this );
   m_pathEditBox->setText( path.c_str() );
   m_pathEditBox->setMaximumHeight( 20 );
   m_pathEditBox->setMaximumWidth( 2048 );
   m_pathEditBox->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
   connect( m_pathEditBox, SIGNAL( editingFinished() ), this, SLOT( itemRenamed() ) );
   layout->addWidget( m_pathEditBox, 1 );

   QPushButton* selectFileButton = new QPushButton( "...", this );
   selectFileButton->setMaximumHeight( 20 );
   selectFileButton->setMinimumWidth( 20 );
   selectFileButton->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
   layout->addWidget( selectFileButton );
   connect( selectFileButton, SIGNAL( clicked( bool ) ), this, SLOT( openFileDialog( bool ) ) );
   
   QPushButton* showFileButton = new QPushButton( "/", this );
   showFileButton->setMaximumHeight( 20 );
   showFileButton->setMinimumWidth( 20 );
   showFileButton->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
   layout->addWidget( showFileButton );
   connect( showFileButton, SIGNAL( clicked( bool ) ), this, SLOT( showSelectedFile( bool ) ) );
}

///////////////////////////////////////////////////////////////////////////////

void PathEditorWidget::openFileDialog( bool )
{
   std::string relPath = m_shaderEditor->selectFileDialog( m_pathEditBox->text().toStdString() );
   if ( !relPath.empty() )
   {
      m_pathEditBox->setText( relPath.c_str() );

      // set the path on the shader
      m_shaderEditor->m_shader.setPath( FilePath( relPath ) );
   }
}

///////////////////////////////////////////////////////////////////////////////

void PathEditorWidget::showSelectedFile( bool )
{
   m_shaderEditor->m_editorTab->openShaderFile();
}

///////////////////////////////////////////////////////////////////////////////

void PathEditorWidget::itemRenamed()
{
   FilePath selectedPath( m_pathEditBox->text().toStdString() );
   m_shaderEditor->m_shader.setPath( selectedPath );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

ShaderEditorTab::ShaderEditorTab( QWidget* parent ) 
   : QFrame( parent ) 
{
   m_layout = new QVBoxLayout( this );
   setLayout( m_layout );
   m_layout->setMargin( 0 );
   m_layout->setSpacing( 0 );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

ExternalFileEditorTab::ExternalFileEditorTab( QWidget* parentWidget, const FilePath& path )
   : ShaderEditorTab( parentWidget )
   , m_path( path )
{
   m_textEdit = new TextEditWidget( this );
   m_layout->addWidget( m_textEdit );

   // copy the file contents to the editor
   QFont font( "Courier", 12 );
   m_textEdit->setFont( font );
   m_textEdit->setSyntaxHighlighter( new ShaderSyntaxHighlighter() );

   // can a file with this name be loaded or created?
   Filesystem& fs = TSingleton< Filesystem >::getInstance();
   File* file = fs.open( path );
   bool fileLoaded = true;
   if ( file == NULL )
   {
      file = fs.open( path, std::ios_base::out );
      fileLoaded = false;
   }

   if ( file )
   {
      if ( fileLoaded )
      {
         // load the file's contents
         StreamBuffer<char> fileContentsBuf( *file );
         QString fileContents = fileContentsBuf.getBuffer();
         m_textEdit->setPlainText( fileContents );
      }
      else
      {
         // the file doesn't exist, so open a clean editor and add the standard header
         QString header;
         header.sprintf( "// @file\t%s\n// @brief\t<put the description here>\n\n", path.c_str() );
         m_textEdit->setPlainText( header );
      }

      delete file;   // load the script file
   }
   else
   {
      LOG( "ExternalFileEditorTab: File '%s' could not be opened or created", path.c_str() );
   }

}

///////////////////////////////////////////////////////////////////////////////

ExternalFileEditorTab::~ExternalFileEditorTab()
{
}

///////////////////////////////////////////////////////////////////////////////

void ExternalFileEditorTab::undo()
{
   m_textEdit->undo();
}

///////////////////////////////////////////////////////////////////////////////

void ExternalFileEditorTab::redo()
{
   m_textEdit->redo();
}

///////////////////////////////////////////////////////////////////////////////

void ExternalFileEditorTab::save()
{
   // this is a regular file - save it as one
   Filesystem& fs = TSingleton< Filesystem >::getInstance();
   File* file = fs.open( m_path, std::ios_base::out );
   if ( !file )
   {
      LOG( "ExternalFileEditorTab: File '%s' could not be opened for writing", m_path.c_str() );
      ASSERT_MSG( file != NULL, "ExternalFileEditorTab: Couldn't open a file" );
      return;
   }

   // save the contents of the file
   std::string script = m_textEdit->toPlainText().toStdString();
   file->write( ( byte* ) script.c_str(), script.length() );

   delete file;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

ShaderLanguageTab::ShaderLanguageTab( QWidget* parentWidget, Shader& shader )
   : ShaderEditorTab( parentWidget )
   , m_shader( shader )
{
   m_tabs = new QTabWidget( this );
   m_tabs->setTabsClosable( false );
   m_layout->addWidget( m_tabs );

   openShaderFile();
}

///////////////////////////////////////////////////////////////////////////////

ShaderLanguageTab::~ShaderLanguageTab()
{
}

///////////////////////////////////////////////////////////////////////////////

void ShaderLanguageTab::openShaderFile()
{
   const FilePath& path = m_shader.getScriptPath();
   if ( path.empty() )
   {
      return;
   }

   // go through every routine for the active language profile
   for ( uint routineIdx = 0; routineIdx < LAST_SHADER_ROUTINE; ++routineIdx )
   {
      // check if the file isn't already open
      TextEditWidget* scriptEditor = NULL;
      const int tabsCount = m_tabs->count();
      for ( int i = 0; i < tabsCount; ++i )
      {
         QString tabFilePath = m_tabs->tabText( i );
         if ( tabFilePath == g_routineId[routineIdx] )
         {
            // the file's already open - just move the focus to it
            m_tabs->setCurrentIndex( i );
            scriptEditor = static_cast< TextEditWidget* >( m_tabs->widget( i ) );
            break;
         }
      }

      if ( !scriptEditor )
      {
         // open an editor in a new tab
         scriptEditor = new TextEditWidget( m_tabs );
         const int tabIdx = m_tabs->addTab( scriptEditor, g_routineId[routineIdx] );

         // customize the editor
         QFont font( "Courier", 12 );
         scriptEditor->setFont( font );
         scriptEditor->setSyntaxHighlighter( new ShaderSyntaxHighlighter() );

         // make the current that the active one
         m_tabs->setCurrentIndex( tabIdx );
      }

      // get the code of the file
      std::string code;
      m_shader.getScript( ( ShaderRoutine ) routineIdx, code );


      // load the script file
      if ( !code.empty() )
      {
         scriptEditor->setPlainText( code.c_str() );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void ShaderLanguageTab::undo()
{
   TextEditWidget* editorWidget = static_cast< TextEditWidget* >( m_tabs->currentWidget() );
   editorWidget->undo();
}

///////////////////////////////////////////////////////////////////////////////

void ShaderLanguageTab::redo()
{
   TextEditWidget* editorWidget = static_cast< TextEditWidget* >( m_tabs->currentWidget() );
   editorWidget->redo();
}

///////////////////////////////////////////////////////////////////////////////

void ShaderLanguageTab::save()
{
   Filesystem& fs = TSingleton< Filesystem >::getInstance();

   // save the contents of every open tab
   const int tabsCount = m_tabs->count();
   for ( int i = 0; i < tabsCount; ++i )
   {
      TextEditWidget* scriptEditor = static_cast< TextEditWidget* >( m_tabs->widget( i ) );

      QString tabFilePath = m_tabs->tabText( i );

      // is it one of the script files
      bool contentsSaved = false;
      for ( uint routineIdx = 0; routineIdx < LAST_SHADER_ROUTINE; ++routineIdx )
      {
         if ( tabFilePath == g_routineId[routineIdx] )
         {
            m_shader.setScript( ( ShaderRoutine ) routineIdx, scriptEditor->toPlainText().toStdString().c_str() );
            contentsSaved = true;
            break;
         }
      }

      if ( !contentsSaved )
      {
         // this is a regular file - save it as one
         File* file = fs.open( FilePath( tabFilePath.toStdString() ), std::ios_base::out );
         ASSERT_MSG( file != NULL, "Couldn't open a file" );

         // save the contents of the file
         TextEditWidget* scriptEditor = static_cast< TextEditWidget* >( m_tabs->widget( i ) );
         std::string script = scriptEditor->toPlainText().toStdString();
         file->write( ( byte* ) script.c_str(), script.length() );
         delete file;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////
