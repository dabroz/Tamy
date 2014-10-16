/// @file   TamyEditor\TextureSamplerPropertyEditor.h
/// @brief  TextureSamplerSettings property editor
#pragma once

#include "core\ReflectionProperty.h"
#include "QPropertyEditor.h"
#include "core-Renderer\TextureSamplerSettings.h"


///////////////////////////////////////////////////////////////////////////////

class QComboBox;

///////////////////////////////////////////////////////////////////////////////

/**
* Editor widget that will edit 3D vector properties.
*/
class TextureSamplerPropertyEditor : public QPropertyEditor
{
   Q_OBJECT

private:
   TEditableReflectionProperty< TextureSamplerSettings >* m_property;

   QComboBox*                             m_addressingMode;
   QComboBox*                             m_filteringMode;

public:
   /**
   * Constructor.
   *
   * @param property   property this editor edits
   */
   TextureSamplerPropertyEditor( TEditableReflectionProperty< TextureSamplerSettings >* property );
   ~TextureSamplerPropertyEditor();

   // -------------------------------------------------------------------------
   // ReflectionPropertyEditor implementation
   // -------------------------------------------------------------------------
   void onPropertyChanged();

public slots:
   void addressingModeChanged( const QString& val );
   void filteringModeChanged( const QString& val );

private:
   void setupUi();

   static TextureAddressingMode addressModeToIndex( const QString& modeStr );  
   static TextureFilteringMode filteringModeToIndex( const QString& modeStr );

};

///////////////////////////////////////////////////////////////////////////////
