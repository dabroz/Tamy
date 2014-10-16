/// @file   TamyEditor/BehaviorTreeVariableEditor.h
/// @brief  a dedicated BehaviorTreeVariable editor
#pragma once

#include <QtWidgets\QFrame>
#include "core\GenericFactory.h"
#include "core\ReflectionObjectChangeListener.h"


///////////////////////////////////////////////////////////////////////////////

class BehaviorTree;
class BehaviorTreeVariable;
class BehaviorTreeRunner;
class RuntimeDataBuffer;
class BehTreeVarEditor;
class QHBoxLayout;
class QCheckBox;
class QSpinBox;
class QDoubleSpinBox;
class QLabel;
class BehTreeVarBool;
class BehTreeVarFloat;
class BehTreeVarInt;
class BehTreeVarVector;
class BehTreeVarReflectionObject;

///////////////////////////////////////////////////////////////////////////////

class BehaviorTreeVariableEditor : public QFrame
{
   Q_OBJECT

private:
   BehaviorTree&                                                  m_behaviorTree;
   BehaviorTreeVariable*                                          m_variable;
   BehaviorTreeRunner*                                            m_activePlayer;

   GenericFactory< BehaviorTreeVariable, BehTreeVarEditor >*      m_editorsFactory;

   QHBoxLayout*                                                   m_variableEditorFrameLayout;

public:
   /**
    * Constructor.
    *
    * @param parent
    * @param BehaviorTree
    * @param activePlayer
    */
   BehaviorTreeVariableEditor( QWidget* parent, BehaviorTree& BehaviorTree, BehaviorTreeRunner* activePlayer );
   ~BehaviorTreeVariableEditor();

   /**
    * Initializes the editor.
    *
    * @param btVariable
    */
   void initialize( BehaviorTreeVariable* btVariable );

public slots:
   void removeVariable( bool );
};

///////////////////////////////////////////////////////////////////////////////

/**
 * A base class for all BehaviorTreeVariable value editors.
 */
class BehTreeVarEditor : public QFrame
{
   Q_OBJECT

protected:
   BehaviorTreeRunner*        m_player;

public:
   /**
    * Constructor.
    */
   BehTreeVarEditor();
   virtual ~BehTreeVarEditor() {}

   /**
    * Sets a pointer to the active instance of BehaviorTreeRunner.
    *
    * @param player
    */
   virtual void initializePlayer( BehaviorTreeRunner* player );
};

///////////////////////////////////////////////////////////////////////////////

class BehTreeVarBoolEditor : public BehTreeVarEditor, public ReflectionObjectChangeListener
{
   Q_OBJECT

private:
   BehTreeVarBool&      m_variable;

   QCheckBox*           m_checkBox;

public:
   /**
    * Constructor
    *
    * @param variable
    */
   BehTreeVarBoolEditor( BehTreeVarBool& variable );
   ~BehTreeVarBoolEditor();

   // -------------------------------------------------------------------------
   // BehTreeVarEditor implementation
   // -------------------------------------------------------------------------
   void initializePlayer( BehaviorTreeRunner* player );

   // -------------------------------------------------------------------------
   // ReflectionObjectChangeListener
   // -------------------------------------------------------------------------
   void onObservedPropertyChanged( ReflectionProperty& property );

public slots:
   void valChanged( int );
};

///////////////////////////////////////////////////////////////////////////////

class BehTreeVarFloatEditor : public BehTreeVarEditor, public ReflectionObjectChangeListener
{
   Q_OBJECT

private:
   BehTreeVarFloat&     m_variable;

   QDoubleSpinBox*      m_val;

public:
   /**
    * Constructor
    *
    * @param variable
    */
   BehTreeVarFloatEditor( BehTreeVarFloat& variable );
   ~BehTreeVarFloatEditor();

   // -------------------------------------------------------------------------
   // BehTreeVarEditor implementation
   // -------------------------------------------------------------------------
   void initializePlayer( BehaviorTreeRunner* player );

   // -------------------------------------------------------------------------
   // ReflectionObjectChangeListener
   // -------------------------------------------------------------------------
   void onObservedPropertyChanged( ReflectionProperty& property );

public slots:
   void valChanged( double val );
};

///////////////////////////////////////////////////////////////////////////////

class BehTreeVarIntEditor : public BehTreeVarEditor, public ReflectionObjectChangeListener
{
   Q_OBJECT

private:
   BehTreeVarInt&       m_variable;

   QSpinBox*            m_val;

public:
   /**
    * Constructor
    *
    * @param variable
    */
   BehTreeVarIntEditor( BehTreeVarInt& variable );
   ~BehTreeVarIntEditor();

   // -------------------------------------------------------------------------
   // BTVEditor implementation
   // -------------------------------------------------------------------------
   void initializePlayer( BehaviorTreeRunner* player );

   // -------------------------------------------------------------------------
   // ReflectionObjectChangeListener
   // -------------------------------------------------------------------------
   void onObservedPropertyChanged( ReflectionProperty& property );

public slots:
   void valChanged( int val );
};

///////////////////////////////////////////////////////////////////////////////

class BehTreeVarVectorEditor : public BehTreeVarEditor, public ReflectionObjectChangeListener
{
   Q_OBJECT

private:
   BehTreeVarVector&       m_variable;

   QDoubleSpinBox*         m_xVal;
   QDoubleSpinBox*         m_yVal;
   QDoubleSpinBox*         m_zVal;
public:
   /**
    * Constructor
    *
    * @param variable
    */
   BehTreeVarVectorEditor( BehTreeVarVector& variable );
   ~BehTreeVarVectorEditor();

   // -------------------------------------------------------------------------
   // BTVEditor implementation
   // -------------------------------------------------------------------------
   void initializePlayer( BehaviorTreeRunner* player );

   // -------------------------------------------------------------------------
   // ReflectionObjectChangeListener
   // -------------------------------------------------------------------------
   void onObservedPropertyChanged( ReflectionProperty& property );

public slots:
   void xValChanged( double val );
   void yValChanged( double val );
   void zValChanged( double val );
};

///////////////////////////////////////////////////////////////////////////////

class BehTreeVarReflectionObjectEditor : public BehTreeVarEditor, public ReflectionObjectChangeListener
{
   Q_OBJECT

private:
   BehTreeVarReflectionObject&      m_variable;

   QLabel*                          m_display;

public:
   /**
    * Constructor
    *
    * @param variable
    */
   BehTreeVarReflectionObjectEditor( BehTreeVarReflectionObject& variable );
   ~BehTreeVarReflectionObjectEditor();

   // -------------------------------------------------------------------------
   // BehTreeVarEditor implementation
   // -------------------------------------------------------------------------
   void initializePlayer( BehaviorTreeRunner* player );

   // -------------------------------------------------------------------------
   // ReflectionObjectChangeListener
   // -------------------------------------------------------------------------
   void onObservedPropertyChanged( ReflectionProperty& property );
};

///////////////////////////////////////////////////////////////////////////////
