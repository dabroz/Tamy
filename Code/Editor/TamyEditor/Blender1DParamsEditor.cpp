#include "Blender1DParamsEditor.h"
#include "core-AI\BlendTree.h"
#include "core-AI\BlendTreeBlender1D.h"
#include <QtWidgets\QTreeView>
#include <QtGui\QStandardItemModel>
#include <QtWidgets\QVBoxLayout>
#include "BlendTreeEditor.h"
#include "Blender1DParamsListDelegate.h"
#include "QPropertiesView.h"


///////////////////////////////////////////////////////////////////////////////

Blender1DParamsEditor::Blender1DParamsEditor( BlendTreeBlender1D& blenderNode )
   : BTBFEditor( blenderNode )
   , m_blenderNode( blenderNode )
{
   QVBoxLayout* layout = new QVBoxLayout( this );
   layout->setMargin( 0 );
   layout->setSpacing( 0 );
   setLayout( layout );

   m_propertiesView = new QPropertiesView();
   {
      m_propertiesView->set( blenderNode );
      layout->addWidget( m_propertiesView );
   }

   m_paramsListView = new QTreeView( this );
   layout->addWidget( m_paramsListView );

   m_dataModel = new QStandardItemModel( this );
   {
      m_dataModel->setColumnCount( 2 );

      QStringList headerLabels;
      headerLabels.push_back( "Node name" );
      headerLabels.push_back( "Parameter value" );
      m_dataModel->setHorizontalHeaderLabels( headerLabels );

      m_paramsListView->setModel( m_dataModel );
   }

   m_editTimeDelegate = new Blender1DParamsListDelegate( this, m_blenderNode );
   m_playTimeItemDelegate = m_paramsListView->itemDelegate();
   setPlayingMode( false );
}

///////////////////////////////////////////////////////////////////////////////

Blender1DParamsEditor::~Blender1DParamsEditor()
{
   delete m_editTimeDelegate;
   m_editTimeDelegate = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void Blender1DParamsEditor::initialize( BlendTreeEditor& mainEditor )
{
   BlendTree& blendTree = mainEditor.getBlendTree();
   blendTree.attachListener( this );

   m_blenderNode.pullStructure( this );
}

///////////////////////////////////////////////////////////////////////////////

void Blender1DParamsEditor::deinitialize( BlendTreeEditor& mainEditor )
{
   BlendTree& blendTree = mainEditor.getBlendTree();
   blendTree.detachListener( this );
}

///////////////////////////////////////////////////////////////////////////////

BlendTreeNode* Blender1DParamsEditor::getNode() const
{
   return &m_blenderNode;
}

///////////////////////////////////////////////////////////////////////////////

void Blender1DParamsEditor::setPlayingMode( bool isPlaying )
{
   if ( isPlaying )
   {
      m_paramsListView->setItemDelegate( m_playTimeItemDelegate );
   }
   else
   {
      m_paramsListView->setItemDelegate( m_editTimeDelegate );
   }
}

///////////////////////////////////////////////////////////////////////////////

void Blender1DParamsEditor::onNodeAdded( BlendTreeNode* parentNode, int insertionIdx, BlendTreeNode* node )
{
   if ( parentNode != &m_blenderNode )
   {
      return;
   }

   float parameterValue = m_blenderNode.getParameterValue( insertionIdx );

   QStandardItem* childNameItem = new QStandardItem( node->getName().c_str() );
   childNameItem->setData( QVariant::fromValue< void* >( node ), BTB1D_PARAMETER_ROLE );

   QStandardItem* paramValueItem = new QStandardItem( QString( "%0" ).arg( parameterValue ) );
   paramValueItem->setData( QVariant( insertionIdx ), BTB1D_PARAMETER_ROLE );

   QList< QStandardItem* > columnItems;
   columnItems.push_back( childNameItem );
   columnItems.push_back( paramValueItem );
   m_dataModel->insertRow( insertionIdx, columnItems );
}

///////////////////////////////////////////////////////////////////////////////

void Blender1DParamsEditor::onNodeRemoved( BlendTreeNode* parentNode, BlendTreeNode* node )
{
   if ( parentNode != &m_blenderNode )
   {
      return;
   }

   // find a row that corresponds to the said child and remove it
   int rowCount = m_dataModel->rowCount();
   for ( int i = 0; i < rowCount; ++i )
   {
      QStandardItem* item = m_dataModel->item( i, 0 );
      BlendTreeNode* checkedNode = ( BlendTreeNode* )( item->data( BTB1D_PARAMETER_ROLE ).value< void* >() );

      if ( checkedNode == node )
      {
         // that's the one
         m_dataModel->removeRow( i );
         break;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void Blender1DParamsEditor::onNodeChanged( BlendTreeNode* node )
{
   int rowCount = m_dataModel->rowCount();
   for ( int i = 0; i < rowCount; ++i )
   {
      QStandardItem* item = m_dataModel->item( i, 0 );
      BlendTreeNode* checkedNode = ( BlendTreeNode* )( item->data( BTB1D_PARAMETER_ROLE ).value< void* >() );

      if ( checkedNode == node )
      {
         // that's the one - update its name
         m_dataModel->item( i, 0 )->setText( node->getName().c_str() );
         break;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////
