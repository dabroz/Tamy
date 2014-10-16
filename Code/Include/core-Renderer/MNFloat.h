/// @file   core-Renderer/MNFloat.h
/// @brief  a node that provides a float value
#pragma once

#include "core-Renderer/MaterialNode.h"


///////////////////////////////////////////////////////////////////////////////

class MSFloatOutput;

///////////////////////////////////////////////////////////////////////////////

class MNFloat : public MaterialNode
{
   DECLARE_ALLOCATOR( MNFloat, AM_ALIGNED_16 );
   DECLARE_CLASS()

private:
   // static data
   float             m_value;

   // runtime data
   MSFloatOutput*    m_output;
 
public:
   /**
    * Constructor.
    */
   MNFloat();

   /**
    * Copy constructor
    */
   MNFloat( const MNFloat& rhs );
   ~MNFloat();

   // -------------------------------------------------------------------------
   // Object implementation
   // -------------------------------------------------------------------------
   void onObjectLoaded() override;

   // -------------------------------------------------------------------------
   // MaterialNode implementation
   // -------------------------------------------------------------------------
   void buildMaterialShader( MaterialShaderBuilder& builder ) override;
};

///////////////////////////////////////////////////////////////////////////////
