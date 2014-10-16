#include "QPropertiesView.h"
#include "core\ReflectionObject.h"
#include <QtWidgets\QWidget>
#include <QtWidgets\QBoxLayout>
#include <QtWidgets\QSpacerItem>

// properties
#include "Vec3PropertyEditor.h"
#include "MtxPropertyEditor.h"
#include "StringPropertyEditor.h"
#include "FloatPropertyEditor.h"
#include "IntPropertyEditor.h"
#include "BoolPropertyEditor.h"
#include "ResourcePropertyEditor.h"
#include "FilePathPropertyEditor.h"
#include "EnumPropertyEditor.h"
#include "ObjectPropertyEditor.h"
#include "QReflectionPropertyEditorComposite.h"
#include "QReflectionObjectEditor.h"
#include "ColorPropertyEditor.h"
#include "TextureSamplerPropertyEditor.h"

// custom properties
#include "StoryChapterPropertyEditor.h"
#include "StoryItemPropertyEditor.h"
#include "StoryActorPropertyEditor.h"


///////////////////////////////////////////////////////////////////////////////

QPropertiesView::QPropertiesView()
   : m_rootEditor( NULL )
{
   // setup the UI
   m_layout = new QVBoxLayout( this );
   m_layout->setAlignment( Qt::AlignTop );
   m_layout->setSpacing(0);
   m_layout->setMargin(0);

   setFrameStyle( QFrame::NoFrame );

   // setup editor factories
   initFactory();
}

///////////////////////////////////////////////////////////////////////////////

QPropertiesView::~QPropertiesView()
{
}

///////////////////////////////////////////////////////////////////////////////

void QPropertiesView::initFactory()
{
   // story objects
   associateAbstract< StoryItem*,         StoryItemPropertyEditor >();
   associateAbstract< StoryActor*,        StoryActorPropertyEditor >();
   associateAbstract< StoryChapter*,      StoryChapterPropertyEditor >();

   // base objects
   associateAbstract< Resource*,          ResourcePropertyEditor >();
   associateAbstract< ReflectionObject*,  ObjectPropertyEditor >();

   // primitives
   associate< Vector,                     Vec3PropertyEditor >();
   associate< Matrix,                     MtxPropertyEditor >();
   associate< std::string,                StringPropertyEditor >();
   associate< bool,                       BoolPropertyEditor >();
   associate< float,                      FloatPropertyEditor >();
   associate< double,                     DoublePropertyEditor >();
   associate< int,                        IntPropertyEditor >();
   associate< unsigned int,               UIntPropertyEditor >();
   associate< long,                       LongPropertyEditor >();
   associate< unsigned long,              ULongPropertyEditor >();
   associate< Color,                      ColorPropertyEditor >();
   associate< TextureSamplerSettings,     TextureSamplerPropertyEditor >();
   associate< FilePath,                   FilePathPropertyEditor >();

   defineEnums< EnumPropertyEditor >();
   defineArray< QReflectionPropertyEditorComposite >();
   defineObjectNode< QReflectionObjectEditor >();
}

///////////////////////////////////////////////////////////////////////////////

ReflectionObject* QPropertiesView::getEditedObject()
{
   if ( m_rootEditor )
   {
      return &m_rootEditor->getEditedObject();
   }
   else
   {
      return NULL;
   }
}

///////////////////////////////////////////////////////////////////////////////

void QPropertiesView::onSet( ReflectionObjectEditor* rootEditor )
{
   m_rootEditor = static_cast< QReflectionObjectEditor* >( rootEditor );
   m_layout->addWidget( m_rootEditor );
}

///////////////////////////////////////////////////////////////////////////////
