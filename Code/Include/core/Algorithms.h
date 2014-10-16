/// @file   core/Algorithms.h
/// @brief  a set of algorithms facilitating code creation

#ifndef _ALGORITHMS_H
#define _ALGORITHMS_H

#include <algorithm>


///////////////////////////////////////////////////////////////////////////////

struct FastFloat;

///////////////////////////////////////////////////////////////////////////////

class Collection
{
public:
   /**
    * Clears a collection.
    *
    * @param collection
    */
   template< typename C >
   static void clear( C& collection );
};

///////////////////////////////////////////////////////////////////////////////

/**
 * Clamps the value to the specified range.
 *
 * @param val
 * @param min
 * @param max
 */
template< typename T >
T clamp( T val, T min, T max );

///////////////////////////////////////////////////////////////////////////////

/**
 * Rounds up a value to the specified precision.
 *
 * @param   value
 * @param   precision
 * @return  rounded value
 */
template< typename T >
T roundEx( T value, T precision );

///////////////////////////////////////////////////////////////////////////////

/**
 * Snaps the value to the specified step.
 *
 * @param   value
 * @para    step
 * @return  snapped value
 */
float snap( float value, float step );

///////////////////////////////////////////////////////////////////////////////

/**
 * Calculates the magnitude of a number.
 *
 * @param   value
 * @return  value magnitude
 */
template< typename T >
float mag( T value );

///////////////////////////////////////////////////////////////////////////////

/**
 * Returns the lower of two values
 *
 * @param   val1
 * @param   val2
 */
template< typename T >
T min2( T val1, T val2 );

///////////////////////////////////////////////////////////////////////////////

/**
 * Returns the higher of two values
 *
 * @param   val1
 * @param   val2
 */
template< typename T >
T max2( T val1, T val2 );

///////////////////////////////////////////////////////////////////////////////

/**
 * Returns the sign of the specified number.
 */
template< typename T >
T sign( T val );

///////////////////////////////////////////////////////////////////////////////

/**
 * Linear interpolation between values 'a' and 'b' = a*f + b*(1-f) 
 *
 * @param a
 * @param b
 * @param f
 */
float lerp( float a, float b, float f );

///////////////////////////////////////////////////////////////////////////////

/**
 * Randomizes an integer from the specified domain. The randomization
 * continues until a number different from the 'excludedNumber' is drawn.
 */
int randExclusive( int domainSize, int excludedNumber );

///////////////////////////////////////////////////////////////////////////////

/**
 * Picks a random float value from within the specified range.
 */
float randRange( float minVal, float maxVal );

///////////////////////////////////////////////////////////////////////////////

/**
 * This function solves the quadratic equation and returns the 
 * smallest >=0 solution
 *
 * @return true if a solution was found, false otherwise
 */
bool solveQuadratic( const FastFloat& a, const FastFloat& b, const FastFloat& c, FastFloat& firstSolution, FastFloat& secondSolution );

///////////////////////////////////////////////////////////////////////////////

/**
 * Checks if two types are approximately equal.
 */
template< typename T >
bool isApproximatelyEqual( const T& rhs, const T& lhs );

/**
 * This macro facilitates the process of defining isApproximatelyEqual
 * functions for types that should simply compare their results in a regular way.
 */
#define DEFINE_DEFAULT_APPROXIMATE_EQUALITY( Type )                           \
   template<> \
   bool isApproximatelyEqual( const Type& rhs, const Type& lhs ) \
   { \
      return rhs == lhs; \
   }

///////////////////////////////////////////////////////////////////////////////

#include "core/Algorithms.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _ALGORITHMS_H