/// @file   ext-2DGameLevel/GL2DCollisionGeometry.h
/// @brief  This component is a building block of the level collision geometry
#pragma once

#include "core-MVC\Component.h"
#include "core\Vector.h"
#include "core\Matrix.h"
#include "core\Array.h"


///////////////////////////////////////////////////////////////////////////////

class InStream;
class OutStream;

///////////////////////////////////////////////////////////////////////////////

struct GL2DCollisionTriangle
{
   DECLARE_ALLOCATOR( GL2DCollisionTriangle, AM_DEFAULT );

   word idx[3];

   /**
    * Default constructor.
    */
   GL2DCollisionTriangle();

   /**
    * Constructor.
    *
    * @param i0
    * @param i1
    * @param i2
    */
   GL2DCollisionTriangle( word i0, word i1, word i2 );

   /**
    * Sets new indices of the triangle.
    *
    * @param i0
    * @param i1
    * @param i2
    */
   void set( word i0, word i1, word i2 );

   // -------------------------------------------------------------------------
   // Serialization support
   // -------------------------------------------------------------------------
   friend OutStream& operator<<( OutStream& stream, const GL2DCollisionTriangle& face );
   friend InStream& operator>>( InStream& stream, GL2DCollisionTriangle& face );
};

///////////////////////////////////////////////////////////////////////////////

/**
 * Collision geometry builder will look for these components in the entities
 * present in a level, so that it can assemble the collision geometry from them.
 */
class GL2DCollisionGeometry : public Component
{
   DECLARE_ALLOCATOR( GL2DCollisionGeometry, AM_DEFAULT );
   DECLARE_CLASS();

private:
   Array< Vector >                        m_vertices;
   Array< GL2DCollisionTriangle >         m_faces;

public:
   /**
    * Default constructor.
    *
    * @param name
    */
   GL2DCollisionGeometry( const char* name = "GL2DCollisionGeometry" );

   /**
    * Copy constructor.
    */
   GL2DCollisionGeometry( const GL2DCollisionGeometry& rhs );
   ~GL2DCollisionGeometry();

   /**
    * Sets new geometry on the component, transforming it if necessary.
    *
    * @param vertices
    * @param faces
    * @param transform
    */
   void setGeometry( const Array< Vector >& vertices, const Array< GL2DCollisionTriangle >& faces, const Matrix& transform = Matrix::IDENTITY );

   /**
    * Returns the vertices of the collision mesh.
    */
   inline const Array< Vector >& getVertices() {
      return m_vertices;
   }

   /**
    * Returns the faces of the collision mesh.
    */
   inline const Array< GL2DCollisionTriangle >& getFaces() const {
      return m_faces;
   }

};

///////////////////////////////////////////////////////////////////////////////