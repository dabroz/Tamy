#include "core-Renderer\StaticGeometry.h"
#include "core-MVC\Entity.h"
#include "core-Renderer\Camera.h"
#include "core-Renderer\Renderer.h"
#include "core-Renderer\Defines.h"
#include "core-Renderer\Shader.h"
#include "core-MVC.h"
#include "core.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( StaticGeometry )
   PARENT( GeometryComponent )
END_OBJECT()

///////////////////////////////////////////////////////////////////////////////

StaticGeometry::StaticGeometry( const char* name )
   : GeometryComponent( name != NULL ? name : "" )
{
}

///////////////////////////////////////////////////////////////////////////////

StaticGeometry::StaticGeometry( const StaticGeometry& rhs )
   : GeometryComponent( rhs )
{
}

///////////////////////////////////////////////////////////////////////////////

StaticGeometry::StaticGeometry( GeometryResource& resource, const char* name  )
   : GeometryComponent( resource, name != NULL ? name : "" )
{
}

///////////////////////////////////////////////////////////////////////////////

StaticGeometry::~StaticGeometry()
{
}

///////////////////////////////////////////////////////////////////////////////
