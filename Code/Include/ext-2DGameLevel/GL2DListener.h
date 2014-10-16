/// @file   ext-2DGameLevel\GL2DListener.h
/// @brief  a 2D game level listener
#pragma once


///////////////////////////////////////////////////////////////////////////////

struct GL2DContactPointManifold;

///////////////////////////////////////////////////////////////////////////////

/**
* Physics world listener.
*/
class GL2DListener
{
public:
   virtual ~GL2DListener( ) {}

   /**
   * Called when 2 bodies collide with each other.
   *
   * @param contactDesc
   */
   virtual void onCollision( const GL2DContactPointManifold& collisionPoints ) {}
};

///////////////////////////////////////////////////////////////////////////////
