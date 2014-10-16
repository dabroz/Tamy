/// @file   TamyEditor/SceneNodesTransformsCache.h
/// @brief  
#pragma once

#include "core-MVC\ModelView.h"
#include "core\Matrix.h"
#include "core\List.h"


///////////////////////////////////////////////////////////////////////////////

class Entity;

///////////////////////////////////////////////////////////////////////////////

class SceneNodesTransformsCache : public ModelView
{
   DECLARE_ALLOCATOR( SceneNodesTransformsCache, AM_DEFAULT );

private:
   struct SceneNodeTransformEntry
   {
      DECLARE_ALLOCATOR( SceneNodeTransformEntry, AM_DEFAULT );

      Entity*        m_entity;
      Matrix         m_cachedTransform;

      SceneNodeTransformEntry( Entity* entity = NULL )
         : m_entity( entity )
      {}

      static bool isEqual( const SceneNodeTransformEntry& entry, const Entity* entity )
      {
         return entry.m_entity == entity;
      }
   };

   List< SceneNodeTransformEntry >           m_entities;

public:
   /**
    * Constructor.
    */
   SceneNodesTransformsCache();
   ~SceneNodesTransformsCache();

   /**
    * Caches/un-caches transforms.
    */
   void cacheTransforms( bool cache );

   // -------------------------------------------------------------------------
   // ModelView implementation
   // -------------------------------------------------------------------------
   void onAttachedToModel( Model& model );
   void onNodeAdded( SceneNode* node );
   void onNodeRemoved( SceneNode* node );
   void resetContents( Model& scene );
};

///////////////////////////////////////////////////////////////////////////////
