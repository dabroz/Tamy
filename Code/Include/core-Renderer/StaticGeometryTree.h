/// @file   core-Renderer\StaticGeometryTree.h
/// @brief  a helper structure that helps assembling a set of geometry components that will store some static geometry
#pragma once

#include "core\MemoryRouter.h"
#include "core\Array.h"
#include "core\List.h"

// math
#include "core\AxisAlignedBox.h"
#include "core\Matrix.h"


///////////////////////////////////////////////////////////////////////////////

class GeometryComponent;
struct Matrix;
class Entity;
class FilePath;
class StaticGeometryNode;
class Material;
class Prefab;
struct AxisAlignedBox;

///////////////////////////////////////////////////////////////////////////////

class StaticGeometryTree
{
   DECLARE_ALLOCATOR( StaticGeometryTree, AM_DEFAULT );

public:
   struct MaterialEntry
   {
      DECLARE_ALLOCATOR( MaterialEntry, AM_DEFAULT );

      uint                                m_entryIdx;
      StaticGeometryNode*                 m_root;
      Material*                           m_material;

      MaterialEntry( uint entryIdx, Material* material, const AxisAlignedBox& bounds );
   };

private:
   AxisAlignedBox             m_sceneBounds;
   List< MaterialEntry* >     m_materialEntries;

public:
   /**
    * Constructor.
    *
    * @param sceneBounds
    */
   StaticGeometryTree( const AxisAlignedBox& sceneBounds );
   ~StaticGeometryTree();

   /**
    * Adds geometry from the prefab to the tree.
    *
    * @param prefab
    * @param transform
    */
   void add( const Prefab* prefab, const Matrix& transform );

   /**
    * Adds a new component to the tree.
    *
    * @param geometry
    * @param transform
    */
   void add( const GeometryComponent* geometry, const Matrix& transform );

   /**
    * Builds the static geometry.
    *
    * @param geometryDir
    * @param transform
    */
   Entity* build( const FilePath& geometryDir, const char* entityName );

private:
   void buildSingleMaterial( const FilePath& geometryDir, const char* entityName, MaterialEntry* entry, Entity* rootEntity );
};

///////////////////////////////////////////////////////////////////////////////

class StaticGeometryNode
{
   DECLARE_ALLOCATOR( StaticGeometryNode, AM_DEFAULT );

public:
   struct Entry
   {
      DECLARE_ALLOCATOR( Entry, AM_DEFAULT );

      const GeometryComponent*            m_geometry;
      Matrix                              m_transform;

      Entry( const GeometryComponent* geometry, const Matrix& transform )
         : m_geometry( geometry )
         , m_transform( transform )
      {}
   };

public:
   AxisAlignedBox                      m_nodeBounds;

   List< Entry* >                      m_geometry;
   Array< StaticGeometryNode* >        m_children;

   uint                                m_currVerticesCount;
   bool                                m_isLeafNode;

public:
   /**
    * Constructor.
    *
    * @param nodeBounds
    */
   StaticGeometryNode( const AxisAlignedBox& nodeBounds );
   ~StaticGeometryNode();

   /**
    * Adds a geometry to the node.
    * If the node has accommodated the geometry, NULL is returned, otherwise a pointer to 
    * a child node that we should try next.
    *
    * @param geometry
    * @param transform
    */
   StaticGeometryNode* add( const GeometryComponent* geometry, const Matrix& transform );

   /**
    * Builds the static geometry stored in this node.
    *
    * @param meshFilePath
    * @param material
    */
   GeometryComponent* build( const FilePath& meshFilePath, Material* material );

   /**
    * Adds an existing geometry entry to this node.
    *
    * @param entry
    */
   void addEntry( Entry* entry );

private:
   /**
    * Subdivides the node into 4 equal parts.
    */
   void subdivide();

   /**
    * Returns a child that's most suitable for accommodating the specified bounding box will be returned.
    *
    * @param boundsWorldSpace
    */
   StaticGeometryNode* findNodeFor( const AxisAlignedBox& boundsWorldSpace );
};

///////////////////////////////////////////////////////////////////////////////
