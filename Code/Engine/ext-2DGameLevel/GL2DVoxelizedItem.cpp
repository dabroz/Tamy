#include "ext-2DGameLevel\GL2DVoxelizedItem.h"

// rendering
#include "core-Renderer\StaticGeometryTree.h"

// prefabs
#include "core-MVC\Prefab.h"
#include "core\FilePath.h"

// utils
#include "core\StringUtils.h"


///////////////////////////////////////////////////////////////////////////////

GL2DVoxelizedItem::GL2DVoxelizedItem( const Prefab* prefab, GL2DVoxelRotation rotation )
: m_prefab( prefab )
, m_rotation( rotation )
{
}

///////////////////////////////////////////////////////////////////////////////

GL2DVoxelizedItem::GL2DVoxelizedItem( const GL2DVoxelizedItem& rhs )
   : m_prefab( rhs.m_prefab )
   , m_rotation( rhs.m_rotation )
{
}

///////////////////////////////////////////////////////////////////////////////

void GL2DVoxelizedItem::render( const Point& pos, StaticGeometryTree& staticGeometry ) const
{
   static Quaternion rotations[] =
   {
      Quaternion( 0.f, 0.f, 0.f, 1.f ),
      Quaternion( 0, 0, 0.707106769f, 0.707106769f ),
      Quaternion( 0.f, 0.f, 1.f, 0.f ),
      Quaternion( 0, 0, 0.707106769f, -0.707106769f )
   };


   // calculate the transform
   Vector translation;
   pos.toVector( translation );

   Matrix transform;
   transform.setRotation( rotations[m_rotation] );
   transform.setPosition<3>( translation );

   // add the prefab to the static geometry
   staticGeometry.add( m_prefab, transform );
}

///////////////////////////////////////////////////////////////////////////////
