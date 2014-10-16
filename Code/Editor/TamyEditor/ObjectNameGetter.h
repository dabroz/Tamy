/// @file   TamyEditor/ObjectNameGetter.h
/// @brief  utility for extracting the names of instances of various ReflectionObject types
#pragma once

#include <QString>

///////////////////////////////////////////////////////////////////////////////

class ReflectionObject;

///////////////////////////////////////////////////////////////////////////////

/**
 * Utility for extracting the names of instances of various ReflectionObject types.
 */
class ObjectNameGetter
{
public:
   /**
    * Returns the instance name of the specified object.
    *
    * @param object
    */
   static QString toString( ReflectionObject* object );
};

///////////////////////////////////////////////////////////////////////////////
