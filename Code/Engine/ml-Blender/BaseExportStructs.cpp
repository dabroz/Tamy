#include "ml-Blender\BaseExportStructs.h"
#include "core\Matrix.h"
#include "core\Vector.h"
#include "core\Quaternion.h"


///////////////////////////////////////////////////////////////////////////////

void ExportMatrix::store( Matrix& mtx ) const
{
   float ALIGN_16 alignedCoords[16];
   for ( uint i = 0; i < 16; ++i )
   {
      alignedCoords[i] = coords[i];
   }
   mtx.set( alignedCoords );
}

///////////////////////////////////////////////////////////////////////////////

void ExportVector::store( Vector& vec ) const
{
   vec.set( coords[0], coords[1], coords[2], coords[3] );
}

///////////////////////////////////////////////////////////////////////////////

void ExportVector::store( Quaternion& quat ) const
{
   quat.set( coords[0], coords[1], coords[2], coords[3] );
}

///////////////////////////////////////////////////////////////////////////////
