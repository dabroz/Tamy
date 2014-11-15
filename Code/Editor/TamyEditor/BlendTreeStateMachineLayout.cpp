#include "BlendTreeStateMachineLayout.h"
#include "BlendTreeStateItem.h"
#include "BlendTreeTransitionItem.h"
#include "core-AI\BlendTree.h"
#include "core-AI\BlendTreeStateMachine.h"
#include "core-AI\BlendTreeStateTransition.h"
#include <QtWidgets\QApplication>
#include <QtGui\QBrush>
#include <QtGui\QPen>
#include <QtGui\QImage>
#include <QtWidgets\QGraphicsSceneMouseEvent>
#include <QtGui\QKeyEvent>
#include <QtWidgets\QGraphicsLineItem>
#include <QtWidgets\QMenu>
#include "core\List.h"
#include "core\ReflectionType.h"

// ui utils
#include "MenuUtils.h"
#include "TypeDescFactory.h"


///////////////////////////////////////////////////////////////////////////////

BlendTreeStateMachineLayout::BlendTreeStateMachineLayout( BlendTreeStateMachine* stateMachine )
   : m_stateMachine( stateMachine )
   , m_drawnTransitionStartItem( NULL )
   , m_drawnTransition( NULL )
{
   moveToThread( QApplication::instance()->thread() );

   QImage image( ":/TamyEditor/Resources/Icons/Editor/pipelineEditorBackground.png" );
   QBrush bgImageBrush( image );
   setBackgroundBrush( bgImageBrush );

   BlendTree* tree = m_stateMachine->getBlendTree();
   tree->attachBlendTreeListener( this );

   m_itemDescriptionsFactory = new TypeDescFactory< BlendTreeNode >( "unknownBlendTreeIcon.png" );

   // populate the layout
   stateMachine->pullStructure( this );
}

///////////////////////////////////////////////////////////////////////////////

BlendTreeStateMachineLayout::~BlendTreeStateMachineLayout()
{
   BlendTree* tree = m_stateMachine->getBlendTree();
   tree->detachBlendTreeListener( this );

   m_stateItems.clear();
   m_transitionItems.clear();

   delete m_itemDescriptionsFactory;
   m_itemDescriptionsFactory = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeStateMachineLayout::onNodeAdded( BlendTreeNode* parentNode, int insertionIdx, BlendTreeNode* node )
{
   if ( parentNode != m_stateMachine )
   {
      // this change doesn't concern us
      return;
   }

   BlendTreeStateItem* item = new BlendTreeStateItem( node );
   m_stateItems.pushBack( item );
   addItem( item );
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeStateMachineLayout::onNodeRemoved( BlendTreeNode* parentNode, BlendTreeNode* node )
{
   if ( parentNode != m_stateMachine )
   {
      // this change doesn't concern us
      return;
   }

   if ( m_selectedObject == node )
   {
      // inform the editor that the node is no longer selected
      m_selectedObject = NULL;
      emit onObjectSelected( NULL );
   }

   // find the item and remove it
   for ( List< BlendTreeStateItem* >::iterator it = m_stateItems.begin(); !it.isEnd(); ++it )
   {
      BlendTreeStateItem* item = *it;
      if ( item->getNode() == node )
      {
         removeItem( item );
         delete item;
         it.markForRemoval();
         break;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeStateMachineLayout::onNodeChanged( BlendTreeNode* node )
{
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeStateMachineLayout::onTransitionAdded( BlendTreeStateMachine* parentStateMachine, BlendTreeStateTransition* transition )
{
   if ( parentStateMachine != m_stateMachine )
   {
      // this change doesn't concern us
      return;
   }

   BlendTreeTransitionItem* transitionItem = new BlendTreeTransitionItem( this, transition );
   m_transitionItems.pushBack( transitionItem );
   addItem( transitionItem );

   // inform the state items this transition connects about it
   BlendTreeStateItem* startStateItem = findStateItem( transition->m_startState );
   BlendTreeStateItem* endStateItem = findStateItem( transition->m_endState );
   ASSERT( startStateItem );
   ASSERT( endStateItem );
   startStateItem->addTransition( transitionItem, false );
   endStateItem->addTransition( transitionItem, true );
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeStateMachineLayout::onTransitionRemoved( BlendTreeStateMachine* parentStateMachine, BlendTreeStateTransition* transition )
{
   if ( parentStateMachine != m_stateMachine )
   {
      // this change doesn't concern us
      return;
   }

   if ( m_selectedObject == transition )
   {
      // inform the editor that the transition is no longer selected
      m_selectedObject = NULL;
      emit onObjectSelected( NULL );
   }

   // find the item and remove it
   for ( List< BlendTreeTransitionItem* >::iterator it = m_transitionItems.begin(); !it.isEnd(); ++it )
   {
      BlendTreeTransitionItem* transitionItem = *it;
      if ( transitionItem->getTransition() == transition )
      {
         // remove records about the transition from the blocks that it used to connect
         BlendTreeStateItem* startStateItem = findStateItem( transition->m_startState );
         BlendTreeStateItem* endStateItem = findStateItem( transition->m_endState );
         ASSERT( startStateItem );
         ASSERT( endStateItem );
         startStateItem->removeTransition( transitionItem, false );
         endStateItem->removeTransition( transitionItem, true );

         removeItem( transitionItem );
         delete transitionItem;
         it.markForRemoval();
         break;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

BlendTreeStateItem* BlendTreeStateMachineLayout::findStateItem( BlendTreeNode* node ) const
{
   for ( List< BlendTreeStateItem* >::const_iterator it = m_stateItems.begin(); !it.isEnd(); ++it )
   {
      BlendTreeStateItem* item = *it;
      if ( item->getNode() == node )
      {
         return item;
      }
   }

   return NULL;
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeStateMachineLayout::startDrawingTransition( BlendTreeStateItem* startStateItem )
{
   ASSERT( startStateItem != NULL );
   ASSERT_MSG( m_drawnTransitionStartItem == NULL, "Another transition is currently being drawn" );

   m_drawnTransitionStartItem = startStateItem;

   // create a temporary line that will represent the created transition
   m_drawnTransition = new QGraphicsLineItem();
   m_drawnTransition->setPen( BlendTreeTransitionItem::s_transitionPen );
   addItem( m_drawnTransition );
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeStateMachineLayout::mouseMoveEvent( QGraphicsSceneMouseEvent* mouseEvent )
{
   QGraphicsScene::mouseMoveEvent( mouseEvent );

   if ( m_drawnTransition )
   {
      // update drawn transition
      ASSERT( m_drawnTransitionStartItem != NULL );
     
      QPointF mousePos = mouseEvent->scenePos();
      QPointF start, end;
      BlendTreeTransitionItem::calculateDrawnTerminatorPos( m_drawnTransitionStartItem, mousePos, start, end );

      m_drawnTransition->setLine( start.rx(), start.ry(), end.rx(), end.ry() );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeStateMachineLayout::mouseReleaseEvent( QGraphicsSceneMouseEvent* event )
{
   QGraphicsScene::mouseReleaseEvent( event );

   if ( event->button() == Qt::MiddleButton )
   {
      BlendTreeStateItem* startStateItem = m_drawnTransitionStartItem;
      clearTransitionDrawing();

      QGraphicsItem* item = itemAt( event->scenePos(), QTransform() );
      BlendTreeStateItem* endStateItem = dynamic_cast< BlendTreeStateItem* >( item );
      if ( item && !endStateItem )
      {
         // we could have released the mouse button over a widget embedded in a block - in that case query for its parent
         endStateItem = dynamic_cast< BlendTreeStateItem* >( item->parentItem() );
      }

      if ( startStateItem && endStateItem && endStateItem != startStateItem )
      {
         // create a transition
         BlendTreeNode* startState = startStateItem->getNode();
         BlendTreeNode* endState = endStateItem->getNode();

         BlendTreeStateTransition* transition = new BlendTreeStateTransition();
         transition->setConnection( startState, endState );
         m_stateMachine->addTransition( transition );
      }

      // consume the event
      event->accept();
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeStateMachineLayout::clearTransitionDrawing()
{
   // delete the temporary line
   removeItem( m_drawnTransition );
   delete m_drawnTransition;
   m_drawnTransition = NULL;

   m_drawnTransitionStartItem = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeStateMachineLayout::objectSelected( ReflectionObject* object )
{
   if ( m_selectedObject == object )
   {
      return;
   }

   m_selectedObject = object;
   emit onObjectSelected( object );
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeStateMachineLayout::keyPressEvent( QKeyEvent* keyEvent )
{
   if ( keyEvent->key() == Qt::Key_Delete )
   {
      if ( m_selectedObject->isA< BlendTreeStateTransition >() )
      {
         BlendTreeStateTransition* removedTransition = static_cast< BlendTreeStateTransition* >( m_selectedObject );
         m_stateMachine->removeTransition( removedTransition );
      }
      else if ( m_selectedObject->isA< BlendTreeNode >() )
      {
         BlendTreeNode* removedState = static_cast< BlendTreeNode* >( m_selectedObject );
         m_stateMachine->remove( removedState );
      }

      keyEvent->accept();
   }
   else
   {
      QGraphicsScene::keyPressEvent( keyEvent );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeStateMachineLayout::createContextMenu( QMenu* menu, const QPointF& scenePos )
{
   QGraphicsItem* item = itemAt( scenePos, QTransform() );

   // removal actions
   {
      BlendTreeStateItem* stateItem = dynamic_cast< BlendTreeStateItem* >( item );
      BlendTreeTransitionItem* transitionItem = dynamic_cast< BlendTreeTransitionItem* >( item );

      if ( stateItem )
      {
         RemoveStateAction* action = new RemoveStateAction( m_stateMachine, stateItem->getNode() );
         menu->addAction( action );
      }
      else if ( transitionItem )
      {
         RemoveTransitionAction* action = new RemoveTransitionAction( m_stateMachine, transitionItem->getTransition() );
         menu->addAction( action );
      }
   }

   // states addition action
   QMenu* addMenu = menu->addMenu( "Add" );
   {
      class AddResourceActionFactory : public MenuActionsFactory
      {
      private:
         BlendTreeStateMachine*        m_stateMachine;
         TreeWidgetDescFactory*        m_itemsFactory;

      public:
         AddResourceActionFactory( BlendTreeStateMachine* stateMachine, TreeWidgetDescFactory* itemsFactory )
            : m_stateMachine( stateMachine )
            , m_itemsFactory( itemsFactory )
         {}

         QAction* create( const QIcon& icon, const QString& desc, int itemTypeIdx, QWidget* parentWidget ) const
         {
            QAction* action = new AddBlendTreeStateAction( icon, desc, itemTypeIdx, m_itemsFactory, m_stateMachine );
            return action;
         }
      };


      // "Add" submenu
      AddResourceActionFactory actionsFactory( m_stateMachine, m_itemDescriptionsFactory );
      MenuUtils::itemsListMenu( m_itemDescriptionsFactory, actionsFactory, addMenu );
   }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

AddBlendTreeStateAction::AddBlendTreeStateAction( const QIcon& icon, const QString& desc, unsigned int typeIdx, TreeWidgetDescFactory* itemsFactory, BlendTreeStateMachine* stateMachine )
   : QAction( icon, desc, NULL )
   , m_typeIdx( typeIdx )
   , m_stateMachine( stateMachine )
   , m_itemsFactory( itemsFactory )
{
   connect( this, SIGNAL( triggered() ), this, SLOT( onTriggered() ) );
}

///////////////////////////////////////////////////////////////////////////////

void AddBlendTreeStateAction::onTriggered()
{
   const SerializableReflectionType* nodeType = m_itemsFactory->getClass( m_typeIdx );
   ASSERT_MSG( nodeType, "Requested a node of an undefined type" );

   BlendTreeNode* newNode = nodeType->instantiate< BlendTreeNode >();
   m_stateMachine->add( newNode );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

RemoveStateAction::RemoveStateAction( BlendTreeStateMachine* stateMachine, BlendTreeNode* state )
   : QAction( QIcon( ":/TamyEditor/Resources/Icons/Editor/quit.png" ), "Remove state", NULL )
   , m_stateMachine( stateMachine )
   , m_state( state )
{
   connect( this, SIGNAL( triggered() ), this, SLOT( onTriggered() ) );
}

///////////////////////////////////////////////////////////////////////////////

void RemoveStateAction::onTriggered()
{
   m_stateMachine->remove( m_state );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

RemoveTransitionAction::RemoveTransitionAction( BlendTreeStateMachine* stateMachine, BlendTreeStateTransition* transition )
   : QAction( QIcon( ":/TamyEditor/Resources/Icons/Editor/quit.png" ), "Remove transition", NULL )
   , m_stateMachine( stateMachine )
   , m_transition( transition )
{
   connect( this, SIGNAL( triggered() ), this, SLOT( onTriggered() ) );
}

///////////////////////////////////////////////////////////////////////////////

void RemoveTransitionAction::onTriggered()
{
   m_stateMachine->removeTransition( m_transition );
}

///////////////////////////////////////////////////////////////////////////////
