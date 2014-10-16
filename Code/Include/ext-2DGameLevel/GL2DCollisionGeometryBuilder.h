/// @file   ext-2DGameLevel\GL2DCollisionGeometryBuilder.h
/// @brief  assembles a static level geometry shape from the GL2DCollisionGeometry components found in the scene
#pragma once

#include "core\MemoryRouter.h"
#include "core\Array.h"
#include "core\List.h"
#include "core\types.h"


///////////////////////////////////////////////////////////////////////////////

class Model;
class Entity;
class SceneNode;
class GL2DBody;
class GL2DShape;
struct Vector;

///////////////////////////////////////////////////////////////////////////////

class GL2DCollisionGeometryBuilder
{
   DECLARE_ALLOCATOR( GL2DCollisionGeometryBuilder, AM_DEFAULT );

private:
   Entity*                 m_rootEntity;

   Array< Vector >         m_vertices;
   Array< word >           m_indices;

   GL2DBody*               m_createdBody;
   GL2DShape*              m_createdShape;

public:
   /**
    * Constructor.
    *
    * @param rootEntity
    */
   GL2DCollisionGeometryBuilder( Entity* rootEntity );

   /**
    * Initializes the job, allowing the user to select the geometry components he wishes to include
    * in the level geometry
    *
    * @param selectedNodes
    */
   void initialize( const List< SceneNode* >& selectedNodes );

   /**
    * Processes the job.
    */
   void process();

private:
   void parseEntity( Entity* entity );
   void parseGeometry( SceneNode* node );
};

///////////////////////////////////////////////////////////////////////////////
