#pragma once

/// @file   core-Renderer\GeometryResource.h
/// @brief  resource with geometry data

#include "core\Resource.h"


///////////////////////////////////////////////////////////////////////////////

struct AxisAlignedBox;
class Renderer;
class Shader;

///////////////////////////////////////////////////////////////////////////////

/**
 * A resource for creating geometry data.
 */
class GeometryResource : public Resource
{
   DECLARE_ALLOCATOR( GeometryResource, AM_DEFAULT );
   DECLARE_CLASS()

public:
   /**
    * Constructor.
    *
    * @param resourceName
    */
   GeometryResource( const FilePath& resourceName = FilePath() ) : Resource( resourceName ) {}
   virtual ~GeometryResource() {}

   /**
    * Calculates a bounding volume around the geometry.
    *
    * @param   new bounding volume instance
    */
   virtual const AxisAlignedBox& getBoundingVolume() { return *( reinterpret_cast< AxisAlignedBox* >( NULL ) ); }

   /**
    * Renders the resource.
    *
    * @param renderer
    */
   virtual void render( Renderer& renderer ) {}

};

///////////////////////////////////////////////////////////////////////////////
