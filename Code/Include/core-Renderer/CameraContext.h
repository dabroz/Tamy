#pragma once

/// @file   core-Renderer\CameraContext.h
/// @brief  observation point from which we want to render the scene


///////////////////////////////////////////////////////////////////////////////

class BoundingVolume;

///////////////////////////////////////////////////////////////////////////////

/**
 * Observation point from which we want to render the scene.
 */
class CameraContext
{
public:
   virtual ~CameraContext() {}

   /**
    * Returns the global space volume that bounds all visible objects
    * in the scene.
    */
   virtual const BoundingVolume& getViewVolume() const = 0;
};

///////////////////////////////////////////////////////////////////////////////

/**
 * A helper CameraContext implementation that allows to use any bounding
 * volume as a camera context.
 */
template <typename T>
struct TCameraContext : public CameraContext
{
   T volume;

   /**
    * Constructor.
    *
    * @param vol  volume we want to use to render the scene contents.
    */
   explicit TCameraContext(const T& vol) : volume(vol) {}

   const BoundingVolume& getViewVolume() const { return volume; }
};

///////////////////////////////////////////////////////////////////////////////
