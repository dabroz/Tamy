/// @file   core\GenericFactory.h
/// @file   abstract factory pattern
#ifndef _GENERIC_FACTORY_H
#define _GENERIC_FACTORY_H

#include <map>
#include <vector>
#include <algorithm>
#include "core\MemoryRouter.h"
#include "core\ReflectionObject.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * An interface of a helper class that will create specialized instances
 * of REPR_BASE (representation base interface).
 *
 * We need this interface so that we can store many creators in a single
 * map and look them up.
 */
template< class ENTITY, class REPR_BASE >
class Creator
{
public:
   virtual ~Creator() {}

   virtual REPR_BASE* operator()( ENTITY* obj ) = 0;
};

///////////////////////////////////////////////////////////////////////////////

/**
 * Specialized creator that creates an implementation REPR_IMPL of the REPR_BASE
 * class.
 */
template< class REPR_BASE, class REPR_IMPL, class ENTITY_BASE, class ENTITY_IMPL >
class TCreator : public Creator< ENTITY_BASE, REPR_BASE >
{
   DECLARE_ALLOCATOR( TCreator, AM_DEFAULT );

public:
   REPR_BASE* operator()( ENTITY_BASE* obj )
   {
      return new REPR_IMPL( *( static_cast< ENTITY_IMPL* >( obj ) ) );
   }
};

///////////////////////////////////////////////////////////////////////////////

/**
 * This factory will create custom objects from one class family 
 * as a representation of custom classes from another class family.
 */
template< class ENTITY, class REPR_BASE >
class GenericFactory
{
   DECLARE_ALLOCATOR( GenericFactory, AM_DEFAULT );

private:
   // -------------------------------------------------------------------------
   // internal types definitions
   // -------------------------------------------------------------------------
   struct TypeDef
   {
      DECLARE_ALLOCATOR( TypeDef, AM_DEFAULT );

      const ReflectionType&               classType;
      Creator<ENTITY, REPR_BASE>*         creator;

      TypeDef( const ReflectionType& _classType, Creator<ENTITY, REPR_BASE>* _creator )
         : classType(_classType)
         , creator(_creator)
      {}
      ~TypeDef()
      {
         delete creator;
         creator = NULL;
      }
   };
   typedef std::vector<TypeDef*> CreatorsVec;

private:
   // -------------------------------------------------------------------------
   // instance members
   // -------------------------------------------------------------------------
   CreatorsVec          m_solidCreators;
   CreatorsVec          m_abstractCreators;

public:
   virtual ~GenericFactory();

   /**
    * This method associates an entity class with another class representing it.
    * From now on whenever a create method on such 'entity' class is 
    * called, this very representation will be created for it.
    *
    * The call to this method can be chained, allowing to set up
    * the whole factory in a single code block.
    *
    * @param ENTITY_IMPL   entity we want to create an implementation for
    * @param REPR_IMPL     specialized representation we'll create for
    *                      this entity
    * @return              instance of the factory - this allows to chain
    *                      the calls
    */
   template< class ENTITY_IMPL, class REPR_IMPL >
   GenericFactory& associate()
   {
      // make a dummy instantiation so that if the entity is a template class,
      // we make sure it gets its rtti type registered
      const ReflectionType& entityClass = ENTITY_IMPL::getStaticRTTI();
      m_solidCreators.push_back( new TypeDef( entityClass, new TCreator< REPR_BASE, REPR_IMPL, ENTITY, ENTITY_IMPL >() ) );

      return *this;
   }

   /**
    * This method associates an abstract entity class with 
    * another class representing it.
    * From now on whenever a create method on such 'entity' class is 
    * called, this very representation will be created for it.
    *
    * The call to this method can be chained, allowing to set up
    * the whole factory in a single code block.
    *
    * @param ABSTRACT_ENTITY_IMPL   abstract entity class we want to create
    * @param REPR_IMPL              an implementation for specialized representation
    *                               we'll create for this entity
    * @return                       instance of the factory - this allows to chain
    *                               the calls
    */
   template< class ABSTRACT_ENTITY_IMPL, class REPR_IMPL >
   GenericFactory& associateAbstract()
   {
      const ReflectionType& entityClass = ABSTRACT_ENTITY_IMPL::getStaticRTTI();
      m_abstractCreators.push_back( new TypeDef( entityClass, new TCreator< REPR_BASE, REPR_IMPL, ENTITY, ABSTRACT_ENTITY_IMPL >() ) );

      return *this;
   }

   /**
    * The method creates a representation for an entity class.
    * Such an entity class needs to be registered with the factory
    * prior to this call.
    *
    * @param entity     entity for which we want to create a representation
    * @return           representation
    */ 
   REPR_BASE* create( ENTITY& entity );

private:
   REPR_BASE* createSolid( ENTITY& entity );
   REPR_BASE* createAbstract( ENTITY& entity );
};

///////////////////////////////////////////////////////////////////////////////

#include "core\GenericFactory.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _GENERIC_FACTORY_H
