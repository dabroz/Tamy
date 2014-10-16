/// @file   core-Renderer/MNInputStruct.h
/// @brief  material routine's input structure
#ifndef _MN_INPUT_STRUCT_H
#define _MN_INPUT_STRUCT_H

#include "core-Renderer/MaterialNode.h"


///////////////////////////////////////////////////////////////////////////////

class MNInputStruct : public MaterialNode
{
   DECLARE_ALLOCATOR( MNInputStruct, AM_ALIGNED_16 );
   DECLARE_CLASS()

public:
   /**
    * Constructor.
    *
    * @param name
    */
   MNInputStruct( const char* name = "MNInputStruct" );

   /**
    * Copy constructor.
    */
   MNInputStruct( const MNInputStruct& rhs );
   ~MNInputStruct();

   // -------------------------------------------------------------------------
   // Object implementation
   // -------------------------------------------------------------------------
   void buildMaterialShader( MaterialShaderBuilder& builder ) override;
};

///////////////////////////////////////////////////////////////////////////////

#include "core-Renderer\MNInputStruct.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _MN_INPUT_STRUCT_H
