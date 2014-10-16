#include "BlendTreeEditorBottomFrame.h"
#include "BTBFEditor.h"
#include "core-AI\BlendTree.h"
#include <QtWidgets\QSlider>
#include <QtWidgets\QVBoxLayout>

// editors
#include "core-AI\BlendTreeAnimation.h"
#include "core-AI\BlendTreeBlender1D.h"
#include "AnimationEventsEditor.h"
#include "Blender1DParamsEditor.h"


///////////////////////////////////////////////////////////////////////////////

BlendTreeEditorBottomFrame::BlendTreeEditorBottomFrame( QWidget* parent, BlendTree& blendTree, BlendTreeEditor& mainEditor )
   : QTabWidget( parent )
   , m_blendTree( blendTree )
   , m_mainEditor( mainEditor )
{
   setTabsClosable( true );
   connect( this, SIGNAL( tabCloseRequested( int ) ), this, SLOT( onCloseTab( int ) ) );
   
   QVBoxLayout* layout = new QVBoxLayout( this );
   layout->setSpacing( 0 );
   layout->setMargin( 0 );
   setLayout( layout );

   m_blendTree.attachListener( this );

   m_editorsFactory = new GenericFactory< BlendTreeNode, BTBFEditor >();
   m_editorsFactory->associate< BlendTreeAnimation, AnimationEventsEditor >();
   m_editorsFactory->associate< BlendTreeBlender1D, Blender1DParamsEditor >();
}

///////////////////////////////////////////////////////////////////////////////

BlendTreeEditorBottomFrame::~BlendTreeEditorBottomFrame()
{
   m_blendTree.detachListener( this );

   int tabsCount = count();
   for( int i = tabsCount - 1; i >= 0; --i )
   {
      QWidget* embeddedWidget = widget( i );
      BTBFEditor* editor = dynamic_cast< BTBFEditor* >( embeddedWidget );
      if ( editor )
      {
         editor->deinitialize( m_mainEditor );
      }
      delete embeddedWidget;

      removeTab( i );
   }

   delete m_editorsFactory;
   m_editorsFactory = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeEditorBottomFrame::createEditor( BlendTreeNode* node )
{
   // remove the old widget
   int tabsCount = count();
   for( int i = 0; i < tabsCount; ++i )
   {
      QWidget* embeddedWidget = widget( i );

      BTBFEditor* editor = dynamic_cast< BTBFEditor* >( embeddedWidget );
      if ( editor && editor->getNode() == node )
      {
         // we already have that editor
         return;
      }
   }

   // plug in the new one
   BTBFEditor* newEditor = m_editorsFactory->create( *node );
   if ( newEditor )
   {
      newEditor->initialize( m_mainEditor );
      int newTabIdx = addTab( newEditor, node->getName().c_str() );
      setCurrentIndex( newTabIdx );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeEditorBottomFrame::setPlayingMode( bool playing )
{
   int tabsCount = count();
   for( int i = 0; i < tabsCount; ++i )
   {
      QWidget* embeddedWidget = widget( i );
      BTBFEditor* editor = dynamic_cast< BTBFEditor* >( embeddedWidget );
      if ( editor )
      {
         editor->setPlayingMode( playing );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeEditorBottomFrame::onNodeRemoved( BlendTreeNode* parentNode, BlendTreeNode* node )
{
   // remove the old widget
   int tabsCount = count();
   for( int i = 0; i < tabsCount; ++i )
   {
      QWidget* embeddedWidget = widget( i );
      BTBFEditor* editor = dynamic_cast< BTBFEditor* >( embeddedWidget );
      if ( editor && editor->getNode() == node )
      {
         editor->deinitialize( m_mainEditor );
         delete embeddedWidget;

         removeTab( i );
         break;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeEditorBottomFrame::onCloseTab( int tabIdx )
{
   // we can only close editor tabs
   QWidget* embeddedWidget = widget( tabIdx );
   BTBFEditor* editor = dynamic_cast< BTBFEditor* >( embeddedWidget );
   if ( editor )
   {
      editor->deinitialize( m_mainEditor );
      delete embeddedWidget;

      removeTab( tabIdx );
   }
}

///////////////////////////////////////////////////////////////////////////////
