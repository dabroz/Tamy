#include "core-Renderer\MNInputStruct.h"
#include "core-Renderer\MaterialSockets.h"
#include "core-Renderer\RenderState.h"
#include "core-Renderer\MaterialShaderBuilder.h"
#include "core-Renderer\MaterialEntryStruct.h"
#include "core\ThreadSystem.h"
#include "core\Thread.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( MNInputStruct );
   PARENT( MaterialNode );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

MNInputStruct::MNInputStruct( const char* name )
   : MaterialNode( name )
{
}

///////////////////////////////////////////////////////////////////////////////

MNInputStruct::MNInputStruct( const MNInputStruct& rhs )
   : MaterialNode( rhs )
{
}

///////////////////////////////////////////////////////////////////////////////

MNInputStruct::~MNInputStruct()
{
}

///////////////////////////////////////////////////////////////////////////////

void MNInputStruct::buildMaterialShader( MaterialShaderBuilder& builder )
{
   MaterialEntryStruct& inputStruct = builder.getInputStruct();
   inputStruct.setName( getName() );

   const std::vector< GBNodeOutput< MaterialNode >* >& outputs = getOutputs();
   const uint count = outputs.size();
   for ( uint i = 0; i < count; ++i )
   {
      const GBNodeOutput< MaterialNode >* output = outputs[i];
      int fieldIdx = inputStruct.addField( output );

      // define a variable for it
      std::string tag;
      inputStruct.getFieldTag( fieldIdx, tag );
      builder.addStructVariable( output, tag.c_str() );
   }
}

///////////////////////////////////////////////////////////////////////////////
