/// @file   core-MVC\Transformable.h
/// @brief  an interface for MVC objects that have a world transform which should be updated every frame
#pragma once


///////////////////////////////////////////////////////////////////////////////

class Transformable
{
public:
   virtual ~Transformable() {}

   /**
    * Updates the world transform of the object.
    */
   virtual void updateTransforms() = 0;
};

///////////////////////////////////////////////////////////////////////////////
