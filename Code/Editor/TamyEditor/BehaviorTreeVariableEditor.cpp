#include "BehaviorTreeVariableEditor.h"
#include "core-AI\BehaviorTree.h"
#include "core-AI\BehaviorTreeRunner.h"
#include "core-AI\BehTreeVariable.h"

// ui elements
#include <QtWidgets\QLineEdit>
#include <QtWidgets\QPushButton>
#include <QtWidgets\QHBoxLayout>
#include <QtWidgets\QCheckBox>
#include <QtWidgets\QSpinBox>
#include <QtWidgets\QDoubleSpinBox>
#include <QtWidgets\QLabel>


///////////////////////////////////////////////////////////////////////////////

BehaviorTreeVariableEditor::BehaviorTreeVariableEditor( QWidget* parent, BehaviorTree& BehaviorTree, BehaviorTreeRunner* activePlayer )
   : QFrame( parent )
   , m_behaviorTree( BehaviorTree )
   , m_activePlayer( activePlayer )
   , m_variable( NULL )
{
   QHBoxLayout* layout = new QHBoxLayout( this );
   layout->setSpacing( 0 );
   layout->setMargin( 0 );
   setLayout( layout );

   QFrame* variableEditorFrame = new QFrame( this );
   {
      layout->addWidget( variableEditorFrame );

      m_variableEditorFrameLayout = new QHBoxLayout( variableEditorFrame );
      m_variableEditorFrameLayout->setSpacing( 0 );
      m_variableEditorFrameLayout->setMargin( 0 );
      variableEditorFrame->setLayout( m_variableEditorFrameLayout );
   }

   if ( !m_activePlayer )
   {
      // variables can only be removed when the scene is not being played
      QPushButton* removeEventButton = new QPushButton( this );
      removeEventButton->setIcon( QIcon( ":/TamyEditor/Resources/Icons/Editor/quit.png" ) );
      connect( removeEventButton, SIGNAL( clicked( bool ) ), this, SLOT( removeVariable( bool ) ) );
      layout->addWidget( removeEventButton );
   }

   // initialize the factory of editors
   m_editorsFactory = new GenericFactory< BehaviorTreeVariable, BehTreeVarEditor >();
   m_editorsFactory->associate< BehTreeVarBool, BehTreeVarBoolEditor >();
   m_editorsFactory->associate< BehTreeVarFloat, BehTreeVarFloatEditor >();
   m_editorsFactory->associate< BehTreeVarInt, BehTreeVarIntEditor >();
   m_editorsFactory->associate< BehTreeVarVector, BehTreeVarVectorEditor >();
   m_editorsFactory->associate< BehTreeVarReflectionObject, BehTreeVarReflectionObjectEditor >();
}

///////////////////////////////////////////////////////////////////////////////

BehaviorTreeVariableEditor::~BehaviorTreeVariableEditor()
{
   delete m_editorsFactory;
   m_editorsFactory = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void BehaviorTreeVariableEditor::initialize( BehaviorTreeVariable* btVariable )
{
   m_variable = btVariable;

   // create an editor for the variable
   BehTreeVarEditor* editor = m_editorsFactory->create( *btVariable );
   editor->initializePlayer( m_activePlayer );
   m_variableEditorFrameLayout->addWidget( editor );
}

///////////////////////////////////////////////////////////////////////////////

void BehaviorTreeVariableEditor::removeVariable( bool )
{
   m_behaviorTree.removeVariable( m_variable );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

BehTreeVarEditor::BehTreeVarEditor() 
   : m_player( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeVarEditor::initializePlayer( BehaviorTreeRunner* player ) 
{ 
   m_player = player;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

BehTreeVarBoolEditor::BehTreeVarBoolEditor( BehTreeVarBool& variable )
   : m_variable( variable )
{
   QHBoxLayout* layout = new QHBoxLayout( this );
   layout->setSpacing( 0 );
   layout->setMargin( 0 );
   setLayout( layout );

   m_checkBox = new QCheckBox( this ); 
   layout->addWidget( m_checkBox );

   connect( m_checkBox, SIGNAL( stateChanged( int ) ), this, SLOT( valChanged( int ) ) );

   m_variable.attachListener( *this );
}

///////////////////////////////////////////////////////////////////////////////

BehTreeVarBoolEditor::~BehTreeVarBoolEditor()
{
   m_variable.detachListener( *this );
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeVarBoolEditor::initializePlayer( BehaviorTreeRunner* player )
{
   BehTreeVarEditor::initializePlayer( player );

   m_checkBox->setChecked( player ? m_variable.getRuntime( player ) : m_variable.get() );
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeVarBoolEditor::onObservedPropertyChanged( ReflectionProperty& property )
{
   if ( property.getName() == "m_value" )
   {
      m_checkBox->setChecked( m_player ? m_variable.getRuntime( m_player ) : m_variable.get() );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeVarBoolEditor::valChanged( int )
{
   bool newVal = m_checkBox->isChecked();
   if ( m_player )
   {
      m_variable.setRuntime( m_player, newVal );
   }
   else
   {
      m_variable.set( newVal );
   }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

BehTreeVarFloatEditor::BehTreeVarFloatEditor( BehTreeVarFloat& variable )
   : m_variable( variable )
{
   QHBoxLayout* layout = new QHBoxLayout( this );
   layout->setSpacing( 0 );
   layout->setMargin( 0 );
   setLayout( layout );

   m_val = new QDoubleSpinBox( this ); 
   layout->addWidget( m_val );
   m_val->setSingleStep( 0.01 );
   m_val->setRange( -FLT_MAX, FLT_MAX );

   connect( m_val, SIGNAL( valueChanged( double ) ), this, SLOT( valChanged( double ) ) );

   m_variable.attachListener( *this );
}

///////////////////////////////////////////////////////////////////////////////

BehTreeVarFloatEditor::~BehTreeVarFloatEditor()
{
   m_variable.detachListener( *this );
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeVarFloatEditor::initializePlayer( BehaviorTreeRunner* player )
{
   BehTreeVarEditor::initializePlayer( player );

   m_val->setValue( player ? m_variable.getRuntime( player ) : m_variable.get() );
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeVarFloatEditor::onObservedPropertyChanged( ReflectionProperty& property )
{
   if ( property.getName() == "m_value" )
   {
      m_val->setValue( m_player ? m_variable.getRuntime( m_player ) : m_variable.get() );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeVarFloatEditor::valChanged( double val )
{
   if ( m_player )
   {
      m_variable.setRuntime( m_player, val );
   }
   else
   {
      m_variable.set( val );
   }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

BehTreeVarIntEditor::BehTreeVarIntEditor( BehTreeVarInt& variable )
   : m_variable( variable )
{
   QHBoxLayout* layout = new QHBoxLayout( this );
   layout->setSpacing( 0 );
   layout->setMargin( 0 );
   setLayout( layout );

   m_val = new QSpinBox( this ); 
   layout->addWidget( m_val );
   m_val->setRange( -INT_MAX, INT_MAX );

   connect( m_val, SIGNAL( valueChanged( int ) ), this, SLOT( valChanged( int ) ) );

   m_variable.attachListener( *this );
}

///////////////////////////////////////////////////////////////////////////////

BehTreeVarIntEditor::~BehTreeVarIntEditor()
{
   m_variable.detachListener( *this );
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeVarIntEditor::initializePlayer( BehaviorTreeRunner* player )
{
   BehTreeVarEditor::initializePlayer( player );

   m_val->setValue( player ? m_variable.getRuntime( player ) : m_variable.get() );
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeVarIntEditor::onObservedPropertyChanged( ReflectionProperty& property )
{
   if ( property.getName() == "m_value" )
   {
      m_val->setValue( m_player ? m_variable.getRuntime( m_player ) : m_variable.get() );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeVarIntEditor::valChanged( int val )
{
   if ( m_player )
   {
      m_variable.setRuntime( m_player, val );
   }
   else
   {
      m_variable.set( val );
   }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

BehTreeVarVectorEditor::BehTreeVarVectorEditor( BehTreeVarVector& variable )
   : m_variable( variable )
{
   QHBoxLayout* layout = new QHBoxLayout( this );
   layout->setSpacing( 0 );
   layout->setMargin( 0 );

   layout->addWidget( new QLabel( "X:", this ) );
   m_xVal = new QDoubleSpinBox( this ); layout->addWidget( m_xVal );
   m_xVal->setRange( -FLT_MAX, FLT_MAX );

   layout->addWidget( new QLabel( "Y:", this ) );
   m_yVal = new QDoubleSpinBox( this ); layout->addWidget( m_yVal );
   m_yVal->setRange( -FLT_MAX, FLT_MAX );

   layout->addWidget( new QLabel( "Z:", this ) );
   m_zVal = new QDoubleSpinBox( this ); layout->addWidget( m_zVal );
   m_zVal->setRange( -FLT_MAX, FLT_MAX );

   connect( m_xVal, SIGNAL( valueChanged( double ) ), this, SLOT( xValChanged( double ) ) );
   connect( m_yVal, SIGNAL( valueChanged( double ) ), this, SLOT( yValChanged( double ) ) );
   connect( m_zVal, SIGNAL( valueChanged( double ) ), this, SLOT( zValChanged( double ) ) );

   m_variable.attachListener( *this );
}

///////////////////////////////////////////////////////////////////////////////

BehTreeVarVectorEditor::~BehTreeVarVectorEditor()
{
   m_variable.detachListener( *this );
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeVarVectorEditor::initializePlayer( BehaviorTreeRunner* player )
{
   BehTreeVarEditor::initializePlayer( player );

   const Vector& currVal = player ? m_variable.getRuntime( player ) : m_variable.get();
   m_xVal->setValue( currVal[0] );
   m_yVal->setValue( currVal[1] );
   m_zVal->setValue( currVal[2] );
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeVarVectorEditor::onObservedPropertyChanged( ReflectionProperty& property )
{
   if ( property.getName() == "m_value" )
   {
      const Vector& currVal = m_player ? m_variable.getRuntime( m_player ) : m_variable.get();
      m_xVal->setValue( currVal[0] );
      m_yVal->setValue( currVal[1] );
      m_zVal->setValue( currVal[2] );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeVarVectorEditor::xValChanged( double val )
{
   if ( m_player )
   {
      Vector currVal = m_variable.getRuntime( m_player );
      currVal[0] = val;
      m_variable.setRuntime( m_player, currVal );
   }
   else
   {
      Vector currVal = m_variable.get();
      currVal[0] = val;
      m_variable.set( currVal );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeVarVectorEditor::yValChanged( double val )
{
   if ( m_player )
   {
      Vector currVal = m_variable.getRuntime( m_player );
      currVal[1] = val;
      m_variable.setRuntime( m_player, currVal );
   }
   else
   {
      Vector currVal = m_variable.get();
      currVal[1] = val;
      m_variable.set( currVal );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeVarVectorEditor::zValChanged( double val )
{
   if ( m_player )
   {
      Vector currVal = m_variable.getRuntime( m_player );
      currVal[2] = val;
      m_variable.setRuntime( m_player, currVal );
   }
   else
   {
      Vector currVal = m_variable.get();
      currVal[2] = val;
      m_variable.set( currVal );
   }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

BehTreeVarReflectionObjectEditor::BehTreeVarReflectionObjectEditor( BehTreeVarReflectionObject& variable )
   : m_variable( variable )
{
   QHBoxLayout* layout = new QHBoxLayout( this );
   layout->setSpacing( 0 );
   layout->setMargin( 0 );
   setLayout( layout );

   m_display = new QLabel( this ); 
   layout->addWidget( m_display );

   m_variable.attachListener( *this );
}

///////////////////////////////////////////////////////////////////////////////

BehTreeVarReflectionObjectEditor::~BehTreeVarReflectionObjectEditor()
{
   m_variable.detachListener( *this );
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeVarReflectionObjectEditor::initializePlayer( BehaviorTreeRunner* player )
{
   BehTreeVarEditor::initializePlayer( player );

   char tmpStr[16];
   if ( player )
   {
      sprintf_s( tmpStr, "%p", m_variable.getRuntime( player ) );
   }
   else
   {
      sprintf_s( tmpStr, "%p", m_variable.get() );
   }
   m_display->setText( tmpStr );
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeVarReflectionObjectEditor::onObservedPropertyChanged( ReflectionProperty& property )
{
   if ( property.getName() == "m_value" )
   {
      char tmpStr[16];
      if ( m_player )
      {
         sprintf_s( tmpStr, "%p", m_variable.getRuntime( m_player ) );
      }
      else
      {
         sprintf_s( tmpStr, "%p", m_variable.get() );
      }
      m_display->setText( tmpStr );
   }
}

///////////////////////////////////////////////////////////////////////////////
