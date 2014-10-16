/// @file   TamyEditor/GizmoOperation.h
/// @brief  gizmo manipulation operation interface
#pragma once

#include "core\types.h"
#include "core\Vector.h"
#include "core\Matrix.h"
#include "core\MemoryRouter.h"


///////////////////////////////////////////////////////////////////////////////

class Entity;
class Camera;

///////////////////////////////////////////////////////////////////////////////

class GizmoOperation
{
   DECLARE_ALLOCATOR( GizmoOperation, AM_DEFAULT );

protected:
   float                m_snapStep;
   
   VectorComparison     m_coordinateMask;
   Vector               m_coordinateMaskValue;

public:
   virtual ~GizmoOperation() {}

   /**
    * Transforms the specified transform.
    *
    * @param editedNode
    * @param camera
    * @param screenSpaceTransformVec
    * @param outTransform
    */
   virtual void transformManipulatedNodes( const Matrix& gizmoMtx, const Camera& camera, const Vector& screenSpaceTransformVec, Matrix& outTransform ) const = 0;

   /**
    * Toggles snapping to grid. Passing a value <= 0.0f will disable the snap
    *
    * @param step
    */
   inline void setSnapToGrid( float step ) {
      m_snapStep = step > 0.f ? step : 0.f;
   }

   /**
    * Allows to define a coordinate mask - the gizmo will prevent movement in that direction, masking it
    * with the specified value.
    *
    * Example: if you set the mask to MASK_NONE, none of the components will get masked.
    *          if you set the mask to MASK_Y, then you'll be able to move in the XZ plane, while the Y component will get replaced
    *          with the Y component of the specified value vector.
    *
    * @param M
    * @param value
    */
   template< VectorComparison::Mask M >
   inline void setCoordinateMask( const Vector& value )
   {
      m_coordinateMask.set< M >();
      m_coordinateMaskValue = value;
   }

protected:
   GizmoOperation();
};

///////////////////////////////////////////////////////////////////////////////

/**
 * When customized with this operation, gizmo axis will translate the edited objects.
 */
class GOTranslationAxis : public GizmoOperation
{
   DECLARE_ALLOCATOR( GOTranslationAxis, AM_DEFAULT );

private:
   byte        m_axisIdx;

public:
   /**
    * Constructor.
    *
    * @param axisIdx
    */
   GOTranslationAxis( byte axisIdx );

   void transformManipulatedNodes( const Matrix& gizmoMtx, const Camera& camera, const Vector& screenSpaceTransformVec, Matrix& outTransform ) const;
};

///////////////////////////////////////////////////////////////////////////////

/**
 * This operation translates object in the camera's view XY plane
 */
class GOTranslationPlane : public GizmoOperation
{
   DECLARE_ALLOCATOR( GOTranslationPlane, AM_DEFAULT );

public:
   void transformManipulatedNodes( const Matrix& gizmoMtx, const Camera& camera, const Vector& screenSpaceTransformVec, Matrix& outTransform ) const;
};

///////////////////////////////////////////////////////////////////////////////

/**
 * When customized with this operation, gizmo axis will translate the edited objects.
 */
class GORotationAxis : public GizmoOperation
{
   DECLARE_ALLOCATOR( GORotationAxis, AM_DEFAULT );

private:
   byte        m_axisIdx;

public:
   /**
    * Constructor.
    *
    * @param axisIdx
    */
   GORotationAxis( byte axisIdx );

   void transformManipulatedNodes( const Matrix& gizmoMtx, const Camera& camera, const Vector& screenSpaceTransformVec, Matrix& outTransform ) const;
};

///////////////////////////////////////////////////////////////////////////////

/**
 * This operation rotates object in the camera's view XY plane
 */
class GORotationPlane : public GizmoOperation
{
   DECLARE_ALLOCATOR( GORotationPlane, AM_DEFAULT );

public:
   void transformManipulatedNodes( const Matrix& gizmoMtx, const Camera& camera, const Vector& screenSpaceTransformVec, Matrix& outTransform ) const;
};

///////////////////////////////////////////////////////////////////////////////
