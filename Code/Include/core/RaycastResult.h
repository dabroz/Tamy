/// @file   core\RaycastResult.h
/// @brief  result of a ray-cast query
#pragma once

#include "core\MemoryRouter.h"
#include "core\Vector.h"
#include "core\FastFloat.h"


///////////////////////////////////////////////////////////////////////////////

ALIGN_16 struct RaycastResult
{
   DECLARE_ALLOCATOR( RaycastResult, AM_DEFAULT );

   FastFloat         m_dist;
   Vector            m_contactPt;
   Vector            m_surfaceNormal;
};

///////////////////////////////////////////////////////////////////////////////
