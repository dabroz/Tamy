/// @file      TamyEditor/DisabledState.h
/// @brief     SceneObjectsManipulator disabled state
#pragma once

#include "core-AI\FSMState.h"
#include "SceneObjectsManipulator.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * SceneObjectsManipulator disabled state.
 *
 * Manipulator is completely disabled when this state is active, meaning that the user
 * can't select objects from the scene, move around that scene nor move any objects around.
 */
class DisabledState : public FSMState< SceneObjectsManipulator >
{
   DECLARE_CLASS()

};

///////////////////////////////////////////////////////////////////////////////
