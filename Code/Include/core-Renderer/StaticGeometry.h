/// @file   core-Renderer\StaticGeometry.h
/// @brief  A static geometry entity
#pragma once

#include "core-Renderer\GeometryComponent.h"
#include "core\ResourceHandle.h"


///////////////////////////////////////////////////////////////////////////////

class VertexShader;
class Entity;

///////////////////////////////////////////////////////////////////////////////

/**
 * A static geometry entity.
 *
 * This class is OBSOLETE - use the GeometryComponent instead. We're leaving it around 'cause there are some assets
 * that still use it, and I don't want to reexport them. But in general, this class is dead and should no longer be used.
 */
class StaticGeometry : public GeometryComponent
{
   DECLARE_ALLOCATOR( StaticGeometry, AM_DEFAULT );
   DECLARE_CLASS()

public:
   /**
    * Default constructor.
    *
    * @param name
    */
   StaticGeometry( const char* name = "StaticGeometry" );

   /**
    * Copy constructor.
    *
    * @param rhs
    */
   StaticGeometry( const StaticGeometry& rhs );

   /**
    * Constructor.
    *
    * @param resource   shared geometry resource
    * @param name
    */
   StaticGeometry( GeometryResource& resource, const char* name = "StaticGeometry" );
   ~StaticGeometry();
};

///////////////////////////////////////////////////////////////////////////////
