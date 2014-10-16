/// @file   core-Renderer/MNVec4.h
/// @brief  a node that provides a vector value
#pragma once

#include "core-Renderer/MaterialNode.h"


///////////////////////////////////////////////////////////////////////////////

class MSVec4Output;

///////////////////////////////////////////////////////////////////////////////

class MNVec4 : public MaterialNode
{
   DECLARE_ALLOCATOR( MNVec4, AM_ALIGNED_16 );
   DECLARE_CLASS()

private:
   // static data
   Vector            m_vector;

   // runtime data
   MSVec4Output*    m_output;
 
public:
   /**
    * Constructor.
    */
   MNVec4();

   /**
    * Copy constructor.
    */
   MNVec4( const MNVec4& rhs );
   ~MNVec4();

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
