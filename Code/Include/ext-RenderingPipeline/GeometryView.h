/// @file   ext-RenderingPipeline\GeometryView.h
/// @brief  a view that gives access to renderable geometry
#pragma once

#include "core\Array.h"
#include "core\List.h"
#include "core-MVC\ModelView.h"
#include "core-MVC\EntitiesGroupView.h"
#include "core\AxisAlignedBox.h"
#include "ext-RenderingPipeline\RPDataProxies.h"


///////////////////////////////////////////////////////////////////////////////

class Entity;
class GeometryStorage;
class GeometryComponent;
class RPBoundingVolume;

///////////////////////////////////////////////////////////////////////////////

class GeometryView : public ModelView, public EntitiesGroupView
{
   DECLARE_ALLOCATOR( GeometryView, AM_ALIGNED_16 );

public:
   GeometryArray*                                           m_visibleGeometry;
   GeometryStorage*                                         m_geometryStorage;

public:
   /**
    * Constructor.
    */
   GeometryView( const AxisAlignedBox& sceneBB );
   ~GeometryView();

   /**
    * Queries the geometry.
    *
    * @param volume        query volume
    * @return              collection of visible geometry
    */
   GeometryArray* queryGeometry( const RPBoundingVolume& volume );

   // ----------------------------------------------------------------------
   // ModelView implementation
   // ----------------------------------------------------------------------
   void onNodeAdded( SceneNode* node );
   void onNodeRemoved( SceneNode* node );
   void onAttachedToModel( Model& model );
   void onDetachedFromModel( Model& model );

   // ----------------------------------------------------------------------
   // EntitiesGroupView implementation
   // ----------------------------------------------------------------------
   void onEntityAdded( Entity* entity );
   void onEntityRemoved( Entity* entity );
   void onAttachedToGroup( EntitiesGroup& group );
   void onDetachedFromGroup( EntitiesGroup& group );
};

///////////////////////////////////////////////////////////////////////////////

