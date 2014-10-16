/// @file   TamyEditor/TamySceneWidgetUtils.h
/// @brief  a set of utility methods used by TamySceneWidget
#pragma once


///////////////////////////////////////////////////////////////////////////////

class TamySceneWidgetUtils
{
public:
   /**
    * Translates a Qt keycode to a DirectX keycode.
    *
    * @param qtKeyCode
    */
   static unsigned char toDXKey( int qtKeyCode );
};

///////////////////////////////////////////////////////////////////////////////
