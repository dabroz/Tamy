#include "TextureSamplerPropertyEditor.h"
#include <QtWidgets\QComboBox>


///////////////////////////////////////////////////////////////////////////////

static const char* g_uvwAddressingModeNames [] = { "Wrap", "Mirror", "Clamp", "Border", "Mirror once" };
static const char* g_filteringModeNames[] = { "None", "Point", "Linear", "Anisotropic" };

///////////////////////////////////////////////////////////////////////////////

TextureSamplerPropertyEditor::TextureSamplerPropertyEditor( TEditableReflectionProperty< TextureSamplerSettings >* property )
   : QPropertyEditor( property->getLabel().c_str() )
   , m_property( property )
{
   setupUi();
}

///////////////////////////////////////////////////////////////////////////////

TextureSamplerPropertyEditor::~TextureSamplerPropertyEditor()
{
   delete m_property;
   m_property = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void TextureSamplerPropertyEditor::setupUi()
{
   // create the edit widget
   m_addressingMode = new QComboBox( this );
   m_filteringMode = new QComboBox( this );
   addWidget( m_addressingMode );
   addWidget( m_filteringMode );

   // fill the widgets with initial data
   for ( uint i = 0; i < TAM_COUNT; ++i )
   {
      m_addressingMode->addItem( g_uvwAddressingModeNames[i] );
   }

   for ( uint i = 0; i < TFM_COUNT; ++i )
   {
      m_filteringMode->addItem( g_filteringModeNames[i] );
   }

   // select the currently held values
   const TextureSamplerSettings& val = reinterpret_cast< const TextureSamplerSettings& >( m_property->get() );
   m_addressingMode->setCurrentIndex( val.m_addressU ); // the same value will be propagated across the all mode at this point
   m_filteringMode->setCurrentIndex( val.m_minFilter );

   // connect the listeners
   connect( m_addressingMode, SIGNAL( currentIndexChanged( const QString& ) ), this, SLOT( addressingModeChanged( const QString& ) ) );
   connect( m_filteringMode, SIGNAL( currentIndexChanged( const QString& ) ), this, SLOT( filteringModeChanged( const QString& ) ) );
}

///////////////////////////////////////////////////////////////////////////////

void TextureSamplerPropertyEditor::onPropertyChanged()
{
   // update the selected values
   const TextureSamplerSettings& val = reinterpret_cast< const TextureSamplerSettings& >( m_property->get() );
   m_addressingMode->setCurrentIndex( val.m_addressU );
   m_filteringMode->setCurrentIndex( val.m_minFilter );
}

///////////////////////////////////////////////////////////////////////////////

void TextureSamplerPropertyEditor::addressingModeChanged( const QString& modeStr )
{
   TextureSamplerSettings val = reinterpret_cast< const TextureSamplerSettings& >( m_property->get() );
   TextureAddressingMode newMode = addressModeToIndex( modeStr );
   val.m_addressU = newMode;
   val.m_addressV = newMode;
   val.m_addressW = newMode;
   m_property->set( val );
}

///////////////////////////////////////////////////////////////////////////////

void TextureSamplerPropertyEditor::filteringModeChanged( const QString& modeStr )
{
   TextureSamplerSettings val = reinterpret_cast< const TextureSamplerSettings& >( m_property->get() );
   TextureFilteringMode newMode = filteringModeToIndex( modeStr );
   val.m_minFilter = newMode;
   val.m_magFilter = newMode;
   val.m_mipFilter = newMode;
   m_property->set( val );
}

///////////////////////////////////////////////////////////////////////////////

TextureAddressingMode TextureSamplerPropertyEditor::addressModeToIndex( const QString& modeStr )
{
   for ( uint i = 0; i < TAM_COUNT; ++i )
   {
      if ( modeStr == g_uvwAddressingModeNames[i] )
      {
         return ( TextureAddressingMode ) i;
      }
   }

   return TAM_WRAP;
}

///////////////////////////////////////////////////////////////////////////////

TextureFilteringMode TextureSamplerPropertyEditor::filteringModeToIndex( const QString& modeStr )
{
   for ( uint i = 0; i < TFM_COUNT; ++i )
   {
      if ( modeStr == g_filteringModeNames[i] )
      {
         return ( TextureFilteringMode ) i;
      }
   }

   return TFM_LINEAR;
}

///////////////////////////////////////////////////////////////////////////////
