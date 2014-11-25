/// @file   core\Vector.h
/// @brief  math vector
#ifndef _VECTOR_H
#define _VECTOR_H

#include "core/MemoryRouter.h"
#include "core/MathDataStorage.h"
#include "core/TVector.h"
#include "core/FastFloat.h"
#include <iostream>


///////////////////////////////////////////////////////////////////////////////

class OutStream;
class InStream;
struct FastFloat;

///////////////////////////////////////////////////////////////////////////////

enum QuadMathConstants
{
   QuadMathConst_0,
   QuadMathConst_1,

   QuadMathConst_Minus1,

   QuadMathConst_1000,
   QuadMathConst_0100,
   QuadMathConst_0010,
   QuadMathConst_0001,
   QuadMathConst_Neg_1000,
   QuadMathConst_Neg_0100,
   QuadMathConst_Neg_0010,
   QuadMathConst_Neg_0001,

   QuadMathConst_Eps_1,
   QuadMathConst_Eps_2,
   QuadMathConst_Eps_3,

   QuadMathConst_MAX
};
extern ALIGN_16 QuadStorage          g_quadConstants[QuadMathConst_MAX];

///////////////////////////////////////////////////////////////////////////////

enum VecConstants
{
   VecConstants_OX,
   VecConstants_OY,
   VecConstants_OZ,
   VecConstants_OW,
   VecConstants_NEG_OX,
   VecConstants_NEG_OY,
   VecConstants_NEG_OZ,
   VecConstants_NEG_OW,
   VecConstants_ZERO,
   VecConstants_ONE,
   VecConstants_NEG_ONE,

   VecConstants_MAX
};
struct Vector;
extern ALIGN_16 Vector           q_vecConstants[VecConstants_MAX];

#define Vector_OX       q_vecConstants[ VecConstants_OX ]
#define Vector_OY       q_vecConstants[ VecConstants_OY ]
#define Vector_OZ       q_vecConstants[ VecConstants_OZ ]
#define Vector_OW       q_vecConstants[ VecConstants_OW ]
#define Vector_NEG_OX   q_vecConstants[ VecConstants_NEG_OX ]
#define Vector_NEG_OY   q_vecConstants[ VecConstants_NEG_OY ]
#define Vector_NEG_OZ   q_vecConstants[ VecConstants_NEG_OZ ]
#define Vector_NEG_OW   q_vecConstants[ VecConstants_NEG_OW ]
#define Vector_ZERO     q_vecConstants[ VecConstants_ZERO ]
#define Vector_ONE      q_vecConstants[ VecConstants_ONE ]
#define Vector_NEG_ONE  q_vecConstants[ VecConstants_NEG_ONE ]

///////////////////////////////////////////////////////////////////////////////

extern int g_maskIndicesLookupTable[16];

///////////////////////////////////////////////////////////////////////////////

/**
 * Result of a comparison of two vectors.
 */
struct VectorComparison
{
   enum Mask
   {
      INDEX_W     = 3,	// Index of the w component
      INDEX_Z     = 2,
      INDEX_Y     = 1,
      INDEX_X     = 0,

      MASK_NONE   = 0x0,					// 0000
      MASK_W      = (1 << INDEX_W),		// 0001
      MASK_Z      = (1 << INDEX_Z),		// 0010
      MASK_ZW     = (MASK_Z | MASK_W),	// 0011

      MASK_Y      = (1 << INDEX_Y),		// 0100
      MASK_YW     = (MASK_Y | MASK_W),	// 0101
      MASK_YZ     = (MASK_Y | MASK_Z),	// 0110
      MASK_YZW    = (MASK_YZ | MASK_W),	// 0111

      MASK_X      = (1 << INDEX_X),		// 1000
      MASK_XW     = (MASK_X | MASK_W),	// 1001
      MASK_XZ     = (MASK_X | MASK_Z),	// 1010
      MASK_XZW    = (MASK_XZ | MASK_W),	// 1011

      MASK_XY     = (MASK_X | MASK_Y),	// 1100
      MASK_XYW    = (MASK_XY | MASK_W),	// 1101
      MASK_XYZ    = (MASK_XY | MASK_Z),	// 1110
      MASK_XYZW   = (MASK_XY | MASK_ZW)	// 1111
   
   };

#ifdef _USE_SIMD
   __m128         m_mask;
#else
   int            m_mask;
#endif

   // -------------------------------------------------------------------------
   // Setters
   // -------------------------------------------------------------------------

   /**
    * Sets the specified mask.
    *
    * @param M     mask
    */
   template< Mask M >
   inline void set();

   /**
    * this = c1 && c2
    */
   inline void setAnd( const VectorComparison& c1, const VectorComparison& c2 );

   /**
    * this = c1 || c2
    */
   inline void setOr( const VectorComparison& c1, const VectorComparison& c2 );

    /**
     * this = !this;
     */
   inline void not();

   // -------------------------------------------------------------------------
   // Boolean tests
   // -------------------------------------------------------------------------

   /**
    * Checks if all fields selected by the Mask are set.
    */
   template< Mask M >
   inline bool areAllSet() const;

   /**
    * Checks if any of the fields selected by the Mask are set.
    */
   template< Mask M >
   inline bool isAnySet() const;

   /**
    * Checks if all fields selected by the Mask are set to 0.
    */
   template< Mask M >
   inline bool areAllClear() const;

   /**
    * If only one component is set, the method will return its index.
    * Otherwise it returns -1.
    */
   inline int toIndex() const;
};

///////////////////////////////////////////////////////////////////////////////

/**
 * Mathematical vector (4d).
 */
ALIGN_16 struct Vector
{
   DECLARE_ALLOCATOR( Vector, AM_ALIGNED_16 );

   QuadStorage       m_quad;

   /**
    * Default constructor.
    */
   inline Vector();

   /**
    * @Obsolete
    *
    * Creates a vector from 4 float values.
    *
    * @param x, y, z, w    coordinates
    */
   inline explicit Vector( float x, float y, float z, float w = 0.0f );

   /**
    * Constructor.
    *
    * @param quad
    */
   inline Vector( const QuadStorage& quad );

   /**
    * Resets the vector to the default state.
    */
   inline void setZero();

   /**
    * Creates a vector from 4 float values
    *
    * @param x, y, z, w    coordinates
    */
   inline void set( float x, float y, float z, float w = 0.0f );

   /**
    * Creates a vector from an array of 4, 16-bytes aligned floats
    *
    * @param coords
    */
   inline void set( float* coords );

   /**
    * creates a vector from 4 fast float values
    *
    * @param x, y, z, w    coordinates
    */
   inline void set( const FastFloat& x, const FastFloat& y, const FastFloat& z, const FastFloat& w = Float_0 );

   /**
    * Creates a vector from a quad
    *
    * @param quad
    */
   inline void set( const QuadStorage& quad );

   /**
    *
    *  @param val
    * Sets all vector components to the specified value.
    */
   inline void setBroadcast( const FastFloat& val );

   // -------------------------------------------------------------------------
   // Operators
   // -------------------------------------------------------------------------
   /**
    * Compares four coordinates of the two vectors.
    */
   inline bool equals4( const Vector& rhs ) const;

   /**
    * Compares three coordinates of the two vectors.
    */
   inline bool operator==(const Vector& rhs) const;
   inline bool operator!=(const Vector& rhs) const;

   /**
    * Assignment operator.
    *
    * @param rhs
    */
   inline void operator=( const Vector& rhs );

   /**
    * Returns the specified vector component.
    *
    * @param idx
    */
   inline float& operator[]( int idx );
   inline float operator[]( int idx ) const;

   /**
    * Returns the specified vector component.
    *
    * @param idx
    */
   inline const FastFloat getComponent( uint idx ) const;

   /**
    * Checks if all off vector components are numbers.
    */
   bool isOk() const;

   // -------------------------------------------------------------------------
   // Operations
   // -------------------------------------------------------------------------

   /**
    * Sets the normalized value of the other vector.
    */
   inline void setNormalized( const Vector& vec );
   inline void setNormalized( float x, float y, float z );
   inline void setNormalized( const FastFloat& x, const FastFloat& y, const FastFloat& z );

   /**
    * Sets individual vector components depending on the specified vector comparison result.
    *
    * @param comparisonResult
    * @param trueVec             if the result for the component is true, a corresponding component from this vector will be selected
    * @param falseVec            opposite to the above
    */
   inline void setSelect( const VectorComparison& comparisonResult, const Vector& trueVec, const Vector& falseVec );

   /**
    * Sets individual vector components depending on the specified vector comparison mask.
    *
    * @param M                   mask
    * @param trueVec             if the result for the component is true, a corresponding component from this vector will be selected
    * @param falseVec            opposite to the above
    */
   template< VectorComparison::Mask M >
   inline void setSelect( const Vector& trueVec, const Vector& falseVec );

   /**
    * this = vec * t
    * 
    * @param vec
    * @param t
    */
   inline void setMul( const Vector& vec, const FastFloat& t );

   /**
    * this = vec1 * vec2 ( component by component )
    * 
    * @param vec1
    * @param vec2
    */
   inline void setMul( const Vector& vec1, const Vector& vec2 );

   /**
    * this = vec1 * t + vec2
    * 
    * @param vec1
    * @param t
    * @param vec2
    */
   inline void setMulAdd( const Vector& vec1, const FastFloat& t, const Vector& vec2 );

   /**
    * this = vec1 * t + vec2 ( component by component )
    * 
    * @param vec1
    * @param t
    * @param vec2
    */
   inline void setMulAdd( const Vector& vec1, const Vector& t, const Vector& vec2 );

   /**
    * this = vec1 / vec2 ( component by component )
    * 
    * @param vec1
    * @param vec2
    */
   inline void setDiv( const Vector& vec1, const Vector& vec2 );

   /**
    * this = vec1 / t
    * 
    * @param vec1
    * @param t
    */
   inline void setDiv( const Vector& vec1, const FastFloat& t );

   /**
    * this = vec1 + vec2
    * 
    * @param vec1
    * @param vec2
    */
   inline void setAdd( const Vector& vec1, const Vector& vec2 );

   /**
    * this = vec1 - vec2
    * 
    * @param vec1
    * @param vec2
    */
   inline void setSub( const Vector& vec1, const Vector& vec2 );

   /**
    * this = this * t
    * 
    * @param t
    */
   inline void mul( const FastFloat& t );

   /**
    * this = this * vec ( component by component )
    * 
    * @param vec
    */
   inline void mul( const Vector& vec );

   /**
    * this = this / t
    * 
    * @param t
    */
   inline void div( const FastFloat& t );

   /**
    * this = this / vec ( component by component )
    * 
    * @param vec
    */
   inline void div( const Vector& vec );

   /**
    * this = this  + vec
    * 
    * @param vec
    */
   inline void add( const Vector& vec );

   /**
    * this = this - vec
    * 
    * @param vec
    */
   inline void sub( const Vector& vec );

   /**
    * this = this * -1
    */
   inline void neg();

   /**
    * Normalizes this vector (in place).
    */
   inline void normalize();

   /**
    * Calculates the floor of each of the vector's components.
    */
   inline void floor();

   /**
    * Initializes this vector with the floor value of each of the 'rhs' vector's components.
    *
    * @param rhs
    */
   inline void setFloor( const Vector& rhs );

   /**
    * Rounds up the value of each of the vector's components.
    */
   inline void round();

   /**
    * Initializes this vector with the rounded up value of each of the 'rhs' vector's components.
    *
    * @param rhs
    */
   inline void setRound( const Vector& rhs );

   /**
    * Linearly interpolates a vector between values a and b ( from a to b )
    * at the distance t.
    *
    * this = a*(1-t) + b*t
    *
    * @param a
    * @param b
    * @param t
    */
   inline void setLerp( const Vector& a, const Vector& b, const FastFloat& t );

   /**
    * Returns the remainder resulting from the division of a specified number by another specified number, according
    * to this equation:
    * IEEERemainder = dividend - (divisor * Math.Round(dividend / divisor))
    *
    * @see http://msdn.microsoft.com/en-us/library/system.math.ieeeremainder%28v=vs.110%29.aspx
    *
    * @param dividend
    * @param divisor
    */
   inline void setFloatRemainder( const Vector& dividend, const FastFloat& divisor );

   /**
    * Returns the remainder resulting from the division of a specified number by another specified number, according
    * to this equation:
    * IEEERemainder = dividend - (divisor * Math.Round(dividend / divisor))
    *
    * @see http://msdn.microsoft.com/en-us/library/system.math.ieeeremainder%28v=vs.110%29.aspx
    *
    * @param dividend
    * @param divisor
    */
   inline void setFloatRemainder( const Vector& dividend, const Vector& divisor );

   /**
    * Returns the remainder resulting from the division of a specified number by another specified number, according
    * to this equation:
    * IEEERemainder = dividend - (divisor * Math.Round(dividend / divisor))
    *
    * NOTE: this vector becomes the dividend
    *
    * @see http://msdn.microsoft.com/en-us/library/system.math.ieeeremainder%28v=vs.110%29.aspx
    *
    * @param divisor
    */
   inline void floatRemainder( const FastFloat& divisor );

   /**
    * Returns the remainder resulting from the division of a specified number by another specified number, according
    * to this equation:
    * IEEERemainder = dividend - (divisor * Math.Round(dividend / divisor))
    *
    * NOTE: this vector becomes the dividend
    *
    * @see http://msdn.microsoft.com/en-us/library/system.math.ieeeremainder%28v=vs.110%29.aspx
    *
     * @param divisor
   */
   inline void floatRemainder( const Vector& divisor );

   /**
    * Calculate a dot product with another vector ( taking only 3 coordinates into account ).
    *
    * @param rhs     other vector we want to use in the calculations
    */
   inline const FastFloat dot( const Vector& rhs ) const;

   /**
    * Calculate a dot product with another vector ( taking all 4 coordinates into account ).
    *
    * @param rhs     other vector we want to use in the calculations
    */
   inline const FastFloat dot4( const Vector& rhs ) const;

   /**
    * Creates a vector that's a result of a cross product between the specified vectors.
    *
    * @param v1
    * @param v2
    */
   inline void setCross( const Vector& v1, const Vector& v2 );

   /**
    * Calculates a cross product between this and the specified vector and stores
    * the results in this vector. 
    * WARNING: It creates a temporary vector, so might be slower than setCross method.
    *
    * this = this x rhs
    *
    * @param rhs
    */
   inline void preCross( const Vector& rhs );

   /**
    * Calculates a cross product between this and the specified vector and stores
    * the results in this vector. 
    * As opposed to the 'preCross' method, this calculates the final vector by crossing
    * the components in a different order.
    * WARNING: It creates a temporary vector, so might be slower than setCross method.
    *
    * this = rhs x this
    *
    * @param rhs
    */
   inline void postCross( const Vector& rhs );

   /**
    * Returns the length of this vector.
    */
   inline const FastFloat length() const;

   /**
    * Returns the squared length of this vector.
    */
   inline const FastFloat lengthSq() const;

   /**
    * Checks if the vector is unit-length.
    */
   bool isNormalized() const;

   /**
    * Returns a component of the lowest value.
    *
    * @param N    how many components should be taken into account during the comparison
    */
   template< int N >
   const FastFloat getMin() const;

   /**
    * Returns a component of the highest value.
    *
    * @param N    how many components should be taken into account during the comparison
    */
   template< int N >
   const FastFloat getMax() const;

   /**
    * this = ( a > b ) ? a : b )
    *
    * @param a
    * @param b
    */ 
   inline void setMax( const Vector& a, const Vector& b );

   /**
    * this = ( a < b ) ? a : b )
    *
    * @param a
    * @param b
    */ 
   inline void setMin( const Vector& a, const Vector& b );

   /**
    * this = min( minVal, max( a, maxVal ) )
    *
    * @param a
    * @param b
    */ 
   inline void setClamped( const Vector& a, const Vector& minVal, const Vector& maxVal );

   /**
    * Initializes this vector with absolute values of the specified vector's components.
    */
   inline void setAbs( const Vector& rhs );

   /**
    * Calculates the absolute value of each of vector's components.
    */
   inline void abs();

   /**
    * Sets this vector to be a reflection of the incoming vector with respect to the specified reflection normal.
    *
    * @param incomingVector
    * @param reflectionNormal    CAUTION: this one needs to be a normal vector
    */
   inline void setReflected( const Vector& incomingVector, const Vector& reflectionNormal );

   /**
   * Reflects this vector about the specified reflection normal.
   *
   * @param reflectionNormal    CAUTION: this one needs to be a normal vector
   */
   inline void reflect( const Vector& reflectionNormal );

   // -------------------------------------------------------------------------
   // Storing/loading
   // -------------------------------------------------------------------------

   /**
    * Stores the contents of this vector in the specified raw vector instance.
    *
    * @param rawVector
    */
   template< int Dim >
   void store( TVector< Dim >& rawVector ) const;

   /**
    * Loads the contents of the specified raw vector instance and initializes this vector with it.
    *
    * @param rawVector
    */
   template< int Dim >
   void load( const TVector< Dim >& rawVector );

   /**
    * Stores the contents of this vector in the specified array of 4 floats. The array needs to be address-aligned.
    *
    * @param arrFloats
    */
   inline void store( float* arrFloats ) const;

   /**
    * Loads the contents of the specified array of 4 floats into this vector. The array needs to be address-aligned.
    *
    * @param arrFloats
    */
   inline void load( const float* arrFloats );

   /**
    * Stores the contents of this vector in the specified quad storage.
    *
    * @param storage
    */
   inline void store( QuadStorage& storage ) const;

   /**
    * Initializes the vector with the contents of the specified quad storage.
    *
    * @param storage
    */
   inline void load( const QuadStorage& storage );

   // -------------------------------------------------------------------------
   // Vector comparison
   // -------------------------------------------------------------------------

   /**
    * Compares two vectors component by component: this < rhs
    *
    * @param rhs
    * @param outResult
    */
   inline void less( const Vector& rhs, VectorComparison& outResult ) const;

   /**
    * Compares two vectors component by component: this <= rhs
    *
    * @param rhs
    * @param outResult
    */
   inline void lessEqual( const Vector& rhs, VectorComparison& outResult ) const;

   /**
    * Compares two vectors component by component: this > rhs
    *
    * @param rhs
    * @param outResult
    */
   inline void greater( const Vector& rhs, VectorComparison& outResult ) const;

   /**
    * Compares two vectors component by component: this >= rhs
    *
    * @param rhs
    * @param outResult
    */
   inline void greaterEqual( const Vector& rhs, VectorComparison& outResult ) const;

   /**
    * Compares two vectors component by component: this == rhs
    *
    * @param rhs
    * @param outResult
    */
   inline void equal( const Vector& rhs, VectorComparison& outResult ) const;

   /**
    * Compares two vectors component by component: this != rhs
    *
    * @param rhs
    * @param outResult
    */
   inline void notEqual( const Vector& rhs, VectorComparison& outResult ) const;

   /**
    * Sets the comparison mask to point at the component with the lowest value.
    *
    * @param N
    * @param outResult
    */
   template< int N >
   void selectMin( VectorComparison& outResult ) const;

   /**
    * Sets the comparison mask to point at the component with the highest value.
    *
    * @param N
    * @param outResult
    */
   template< int N >
   void selectMax( VectorComparison& outResult ) const;

   // -------------------------------------------------------------------------
   // Serialization support
   // -------------------------------------------------------------------------
   friend std::ostream& operator<<(std::ostream& stream, const Vector& rhs);
   friend OutStream& operator<<( OutStream& serializer, const Vector& rhs );
   friend InStream& operator>>( InStream& serializer, Vector& rhs );
};

///////////////////////////////////////////////////////////////////////////////

#ifdef _USE_SIMD
   #include "core/VectorSimd.inl"
#else
   #include "core/VectorFpu.inl"
#endif

///////////////////////////////////////////////////////////////////////////////

#endif // _VECTOR_H
