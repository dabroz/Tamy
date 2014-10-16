#include "ml-Blender/TamyScene.h"


///////////////////////////////////////////////////////////////////////////////

void TamyMatrix::applyTo( Matrix& outTransform ) const
{
   rotationMtx.store( outTransform );

   Vector tamyPos;
   position.store( tamyPos );

   outTransform.setPosition<3>( tamyPos );
}

///////////////////////////////////////////////////////////////////////////////

void TamyTransform::applyTo( Transform& outTransform ) const
{
   translation.store( outTransform.m_translation );
   rotation.store( outTransform.m_rotation );
}

///////////////////////////////////////////////////////////////////////////////
