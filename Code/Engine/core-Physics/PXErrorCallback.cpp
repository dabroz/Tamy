#include "core-Physics\PXErrorCallback.h"
#include "core\Assert.h"
#include "core\Log.h"


///////////////////////////////////////////////////////////////////////////////

void PXErrorCallback::reportError( physx::PxErrorCode::Enum code, const char* message, const char* file, int line )
{
   switch ( code )
   {
      case physx::PxErrorCode::eABORT:
      case physx::PxErrorCode::eINVALID_PARAMETER:
      case physx::PxErrorCode::eINVALID_OPERATION:
      case physx::PxErrorCode::eINTERNAL_ERROR:
      case physx::PxErrorCode::eOUT_OF_MEMORY:
      {
         AssertionHandle handler( "<PhysX_internal>", file, line );
         _tamyAssert( message, handler );

         // fall through and log such error as well
      }

      default:
      {
         LOG( "%s : %d %s\n", file, line, message );
         break;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////
