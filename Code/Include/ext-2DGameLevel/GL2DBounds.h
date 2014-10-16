/// @file   ext-2DGameLevel\GL2DBounds.h
/// @brief  a 2D, integer valued bounding box - to be used with grid based objects
#pragma once

#include "core\MemoryRouter.h"
#include "core\AxisAlignedBox.h"


///////////////////////////////////////////////////////////////////////////////

struct Point;

///////////////////////////////////////////////////////////////////////////////

class GL2DBounds
{
   DECLARE_ALLOCATOR( GL2DBounds, AM_DEFAULT );

private:
   AxisAlignedBox box;

public:
   /**
    * Constructor.
    */
   GL2DBounds();

   /**
    * Resets the bounds.
    */
   void reset();

   /**
    * Sets the new maxima of the box.
    *
    * @param min
    * @param max
    */
   void set( const Point& min, const Point& max );

   /**
    * Returns the maxima of the box.
    *
    * @param outMin
    * @param outMax
    */
   void get( Point& outMin, Point& outMax ) const;

   /**
    * Returns the extents of the box.
    *
    * @param outExtents.
    */
   void getExtents( Point& extents ) const;

   /**
    * Returns the center of this box.
    *
    * @param outCenter
    */
   void getCenter( Point& center ) const;

   /**
    * Translates this box.
    *
    * @param offset
    */
   void translate( const Point& offset );

   /**
    * Creates a union of this box with the specified one.
    *
    * @param otherBounds
    */
   void add( const GL2DBounds& otherBounds );

   /**
    * Includes the specified point in the bounds.
    *
    * @param pt
    */
   void include( const Point& pt );

   /**
    * Tests the intersection of the  boxes.
    *
    * @param rhs
    */
   bool testIntersection( const GL2DBounds& rhs ) const;

   /**
    * Finds a translation that applied to this box will prevent
    * the boxes from overlapping.
    *
    * @param rhs
    * @param outTranslation
    */
   bool findIntersectionRemovalVector( const GL2DBounds& rhs, Point& outTranslation ) const;

   /**
    * Returns a distance from the center of this box to the center of the other box.
    *
    * @param rhs
    */
   uint distanceTo( const GL2DBounds& rhs ) const;

   /**
    * Returns the four corners of this box. The points will be located outside the outline
    * of the room.
    *
    * @param arrCornerPoints.
    */
   void getCorners( Point* arrCornerPoints ) const;

   /**
    * Returns 2 points that belong to each of the specified bounding boxes that
    * lie closest to one another.
    *
    * @param rhs
    * @param outPt1
    * @param outPt2
    */
   void getClosestPoints( const GL2DBounds& rhs, Point& outPt1, Point& outPt2 );
};

///////////////////////////////////////////////////////////////////////////////
