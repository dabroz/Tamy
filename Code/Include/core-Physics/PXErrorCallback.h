/// @file   core-Physix\PXErrorCallback.h
/// @brief  an implementation of PhysX's error callback interface
#pragma once

#include "core\MemoryRouter.h"
#include "foundation\PxErrorCallback.h"


///////////////////////////////////////////////////////////////////////////////

class PXErrorCallback : public physx::PxErrorCallback
{
   DECLARE_ALLOCATOR( PXErrorCallback, AM_DEFAULT );

public:
   // -------------------------------------------------------------------------
   // PxErrorCallback implementation
   // -------------------------------------------------------------------------
   void reportError( physx::PxErrorCode::Enum code, const char* message, const char* file, int line );
};

///////////////////////////////////////////////////////////////////////////////
