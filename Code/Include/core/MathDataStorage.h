/// @file   core/MathDataStorage.h
/// @brief  data storage for 4 float values used by mathematical types
#ifndef _QUAD_STORAGE_H
#define _QUAD_STORAGE_H

#include "core\EngineDefines.h"
#include "core\types.h"


///////////////////////////////////////////////////////////////////////////////

#define Float_Minus1       g_floatConstants[FloatMathConst_Minus1]
#define Float_Minus2       g_floatConstants[FloatMathConst_Minus2]

#define Float_0            g_floatConstants[FloatMathConst_0]
#define Float_1            g_floatConstants[FloatMathConst_1]
#define Float_2            g_floatConstants[FloatMathConst_2]
#define Float_3            g_floatConstants[FloatMathConst_3]
#define Float_4            g_floatConstants[FloatMathConst_4]
#define Float_5            g_floatConstants[FloatMathConst_5]
#define Float_6            g_floatConstants[FloatMathConst_6]
#define Float_7            g_floatConstants[FloatMathConst_7]
#define Float_8            g_floatConstants[FloatMathConst_8]
#define Float_9            g_floatConstants[FloatMathConst_9]

#define Float_1e_1         g_floatConstants[FloatMathConst_1e_1]
#define Float_1e_2         g_floatConstants[FloatMathConst_1e_2]
#define Float_1e_3         g_floatConstants[FloatMathConst_1e_3]
#define Float_1e_4         g_floatConstants[FloatMathConst_1e_4]
#define Float_Minus1e_1    g_floatConstants[FloatMathConst_Minus1e_1]
#define Float_Minus1e_2    g_floatConstants[FloatMathConst_Minus1e_2]
#define Float_Minus1e_3    g_floatConstants[FloatMathConst_Minus1e_3]
#define Float_Minus1e_4    g_floatConstants[FloatMathConst_Minus1e_4]

#define Float_Inv2         g_floatConstants[FloatMathConst_Inv2]
#define Float_Inv3         g_floatConstants[FloatMathConst_Inv3]
#define Float_Inv4         g_floatConstants[FloatMathConst_Inv4]
#define Float_MinusInv2    g_floatConstants[FloatMathConst_MinusInv2]
#define Float_MinusInv3    g_floatConstants[FloatMathConst_MinusInv3]
#define Float_MinusInv4    g_floatConstants[FloatMathConst_MinusInv4]

#define Float_PI           g_floatConstants[FloatMathConst_PI]
#define Float_Rad2Deg      g_floatConstants[FloatMathConst_Rad2Deg]
#define Float_Deg2Rad      g_floatConstants[FloatMathConst_Deg2Rad]

#define Float_INF          g_floatConstants[FloatMathConst_INF]
#define Float_NegINF       g_floatConstants[FloatMathConst_NegINF]

///////////////////////////////////////////////////////////////////////////////

#define Quad_Minus1        g_quadConstants[QuadMathConst_Minus1]

#define Quad_0             g_quadConstants[QuadMathConst_0]
#define Quad_1             g_quadConstants[QuadMathConst_1]

#define Quad_1000          g_quadConstants[QuadMathConst_1000]
#define Quad_0100          g_quadConstants[QuadMathConst_0100]
#define Quad_0010          g_quadConstants[QuadMathConst_0010]
#define Quad_0001          g_quadConstants[QuadMathConst_0001]
#define Quad_Neg_1000      g_quadConstants[QuadMathConst_Neg_1000]
#define Quad_Neg_0100      g_quadConstants[QuadMathConst_Neg_0100]
#define Quad_Neg_0010      g_quadConstants[QuadMathConst_Neg_0010]
#define Quad_Neg_0001      g_quadConstants[QuadMathConst_Neg_0001]

#define Quad_Eps_1         g_quadConstants[QuadMathConst_Eps_1]
#define Quad_Eps_2         g_quadConstants[QuadMathConst_Eps_2]
#define Quad_Eps_3         g_quadConstants[QuadMathConst_Eps_3]

///////////////////////////////////////////////////////////////////////////////

#ifdef _USE_SIMD

   typedef __m128                         FastFloatStorage;
   typedef __m128                         QuadStorage;

   #define FLOAT_CONSTANT( a )            FastFloat( _mm_set_ps( a, a, a, a ) )
   #define QUAD_CONSTANT( a, b, c, d )    _mm_setr_ps( a, b, c, d )

#else


   typedef float                          FastFloatStorage;
   typedef struct{ ALIGN_16 float v[4]; } QuadStorage;

   #define FLOAT_CONSTANT( a )            FastFloat( a )
   #define QUAD_CONSTANT( a, b, c, d )    { a, b, c, d }

#endif

typedef bool                              FastFloatComparison;

///////////////////////////////////////////////////////////////////////////////

#endif // _QUAD_STORAGE_H
