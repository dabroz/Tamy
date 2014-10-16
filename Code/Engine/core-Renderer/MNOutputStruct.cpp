#include "core-Renderer\MNOutputStruct.h"
#include "core-Renderer\MaterialSockets.h"
#include "core-Renderer\RenderState.h"
#include "core-Renderer\MaterialShaderBuilder.h"
#include "core-Renderer\MaterialEntryStruct.h"
#include "core\ThreadSystem.h"
#include "core\Thread.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( MNOutputStruct );
   PARENT( MaterialNode );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

MNOutputStruct::MNOutputStruct( const char* name )
   : MaterialNode( name )
{
}

///////////////////////////////////////////////////////////////////////////////

MNOutputStruct::MNOutputStruct( const MNOutputStruct& rhs )
   : MaterialNode( rhs )
{
}

///////////////////////////////////////////////////////////////////////////////

MNOutputStruct::~MNOutputStruct()
{
}

///////////////////////////////////////////////////////////////////////////////

void MNOutputStruct::buildMaterialShader( MaterialShaderBuilder& builder )
{
   // define the output structure and connect the inputs
   MaterialEntryStruct& outputStruct = builder.getOutputStruct();
   outputStruct.setName( getName() );

   const std::vector< GBNodeInput< MaterialNode >* >& inputs = getInputs();
   const uint count = inputs.size();
   for ( uint i = 0; i < count; ++i )
   {
      const GBNodeInput< MaterialNode >* input = inputs[i];

      // define the field
      uint fieldIdx = outputStruct.addField( input );

      // if the dedicated socket is connected, create a piece code that
      // accomplishes it
      if ( input->isConnected() )
      {
         const char* connectedOutputName = builder.getVariable( input );
         std::string inputTag;
         outputStruct.getFieldTag( i, inputTag );

         builder.addCodeLine( "%s = %s;", inputTag.c_str(), connectedOutputName );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////
