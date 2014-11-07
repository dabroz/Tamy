/// @file   ext-StoryTeller\RandomFactory.h
/// @brief  an abstract factory that generates an implementation of an interface at random
#ifndef _RANDOM_FACTORY_H
#define _RANDOM_FACTORY_H

#include "core\Array.h"


///////////////////////////////////////////////////////////////////////////////

class Evidence;

///////////////////////////////////////////////////////////////////////////////

template< typename CreatedBaseType >
class RandomFactory
{
   DECLARE_ALLOCATOR( RandomFactory, AM_DEFAULT );

private:
   class Creator
   {
      DECLARE_ALLOCATOR( Creator, AM_DEFAULT );

   public:
      virtual ~Creator() {}

      virtual CreatedBaseType* create() const = 0;
   };

   template< class T >
   class TCreator : public Creator
   {
      DECLARE_ALLOCATOR( TCreator, AM_DEFAULT );

   public:
      CreatedBaseType* create() const override
      {
         return new T();
      }
   };

private:
   Array< Creator* >       m_creators;

public:
   ~RandomFactory();

   /**
    * Registers a new type of implementation with the factory.
    */
   template< class T >
   void addType();

   /**
    * Gives access (RW ) to the array of creators.
    */
   inline Array< Creator* >& accessCreators() {
      return m_creators;
   }

   /**
    * Gives access (RO) to the array of creators.
    */
   inline const Array< Creator* >& getCreators() const {
      return m_creators;
   }
};

///////////////////////////////////////////////////////////////////////////////

#include "ext-StoryTeller\RandomFactory.inl"

///////////////////////////////////////////////////////////////////////////////

#endif _RANDOM_FACTORY_H
