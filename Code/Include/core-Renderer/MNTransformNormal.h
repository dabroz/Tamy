/// @file   core-Renderer/MNTransformNormal.h
/// @brief  transforms a normal from one space to another ( clip, view etc )
#pragma once

#include "core-Renderer/MaterialNode.h"


///////////////////////////////////////////////////////////////////////////////

class MSMatrixInput;
class MSVec3Input;
class MSVec3Output;

///////////////////////////////////////////////////////////////////////////////

/**
 * Transforms a normal from one space to another ( clip, view etc ).
 * The resulting Vector3 ( normals are expressed as such ) will still be normalized.
 */
class MNTransformNormal : public MaterialNode
{
   DECLARE_ALLOCATOR( MNTransformNormal, AM_ALIGNED_16 );
   DECLARE_CLASS()

private:
   // sockets
   MSMatrixInput*       m_transform;
   MSVec3Input*         m_input;
   MSVec3Output*        m_output;

public:
   /**
    * Constructor.
    *
    * @param name
    */
   MNTransformNormal( const char* name = "MNTransformNormal" );

   /**
    * Copy constructor.
    */
   MNTransformNormal( const MNTransformNormal& rhs );
   ~MNTransformNormal();

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
