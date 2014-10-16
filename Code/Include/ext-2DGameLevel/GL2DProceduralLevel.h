/// @file   ext-2DGameLevel\GL2DProceduralLevel.h
/// @brief  a description of how a procedural level should be generated
#pragma once

#include "core-MVC\Component.h"
#include "core\AxisAlignedBox.h"
#include "core\List.h"
#include "core\Array.h"


///////////////////////////////////////////////////////////////////////////////

class Prefab;
struct Point;

typedef uint( *CostFunc )( const Point&, const Point& );

///////////////////////////////////////////////////////////////////////////////

enum CostFunctionEnum
{
   GL2D_StraightCorridors,
   GL2D_BendyCorridors,
};

///////////////////////////////////////////////////////////////////////////////

class GL2DProceduralLevel : public Component
{
   DECLARE_ALLOCATOR( GL2DProceduralLevel, AM_DEFAULT );
   DECLARE_CLASS();

public:
   // static data
   FilePath             m_geometryVoxelsDir;
   FilePath             m_geometryDeploymentDir;
   uint                 m_voxelsPerCell;
   uint                 m_roomsCount;
   uint                 m_maxSideCorridorsDepth;      // how many side rooms will get connected by a side corridor
   uint                 m_maxExpandedChildrenCount;   // how many connections of every analyzed side room will be expanded

   CostFunctionEnum     m_corridorType;

public:
   /**
    * Default constructor.
    *
    * @param name
    */
   GL2DProceduralLevel( const char* name = "GL2DProceduralLevel" );

   /**
    * Copy constructor
    *
    * @param rhs
    */
   GL2DProceduralLevel( const GL2DProceduralLevel& rhs );

   /**
    * Returns the selected cost function.
    */
   CostFunc getCostFunction() const;
};

///////////////////////////////////////////////////////////////////////////////
