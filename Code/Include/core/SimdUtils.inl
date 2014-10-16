#ifndef _SIMD_UTILS_H
#error "This file can be included only in SimdUtils.h"
#else

///////////////////////////////////////////////////////////////////////////////

template< int N >
void SimdUtils::normalize( const __m128* vec, __m128* outNormalVec )
{
   __m128 dot;
   SimdUtils::dot<N>( vec, vec, &dot );

   // at this point we have a squared length of the plane normal - so calculate its square root
   __m128 equalsZero = _mm_cmple_ps( dot, _mm_setzero_ps() );
   __m128 sqrtDot = _mm_sqrt_ps( dot );
   __m128 length = _mm_andnot_ps( equalsZero, sqrtDot );

   *outNormalVec = _mm_div_ps( *vec, length );
}

///////////////////////////////////////////////////////////////////////////////

#endif // _SIMD_UTILS_H
