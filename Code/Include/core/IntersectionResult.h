/// @file   core\IntersectionResult.h
/// @brief  shapes intersection query result
#pragma once

#include "core\MemoryRouter.h"
#include "core\Vector.h"


///////////////////////////////////////////////////////////////////////////////

#define MAX_CONTACT_POINTS_COUNT 3

///////////////////////////////////////////////////////////////////////////////

ALIGN_16 struct IntersectionResult
{
   DECLARE_ALLOCATOR( IntersectionResult, AM_DEFAULT );

   Vector            m_contactPoint;
   Vector            m_contactNormal;
   FastFloat         m_penetrationDepth;
};

///////////////////////////////////////////////////////////////////////////////

ALIGN_16 struct ContactPointsCollector
{
   DECLARE_ALLOCATOR( ContactPointsCollector, AM_DEFAULT );

   IntersectionResult   m_contactPoint[MAX_CONTACT_POINTS_COUNT];
   int                  m_contactsCount;

   inline void reset()
   {
      m_contactsCount = 0;
   }

   inline IntersectionResult& allocateContactPoint()
   {
      if ( m_contactsCount < MAX_CONTACT_POINTS_COUNT )
      {
         return m_contactPoint[m_contactsCount++];
      }
      else
      {
         // overwrite the last contact point if we ran out of storage
         return m_contactPoint[MAX_CONTACT_POINTS_COUNT-1];
      }
   }
};

///////////////////////////////////////////////////////////////////////////////
