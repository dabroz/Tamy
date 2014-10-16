#include "core-Renderer\RendererUtils.h"

// math
#include "core\AxisAlignedBox.h"
#include "core\Vector.h"

// collections
#include "core\List.h"

// scene
#include "core-MVC\Model.h"
#include "core-MVC\Entity.h"
#include "core-MVC\EntityUtils.h"

// geometry
#include "core-Renderer\GeometryComponent.h"


///////////////////////////////////////////////////////////////////////////////

void RendererUtils::calculateSceneBounds( const Model& scene, AxisAlignedBox& outBounds )
{
   List< const GeometryComponent* > geometry;
   EntityUtils::collectNodesByType< GeometryComponent >( scene.getRoot(), geometry );

   AxisAlignedBox nodeBounds;
   for ( List< const GeometryComponent* >::iterator it = geometry.begin(); !it.isEnd(); ++it )
   {
      const GeometryComponent* geomComp = *it;
      geomComp->getBoundingBox( nodeBounds );

      outBounds.add( nodeBounds, outBounds );
   }
}

///////////////////////////////////////////////////////////////////////////////

void RendererUtils::ptrToVec( void* ptr, Vector& outVec )
{
   PtrAsBytes ptrRep;
   ptrRep.ptr = ( long ) ptr;

   outVec[0] = ( float ) ( ptrRep.b[0] / 255.f );
   outVec[1] = ( float ) ( ptrRep.b[1] / 255.f );
   outVec[2] = ( float ) ( ptrRep.b[2] / 255.f );
   outVec[3] = ( float ) ( ptrRep.b[3] / 255.f );
}

///////////////////////////////////////////////////////////////////////////////

void* RendererUtils::vecToPtr( const Vector& vec )
{
   PtrAsBytes ptrRep;
   unsigned int r = ( unsigned int ) ( vec[0] * 255.f );
   unsigned int g = ( unsigned int ) ( vec[1] * 255.f );
   unsigned int b = ( unsigned int ) ( vec[2] * 255.f );
   unsigned int a = ( unsigned int ) ( vec[3] * 255.f );

   ptrRep.b[0] = r;
   ptrRep.b[1] = g;
   ptrRep.b[2] = b;
   ptrRep.b[3] = a;

   return ( void* ) ( ptrRep.ptr );
}

///////////////////////////////////////////////////////////////////////////////
