/// @file   core-Renderer/MNOutputStruct.h
/// @brief  material routine's output structure
#ifndef _MN_OUTPUT_STRUCT_H
#define _MN_OUTPUT_STRUCT_H

#include "core-Renderer/MaterialNode.h"


///////////////////////////////////////////////////////////////////////////////

class MNOutputStruct : public MaterialNode
{
   DECLARE_ALLOCATOR( MNOutputStruct, AM_ALIGNED_16 );
   DECLARE_CLASS()

public:
   /**
    * Constructor.
    *
    * @param name
    */
   MNOutputStruct( const char* name = "MNOutputStruct" );

   /**
    * Copy constructor.
    */
   MNOutputStruct( const MNOutputStruct& rhs );
   ~MNOutputStruct();

   // -------------------------------------------------------------------------
   // Object implementation
   // -------------------------------------------------------------------------
   void buildMaterialShader( MaterialShaderBuilder& builder ) override;
};

///////////////////////////////////////////////////////////////////////////////

#include "core-Renderer\MNOutputStruct.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _MN_OUTPUT_STRUCT_H
