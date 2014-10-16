#include "GizmoOperation.h"
#include "core-Renderer\Camera.h"
#include "core-MVC\Entity.h"
#include "core\Transform.h"


///////////////////////////////////////////////////////////////////////////////

GizmoOperation::GizmoOperation()
   : m_snapStep( 0.0f )
   , m_coordinateMaskValue( Vector_ZERO )
{
   m_coordinateMask.set< VectorComparison::MASK_NONE >();
}

///////////////////////////////////////////////////////////////////////////////

GOTranslationAxis::GOTranslationAxis( byte axisIdx )
   : m_axisIdx( axisIdx )
{
}

///////////////////////////////////////////////////////////////////////////////

void GOTranslationAxis::transformManipulatedNodes( const Matrix& gizmoMtx, const Camera& camera, const Vector& screenSpaceTransformVec, Matrix& outTransform ) const
{
   // translation speed is dependent of the distance from the camera to node,
   // since the user expects the object to move where the mouse cursor would, meaning
   // that the further away from it are we, the larger distances it has to cover
   // for the same cursor displacements
   Vector transValue;
   {
      Vector dirCameraToGizmo;
      dirCameraToGizmo.setSub( gizmoMtx.position(), camera.getGlobalMtx().position() );

      float distDependentMultiplier = dirCameraToGizmo.length().getFloat();
      transValue.setMul( screenSpaceTransformVec, FastFloat::fromFloat( distDependentMultiplier ) );
   }

   const Vector& manipulationAxis = gizmoMtx.getRow( m_axisIdx );

   FastFloat transformationValue = Float_0;
   {
      Matrix viewProjMtx;
      viewProjMtx.setMul( camera.getViewMtx(), camera.getProjectionMtx() );
      Vector viewportSpaceManipulationAxis;
      viewProjMtx.transformNorm( manipulationAxis, viewportSpaceManipulationAxis );

      transformationValue = viewportSpaceManipulationAxis.dot( transValue );

      // snap to grid, if it's enabled
      if ( m_snapStep > 0.0f )
      {
         float val = snap( transformationValue.getFloat(), m_snapStep );
         
         transformationValue.setFromFloat( val );
      }
   }

   Vector translation;
   translation.setMul( manipulationAxis, transformationValue );

   // apply the mask
   translation.setSelect( m_coordinateMask, m_coordinateMaskValue, translation );

   Matrix transformMtx;
   transformMtx.setTranslation( translation );

   outTransform.setMul( gizmoMtx, transformMtx );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void GOTranslationPlane::transformManipulatedNodes( const Matrix& gizmoMtx, const Camera& camera, const Vector& screenSpaceTransformVec, Matrix& outTransform ) const
{
   // translation speed is dependent of the distance from the camera to node,
   // since the user expects the object to move where the mouse cursor would, meaning
   // that the further away from it are we, the larger distances it has to cover
   // for the same cursor displacements
   Vector transValue;
   {
      Vector dirCameraToGizmo;
      dirCameraToGizmo.setSub( gizmoMtx.position(), camera.getGlobalMtx().position() );

      float distDependentMultiplier = dirCameraToGizmo.length().getFloat();
      transValue.setMul( screenSpaceTransformVec, FastFloat::fromFloat( distDependentMultiplier ) );
   }

   const Matrix& cameraGlobalMtx = camera.getGlobalMtx();

   Vector sideDisplacement, upDisplacement;
   sideDisplacement.setMul( cameraGlobalMtx.sideVec(), transValue.getComponent(0) );
   upDisplacement.setMul( cameraGlobalMtx.upVec(), transValue.getComponent(1) );

   Vector displacement;
   displacement.setAdd( sideDisplacement, upDisplacement );

   // snap to grid, if it's enabled
   if ( m_snapStep > 0.0f )
   {
      displacement[0] = snap( displacement[0], m_snapStep );
      displacement[1] = snap( displacement[1], m_snapStep );
      displacement[2] = snap( displacement[2], m_snapStep );
   }

   // apply the mask
   displacement.setSelect( m_coordinateMask, m_coordinateMaskValue, displacement );

   Matrix transformMtx;
   transformMtx.setTranslation( displacement );

   outTransform.setMul( gizmoMtx, transformMtx );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

GORotationAxis::GORotationAxis( byte axisIdx )
   : m_axisIdx( axisIdx )
{
}

///////////////////////////////////////////////////////////////////////////////

void GORotationAxis::transformManipulatedNodes( const Matrix& gizmoMtx, const Camera& camera, const Vector& screenSpaceTransformVec, Matrix& outTransform ) const
{
   // rotation speed is independent of the distance from the camera to node
   Vector rotValue;
   rotValue.setMul( screenSpaceTransformVec, FastFloat::fromFloat( 2.0f ) );

   const Vector& manipulationAxis = gizmoMtx.getRow( m_axisIdx );

   FastFloat transformationValue = Float_0;
   {
      Matrix viewProjMtx;
      viewProjMtx.setMul( camera.getViewMtx(), camera.getProjectionMtx() );
      Vector viewportSpaceManipulationAxis;
      viewProjMtx.transformNorm( manipulationAxis, viewportSpaceManipulationAxis );
      transformationValue = viewportSpaceManipulationAxis.dot( rotValue );

      // snap to grid, if it's enabled
      if ( m_snapStep > 0.0f )
      {
         const float angle = RAD2DEG( transformationValue.getFloat() );
         float snappedAngle = snap( angle, m_snapStep );

         transformationValue.setFromFloat( DEG2RAD( snappedAngle ) );
      }
   }

   Quaternion rotQ;
   rotQ.setAxisAngle( manipulationAxis, transformationValue );

   Matrix transformMtx;
   transformMtx.setRotation( rotQ );

   Matrix gizmoRotMtx = gizmoMtx;
   gizmoRotMtx.setPosition<4>( Quad_0001 );

   outTransform.setMul( gizmoRotMtx, transformMtx );
   outTransform.setPosition<4>( gizmoMtx.position() );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void GORotationPlane::transformManipulatedNodes( const Matrix& gizmoMtx, const Camera& camera, const Vector& screenSpaceTransformVec, Matrix& outTransform ) const
{
   // rotation speed is independent of the distance from the camera to node
   Vector rotValue;
   rotValue.setMul( screenSpaceTransformVec, FastFloat::fromFloat( 20.0f ) );

   const Matrix& cameraGlobalMtx = camera.getGlobalMtx();

   const FastFloat valChange0 = rotValue.getComponent(0);
   const FastFloat valChange1 = rotValue.getComponent(1);
   FastFloat valChange0Sq;
   valChange0Sq.setMul( valChange0, valChange0 );
   FastFloat valChange1Sq;
   valChange1Sq.setMul( valChange1, valChange1 );

   FastFloat rollAngleMag;
   rollAngleMag.setAdd( valChange0Sq, valChange1Sq);
   rollAngleMag.sqrt();

   FastFloat rollAngleSign;
   rollAngleSign.setAdd( valChange0, valChange1 );
   rollAngleSign.neg();

   FastFloat rollAngle;
   rollAngle.setFlipSign( rollAngleMag, rollAngleSign );
   rollAngle.mul( Float_Deg2Rad );

   Quaternion rollQuat;
   rollQuat.setAxisAngle( cameraGlobalMtx.forwardVec(), rollAngle );

   Matrix transformMtx;
   transformMtx.setRotation( rollQuat );

   Matrix gizmoRotMtx = gizmoMtx;
   gizmoRotMtx.setPosition<4>( Quad_0001 );

   outTransform.setMul( gizmoRotMtx, transformMtx );
   outTransform.setPosition<4>( gizmoMtx.position() );
}

///////////////////////////////////////////////////////////////////////////////
