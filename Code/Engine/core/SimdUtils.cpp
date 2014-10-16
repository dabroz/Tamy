#include "core/SimdUtils.h"
#include "core/types.h"

#ifdef _USE_SIMD


///////////////////////////////////////////////////////////////////////////////

SimdMaskEntry g_simdMask[] = {
   { 0xffffffff, 0xffffffff, 0xffffffff, 0x00000000 },         // SM_DISCARD_W
   { 0x00000000, 0x00000000, 0x00000000, 0x3f800000 },         // SM_SET_W
   { 0x80000000, 0x80000000, 0x80000000, 0x80000000 },         // SM_SIGN
   { 0x7fffffff, 0x7fffffff, 0x7fffffff, 0x7fffffff },         // SM_ABS_VAL
   { 0x80000000, 0x80000000, 0x80000000, 0x00000000 },         // SM_QUAT_CONJUGATE
   { 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff },         // SM_PASSTHROUGH
};

///////////////////////////////////////////////////////////////////////////////

template<>
void SimdUtils::dot<2>( const __m128* vecA, const __m128* vecB, __m128* outDot )
{
   *outDot = _mm_setzero_ps();
}

///////////////////////////////////////////////////////////////////////////////

template<>
void SimdUtils::dot<3>( const __m128* vecA, const __m128* vecB, __m128* outDot )
{
   __m128 dot = _mm_mul_ps( *vecA, *vecB );

   // we need to discard the W component of the dot product - as the last component we want to add the 
   dot = _mm_and_ps( dot, _MM_DISCARD_W_MASK );

   __m128 shuffledMulRes = _mm_movehl_ps( dot, dot );
   dot = _mm_add_ps( dot, shuffledMulRes );
   shuffledMulRes = _mm_shuffle_ps( dot, dot, _MM_SHUFFLE( 2, 3, 0, 1 ) );
   dot = _mm_add_ps( dot, shuffledMulRes );
   dot = _mm_movelh_ps( dot, dot );

   *outDot = dot;
}

///////////////////////////////////////////////////////////////////////////////

template<>
void SimdUtils::dot<4>( const __m128* vecA, const __m128* vecB, __m128* outDot )
{
   __m128 dot = _mm_mul_ps( *vecA, *vecB );

   __m128 shuffledMulRes = _mm_movehl_ps( dot, dot );
   dot = _mm_add_ps( dot, shuffledMulRes );
   shuffledMulRes = _mm_shuffle_ps( dot, dot, _MM_SHUFFLE( 2, 3, 0, 1 ) );
   dot = _mm_add_ps( dot, shuffledMulRes );
   dot = _mm_movelh_ps( dot, dot );

   *outDot = dot;
}

///////////////////////////////////////////////////////////////////////////////

void SimdUtils::cross( const __m128* vecA, const __m128* vecB, __m128* outCross )
{
   const __m128 cross0 = _mm_mul_ps(
      _mm_shuffle_ps( *vecA, *vecA, _MM_SHUFFLE( 3, 0, 2, 1 ) ),
      _mm_shuffle_ps( *vecB, *vecB, _MM_SHUFFLE( 3, 1, 0, 2 ) ) );
   const __m128 cross1 = _mm_mul_ps(
      _mm_shuffle_ps( *vecA, *vecA, _MM_SHUFFLE( 3, 1, 0, 2 ) ),
      _mm_shuffle_ps( *vecB, *vecB, _MM_SHUFFLE( 3, 0, 2, 1 ) ) );

   *outCross = _mm_sub_ps( cross0, cross1 );
}

///////////////////////////////////////////////////////////////////////////////

void SimdUtils::addMul( const __m128* quadA, const __m128* t, const __m128* quadB, __m128* outResult )
{
   *outResult = _mm_add_ps( *quadA, _mm_mul_ps( *t, *quadB ) );
}

///////////////////////////////////////////////////////////////////////////////

void SimdUtils::flipSign( const __m128* quad, const __m128* mask, __m128* outQuad )
{
   *outQuad = _mm_xor_ps( *quad, _mm_and_ps( *mask, _MM_SIGN_MASK ) );
}

///////////////////////////////////////////////////////////////////////////////

void SimdUtils::sqrt( const __m128* quad, __m128* outQuad )
{
   *outQuad = _mm_sqrt_ps( *quad );
}

///////////////////////////////////////////////////////////////////////////////

void SimdUtils::sqrtInverse( const __m128* quad, __m128* outQuad )
{
   *outQuad = _mm_rsqrt_ps( *quad );
}

///////////////////////////////////////////////////////////////////////////////

void SimdUtils::toSimd( float a, float b, float c, float d, __m128* outSimdVal )
{
   *outSimdVal = _mm_setr_ps( a, b, c, d );
}

///////////////////////////////////////////////////////////////////////////////

const __m128 SimdUtils::fromFloat( float val )
{
   return _mm_set1_ps( val );
}

///////////////////////////////////////////////////////////////////////////////

void SimdUtils::setXYZ_W( const __m128* xyz, const __m128* w, __m128* outQuad )
{
#if SSE_VERSION >= 0x41
   *outQuad = _mm_blend_ps( *xyz, *w, 0x8 );
#else
   *outQuad = _mm_shuffle_ps( *xyz, _mm_unpackhi_ps( *xyz, *w ), _MM_SHUFFLE( 3,0,1,0 ) );
#endif
}

///////////////////////////////////////////////////////////////////////////////

void SimdUtils::getComponent( const __m128* quad, const int idx, __m128* outComponent )
{
   static ALIGN_16 const uint indexToMask[16] = 
   {
      0xffffffff, 0x00000000, 0x00000000, 0x00000000,
      0x00000000, 0xffffffff, 0x00000000, 0x00000000,
      0x00000000, 0x00000000, 0xffffffff, 0x00000000,
      0x00000000, 0x00000000, 0x00000000, 0xffffffff
   };


   const __m128 mask = *( const __m128* )&indexToMask[ idx * 4 ];
   __m128 selected = _mm_and_ps( mask, *quad ); 

   const __m128 zwxy = _mm_shuffle_ps( selected, selected, 0x4e ); // 0x4e == _MM_SHUFFLE( 1,0,3,2 )
   selected = _mm_or_ps( selected, zwxy );
   const __m128 yxwz = _mm_shuffle_ps( selected, selected, 0xb1 ); // 0xb1 == _MM_SHUFFLE( 2,3,0,1 )
   *outComponent = _mm_or_ps( selected, yxwz );
}

///////////////////////////////////////////////////////////////////////////////

void SimdUtils::transpose( const __m128* inRows, __m128* outRows )
{
   __m128 tmp0, tmp1, tmp2, tmp3;

   tmp0 = _mm_shuffle_ps( inRows[0], inRows[1], 0x44);
   tmp2 = _mm_shuffle_ps( inRows[0], inRows[1], 0xEE);
   tmp1 = _mm_shuffle_ps( inRows[2], inRows[3], 0x44);
   tmp3 = _mm_shuffle_ps( inRows[2], inRows[3], 0xEE);

   outRows[0] = _mm_shuffle_ps( tmp0, tmp1, 0x88 );
   outRows[1] = _mm_shuffle_ps( tmp0, tmp1, 0xDD );
   outRows[2] = _mm_shuffle_ps( tmp2, tmp3, 0x88 );
   outRows[3] = _mm_shuffle_ps( tmp2, tmp3, 0xDD );
}

///////////////////////////////////////////////////////////////////////////////

void SimdUtils::floor( const __m128* inVal, __m128* outVal ) 
{ 
#if SSE_VERSION >= 0x41
   *outVal = _mm_floor_ss( *inVal, *inVal );
#else
   static ALIGN_16 const uint two23[4]  = { 0x4B000000, 0x4B000000, 0x4B000000, 0x4B000000 }; // 2^23 as float

   const __m128 b = _mm_castsi128_ps( _mm_srli_epi32( _mm_slli_epi32( _mm_castps_si128( *inVal ), 1 ), 1 ) ); // fabs(v)
   const __m128 d = _mm_sub_ps( _mm_add_ps( _mm_add_ps( _mm_sub_ps( *inVal, *(__m128*)&two23 ), *(__m128*)&two23 ), *(__m128*)&two23 ), *(__m128*)&two23 ); // the meat of floor
   const __m128 largeMaskE = _mm_cmpgt_ps( b, *(__m128*)&two23 ); // $ffffffff if v >= 2^23
   const __m128 g = _mm_cmplt_ps( *inVal, d ); // check for possible off by one error
   const __m128 h = _mm_cvtepi32_ps( _mm_castps_si128( g ) ); // convert positive check result to -1.0, negative to 0.0
   const __m128 t = _mm_add_ps( d, h ); // add in the error if there is one

   // Select between output result and input value based on v >= 2^23
   *outVal = _mm_or_ps( _mm_and_ps( largeMaskE, *inVal ), _mm_andnot_ps( largeMaskE, t ) );
#endif
}

///////////////////////////////////////////////////////////////////////////////

void SimdUtils::lessMask( const __m128* quadA, const __m128* quadB, __m128* outMask )
{
   *outMask = _mm_cmplt_ps( *quadA, *quadB );
}

///////////////////////////////////////////////////////////////////////////////

void SimdUtils::lessEqualMask( const __m128* quadA, const __m128* quadB, __m128* outMask )
{
   *outMask = _mm_cmple_ps( *quadA, *quadB );
}

///////////////////////////////////////////////////////////////////////////////

void SimdUtils::greaterMask( const __m128* quadA, const __m128* quadB, __m128* outMask )
{
   *outMask = _mm_cmpgt_ps( *quadA, *quadB );
}

///////////////////////////////////////////////////////////////////////////////

void SimdUtils::greaterEqualMask( const __m128* quadA, const __m128* quadB, __m128* outMask )
{
   *outMask = _mm_cmpge_ps( *quadA, *quadB );
}

///////////////////////////////////////////////////////////////////////////////

void SimdUtils::equalMask( const __m128* quadA, const __m128* quadB, __m128* outMask )
{
   *outMask = _mm_cmpeq_ps( *quadA, *quadB );
}

///////////////////////////////////////////////////////////////////////////////

void SimdUtils::notEqualMask( const __m128* quadA, const __m128* quadB, __m128* outMask )
{
   *outMask = _mm_cmpneq_ps( *quadA, *quadB );
}

///////////////////////////////////////////////////////////////////////////////

void SimdUtils::lessZeroMask( const __m128* quad, __m128* outMask )
{
   *outMask = _mm_cmplt_ps( *quad, _mm_setzero_ps() );
}

///////////////////////////////////////////////////////////////////////////////

void SimdUtils::lessEqualZeroMask( const __m128* quad, __m128* outMask )
{
   *outMask = _mm_cmple_ps( *quad, _mm_setzero_ps() );
}

///////////////////////////////////////////////////////////////////////////////

void SimdUtils::greaterZeroMask( const __m128* quad, __m128* outMask )
{
   *outMask = _mm_cmpgt_ps( *quad, _mm_setzero_ps() );
}

///////////////////////////////////////////////////////////////////////////////

void SimdUtils::greaterEqualZeroMask( const __m128* quad, __m128* outMask )
{
   *outMask = _mm_cmpge_ps( *quad, _mm_setzero_ps() );
}

///////////////////////////////////////////////////////////////////////////////

void SimdUtils::equalZeroMask( const __m128* quad, __m128* outMask )
{
   *outMask = _mm_cmpeq_ps( *quad, _mm_setzero_ps() );
}

///////////////////////////////////////////////////////////////////////////////

void SimdUtils::notEqualZeroMask( const __m128* quad, __m128* outMask )
{
   *outMask = _mm_cmpneq_ps( *quad, _mm_setzero_ps() );
}

///////////////////////////////////////////////////////////////////////////////

#endif // _USE_SIMD
