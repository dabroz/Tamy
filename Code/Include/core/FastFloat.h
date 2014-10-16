/// @file   core/FastFloat.h
/// @brief  a primitive type that represents a floating point number implemented using optimized instruction sets
#ifndef _FAST_FLOAT_H
#define _FAST_FLOAT_H

#include "core\MemoryRouter.h"
#include "core\MathDataStorage.h"
#include "core\types.h"
#include <iostream>


///////////////////////////////////////////////////////////////////////////////

class OutStream;
class InStream;

///////////////////////////////////////////////////////////////////////////////

enum FloatMathConstants
{
   FloatMathConst_0,
   FloatMathConst_1,
   FloatMathConst_2,
   FloatMathConst_3,
   FloatMathConst_4,
   FloatMathConst_5,
   FloatMathConst_6,
   FloatMathConst_7,
   FloatMathConst_8,
   FloatMathConst_9,

   FloatMathConst_Minus1,
   FloatMathConst_Minus2,

   FloatMathConst_1e_1,
   FloatMathConst_1e_2,
   FloatMathConst_1e_3,
   FloatMathConst_1e_4,
   FloatMathConst_Minus1e_1,
   FloatMathConst_Minus1e_2,
   FloatMathConst_Minus1e_3,
   FloatMathConst_Minus1e_4,

   FloatMathConst_Inv2,
   FloatMathConst_Inv3,
   FloatMathConst_Inv4,

   FloatMathConst_MinusInv2,
   FloatMathConst_MinusInv3,
   FloatMathConst_MinusInv4,

   FloatMathConst_PI,
   FloatMathConst_Rad2Deg,
   FloatMathConst_Deg2Rad,

   FloatMathConst_INF,
   FloatMathConst_NegINF,

   FloatMathConst_MAX
};
struct FastFloat;
extern ALIGN_16 FastFloat           g_floatConstants[FloatMathConst_MAX];

///////////////////////////////////////////////////////////////////////////////

/**
 * A primitive type that represents a floating point number implemented using SIMD instructions.
 */
ALIGN_16 struct FastFloat
{
   DECLARE_ALLOCATOR( FastFloat, AM_ALIGNED_16 );

   FastFloatStorage     m_val;

   /**
    * Default constructor.
    */
   inline FastFloat() : m_val( Float_0.m_val ) {}

   /**
    * Copy constructor.
    *
    * @param rhs
    */
   inline FastFloat( const FastFloat& rhs ) : m_val( rhs.m_val ) {}

   /**
    * Copy constructor.
    *
    * @param rhs
    */
   inline FastFloat( const FastFloatStorage& rhs ) : m_val( rhs ) {}


   /**
    * Assignment operator
    *
    * @param val
    */
   inline void operator=( const FastFloat& rhs );
   inline void operator=( const FastFloatStorage& rhs );

   /**
    * Static conversion methods
    *
    * @param val
    */
   static const FastFloat fromFloat( float val );
   static const FastFloat fromInt( int val );
   static const FastFloat fromByte( byte val );
   static const FastFloat fromWord( word val );

   /**
    * Sets the value from various primitive types.
    *
    * @param val
    */
   inline void setFromFloat( float val );
   inline void setFromInt( int val );
   inline void setFromByte( byte val );
   inline void setFromWord( word val );

   /**
    * Converts the number back to float.
    */
   inline float getFloat() const;

   // -------------------------------------------------------------------------
   // Operators
   // -------------------------------------------------------------------------
   /**
    * this = a + b
    *
    * @param a
    * @param b
    */
   inline void setAdd( const FastFloat& a, const FastFloat& b );

   /**
    * this = a - b
    *
    * @param a
    * @param b
    */
   inline void setSub( const FastFloat& a, const FastFloat& b );

   /**
    * this = a * b
    *
    * @param a
    * @param b
    */
   inline void setMul( const FastFloat& a, const FastFloat& b );

   /**
    * this = a / b
    *
    * @param a
    * @param b
    */
   inline void setDiv( const FastFloat& a, const FastFloat& b );

   /**
    * this += a
    *
    * @param a
    */
   inline void add( const FastFloat& a );

   /**
    * this -= a
    *
    * @param a
    */
   inline void sub( const FastFloat& a );

   /**
    * this *= a
    *
    * @param a
    */
   inline void mul( const FastFloat& a );

   /**
    * this /= a
    *
    * @param a
    */
   inline void div( const FastFloat& a );

   // -------------------------------------------------------------------------
   // C++ operators
   // -------------------------------------------------------------------------
   inline FastFloat operator+( const FastFloat& a ) const;
   inline FastFloat operator-( const FastFloat& a ) const;
   inline FastFloat operator*( const FastFloat& a ) const;
   inline FastFloat operator/( const FastFloat& a ) const;
   inline FastFloat& operator+=( const FastFloat& a );
   inline FastFloat& operator-=( const FastFloat& a );
   inline FastFloat& operator*=( const FastFloat& a );
   inline FastFloat& operator/=( const FastFloat& a );
   inline FastFloat operator-() const;

   // -------------------------------------------------------------------------
   // Boolean comparisons
   // -------------------------------------------------------------------------
   inline bool operator<( const FastFloat& rhs ) const;
   inline bool operator<=( const FastFloat& rhs ) const;
   inline bool operator>( const FastFloat& rhs ) const;
   inline bool operator>=( const FastFloat& rhs ) const;
   inline bool operator==( const FastFloat& rhs ) const;
   inline bool operator!=( const FastFloat& rhs ) const;

   // -------------------------------------------------------------------------
   // Other methods
   // -------------------------------------------------------------------------
   /**
    * this = ( a > b ) ? a : b )
    *
    * @param a
    * @param b
    */ 
   inline void setMax( const FastFloat& a, const FastFloat& b );

   /**
    * this = ( a < b ) ? a : b )
    *
    * @param a
    * @param b
    */ 
   inline void setMin( const FastFloat& a, const FastFloat& b );

   /**
    * this = min( minVal, max( a, maxVal ) )
    *
    * @param a
    * @param b
    */ 
   inline void setClamped( const FastFloat& a, const FastFloat& minVal, const FastFloat& maxVal );

   /**
    * this = abs(a)
    *
    * @param a
    */
   inline void setAbs( const FastFloat& a );

   /**
    * this = abs(this)
    */
   inline void abs();

   /**
    * this = sign >= 0 ? a : -a
    *
    * @param a
    * @param sign
    */
   inline void setFlipSign( const FastFloat& a, const FastFloat& sign );

   /**
    * this = sign( a )
    *
    * @param a
    */
   inline void setSign( const FastFloat& a );

   /**
    * this = sign( this )
    */
   inline void sign();

   /**
    * abs( this - a ) <= eps
    *
    * @param a
    * @param eps
    */
   inline bool approxEqual( const FastFloat& a, const FastFloat& eps ) const;

   /**
    * this = sqrt( a )
    *
    * @param a
    */
   inline void setSqrt( const FastFloat& a );

   /**
    * this = sqrt( this )
    */
   inline void sqrt();

   /**
    * this = 1 / a
    *
    * @param a
    */
   inline void setReciprocal( const FastFloat& a );

   /**
    * this = 1 / this
    */
   inline void reciprocal();

   /**
    * this = -a;
    *
    * @param a
    */
   inline void setNeg( const FastFloat& a );

   /**
    * this = -this;
    */
   inline void neg();

   // -------------------------------------------------------------------------
   // Trigonometry methods
   // -------------------------------------------------------------------------

   /**
    * this = sin(this)
    */
   inline void sin();

   /**
    * this = cos(this)
    */
   inline void cos();

   /**
    * this = sin(val)
    */
   inline void setSin( const FastFloat& val );

   /**
    * this = cos(val)
    */
   inline void setCos( const FastFloat& val );

   /**
    * this = asin(this)
    */
   inline void asin();

   /**
    * this = acos(this)
    */
   inline void acos();

   /**
    * this = sin(val)
    */
   inline void setAsin( const FastFloat& val );

   /**
    * this = cos(val)
    */
   inline void setAcos( const FastFloat& val );

   // -------------------------------------------------------------------------
   // Serialization support
   // -------------------------------------------------------------------------
   friend std::ostream& operator<<( std::ostream& stream, const FastFloat& rhs );
   friend OutStream& operator<<( OutStream& serializer, const FastFloat& rhs );
   friend InStream& operator>>( InStream& serializer, FastFloat& rhs );
};

///////////////////////////////////////////////////////////////////////////////

#ifdef _USE_SIMD
   #include "core/FastFloatSimd.inl"
#else
   #include "core/FastFloatFpu.inl"
#endif

///////////////////////////////////////////////////////////////////////////////

#endif // _FAST_FLOAT_H
