/// @file   core/CallstackTracer.h
/// @brief  provides access to callstack information
#pragma once

#include "core/types.h"
#include <iostream>


///////////////////////////////////////////////////////////////////////////////

/**
 * Provides access to callstack information.
 */
class CallstackTracer
{
private:
   void*       m_platformImpl;

public:
   /**
    * Constructor.
    */
   CallstackTracer();
   ~CallstackTracer();

   /**
    * Creates a callstack trace.
    *
    * @param outArrTrace
    * @param traceMaxSize     how many items can 'outTrace' array accommodate
    * @return                 number of traces
    */
   uint getStackTrace( ulong* outArrTrace, uint traceMaxSize ) const;

   /**
    * Returns a function name corresponding to the specified trace.
    *
    * @param traceId
    * @param outFileName
    * @param maxFileNameLength
    * @param outMethodName
    * @param maxMethodNameLength
    * @param outLineNumber
    */
   void getTraceName( ulong traceId, char* outFileName, uint maxFileNameLength, char* outMethodName, uint maxMethodNameLength, uint& outLineNumber ) const;

   /**
    * Prints a callstack to the standard debug output.
    */
   void printCallstack( std::ostream& outputStream, ulong* arrTrace, uint traceSize ) const;
};

///////////////////////////////////////////////////////////////////////////////
