/// @file   core-AppFlow\UserInputController.h
/// @brief  user input handling mechanism
#pragma once

#include "core\Point.h"


///////////////////////////////////////////////////////////////////////////////

typedef TVector<2> MouseSpeedVec;

///////////////////////////////////////////////////////////////////////////////

/**
 * User input handling mechanism.
 */
class UserInputController
{
public:
   virtual ~UserInputController() {}

   /**
    * Returns the present mouse position.
    */
   virtual const Point& getMousePos() const = 0;

   /**
    * Returns the current mouse speed.
    */
   virtual const MouseSpeedVec& getMouseSpeed() const = 0;

   /**
    * Tells if the specified key is currently pressed.
    *
    * @param keyCode
    */
   virtual bool isKeyPressed( unsigned char keyCode ) const = 0;
};

///////////////////////////////////////////////////////////////////////////////
