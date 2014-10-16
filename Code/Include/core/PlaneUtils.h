/// @file   core/PlaneUtils.h
/// @brief  utility methods that operate on mathematical planes
#pragma once


///////////////////////////////////////////////////////////////////////////////

struct Vector;
struct Plane;

///////////////////////////////////////////////////////////////////////////////

/**
 * Utility methods that operate on mathematical planes.
 */
class PlaneUtils
{
public:
   /**
    * Calculates the intersection point of three planes. 
    *
    * @param p1
    * @param p2
    * @param p3
    * @param outIntersectionPt
    * @return  true if the point was found, false otherwise ( if two or more planes were perpendicular )
    */
   static bool calculatePlanesIntersection( const Plane& p1, const Plane& p2, const Plane& p3, Vector& outIntersectionPt );
};

///////////////////////////////////////////////////////////////////////////////