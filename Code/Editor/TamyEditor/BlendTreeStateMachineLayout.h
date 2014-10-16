/// @file   TamyEditor/BlendTreeStateMachineLayout.h
/// @brief  blend tree state machine's layout
#pragma once

#include <QtWidgets\QGraphicsScene>
#include <QtWidgets\QAction>
#include "core-AI\BlendTreeListener.h"
#include "core\List.h"


///////////////////////////////////////////////////////////////////////////////

class BlendTreeStateMachine;
class BlendTreeStateTransition;
class BlendTreeStateItem;
class BlendTreeTransitionItem;
class QGraphicsLineItem;
class ReflectionObject;
class QMenu;
class TreeWidgetDescFactory;
class QIcon;

///////////////////////////////////////////////////////////////////////////////

/**
 * Layout of a blend tree state machine.
 */
class BlendTreeStateMachineLayout : public QGraphicsScene, public BlendTreeListener
{
   Q_OBJECT 

private:
   BlendTreeStateMachine*              m_stateMachine;

   TreeWidgetDescFactory*              m_itemDescriptionsFactory;

   List< BlendTreeStateItem* >         m_stateItems;
   List< BlendTreeTransitionItem* >    m_transitionItems;

   BlendTreeStateItem*                 m_drawnTransitionStartItem;
   QGraphicsLineItem*                  m_drawnTransition;

   ReflectionObject*                   m_selectedObject;

public:
   /**
    * Constructor.
    *
    * @param stateMachine
    */
   BlendTreeStateMachineLayout( BlendTreeStateMachine* stateMachine );
   ~BlendTreeStateMachineLayout();

   /**
    * Searches for an item that corresponds to the specified node.
    *
    * @param node
    */
   BlendTreeStateItem* findStateItem( BlendTreeNode* node ) const;

   /**
    * Called when an item that represents a blend tree object gets selected.
    *
    * @param object
    */
   void objectSelected( ReflectionObject* object );

   /**
    * Called when a popup menu is about to be displayed.
    * Gives a chance to fill the menu with context-sensitive options.
    *
    * @param menu
    * @param scenePos
    */
   void createContextMenu( QMenu* menu, const QPointF& scenePos );

   // -------------------------------------------------------------------------
   // Transition drawing
   // -------------------------------------------------------------------------

   /**
    * Starts negotiating a transition between states.
    *
    * @param startStateItem
    */
   void startDrawingTransition( BlendTreeStateItem* startStateItem );

   // -------------------------------------------------------------------------
   // BlendTreeListener implementation
   // -------------------------------------------------------------------------
   void onNodeAdded( BlendTreeNode* parentNode, int insertionIdx, BlendTreeNode* node );
   void onNodeRemoved( BlendTreeNode* parentNode, BlendTreeNode* node );
   void onNodeChanged( BlendTreeNode* node );
   void onTransitionAdded( BlendTreeStateMachine* parentStateMachine, BlendTreeStateTransition* transition );
   void onTransitionRemoved( BlendTreeStateMachine* parentStateMachine, BlendTreeStateTransition* transition );

   // -------------------------------------------------------------------------
   // QGraphicsScene implementation
   // -------------------------------------------------------------------------
   void mouseMoveEvent( QGraphicsSceneMouseEvent* mouseEvent );
   void mouseReleaseEvent( QGraphicsSceneMouseEvent* event );
   void keyPressEvent( QKeyEvent* keyEvent );

signals:
   /**
    * Called when one of the objects in the scene is selected.
    *
    * @param object
    */
   void onObjectSelected( ReflectionObject* object );

private:
   void clearTransitionDrawing();
};

///////////////////////////////////////////////////////////////////////////////

class AddBlendTreeStateAction : public QAction
{
   Q_OBJECT

private:
   TreeWidgetDescFactory*              m_itemsFactory;
   BlendTreeStateMachine*              m_stateMachine;
   int                                 m_typeIdx;

public:
   /**
    * Constructor.
    *
    * @param icon
    * @param desc
    * @param typeIdx
    * @param itemsFactory
    * @param stateMachine
    */
   AddBlendTreeStateAction( const QIcon& icon, const QString& desc, unsigned int typeIdx, TreeWidgetDescFactory* itemsFactory, BlendTreeStateMachine* stateMachine );

public slots:
   void onTriggered();
};

///////////////////////////////////////////////////////////////////////////////

class RemoveStateAction : public QAction
{
   Q_OBJECT

private:
   BlendTreeStateMachine*              m_stateMachine;
   BlendTreeNode*                      m_state;

public:
   /**
    * Constructor.
    *
    * @param stateMachine
    * @param state
    */
   RemoveStateAction( BlendTreeStateMachine* stateMachine, BlendTreeNode* state );

public slots:
   void onTriggered();
};

///////////////////////////////////////////////////////////////////////////////

class RemoveTransitionAction : public QAction
{
   Q_OBJECT

private:
   BlendTreeStateMachine*              m_stateMachine;
   BlendTreeStateTransition*           m_transition;

public:
   /**
    * Constructor.
    *
    * @param stateMachine
    * @param transition
    */
   RemoveTransitionAction( BlendTreeStateMachine* stateMachine, BlendTreeStateTransition* transition );

public slots:
   void onTriggered();
};

///////////////////////////////////////////////////////////////////////////////
