/// @file   core\MathDefs.h
/// @brief  mathematical constants definitions
#pragma once


///////////////////////////////////////////////////////////////////////////////

#undef  M_PI
#undef  M_PI_2
#undef  M_PI_4
#define M_PI       3.14159265358979323846
#define M_PI_2     1.57079632679489661923
#define M_PI_4     0.785398163397448309616
#define M2_PI     (float)(2.f * M_PI)

#define RAD2DEG(radians) ( radians * 180.f / (float)M_PI )
#define DEG2RAD(degrees) ( degrees * (float)M_PI / 180.f )

///////////////////////////////////////////////////////////////////////////////
