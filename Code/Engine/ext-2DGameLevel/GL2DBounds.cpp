#include "ext-2DGameLevel\GL2DBounds.h"
#include "core\Point.h"


///////////////////////////////////////////////////////////////////////////////

GL2DBounds::GL2DBounds()
{}

///////////////////////////////////////////////////////////////////////////////

void GL2DBounds::reset()
{
   box.reset();
}

///////////////////////////////////////////////////////////////////////////////

void GL2DBounds::set( const Point& min, const Point& max )
{
   box.min.set( ( float ) min.x, ( float ) min.y, 0 );
   box.max.set( ( float ) max.x, ( float ) max.y, 0 );
}

///////////////////////////////////////////////////////////////////////////////

void GL2DBounds::get( Point& min, Point& max ) const
{
   min.x = ( int ) round( box.min[0] );
   min.y = ( int ) round( box.min[1] );
   max.x = ( int ) round( box.max[0] );
   max.y = ( int ) round( box.max[1] );
}

///////////////////////////////////////////////////////////////////////////////

void GL2DBounds::getExtents( Point& extents ) const
{
   int minX = ( int ) round( box.min[0] );
   int minY = ( int ) round( box.min[1] );
   int maxX = ( int ) round( box.max[0] );
   int maxY = ( int ) round( box.max[1] );

   extents.x = abs( maxX - minX );
   extents.y = abs( maxY - minY );
}

///////////////////////////////////////////////////////////////////////////////

void GL2DBounds::getCenter( Point& center ) const
{
   int minX = ( int ) round( box.min[0] );
   int minY = ( int ) round( box.min[1] );
   int maxX = ( int ) round( box.max[0] );
   int maxY = ( int ) round( box.max[1] );


   center.x = ( maxX + minX ) / 2;
   center.y = ( maxY + minY ) / 2;
}

///////////////////////////////////////////////////////////////////////////////

void GL2DBounds::translate( const Point& offset )
{
   Vector t( ( float ) offset.x, ( float ) offset.y, 0.f );
   box.min.add( t );
   box.max.add( t );
}

///////////////////////////////////////////////////////////////////////////////

void GL2DBounds::add( const GL2DBounds& otherBounds )
{
   box.add( otherBounds.box, box );
}

///////////////////////////////////////////////////////////////////////////////

void GL2DBounds::include( const Point& pt )
{
   Vector t;
   pt.toVector( t );
   box.include( t );
}

///////////////////////////////////////////////////////////////////////////////

bool GL2DBounds::testIntersection( const GL2DBounds& rhs ) const
{
   return box.testIntersection( rhs.box );
}

///////////////////////////////////////////////////////////////////////////////

bool GL2DBounds::findIntersectionRemovalVector( const GL2DBounds& rhs, Point& outTranslation ) const
{
   if ( !box.testIntersection( rhs.box ) )
   {
      return false;
   }

   Vector vec;
   box.findIntersectionRemovalVector( rhs.box, vec );
   outTranslation.x = ( int ) round( vec[0] );
   outTranslation.y = ( int ) round( vec[1] );

   return true;
}

///////////////////////////////////////////////////////////////////////////////

uint GL2DBounds::distanceTo( const GL2DBounds& rhs ) const
{
   if ( box.testIntersection( rhs.box ) )
   {
      // the boxes are overlapping
      return 0;
   }

   Point pt[4];
   get( pt[0], pt[1] );
   rhs.get( pt[2], pt[3] );

   float dist, minDist;
   minDist = 100000.f;
   for ( byte i = 0; i < 4; ++i )
   {
      for ( byte j = i + 1; j < 4; ++j )
      {
         dist = pt[i].distanceSqTo( pt[j] );
         if ( dist < minDist )
         {
            minDist = dist;
         }
      }
   }

   return ( uint ) sqrt( minDist );
}

///////////////////////////////////////////////////////////////////////////////

void GL2DBounds::getCorners( Point* arrCornerPoints ) const
{
   get( arrCornerPoints[0], arrCornerPoints[2] );
   arrCornerPoints[1].x = arrCornerPoints[0].x;
   arrCornerPoints[1].y = arrCornerPoints[2].y;
   arrCornerPoints[3].x = arrCornerPoints[2].x;
   arrCornerPoints[3].y = arrCornerPoints[0].y;

   // move the points out of the bounds
   arrCornerPoints[0].x -= 1;
   arrCornerPoints[2].y -= 1;
}

///////////////////////////////////////////////////////////////////////////////

void GL2DBounds::getClosestPoints( const GL2DBounds& rhs, Point& outPt1, Point& outPt2 )
{
   Point theseCorners[4];
   Point rhsCorners[4];
   getCorners( theseCorners );
   rhs.getCorners( rhsCorners );

   float dist, minDist;
   minDist = 100000.f;
   for ( byte i = 0; i < 4; ++i )
   {
      for ( byte j = 0; j < 4; ++j )
      {
         dist = theseCorners[i].distanceSqTo( rhsCorners[j] );
         if ( dist < minDist )
         {
            minDist = dist;
            outPt1 = theseCorners[i];
            outPt2 = rhsCorners[j];
         }
      }
   }
}

///////////////////////////////////////////////////////////////////////////////
