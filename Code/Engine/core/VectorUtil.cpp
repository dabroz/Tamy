#include "core.h"
#include "core\VectorUtil.h"
#include "core\Vector.h"
#include "core\FastFloat.h"
#include <math.h>


///////////////////////////////////////////////////////////////////////////////

void VectorUtil::calculatePerpendicularVector( const Vector& inVec, Vector& outPerpVec )
{
   // find the indices of (one of) the smallest component(s) and the indices of
   // the remaining two components in the vector
   int min = 0;
   int ok1 = 1;
   int ok2 = 2;

   float a0 = abs( inVec[0] );
   float a1 = abs( inVec[1] );
   float a2 = abs( inVec[2] );

   // sort the indices to make min index point to the smallest
   if( a1 < a0 )
   {
      ok1 = 0;
      min = 1;
      a0 = a1;
   }

   if(a2 < a0)
   {
      ok2 = min;
      min = 2;
   }

   outPerpVec.setZero();
   outPerpVec[ok1] = inVec[ok2];
   outPerpVec[ok2] = -inVec[ok1];
}

///////////////////////////////////////////////////////////////////////////////

void VectorUtil::calculatePerpendicularVector2D( const Vector& inVec, Vector& outPerpVec )
{
   outPerpVec.setZero( );
   outPerpVec[1] = inVec[0];
   outPerpVec[0] = -inVec[1];
}

///////////////////////////////////////////////////////////////////////////////

void VectorUtil::limitLength( const Vector& inVec, const FastFloat& maxLen, Vector& outVec )
{
   const FastFloat currLen = inVec.length();
   if ( currLen < Float_1e_4 )
   {
      outVec = inVec;
   }
   else
   {
      FastFloat limitedLen;
      limitedLen.setMin( currLen, maxLen );

      FastFloat lenModifier;
      lenModifier.setDiv( limitedLen, currLen );
      outVec.setMul( inVec, lenModifier );
   }
}

///////////////////////////////////////////////////////////////////////////////

void VectorUtil::calcAngle( const Vector& startVec, const Vector& endVec, const Vector& rotationAxis, FastFloat& outAngle )
{
   Vector normStartVec, normEndVec;

   if ( startVec.lengthSq() < Float_1e_4 || endVec.lengthSq() < Float_1e_4 )
   {
      outAngle = Float_0;
      return;
   }

   normStartVec.setNormalized( startVec );
   normEndVec.setNormalized( endVec );
   
   calcAngleNormalized( normStartVec, normEndVec, rotationAxis, outAngle );
}

///////////////////////////////////////////////////////////////////////////////

void VectorUtil::calcAngleNormalized( const Vector& startVec, const Vector& endVec, const Vector& rotationAxis, FastFloat& outAngle )
{
   FastFloat angleCos = startVec.dot( endVec );
   FastFloat angle;
   angle.setAcos( angleCos );

   Vector sideVec;
   sideVec.setCross( rotationAxis, startVec );
   FastFloat side = endVec.dot( sideVec ) * Float_Minus1;
   outAngle.setFlipSign( angle, side );
}

///////////////////////////////////////////////////////////////////////////////
