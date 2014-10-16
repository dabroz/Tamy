/// @file   ext-RenderingPipeline/RPDataProxies.h
/// @brief  proxies for non-RTTI data structures sent through the rendering pipeline
#pragma once

#include "core\ReflectionObject.h"
#include "core\Array.h"
#include "core\Frustum.h"
#include "core\AxisAlignedBox.h"


///////////////////////////////////////////////////////////////////////////////

class Light;
class GeometryComponent;
struct AxisAlignedBox;
struct Frustum;

///////////////////////////////////////////////////////////////////////////////

struct LightsArray : public ReflectionObject
{
   DECLARE_ALLOCATOR( LightsArray, AM_DEFAULT );
   DECLARE_STRUCT();

   Array< Light* >                     m_collection;

   /**
    * Constructor.
    *
    * @param initialCapacity
    */
   LightsArray( uint initialCapacity = 1 );
};

///////////////////////////////////////////////////////////////////////////////

struct GeometryArray : public ReflectionObject
{
   DECLARE_ALLOCATOR( GeometryArray, AM_DEFAULT );
   DECLARE_STRUCT();

   Array< GeometryComponent* >         m_collection;

   /**
    * Constructor.
    *
    * @param initialCapacity
    */
   GeometryArray( uint initialCapacity = 1 );
};

///////////////////////////////////////////////////////////////////////////////

class RPBoundingVolume : public ReflectionObject
{
   DECLARE_ALLOCATOR( RPBoundingVolume, AM_DEFAULT );
   DECLARE_STRUCT();

public:
   virtual ~RPBoundingVolume() {}

   /**
    * Tests the intersection of this volume with an axis aligned box.
    *
    * @param aab
    */
   virtual bool testIntersection( const AxisAlignedBox& aab ) const = 0;
};

///////////////////////////////////////////////////////////////////////////////

class RPVolFrustum : public RPBoundingVolume
{
   DECLARE_ALLOCATOR( RPVolFrustum, AM_DEFAULT );
   DECLARE_STRUCT();

public:
   Frustum                             m_volume;

public:
   // -------------------------------------------------------------------------
   // RPBoundingVolume implementation
   // -------------------------------------------------------------------------
   bool testIntersection( const AxisAlignedBox& aab ) const;
};

///////////////////////////////////////////////////////////////////////////////

class RPVolAxisAlignedBox : public RPBoundingVolume
{
   DECLARE_ALLOCATOR( RPVolAxisAlignedBox, AM_DEFAULT );
   DECLARE_STRUCT();

public:
   AxisAlignedBox                      m_volume;

public:
   // -------------------------------------------------------------------------
   // RPBoundingVolume implementation
   // -------------------------------------------------------------------------
   bool testIntersection( const AxisAlignedBox& aab ) const;
};

///////////////////////////////////////////////////////////////////////////////
