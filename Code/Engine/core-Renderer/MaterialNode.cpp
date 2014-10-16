#include "core-Renderer/MaterialNode.h"
#include "core-Renderer/RenderState.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( MaterialNode );
   PARENT( ReflectionObject );
   PROPERTY( InputsMap, m_inputs );
   PROPERTY( OutputsMap, m_outputs );
   PROPERTY( Vector, m_position );
   PROPERTY( std::string, m_name );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

MaterialNode::MaterialNode( const char* name )
   : GraphBuilderNode< MaterialNode >( name )
{
}

///////////////////////////////////////////////////////////////////////////////

MaterialNode::MaterialNode( const MaterialNode& rhs )
   : GraphBuilderNode< MaterialNode >( rhs )
{
}

///////////////////////////////////////////////////////////////////////////////

MaterialNode::~MaterialNode()
{
}

///////////////////////////////////////////////////////////////////////////////

void MaterialNode::createLayout( const RenderState& host ) const
{
   for( OutputsMap::const_iterator it = m_outputs.begin(); it != m_outputs.end(); ++it )
   {
      (*it)->createLayout( host.data() );
   }

   onCreateLayout( host );
}

///////////////////////////////////////////////////////////////////////////////

void MaterialNode::destroyLayout( const RenderState& host ) const
{
   onDestroyLayout( host );
}

///////////////////////////////////////////////////////////////////////////////

GBNodeInput< MaterialNode >* MaterialNode::createInput( const ReflectionType& dataType, const std::string& name ) const
{
   if ( dataType.m_name == "float" )
   {
      return new MSFloatInput( name );
   } 
   else if ( dataType.m_name == "Matrix" )
   {
      return new MSMatrixInput( name );
   } 
   else if ( dataType.m_name == "Vector" )
   {
      return new MSVec4Input( name );
   }
   else if ( dataType.m_name == "bool" )
   {
      return new MSBoolInput( name );
   } 
   else if ( dataType.m_name == "int" )
   {
      return new MSIntInput( name );
   } 


   return NULL;
}

///////////////////////////////////////////////////////////////////////////////
