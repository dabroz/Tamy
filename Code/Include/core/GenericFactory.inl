#ifndef _GENERIC_FACTORY_H
#error "This file can only be included from GenericFactory.h"
#else


///////////////////////////////////////////////////////////////////////////////

template< class ENTITY, class REPR_BASE >
GenericFactory< ENTITY, REPR_BASE  >::~GenericFactory()
{
   for ( CreatorsVec::iterator it = m_solidCreators.begin(); it != m_solidCreators.end(); ++it )
   {
      delete *it;
   }
   m_solidCreators.clear();

   for ( CreatorsVec::iterator it = m_abstractCreators.begin(); it != m_abstractCreators.end(); ++it )
   {
      delete *it;
   }
   m_abstractCreators.clear();
}

///////////////////////////////////////////////////////////////////////////////

template< class ENTITY, class REPR_BASE>
REPR_BASE* GenericFactory< ENTITY, REPR_BASE >::create(ENTITY& entity)
{
   // first try to find a solid class creator for it...
   REPR_BASE* instance = createSolid(entity);

   if (!instance)
   {
      // ...if that's not possible - check if the class matches to 
      // any of the registered abstract types
      instance = createAbstract(entity);
   }

   return instance;
}

///////////////////////////////////////////////////////////////////////////////

template<class ENTITY, class REPR_BASE>
REPR_BASE* GenericFactory<ENTITY, REPR_BASE>::createSolid(ENTITY& entity)
{
   const ReflectionType& checkedClassType = entity.getVirtualRTTI();

   for ( CreatorsVec::iterator it = m_solidCreators.begin(); it != m_solidCreators.end(); ++it )
   {
      const ReflectionType& refClassType = (*it)->classType;
      if ( refClassType.isExactlyA( checkedClassType ) == true )
      {
         REPR_BASE* rep = ( *(*it)->creator )( &entity );

         return rep;
      }
   }
   return NULL;
}

///////////////////////////////////////////////////////////////////////////////

template< class ENTITY, class REPR_BASE >
REPR_BASE* GenericFactory< ENTITY, REPR_BASE >::createAbstract( ENTITY& entity )
{
   const ReflectionType& checkedClassType = entity.getVirtualRTTI();

   for ( CreatorsVec::iterator it = m_abstractCreators.begin(); it != m_abstractCreators.end(); ++it )
   {
      const ReflectionType& refClassType = (*it)->classType;
      if ( checkedClassType.isA( refClassType ) == true )
      {
         REPR_BASE* rep = ( *( ( *it )->creator ) )( &entity );

         return rep;
      }
   }

   return NULL;
}

///////////////////////////////////////////////////////////////////////////////

#endif // _GENERIC_FACTORY_H
