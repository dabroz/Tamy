#include "SceneObjectsManipulator.h"
#include "TamySceneWidget.h"
#include "core-MVC/Entity.h"
#include "SceneEditor.h"
#include "SelectionManager.h"

// states
#include "NavigationState.h"
#include "GizmoNodeManipulationState.h"
#include "DisabledState.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( SceneObjectsManipulator );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

SceneObjectsManipulator::SceneObjectsManipulator()
   : m_widget( NULL )
{
   registerState< NavigationState >();
   registerState< GizmoNodeManipulationState >();
   registerState< DisabledState >();
   begin< NavigationState >();
}

///////////////////////////////////////////////////////////////////////////////

SceneObjectsManipulator::~SceneObjectsManipulator()
{
}

///////////////////////////////////////////////////////////////////////////////

void SceneObjectsManipulator::initialize( TamySceneWidget& widget )
{
   m_widget = &widget;

   // initialize the states as input receivers
   getState< NavigationState >().initInput( widget );
   getState< GizmoNodeManipulationState >().initInput( widget );
}

///////////////////////////////////////////////////////////////////////////////

void SceneObjectsManipulator::enable( bool flag )
{
   if ( flag )
   {
      transitionTo< NavigationState >();
   }
   else
   {
     transitionTo< DisabledState >();
   }
}

///////////////////////////////////////////////////////////////////////////////
