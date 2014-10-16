#include "NavigationState.h"
#include "TamySceneWidget.h"
#include "SceneEditor.h"
#include "SelectionManager.h"
#include "GizmoAxis.h"
#include "core\Point.h"
#include "core\CollectionUtils.h"
#include "core-MVC\Entity.h"
#include "core-Renderer\Renderer.h"
#include "core-Renderer\Camera.h"
#include "TamyCameraController.h"
#include "GizmoNodeManipulationState.h"

// queries
#include "core-Renderer\RQEntitySelection.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( NavigationState )
END_OBJECT()

///////////////////////////////////////////////////////////////////////////////

NavigationState::NavigationState()
   : m_objectsSelectionEnabled( true )
   , m_sceneWidget( NULL )
   , m_uic( NULL )
   , m_renderer( NULL )
   , m_cameraController( NULL )
   , m_rotating( false )
   , m_selectionQueryMode( SM_NONE )
{
   memset( m_movementDir, false, sizeof( bool ) * 4 );
}

///////////////////////////////////////////////////////////////////////////////

NavigationState::~NavigationState()
{
   if ( m_sceneWidget )
   {
      m_sceneWidget->setRelativeMouseMovement( false );
   }

   delete m_cameraController;
   m_cameraController = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void NavigationState::initInput( TamySceneWidget& widget )
{
   m_sceneWidget = &widget;
   m_uic = &widget.getUserInputController();
   m_renderer = &widget.getRenderer();

   Camera& camera = widget.getCamera();
   m_cameraController = new OrbitingMotionController( camera );
}

///////////////////////////////////////////////////////////////////////////////

void NavigationState::setCameraControlMode( CameraControlMode mode )
{
   Camera& camera = m_cameraController->getCamera();
   delete m_cameraController;

   switch ( mode )
   {
   case CCM_ORBIT:
   {
      m_cameraController = new OrbitingMotionController( camera );
      break;
   }

   case CCM_FREE:
   {
      m_cameraController = new UnconstrainedMotionController( camera );
      break;
   }
   }
}

///////////////////////////////////////////////////////////////////////////////

void NavigationState::enableObjectsSelection( bool enable )
{
   m_objectsSelectionEnabled = enable;
   if ( !enable )
   {
      m_selectionQueryMode = SM_NONE;
   }
}

///////////////////////////////////////////////////////////////////////////////

void NavigationState::activate()
{
   m_selectionQueryMode = SM_NONE;
   m_rotating = false;
   memset( m_movementDir, false, sizeof( bool ) * 4 );

   if ( m_sceneWidget )
   {
      m_sceneWidget->setRelativeMouseMovement( false );
   }
}

///////////////////////////////////////////////////////////////////////////////

void NavigationState::deactivate()
{
   m_selectionQueryMode = SM_NONE;
   if ( m_sceneWidget )
   {
      m_sceneWidget->setRelativeMouseMovement( false );
   }
}

///////////////////////////////////////////////////////////////////////////////

void NavigationState::execute( float timeElapsed )
{
   updateCamera( timeElapsed );

   if ( !m_objectsSelectionEnabled )
   {
      return;
   }

   {
      SelectionManager& selectionMgr = m_sceneWidget->getSelectionMgr();
      bool appendSelection = m_uic->isKeyPressed( VK_SHIFT );
      selectionMgr.enableAppendedSelectionMode( appendSelection );
   }

   if ( m_selectionQueryMode != SM_NONE )
   {
      return;
   }

   Point mousePos = m_uic->getMousePos();
   if ( mousePos.x < 0 || mousePos.y < 0 || mousePos.x >= m_sceneWidget->width() || mousePos.y >= m_sceneWidget->height() )
   {
      return;
   }

   bool issueQuery = false;
   if ( m_uic->isKeyPressed( VK_RBUTTON ) )
   {
      issueQuery = true;
      m_selectionQueryMode = SM_NODES;
   }
   else if ( m_uic->isKeyPressed( VK_LBUTTON ) )
   {
      issueQuery = true;
      m_selectionQueryMode = SM_GIZMO;
   }

   if ( issueQuery )
   {
            
      new ( m_renderer->query() ) RQSelectEntity( mousePos, this );
   }
}

///////////////////////////////////////////////////////////////////////////////

void NavigationState::onEntitiesSelected( const List< Entity* >& entities )
{
   if ( !m_objectsSelectionEnabled )
   {
      return;
   }

   switch( m_selectionQueryMode )
   {
   case SM_NONE:
      {
         // an old query must have come late - ignore
         break;
      }

   case SM_NODES:
      {
         SelectionManager& selectionMgr = m_sceneWidget->getSelectionMgr();

         // remove any selected gizmo axes
         bool wasGizmoAxisFound = false;
         List< SceneNode* > filteredNodes;
         for ( List< Entity* >::const_iterator it = entities.begin(); !it.isEnd(); ++it )
         {
            Entity* entity = *it;
            if ( !entity->isA< GizmoAxis >() )
            {
               filteredNodes.pushBack( entity );
            }
            else
            {
               wasGizmoAxisFound = true;
            }
         }

         // pass the selection onto the selection manager
         if ( !wasGizmoAxisFound )
         {
            selectionMgr.selectNodesFromViewport( filteredNodes );
         }

         break;
      }

   case SM_GIZMO:
      {
         GizmoAxis* selectedAxis = NULL;
         if ( entities.size() == 1 )
         {
            Entity* entity = entities.front();
            if ( entity->isA< GizmoAxis >() )
            {
               selectedAxis = static_cast< GizmoAxis* >( entity );
            }
         }

         transitionTo< GizmoNodeManipulationState >().setGizmoAxis( selectedAxis );

         break;
      }
   }

   m_selectionQueryMode = SM_NONE;
}

///////////////////////////////////////////////////////////////////////////////

void NavigationState::updateCamera( float timeElapsed )
{
   float speedMul = m_uic->isKeyPressed( VK_SHIFT ) ? 4.0f : 1.0f;
   FastFloat movementSpeed; movementSpeed.setFromFloat( 10.0f * speedMul * timeElapsed );
   FastFloat negMovementSpeed; negMovementSpeed.setNeg( movementSpeed );
   float rotationSpeed = 0.1f * timeElapsed;

   // check which keys are pressed
   m_movementDir[MD_FRONT] = m_uic->isKeyPressed( 'W' );
   m_movementDir[MD_BACK] = m_uic->isKeyPressed( 'S' );
   m_movementDir[MD_LEFT] = m_uic->isKeyPressed( 'A' );
   m_movementDir[MD_RIGHT] = m_uic->isKeyPressed( 'D' );

   m_rotating = m_uic->isKeyPressed( VK_MBUTTON );
   m_sceneWidget->setRelativeMouseMovement( m_rotating );

   if ( m_rotating )
   {
      float mouseSpeedX = m_uic->getMouseSpeed().v[0] * rotationSpeed;
      float mouseSpeedY = m_uic->getMouseSpeed().v[1] * rotationSpeed;
      m_cameraController->rotate( mouseSpeedY, mouseSpeedX );
   }

   // process the keys
   Vector tmpVec, moveVec;
   if ( m_movementDir[MD_FRONT] )  
   { 
      m_cameraController->getLookVec( tmpVec );
      moveVec.setMul( tmpVec, movementSpeed );
      m_cameraController->move( moveVec ); 
   }

   if ( m_movementDir[MD_BACK] )
   { 
      m_cameraController->getLookVec( tmpVec );
      moveVec.setMul( tmpVec, negMovementSpeed );
      m_cameraController->move( moveVec ); 
   }

   if ( m_movementDir[MD_LEFT] )   
   { 
      m_cameraController->getRightVec( tmpVec );
      moveVec.setMul( tmpVec, negMovementSpeed );
      m_cameraController->move( moveVec ); 
   }

   if ( m_movementDir[MD_RIGHT] )
   { 
      m_cameraController->getRightVec( tmpVec );
      moveVec.setMul( tmpVec, movementSpeed );
      m_cameraController->move( moveVec ); 
   }
}

///////////////////////////////////////////////////////////////////////////////
