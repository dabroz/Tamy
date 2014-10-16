#include "core\Algorithms.h"
#include "core\math.h"
#include "core\types.h"
#include "core\FilePath.h"
#include "core\Color.h"
#include "core\Point.h"
#include <string>


///////////////////////////////////////////////////////////////////////////////

DEFINE_DEFAULT_APPROXIMATE_EQUALITY( int );
DEFINE_DEFAULT_APPROXIMATE_EQUALITY( unsigned int );
DEFINE_DEFAULT_APPROXIMATE_EQUALITY( char );
DEFINE_DEFAULT_APPROXIMATE_EQUALITY( unsigned char );
DEFINE_DEFAULT_APPROXIMATE_EQUALITY( long );
DEFINE_DEFAULT_APPROXIMATE_EQUALITY( unsigned long );
DEFINE_DEFAULT_APPROXIMATE_EQUALITY( short );
DEFINE_DEFAULT_APPROXIMATE_EQUALITY( unsigned short );
DEFINE_DEFAULT_APPROXIMATE_EQUALITY( bool );
DEFINE_DEFAULT_APPROXIMATE_EQUALITY( float );
DEFINE_DEFAULT_APPROXIMATE_EQUALITY( double );
DEFINE_DEFAULT_APPROXIMATE_EQUALITY( std::string );

DEFINE_DEFAULT_APPROXIMATE_EQUALITY( EulerAngles );
DEFINE_DEFAULT_APPROXIMATE_EQUALITY( Plane );
DEFINE_DEFAULT_APPROXIMATE_EQUALITY( Quaternion );
DEFINE_DEFAULT_APPROXIMATE_EQUALITY( Transform );
DEFINE_DEFAULT_APPROXIMATE_EQUALITY( Color );
DEFINE_DEFAULT_APPROXIMATE_EQUALITY( FilePath );
DEFINE_DEFAULT_APPROXIMATE_EQUALITY( Point );

///////////////////////////////////////////////////////////////////////////////

template<>
bool isApproximatelyEqual( const Matrix& rhs, const Matrix& lhs )
{
   Vector t0; t0.setSub( rhs.getRow(0), lhs.getRow(0) ); t0.abs();
   Vector t1; t1.setSub( rhs.getRow(1), lhs.getRow(1) ); t1.abs();
   Vector t2; t2.setSub( rhs.getRow(2), lhs.getRow(2) ); t2.abs();
   Vector t3; t3.setSub( rhs.getRow(3), lhs.getRow(3) ); t3.abs();

   t0.setMax( t0, t1 );
   t2.setMax( t2, t3 );
   t0.setMax( t0, t2 );

   VectorComparison result;
   t0.lessEqual( Quad_Eps_3, result );

   return result.areAllSet< VectorComparison::MASK_XYZW >();
}

///////////////////////////////////////////////////////////////////////////////

template<>
bool isApproximatelyEqual( const Vector& rhs, const Vector& lhs )
{
   Vector t; t.setSub( rhs, lhs); t.abs();

   VectorComparison result;
   t.lessEqual( Quad_Eps_3, result );

   return result.areAllSet< VectorComparison::MASK_XYZW >();
}

///////////////////////////////////////////////////////////////////////////////

template<>
bool isApproximatelyEqual( const Sphere& rhs, const Sphere& lhs )
{
   Vector t; t.setSub( rhs.origin, lhs.origin ); t.abs( );

   VectorComparison result;
   t.lessEqual( Quad_Eps_3, result );

   FastFloat s;
   s.setAbs( rhs.radius - lhs.radius );

   return result.areAllSet< VectorComparison::MASK_XYZW >( ) && s < Float_1e_3;
}

///////////////////////////////////////////////////////////////////////////////

template<>
bool isApproximatelyEqual( const AxisAlignedBox& rhs, const AxisAlignedBox& lhs )
{
   Vector t; t.setSub( rhs.min, lhs.min );
   Vector s; s.setSub( rhs.max, lhs.max );
   t.setSub( t, s );
   t.abs();

   VectorComparison result;
   t.lessEqual( Quad_Eps_3, result );

   return result.areAllSet< VectorComparison::MASK_XYZW >( ) ;
}

///////////////////////////////////////////////////////////////////////////////

bool solveQuadratic( const FastFloat& a, const FastFloat& b, const FastFloat& c, FastFloat& firstSolution, FastFloat& secondSolution )
{
   FastFloat d, oneOverTwoA, t0, t1, temp;

   d = b*b - Float_4*a*c;
   if ( d < Float_0 ) 
   {
      return false;
   }

   d.setSqrt( d );
   oneOverTwoA = Float_1 / (Float_2 * a);

   t0 = (-b - d) * oneOverTwoA;
   t1 = (-b + d) * oneOverTwoA;

   if (t1 < t0) 
   { 
      temp = t0; 
      t0 = t1; 
      t1 = temp; 
   }

   firstSolution = t0;
   secondSolution = t1;
  
   return true;
}

///////////////////////////////////////////////////////////////////////////////

float snap( float value, float step )
{
   const float snappedVal = ( float ) ( ( int ) round( value / step ) ) * step;
   return snappedVal;
}

///////////////////////////////////////////////////////////////////////////////

int randExclusive( int domainSize, int excludedNumber )
{
   int num = rand() % domainSize;
   if ( num == excludedNumber )
   {
      num = ( num + 1 ) % domainSize;
   }

   return num;
}

///////////////////////////////////////////////////////////////////////////////

float randRange( float minVal, float maxVal )
{
   const float range = maxVal - minVal;
   const float randVal = ( ( float ) rand() / ( float ) RAND_MAX ) * range + minVal;
   return randVal;
}

///////////////////////////////////////////////////////////////////////////////

float lerp( float a, float b, float f )
{
   return a * f + b * ( 1 - f );
}

///////////////////////////////////////////////////////////////////////////////
