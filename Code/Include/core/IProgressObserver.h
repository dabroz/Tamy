/// @file   core/IProgressObserver.h
/// @brief  common interface for all progress observers

#pragma once

#include <string>


///////////////////////////////////////////////////////////////////////////////

/**
 * Common interface for all progress observers.
 */
class IProgressObserver
{
public:
   virtual ~IProgressObserver() {}

   /**
    * Call this method to initialize a progress counting sequence.
    *
    * @param elemsCount    number of elements in the observed section
    */
   virtual void initialize( unsigned int elemsCount ) = 0;

   /**
    * Changes the observer status - an easy way to let the user know what actually is taking place at the moment.
    */
   virtual void setStatus( const char* status ) = 0;

   /**
    * Call this method to advance the progress counting sequence.
    */ 
   virtual void advance() = 0;

   /**
    * Cancels the progress due to an error.
    */
   virtual void cancel() = 0;
};

///////////////////////////////////////////////////////////////////////////////

class NullProgressObserver : public IProgressObserver
{
public:
   void initialize( unsigned int elemsCount ) {}
   void setStatus( const char* status ) {}
   void advance() {}
   void cancel() {}
};

///////////////////////////////////////////////////////////////////////////////
