#include "MaterialLayout.h"
#include "core.h"
#include "core-Renderer.h"
#include "ext-RenderingPipeline\DeferredRenderingMechanism.h"
#include <QtWidgets\QMenu>


// blocks & sockets
#include "MaterialSocketRepresentation.h"


///////////////////////////////////////////////////////////////////////////////

void GNAAddVertexShaderInputSockets::onCreateContextMenu( GraphLayout* layout, QMenu* menu, GraphBlock* block )
{
   TGraphBlock< MaterialNode >* mnBlock = static_cast< TGraphBlock< MaterialNode >* >( block );
   MNInputStruct* node = static_cast< MNInputStruct* >( mnBlock->getNode() );

   menu->addAction( new AddTriangleMeshLayout( node ) );
   menu->addAction( new AddSkinningLayout( node ) );
   menu->addAction( new ResetOutputs( node, layout ) );
}

///////////////////////////////////////////////////////////////////////////////

void GNAAddVertexShaderOutputSockets::onCreateContextMenu( GraphLayout* layout, QMenu* menu, GraphBlock* block )
{
   TGraphBlock< MaterialNode >* mnBlock = static_cast< TGraphBlock< MaterialNode >* >( block );
   MNOutputStruct* node = static_cast< MNOutputStruct* >( mnBlock->getNode() );

   if ( !node->findInput( "positionVS" ) )
   {
      menu->addAction( new AddVec4Input( node, "positionVS" ) );
   }

   if ( !node->findInput( "normalVS" ) )
   {
      menu->addAction( new AddVec3Input( node, "normalVS" ) );
   }

   if ( !node->findInput( "tangentVS" ) )
   {
      menu->addAction( new AddVec3Input( node, "tangentVS" ) );
   }

   if ( !node->findInput( "uv0" ) )
   {
      menu->addAction( new AddVec2Input( node, "uv0" ) );
   }

   menu->addAction( new ResetInputs( node, layout ) );
}

///////////////////////////////////////////////////////////////////////////////

void GNAAddFragmentShaderInputSockets::onCreateContextMenu( GraphLayout* layout, QMenu* menu, GraphBlock* block )
{
   TGraphBlock< MaterialNode >* mnBlock = static_cast< TGraphBlock< MaterialNode >* >( block );
   MNInputStruct* node = static_cast< MNInputStruct* >( mnBlock->getNode() );

   if ( !node->findOutput( "positionVS" ) )
   {
      menu->addAction( new AddVec4Output( node, "positionVS" ) );
   }

   if ( !node->findOutput( "normalVS" ) )
   {
      menu->addAction( new AddVec3Output( node, "normalVS" ) );
   }

   if ( !node->findOutput( "tangentVS" ) )
   {
      menu->addAction( new AddVec3Output( node, "tangentVS" ) );
   }

   if ( !node->findOutput( "uv0" ) )
   {
      menu->addAction( new AddVec2Output( node, "uv0" ) );
   }

   menu->addAction( new ResetOutputs( node, layout ) );
}

///////////////////////////////////////////////////////////////////////////////

void GNAAddFragmentShaderOutputSockets::onCreateContextMenu( GraphLayout* layout, QMenu* menu, GraphBlock* block )
{
   TGraphBlock< MaterialNode >* mnBlock = static_cast< TGraphBlock< MaterialNode >* >( block );
   MNOutputStruct* node = static_cast< MNOutputStruct* >( mnBlock->getNode() );

   menu->addAction( new AddColorLayout( node ) );
   menu->addAction( new AddGBufferLayout( node ) );
   menu->addAction( new ResetInputs( node, layout ) );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

AddTriangleMeshLayout::AddTriangleMeshLayout( MNInputStruct* node )
   : QAction( "TriangleMesh", NULL )
   , m_node( node )
{
   connect( this, SIGNAL( triggered() ), this, SLOT( onTriggered() ) );
}

///////////////////////////////////////////////////////////////////////////////

void AddTriangleMeshLayout::onTriggered()
{
   m_node->removeAllInputs();
   m_node->removeAllOutputs();

   std::vector< GBNodeOutput< MaterialNode >* > outputs;
   outputs.push_back( new MSVec4Output( "vPosition" ) );
   outputs.push_back( new MSVec3Output( "vNormal" ) );
   outputs.push_back( new MSVec3Output( "vTangent" ) );
   outputs.push_back( new MSVec2Output( "vUV0" ) );
   m_node->defineOutputs( outputs );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

AddSkinningLayout::AddSkinningLayout( MNInputStruct* node )
   : QAction( "SkinnedMesh", NULL )
   , m_node( node )
{
   connect( this, SIGNAL( triggered() ), this, SLOT( onTriggered() ) );
}

///////////////////////////////////////////////////////////////////////////////

void AddSkinningLayout::onTriggered()
{
   m_node->removeAllInputs();
   m_node->removeAllOutputs();

   std::vector< GBNodeOutput< MaterialNode >* > outputs;
   outputs.push_back( new MSVec4Output( "vPosition" ) );
   outputs.push_back( new MSVec3Output( "vNormal" ) );
   outputs.push_back( new MSVec3Output( "vTangent" ) );
   outputs.push_back( new MSVec2Output( "vUV0" ) );

   outputs.push_back( new MSVec4Output( "vBlendWeights" ) );
   outputs.push_back( new MSVec4Output( "vBlendIndices" ) );

   m_node->defineOutputs( outputs );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

AddColorLayout::AddColorLayout( MNOutputStruct* node )
   : QAction( "Color", NULL )
   , m_node( node )
{
   connect( this, SIGNAL( triggered() ), this, SLOT( onTriggered() ) );
}

///////////////////////////////////////////////////////////////////////////////

void AddColorLayout::onTriggered()
{
   m_node->removeAllInputs();
   m_node->removeAllOutputs();

   m_node->defineInput( new MSColorInput( "color_0" ) );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

AddGBufferLayout::AddGBufferLayout( MNOutputStruct* node )
   : QAction( "G-Buffer", NULL )
   , m_node( node )
{
   connect( this, SIGNAL( triggered() ), this, SLOT( onTriggered() ) );
}

///////////////////////////////////////////////////////////////////////////////

void AddGBufferLayout::onTriggered()
{
   DeferredRenderingMechanism::defineFragmentShaderOutput( m_node );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

AddVec2Input::AddVec2Input( MNOutputStruct* node, const char* name )
   : QAction( name, NULL )
   , m_node( node )
   , m_name( name )
{
   connect( this, SIGNAL( triggered() ), this, SLOT( onTriggered() ) );
}

///////////////////////////////////////////////////////////////////////////////

void AddVec2Input::onTriggered()
{
   m_node->defineInput( new MSVec2Input( m_name ) );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

AddVec3Input::AddVec3Input( MNOutputStruct* node, const char* name )
   : QAction( name, NULL )
   , m_node( node )
   , m_name( name )
{
   connect( this, SIGNAL( triggered() ), this, SLOT( onTriggered() ) );
}

///////////////////////////////////////////////////////////////////////////////

void AddVec3Input::onTriggered()
{
   m_node->defineInput( new MSVec3Input( m_name ) );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

AddVec4Input::AddVec4Input( MNOutputStruct* node, const char* name )
   : QAction( name, NULL )
   , m_node( node )
   , m_name( name )
{
   connect( this, SIGNAL( triggered() ), this, SLOT( onTriggered() ) );
}

///////////////////////////////////////////////////////////////////////////////

void AddVec4Input::onTriggered()
{
   m_node->defineInput( new MSVec4Input( m_name ) );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

AddColorInput::AddColorInput( MNOutputStruct* node, const char* name )
   : QAction( name, NULL )
   , m_node( node )
   , m_name( name )
{
   connect( this, SIGNAL( triggered() ), this, SLOT( onTriggered() ) );
}

///////////////////////////////////////////////////////////////////////////////

void AddColorInput::onTriggered()
{
   m_node->defineInput( new MSColorInput( m_name ) );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

AddVec2Output::AddVec2Output( MNInputStruct* node, const char* name )
   : QAction( name, NULL )
   , m_node( node )
   , m_name( name )
{
   connect( this, SIGNAL( triggered() ), this, SLOT( onTriggered() ) );
}

///////////////////////////////////////////////////////////////////////////////

void AddVec2Output::onTriggered()
{
   m_node->defineOutput( new MSVec2Output( m_name ) );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

AddVec3Output::AddVec3Output( MNInputStruct* node, const char* name )
   : QAction( name, NULL )
   , m_node( node )
   , m_name( name )
{
   connect( this, SIGNAL( triggered() ), this, SLOT( onTriggered() ) );
}

///////////////////////////////////////////////////////////////////////////////

void AddVec3Output::onTriggered()
{
   m_node->defineOutput( new MSVec3Output( m_name ) );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

AddVec4Output::AddVec4Output( MNInputStruct* node, const char* name )
   : QAction( name, NULL )
   , m_node( node )
   , m_name( name )
{
   connect( this, SIGNAL( triggered() ), this, SLOT( onTriggered() ) );
}

///////////////////////////////////////////////////////////////////////////////

void AddVec4Output::onTriggered()
{
   m_node->defineOutput( new MSVec4Output( m_name ) );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

ResetInputs::ResetInputs( MNOutputStruct* node, GraphLayout* layout )
: QAction( "Reset sockets", NULL )
, m_node( node )
, m_layout( layout )
{
   connect( this, SIGNAL( triggered() ), this, SLOT( onTriggered() ) );
}

///////////////////////////////////////////////////////////////////////////////

void ResetInputs::onTriggered()
{
   m_node->removeAllInputs();

   const int activeRoutineIdx = static_cast< MaterialLayout* >( m_layout )->getActiveRoutineIdx();
   MaterialLayout::createMandatorySockets( m_node, activeRoutineIdx );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

ResetOutputs::ResetOutputs( MNInputStruct* node, GraphLayout* layout )
   : QAction( "Reset sockets", NULL )
   , m_node( node )
   , m_layout( layout )
{
   connect( this, SIGNAL( triggered() ), this, SLOT( onTriggered() ) );
}

///////////////////////////////////////////////////////////////////////////////

void ResetOutputs::onTriggered()
{
   m_node->removeAllOutputs();

   const int activeRoutineIdx = static_cast< MaterialLayout* >( m_layout )->getActiveRoutineIdx();
   MaterialLayout::createMandatorySockets( m_node, activeRoutineIdx );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

MaterialLayout::MaterialLayout( GraphBuilder< MaterialNode >& graphBuilder, int& activeRoutineIdx )
   : TGraphLayout< MaterialNode >( graphBuilder )
   , m_activeRoutineIdx( activeRoutineIdx )
{
   initialize();

   defineUnremovableType< MNInputStruct >();
   defineUnremovableType< MNOutputStruct >();
}

///////////////////////////////////////////////////////////////////////////////

void MaterialLayout::initSocketsFactory( SocketsFactory& factory )
{
   factory.associate< MSFloatInput, MSSFloat >();
   factory.associate< MSFloatOutput, MSSFloat >();
   factory.associate< MSVec2Input, MSSVec2 >();
   factory.associate< MSVec2Output, MSSVec2 >();
   factory.associate< MSVec3Input, MSSVec3 >();
   factory.associate< MSVec3Output, MSSVec3 >();
   factory.associate< MSVec4Input, MSSVec4 >();
   factory.associate< MSVec4Output, MSSVec4 >();
   factory.associate< MSMatrixInput, MSSMatrix >();
   factory.associate< MSMatrixOutput, MSSMatrix >();
   factory.associate< MSMatrixArrInput, MSSMatrixArr >();
   factory.associate< MSMatrixArrOutput, MSSMatrixArr >();
   factory.associate< MSBoolInput, MSSBool >();
   factory.associate< MSBoolOutput, MSSBool >();
   factory.associate< MSIntInput, MSSInt >();
   factory.associate< MSIntOutput, MSSInt >();
   factory.associate< MSColorInput, MSSColor >();
   factory.associate< MSColorOutput, MSSColor >();
}

///////////////////////////////////////////////////////////////////////////////

void MaterialLayout::createMandatorySockets( MNInputStruct* inputStruct, int routineIdx )
{
   // no conditions as of yet - look at the MNOutputStruct counterpart method to see how
   // it should be defined if needs be
}

///////////////////////////////////////////////////////////////////////////////

void MaterialLayout::createMandatorySockets( MNOutputStruct* outputStruct, int routineIdx )
{
   switch ( routineIdx )
   {
   case SHADER_VERTEX:
   {
      outputStruct->defineInput( new MSVec4Input( "positionCS" ) );
      break;
   }

   case SHADER_FRAGMENT:
   {
      outputStruct->defineInput( new MSColorInput( "color_0" ) );
      break;
   }

   // add other routine handlers here
   }

}

///////////////////////////////////////////////////////////////////////////////
