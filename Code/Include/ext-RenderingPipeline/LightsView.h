/// @file   ext-RenderingPipeline\LightsView.h
/// @brief  a view that gives access to visible lights
#pragma once

#include "core\Array.h"
#include "core-MVC\ModelView.h"
#include "core\LinearStorage.h"
#include "core\AxisAlignedBox.h"
#include "ext-RenderingPipeline\RPDataProxies.h"


///////////////////////////////////////////////////////////////////////////////

class Entity;
class Light;
class RPBoundingVolume;

///////////////////////////////////////////////////////////////////////////////

class LightsView : public ModelView
{
   DECLARE_ALLOCATOR( LightsView, AM_ALIGNED_16 );

public:
   LightsArray*                                             m_visibleLights;

private:
   LinearStorage< Light >*                                  m_lightsStorage;

public:
   /**
    * Constructor.
    */
   LightsView( const AxisAlignedBox& sceneBB );
   ~LightsView();

   /**
    * Queries the geometry.
    *
    * @param volume        query volume
    * @return              collection of visible lights
    */
   LightsArray* queryLights( const RPBoundingVolume& volume );


   // ----------------------------------------------------------------------
   // ModelView implementation
   // ----------------------------------------------------------------------
   void onNodeAdded( SceneNode* node );
   void onNodeRemoved( SceneNode* node );
   void onAttachedToModel( Model& model );
   void resetContents( Model& scene );
};

///////////////////////////////////////////////////////////////////////////////

