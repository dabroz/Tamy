/// @file   core-Renderer/MNTransformVector.h
/// @brief  transforms a vector from one space to another ( clip, view etc )
#pragma once

#include "core-Renderer/MaterialNode.h"


///////////////////////////////////////////////////////////////////////////////

class MSMatrixInput;
class MSVec4Input;
class MSVec4Output;

///////////////////////////////////////////////////////////////////////////////

class MNTransformVector : public MaterialNode
{
   DECLARE_ALLOCATOR( MNTransformVector, AM_ALIGNED_16 );
   DECLARE_CLASS()

private:
   // sockets
   MSMatrixInput*       m_transform;
   MSVec4Input*         m_input;
   MSVec4Output*        m_output;

public:
   /**
    * Constructor.
    *
    * @param name
    */
   MNTransformVector( const char* name = "MNTransformVector" );

   /**
    * Copy constructor.
    */
   MNTransformVector( const MNTransformVector& rhs );
   ~MNTransformVector();

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
