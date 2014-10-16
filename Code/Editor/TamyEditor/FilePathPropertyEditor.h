/// @file   TamyEditor\FilePathPropertyEditor.h
/// @brief  FilePath property editor
#pragma once

#include "core\ReflectionProperty.h"
#include "QPropertyEditor.h"
#include "core\FilePath.h"
#include "core\Array.h"
#include <string>


///////////////////////////////////////////////////////////////////////////////

class DropArea;

///////////////////////////////////////////////////////////////////////////////

/**
* Editor widget that will edit FilePath properties.
*/
class FilePathPropertyEditor : public QPropertyEditor
{
   Q_OBJECT

private:
   TEditableReflectionProperty< FilePath >*  m_property;

   DropArea*                                 m_droppedFilePath;
   Array< FilePath >                         m_paths;

public:
   /**
   * Constructor.
   *
   * @param property         property this editor edits
   */
   FilePathPropertyEditor( TEditableReflectionProperty< FilePath >* property );
   ~FilePathPropertyEditor();

   // -------------------------------------------------------------------------
   // ReflectionPropertyEditor implementation
   // -------------------------------------------------------------------------
   void onPropertyChanged();

public slots:
   void valChanged();
   void valErased( bool );

protected:
   void onInitialize();

private:
   void refreshPropertyName();
};

///////////////////////////////////////////////////////////////////////////////
