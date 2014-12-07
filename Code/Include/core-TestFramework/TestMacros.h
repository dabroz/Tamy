#pragma once

#include "core\Quaternion.h"


///////////////////////////////////////////////////////////////////////////////

#define COMPARE_EULER(v1, v2)                                                 \
   CPPUNIT_ASSERT(fabs(v1.getPitch() - v2.getPitch()) < 0.0001f);             \
   CPPUNIT_ASSERT(fabs(v1.getYaw() - v2.getYaw()) < 0.0001f);                 \
   CPPUNIT_ASSERT(fabs(v1.getRoll() - v2.getRoll()) < 0.0001f);

///////////////////////////////////////////////////////////////////////////////

#define COMPARE_PT(p1, p2)                                                    \
   CPPUNIT_ASSERT_EQUAL( p1.x, p2.x );                                        \
   CPPUNIT_ASSERT_EQUAL( p1.y, p2.y );

///////////////////////////////////////////////////////////////////////////////

#define COMPARE_VEC(v1, v2)                                                   \
   CPPUNIT_ASSERT(fabs(v1[0] - v2[0]) < 0.0001f);                             \
   CPPUNIT_ASSERT(fabs(v1[1] - v2[1]) < 0.0001f);                             \
   CPPUNIT_ASSERT(fabs(v1[2] - v2[2]) < 0.0001f);

///////////////////////////////////////////////////////////////////////////////

#define COMPARE_VEC4(v1, v2)                                                  \
   CPPUNIT_ASSERT(fabs(v1[0] - v2[0]) < 0.0001f);                             \
   CPPUNIT_ASSERT(fabs(v1[1] - v2[1]) < 0.0001f);                             \
   CPPUNIT_ASSERT(fabs(v1[2] - v2[2]) < 0.0001f);                             \
   CPPUNIT_ASSERT(fabs(v1[3] - v2[3]) < 0.0001f);

///////////////////////////////////////////////////////////////////////////////

#define COMPARE_QUAT(v1, v2)                                                  \
   if ( fabs(v1[0] - v2[0]) >= 0.001f ||                                      \
        fabs(v1[1] - v2[1]) >= 0.001f ||                                      \
        fabs(v1[2] - v2[2]) >= 0.001f ||                                      \
        fabs(v1[3] - v2[3]) >= 0.001f )                                       \
   {                                                                          \
      CPPUNIT_ASSERT( false );                                                \
   }

///////////////////////////////////////////////////////////////////////////////

#define COMPARE_QUAT_ALLOW_CONJUGATION(v1, v2)                                \
   {                                                                          \
      Quaternion conjV2;                                                      \
      conjV2 = v2; conjV2.conjugate();                                        \
      if ( fabs(v1[0] - v2[0]) >= 0.001f || fabs(v1[1] - v2[1]) >= 0.001f || fabs(v1[2] - v2[2]) >= 0.001f || fabs(v1[3] - v2[3]) >= 0.001f ) \
      {                                                                       \
         if ( fabs(v1[0] + v2[0]) >= 0.001f || fabs(v1[1] + v2[1]) >= 0.001f || fabs(v1[2] + v2[2]) >= 0.001f || fabs(v1[3] + v2[3]) >= 0.001f ) \
         {                                                                    \
            if ( fabs(v1[0] - conjV2[0]) >= 0.001f || fabs(v1[1] - conjV2[1]) >= 0.001f || fabs(v1[2] - conjV2[2]) >= 0.001f || fabs(v1[3] - conjV2[3]) >= 0.001f ) \
            {                                                                 \
               if ( fabs(v1[0] + conjV2[0]) >= 0.001f || fabs(v1[1] + conjV2[1]) >= 0.001f || fabs(v1[2] + conjV2[2]) >= 0.001f || fabs(v1[3] + conjV2[3]) >= 0.001f ) \
               {                                                              \
                  CPPUNIT_ASSERT( false );                                    \
               }                                                              \
            }                                                                 \
         }                                                                    \
      }                                                                       \
   }

///////////////////////////////////////////////////////////////////////////////

#define COMPARE_QUAT_AXIS_ANGLE(q, axis, angle)                               \
   {                                                                          \
      FastFloat cqaaAngle; Vector cqaaAxis;                                   \
      q.getAngle( cqaaAngle );                                                \
      cqaaAngle.mul( Float_Rad2Deg );                                         \
      q.getAxis( cqaaAxis );                                                  \
      CPPUNIT_ASSERT(fabs(cqaaAngle.getFloat() - angle) < 0.0001f);           \
      COMPARE_VEC( axis, cqaaAxis );                                          \
   }

///////////////////////////////////////////////////////////////////////////////

#define COMPARE_PLANE(v1, v2)                                                 \
   CPPUNIT_ASSERT(fabs(v1.a - v2[0]) < 0.0001f);                               \
   CPPUNIT_ASSERT(fabs(v1.b - v2[1]) < 0.0001f);                               \
   CPPUNIT_ASSERT(fabs(v1.c - v2[2]) < 0.0001f);                               \
   CPPUNIT_ASSERT(fabs(v1.d - v2[3]) < 0.0001f);

///////////////////////////////////////////////////////////////////////////////

#define COMPARE_MTX(rhs, lhs)                                                 \
{                                                                             \
   bool err = false;                                                          \
   for ( char col = 0; col < 4 && !err ; ++col )                              \
   {                                                                          \
      for ( char row = 0; row < 4 && !err; ++row )                            \
      {                                                                       \
         if ( fabs( rhs( col, row ) - lhs( col, row ) ) >= 0.0001f )          \
         {                                                                    \
            CPPUNIT_ASSERT( false );                                          \
            err = true;                                                       \
            break;                                                            \
         }                                                                    \
      }                                                                       \
   }                                                                          \
}

///////////////////////////////////////////////////////////////////////////////

#define COMPARE_TRANSFORM(rhs, lhs)                                           \
   COMPARE_VEC( rhs.m_translation, lhs.m_translation );                       \
   COMPARE_QUAT( rhs.m_rotation, lhs.m_rotation );

///////////////////////////////////////////////////////////////////////////////
