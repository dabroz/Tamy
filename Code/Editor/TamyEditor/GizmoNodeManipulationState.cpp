#include "GizmoNodeManipulationState.h"
#include "NavigationState.h"
#include "TamySceneWidget.h"
#include "SceneEditor.h"
#include "GizmoAxis.h"
#include "core-Renderer\Renderer.h"
#include "core-Renderer\Camera.h"
#include "core-MVC\Entity.h"
#include "core-MVC\EntityUtils.h"
#include "core\Matrix.h"
#include "GizmoOperation.h"


///////////////////////////////////////////////////////////////////////////////

namespace // anonymous
{
   Vector GLOBAL_AXES[] = {
      Vector_OX,
      Vector_OY,
      Vector_OZ
   };
}

///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( GizmoNodeManipulationState )
END_OBJECT()

///////////////////////////////////////////////////////////////////////////////

GizmoNodeManipulationState::GizmoNodeManipulationState()
   : m_sceneWidget( NULL )
   , m_uic( NULL )
   , m_renderer( NULL )
   , m_camera( NULL )
   , m_selectedGizmoAxis( NULL )
{
    memset( m_movementDir, false, sizeof( bool ) * 4 );
}

///////////////////////////////////////////////////////////////////////////////

GizmoNodeManipulationState::~GizmoNodeManipulationState()
{
}

///////////////////////////////////////////////////////////////////////////////

void GizmoNodeManipulationState::initInput( TamySceneWidget& widget )
{
   m_sceneWidget = &widget;
   m_uic = &widget.getUserInputController();
   m_renderer = &widget.getRenderer();
   m_camera = &m_renderer->getActiveCamera();
}

///////////////////////////////////////////////////////////////////////////////

void GizmoNodeManipulationState::activate()
{
   Gizmo* gizmo = m_sceneWidget->getGizmo();
   const List< Entity* >& selectedEntities = gizmo->getManipulatedEntities();

   if ( selectedEntities.empty() )
   {
      transitionTo< NavigationState >();
   }
   else
   {
      if ( m_sceneWidget )
      {
         m_sceneWidget->setRelativeMouseMovement( true );
      }
      setupController();
   }
}

///////////////////////////////////////////////////////////////////////////////

void GizmoNodeManipulationState::deactivate()
{
   if ( m_sceneWidget )
   {
      m_sceneWidget->setRelativeMouseMovement( false );
   }
}

///////////////////////////////////////////////////////////////////////////////

void GizmoNodeManipulationState::execute( float timeElapsed )
{
   if ( m_uic->isKeyPressed( VK_LBUTTON ) )
   {
      update( timeElapsed );
   }
   else
   {
      transitionTo< NavigationState >();
   }
}

///////////////////////////////////////////////////////////////////////////////

void GizmoNodeManipulationState::setupController()
{
   memset( m_movementDir, false, sizeof( bool ) * 4 );

   // store gizmo's bind matrix
   Gizmo* gizmo = m_sceneWidget->getGizmo();
   m_gizmoBaseTransform = gizmo->getGlobalMtx();
   m_gizmoBindMtx.setInverse( m_gizmoBaseTransform );

   m_viewportSpaceTransformation.setZero();
   m_prevNodesTransformation.setIdentity();
}

///////////////////////////////////////////////////////////////////////////////

void GizmoNodeManipulationState::update( float timeElapsed )
{
   GizmoOperation* operation = m_sceneWidget->getGizmoOperation( m_selectedGizmoAxis );
   Gizmo* gizmo = m_sceneWidget->getGizmo();

   // check which keys are pressed
   m_movementDir[MD_FRONT] = m_uic->isKeyPressed( 'W' );
   m_movementDir[MD_BACK] = m_uic->isKeyPressed( 'S' );
   m_movementDir[MD_LEFT] = m_uic->isKeyPressed( 'A' );
   m_movementDir[MD_RIGHT] = m_uic->isKeyPressed( 'D' );

   // calculate the parameter change factor ( relative to the camera orientation )
   float movementSpeed = 0.001f * timeElapsed;
   bool anyKeyHeld = m_movementDir[MD_FRONT] || m_movementDir[MD_BACK] || m_movementDir[MD_LEFT] || m_movementDir[MD_RIGHT];
   if ( anyKeyHeld )
   {
      if ( m_movementDir[MD_FRONT] )  { m_viewportSpaceTransformation[1] += movementSpeed; }
      if ( m_movementDir[MD_BACK] )   { m_viewportSpaceTransformation[1] -= movementSpeed; }
      if ( m_movementDir[MD_LEFT] )   { m_viewportSpaceTransformation[0] -= movementSpeed; }
      if ( m_movementDir[MD_RIGHT] )  { m_viewportSpaceTransformation[0] += movementSpeed; }
   }
   else
   {
      m_viewportSpaceTransformation[0] += m_uic->getMouseSpeed().v[0] * movementSpeed;

      // we need to negate this value, 'cause we want it expressed in viewport space ( where Y axis goes up, not down )
      m_viewportSpaceTransformation[1] += -m_uic->getMouseSpeed().v[1] * movementSpeed;
   }

   // calculate a transformation matrix
   Matrix dNodesTransformation;
   {
      Matrix newGizmoMtx;
      operation->transformManipulatedNodes( m_gizmoBaseTransform, *m_camera, m_viewportSpaceTransformation, newGizmoMtx );
      gizmo->setLocalMtx( newGizmoMtx );

      Matrix invGizmoMtx;
      invGizmoMtx.setInverse( m_gizmoBaseTransform );
      
      Matrix newNodesTransformation;
      newNodesTransformation.setMul( invGizmoMtx, newGizmoMtx );

      Matrix invPrevNodesTransformation;
      invPrevNodesTransformation.setInverse( m_prevNodesTransformation );

      dNodesTransformation.setMul( invPrevNodesTransformation, newNodesTransformation );
      m_prevNodesTransformation = newNodesTransformation;
   }

   // transform all selected nodes
   const List< Entity* >& selectedEntities = gizmo->getManipulatedEntities();
   for ( List< Entity* >::const_iterator it = selectedEntities.begin(); !it.isEnd(); ++it )
   {
      Entity* editedNode = *it;
      const Matrix& parentMtx = editedNode->getParentWorldMtx();

      Matrix localNodeTransformation;

      MatrixUtils::changeReferenceSystem( dNodesTransformation, parentMtx, localNodeTransformation );

      const Matrix& nodeLocalMtx = editedNode->getLocalMtx();
      Matrix newLocalMtx;
      newLocalMtx.setMul( nodeLocalMtx, localNodeTransformation );
      MatrixUtils::regenerateAxes( newLocalMtx );

      editedNode->setLocalMtx( newLocalMtx );

      // notify the object that it's value has externally changed ( other editors might be watching )
      ReflectionProperty* property = editedNode->getProperty( "m_localMtx" );
      editedNode->notifyPropertyChange( *property );
      delete property;
   }
}

///////////////////////////////////////////////////////////////////////////////

