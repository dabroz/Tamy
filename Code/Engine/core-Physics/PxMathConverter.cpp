#include "core-Physics\PxMathConverter.h"

// Booster math
#include "core\Vector.h"
#include "core\Quaternion.h"
#include "core\Matrix.h"
#include "core\Transform.h"

// PhysX math
#include "foundation\PxVec3.h"
#include "characterkinematic\PxExtended.h"
#include "foundation\PxTransform.h"


///////////////////////////////////////////////////////////////////////////////

void PxMathConverter::convert( const Vector& boosterVec, physx::PxVec3& outPhysXVec )
{
   ALIGN_16 float v[4];
   boosterVec.store( v );
   outPhysXVec.x = v[0];
   outPhysXVec.y = v[1];
   outPhysXVec.z = v[2];
}

///////////////////////////////////////////////////////////////////////////////

void PxMathConverter::convert( const physx::PxVec3& physXVec, Vector& outBoosterVec )
{
   ALIGN_16 float v[4];
   v[0] = physXVec.x;
   v[1] = physXVec.y;
   v[2] = physXVec.z;
   v[3] = 0.f;
   outBoosterVec.load( v );
}

///////////////////////////////////////////////////////////////////////////////

void PxMathConverter::convert( const Vector& boosterVec, physx::PxExtendedVec3& outPhysXVec )
{
   ALIGN_16 float v[4];
   boosterVec.store( v );
   outPhysXVec.x = v[0];
   outPhysXVec.y = v[1];
   outPhysXVec.z = v[2];
}

///////////////////////////////////////////////////////////////////////////////

void PxMathConverter::convert( const physx::PxExtendedVec3& physXVec, Vector& outBoosterVec )
{
   ALIGN_16 float v[4];
   v[0] = (float)physXVec.x;
   v[1] = ( float ) physXVec.y;
   v[2] = ( float ) physXVec.z;
   v[3] = 0.f;
   outBoosterVec.load( v );
}

///////////////////////////////////////////////////////////////////////////////

void PxMathConverter::convert( const Quaternion& boosterQuat, physx::PxQuat& outPhysXQuat )
{
   ALIGN_16 float v[4];
   boosterQuat.store( v );
   outPhysXQuat.x = v[0];
   outPhysXQuat.y = v[1];
   outPhysXQuat.z = v[2];
   outPhysXQuat.w = v[3];
}

///////////////////////////////////////////////////////////////////////////////

void PxMathConverter::convert( const physx::PxQuat& physXQuat, Quaternion& outBoosterQuat )
{
   ALIGN_16 float v[4];
   v[0] = physXQuat.x;
   v[1] = physXQuat.y;
   v[2] = physXQuat.z;
   v[3] = physXQuat.w;
   outBoosterQuat.load( v );
}

///////////////////////////////////////////////////////////////////////////////

void PxMathConverter::convert( const Matrix& boosterMtx, physx::PxTransform& outPhysXTransform )
{
   Transform boosterTrans;
   boosterTrans.set( boosterMtx );

   convert( boosterTrans.m_translation, outPhysXTransform.p );
   convert( boosterTrans.m_rotation, outPhysXTransform.q );
}

///////////////////////////////////////////////////////////////////////////////

void PxMathConverter::convert( const physx::PxTransform& physXTransform, Matrix& outBoosterMtx )
{
   Transform boosterTrans;
   convert( physXTransform.p, boosterTrans.m_translation );
   convert( physXTransform.q, boosterTrans.m_rotation );

   boosterTrans.toMatrix( outBoosterMtx );
}

///////////////////////////////////////////////////////////////////////////////
