/// @file   core-Renderer/MNBool.h
/// @brief  a node that provides a bool value
#pragma once

#include "core-Renderer/MaterialNode.h"


///////////////////////////////////////////////////////////////////////////////

class MSBoolOutput;

///////////////////////////////////////////////////////////////////////////////

class MNBool : public MaterialNode
{
   DECLARE_ALLOCATOR( MNBool, AM_ALIGNED_16 );
   DECLARE_CLASS()

private:
   // static data
   bool             m_value;

   // runtime data
   MSBoolOutput*    m_output;
 
public:
   /**
    * Constructor.
    */
   MNBool();

   /**
    * Copy constructor.
    */
   MNBool( const MNBool& rhs );
   ~MNBool();

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
