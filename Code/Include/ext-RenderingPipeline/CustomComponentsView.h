/// @file   ext-RenderingPipeline\CustomComponentsView.h
/// @brief  a view that gives access to various custom rendering related components
#pragma once

#include "core\Array.h"
#include "core-MVC\ModelView.h"
#include "ext-RenderingPipeline\RPDataProxies.h"


///////////////////////////////////////////////////////////////////////////////

class Entity;
class RPBoundingVolume;

///////////////////////////////////////////////////////////////////////////////

class CustomComponentsView : public ModelView
{
   DECLARE_ALLOCATOR( CustomComponentsView, AM_ALIGNED_16 );

public:
   class LocationRenderSettings*                            m_locationSettings;

public:
   /**
    * Constructor.
    */
   CustomComponentsView( );
   ~CustomComponentsView();

   /**
    * Queries the geometry.
    *
    * @param volume        query volume
    */
   void query( const RPBoundingVolume& volume );

   // ----------------------------------------------------------------------
   // ModelView implementation
   // ----------------------------------------------------------------------
   void onNodeAdded( SceneNode* node );
   void onNodeRemoved( SceneNode* node );
   void onAttachedToModel( Model& model );
   void resetContents( Model& scene );
};

///////////////////////////////////////////////////////////////////////////////

