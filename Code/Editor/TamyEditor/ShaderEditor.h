/// @file   TamyEditor\ShaderEditor.h
/// @brief  Shader resource editor
#pragma once

#include "ResourceEditor.h"
#include <QtGui\QIcon>
#include <QtWidgets\QTabWidget>
#include "core\Array.h"
#include "core\FilePath.h"
#include <string>


///////////////////////////////////////////////////////////////////////////////

class Shader;
class QTabWidget;
class FilePath;
class QFrame;
class PathEditorWidget;
class QLineEdit;
enum ShaderRoutine;

///////////////////////////////////////////////////////////////////////////////

/**
 * Shader resource editor.
 */
class ShaderEditor : public ResourceEditor
{
   Q_OBJECT

private:
   Shader&                             m_shader;

   QTabWidget*                         m_languageTabs;
   class ShaderLanguageTab*            m_editorTab;
   PathEditorWidget*                   m_shaderPathEditor;

public:
   /**
   * Constructor.
   *
   * @param shader     edited shader
   */
   ShaderEditor( Shader& shader );
   ~ShaderEditor();

   // -------------------------------------------------------------------------
   // ResourceEditor implementation
   // -------------------------------------------------------------------------
   void onInitialize();
   void onDeinitialize( bool saveProgress );

public slots:
   void openFile();
   void closeFile( int tabIdx );
   void save();
   void undo();
   void redo();
   void compile();
   
private:
   friend class PathEditorWidget;

   void openFile( const FilePath& path );
   void loadOriginalScriptContents( std::string& outContents ) const;
   void restoreOriginalScriptContents( const std::string& contents ) const;
   void saveAllFiles() const;

   std::string selectFileDialog( const std::string& currPath );
};

///////////////////////////////////////////////////////////////////////////////

class PathEditorWidget : public QFrame
{
   Q_OBJECT

private:
   ShaderEditor*        m_shaderEditor;
   QLineEdit*           m_pathEditBox;

public:
   PathEditorWidget( ShaderEditor* parent, const FilePath& path );

public slots:
   void openFileDialog( bool );
   void showSelectedFile( bool );
   void itemRenamed();
};

///////////////////////////////////////////////////////////////////////////////

class ShaderEditorTab : public QFrame
{
   Q_OBJECT

protected:
   class QVBoxLayout*         m_layout;

public:
   ShaderEditorTab( QWidget* parent );
   virtual ~ShaderEditorTab() {}

   virtual void undo() {}
   virtual void redo() {}
   virtual void save() {}
};

///////////////////////////////////////////////////////////////////////////////

class ExternalFileEditorTab : public ShaderEditorTab
{
   Q_OBJECT

private:
   FilePath                m_path;
   class TextEditWidget*   m_textEdit;

public:
   /**
   * Constructor.
   *
   * @param parentWidget
   * @param path
   */
   ExternalFileEditorTab( QWidget* parentWidget, const FilePath& path );
   ~ExternalFileEditorTab();

   // -------------------------------------------------------------------------
   // ShaderEditorTab implementation
   // -------------------------------------------------------------------------
   void undo() override;
   void redo() override;
   void save() override;

};

///////////////////////////////////////////////////////////////////////////////

class ShaderLanguageTab : public ShaderEditorTab
{
   Q_OBJECT

private:
   Shader&           m_shader;
   QTabWidget*       m_tabs;

public:
   /**
    * Constructor.
    *
    * @param parentWidget
    * @param shader     edited shader
    */
   ShaderLanguageTab( QWidget* parentWidget, Shader& shader );
   ~ShaderLanguageTab();

   /**
    * Opens the files associated with the shader.
    */
   void openShaderFile();

   // -------------------------------------------------------------------------
   // ShaderEditorTab implementation
   // -------------------------------------------------------------------------
   void undo() override;
   void redo() override;
   void save() override;
};

///////////////////////////////////////////////////////////////////////////////
