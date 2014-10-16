/// @file   TamyEditor/BlendTreeVariableEditor.h
/// @brief  a dedicated BlendTreeVariable editor
#pragma once

#include <QtWidgets\QFrame>
#include "core\GenericFactory.h"
#include "core\ReflectionObjectChangeListener.h"


///////////////////////////////////////////////////////////////////////////////

class BlendTree;
class BlendTreeVariable;
class BlendTreePlayer;
class RuntimeDataBuffer;
class BTVEditor;
class QHBoxLayout;
class QCheckBox;
class QSpinBox;
class QDoubleSpinBox;
class QLabel;
class BTVarBool;
class BTVarFloat;
class BTVarInt;
class BTVarVector;

///////////////////////////////////////////////////////////////////////////////

class BlendTreeVariableEditor : public QFrame
{
   Q_OBJECT

private:
   BlendTree&                                            m_blendTree;
   BlendTreeVariable*                                    m_variable;
   BlendTreePlayer*                                      m_activePlayer;

   GenericFactory< BlendTreeVariable, BTVEditor >*       m_editorsFactory;

   QHBoxLayout*                                          m_variableEditorFrameLayout;

public:
   /**
    * Constructor.
    *
    * @param parent
    * @param blendTree
    * @param activePlayer
    */
   BlendTreeVariableEditor( QWidget* parent, BlendTree& blendTree, BlendTreePlayer* activePlayer );
   ~BlendTreeVariableEditor();

   /**
    * Initializes the editor.
    *
    * @param btVariable
    */
   void initialize( BlendTreeVariable* btVariable );

public slots:
   void removeVariable( bool );
};

///////////////////////////////////////////////////////////////////////////////

/**
 * A base class for all BlendTreeVariable value editors.
 */
class BTVEditor : public QFrame
{
   Q_OBJECT

protected:
   BlendTreePlayer*        m_player;

public:
   /**
    * Constructor.
    */
   BTVEditor();
   virtual ~BTVEditor() {}

   /**
    * Sets a pointer to the active instance of BlendTreePlayer.
    *
    * @param player
    */
   virtual void initializePlayer( BlendTreePlayer* player );
};

///////////////////////////////////////////////////////////////////////////////

class BTVBool : public BTVEditor, public ReflectionObjectChangeListener
{
   Q_OBJECT

private:
   BTVarBool&           m_variable;

   QCheckBox*           m_checkBox;

public:
   /**
    * Constructor
    *
    * @param variable
    */
   BTVBool( BTVarBool& variable );
   ~BTVBool();

   // -------------------------------------------------------------------------
   // BTVEditor implementation
   // -------------------------------------------------------------------------
   void initializePlayer( BlendTreePlayer* player );

   // -------------------------------------------------------------------------
   // ReflectionObjectChangeListener
   // -------------------------------------------------------------------------
   void onObservedPropertyChanged( ReflectionProperty& property );

public slots:
   void valChanged( int );
};

///////////////////////////////////////////////////////////////////////////////

class BTVFloat : public BTVEditor, public ReflectionObjectChangeListener
{
   Q_OBJECT

private:
   BTVarFloat&          m_variable;

   QDoubleSpinBox*      m_val;

public:
   /**
    * Constructor
    *
    * @param variable
    */
   BTVFloat( BTVarFloat& variable );
   ~BTVFloat();

   // -------------------------------------------------------------------------
   // BTVEditor implementation
   // -------------------------------------------------------------------------
   void initializePlayer( BlendTreePlayer* player );

   // -------------------------------------------------------------------------
   // ReflectionObjectChangeListener
   // -------------------------------------------------------------------------
   void onObservedPropertyChanged( ReflectionProperty& property );

public slots:
   void valChanged( double val );
};

///////////////////////////////////////////////////////////////////////////////

class BTVInt : public BTVEditor, public ReflectionObjectChangeListener
{
   Q_OBJECT

private:
   BTVarInt&            m_variable;

   QSpinBox*            m_val;

public:
   /**
    * Constructor
    *
    * @param variable
    */
   BTVInt( BTVarInt& variable );
   ~BTVInt();

   // -------------------------------------------------------------------------
   // BTVEditor implementation
   // -------------------------------------------------------------------------
   void initializePlayer( BlendTreePlayer* player );

   // -------------------------------------------------------------------------
   // ReflectionObjectChangeListener
   // -------------------------------------------------------------------------
   void onObservedPropertyChanged( ReflectionProperty& property );

public slots:
   void valChanged( int val );
};

///////////////////////////////////////////////////////////////////////////////

class BTVVector : public BTVEditor, public ReflectionObjectChangeListener
{
   Q_OBJECT

private:
   BTVarVector&            m_variable;

   QDoubleSpinBox*         m_xVal;
   QDoubleSpinBox*         m_yVal;
   QDoubleSpinBox*         m_zVal;
public:
   /**
    * Constructor
    *
    * @param variable
    */
   BTVVector( BTVarVector& variable );
   ~BTVVector();

   // -------------------------------------------------------------------------
   // BTVEditor implementation
   // -------------------------------------------------------------------------
   void initializePlayer( BlendTreePlayer* player );

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
