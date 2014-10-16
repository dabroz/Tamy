#include "BlendTreeVariableEditor.h"
#include "core-AI\BlendTree.h"
#include "core-AI\BlendTreePlayer.h"
#include "core-AI\BlendTreeVariable.h"

// ui elements
#include <QtWidgets\QLineEdit>
#include <QtWidgets\QPushButton>
#include <QtWidgets\QHBoxLayout>
#include <QtWidgets\QCheckBox>
#include <QtWidgets\QSpinBox>
#include <QtWidgets\QDoubleSpinBox>
#include <QtWidgets\QLabel>


///////////////////////////////////////////////////////////////////////////////

BlendTreeVariableEditor::BlendTreeVariableEditor( QWidget* parent, BlendTree& blendTree, BlendTreePlayer* activePlayer )
   : QFrame( parent )
   , m_blendTree( blendTree )
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
   m_editorsFactory = new GenericFactory< BlendTreeVariable, BTVEditor >();
   m_editorsFactory->associate< BTVarBool, BTVBool >();
   m_editorsFactory->associate< BTVarFloat, BTVFloat >();
   m_editorsFactory->associate< BTVarInt, BTVInt >();
   m_editorsFactory->associate< BTVarVector, BTVVector >();
}

///////////////////////////////////////////////////////////////////////////////

BlendTreeVariableEditor::~BlendTreeVariableEditor()
{
   delete m_editorsFactory;
   m_editorsFactory = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeVariableEditor::initialize( BlendTreeVariable* btVariable )
{
   m_variable = btVariable;

   // create an editor for the variable
   BTVEditor* editor = m_editorsFactory->create( *btVariable );
   editor->initializePlayer( m_activePlayer );
   m_variableEditorFrameLayout->addWidget( editor );
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeVariableEditor::removeVariable( bool )
{
   m_blendTree.removeVariable( m_variable );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

BTVEditor::BTVEditor() 
   : m_player( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////

void BTVEditor::initializePlayer( BlendTreePlayer* player ) 
{ 
   m_player = player;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

BTVBool::BTVBool( BTVarBool& variable )
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

BTVBool::~BTVBool()
{
   m_variable.detachListener( *this );
}

///////////////////////////////////////////////////////////////////////////////

void BTVBool::initializePlayer( BlendTreePlayer* player )
{
   BTVEditor::initializePlayer( player );

   m_checkBox->setChecked( player ? m_variable.getRuntime( player ) : m_variable.get() );
}

///////////////////////////////////////////////////////////////////////////////

void BTVBool::onObservedPropertyChanged( ReflectionProperty& property )
{
   if ( property.getName() == "m_value" )
   {
      m_checkBox->setChecked( m_player ? m_variable.getRuntime( m_player ) : m_variable.get() );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BTVBool::valChanged( int )
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

BTVFloat::BTVFloat( BTVarFloat& variable )
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

BTVFloat::~BTVFloat()
{
   m_variable.detachListener( *this );
}

///////////////////////////////////////////////////////////////////////////////

void BTVFloat::initializePlayer( BlendTreePlayer* player )
{
   BTVEditor::initializePlayer( player );

   m_val->setValue( player ? m_variable.getRuntime( player ) : m_variable.get() );
}

///////////////////////////////////////////////////////////////////////////////

void BTVFloat::onObservedPropertyChanged( ReflectionProperty& property )
{
   if ( property.getName() == "m_value" )
   {
      m_val->setValue( m_player ? m_variable.getRuntime( m_player ) : m_variable.get() );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BTVFloat::valChanged( double val )
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

BTVInt::BTVInt( BTVarInt& variable )
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

BTVInt::~BTVInt()
{
   m_variable.detachListener( *this );
}

///////////////////////////////////////////////////////////////////////////////

void BTVInt::initializePlayer( BlendTreePlayer* player )
{
   BTVEditor::initializePlayer( player );

   m_val->setValue( player ? m_variable.getRuntime( player ) : m_variable.get() );
}

///////////////////////////////////////////////////////////////////////////////

void BTVInt::onObservedPropertyChanged( ReflectionProperty& property )
{
   if ( property.getName() == "m_value" )
   {
      m_val->setValue( m_player ? m_variable.getRuntime( m_player ) : m_variable.get() );
   }
}

///////////////////////////////////////////////////////////////////////////////

void BTVInt::valChanged( int val )
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

BTVVector::BTVVector( BTVarVector& variable )
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

BTVVector::~BTVVector()
{
   m_variable.detachListener( *this );
}

///////////////////////////////////////////////////////////////////////////////

void BTVVector::initializePlayer( BlendTreePlayer* player )
{
   BTVEditor::initializePlayer( player );

   const Vector& currVal = player ? m_variable.getRuntime( player ) : m_variable.get();
   m_xVal->setValue( currVal[0] );
   m_yVal->setValue( currVal[1] );
   m_zVal->setValue( currVal[2] );
}

///////////////////////////////////////////////////////////////////////////////

void BTVVector::onObservedPropertyChanged( ReflectionProperty& property )
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

void BTVVector::xValChanged( double val )
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

void BTVVector::yValChanged( double val )
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

void BTVVector::zValChanged( double val )
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
