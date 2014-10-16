/// @file   core/Singleton.h
/// @brief  Singleton design pattern

#ifndef _SINGLETON_H
#define _SINGLETON_H

#include "core\types.h"


///////////////////////////////////////////////////////////////////////////////

struct SingletonConstruct
{
};

///////////////////////////////////////////////////////////////////////////////

/**
 * Singleton design pattern.
 */
template< typename T >
class TSingleton 
{
private:
   static T*          s_theInstance;

public:
   /**
    * Initializes the singleton with the specified instance.
    */
   static T& initialize();

   /**
    * Deinitializes the singleton.
    *
    * @param calledByUser
    */
   static void deinitialize( bool calledByUser = true );

   /**
    * Returns the singleton instance of the class.
    */
   static T& getInstance();
};

///////////////////////////////////////////////////////////////////////////////

class SingletonsManager
{
private:
   class Entry
   {
   public:
      Entry*      m_next;
      uint        m_order;

   public:
      Entry();
      virtual ~Entry() {}

      virtual bool compare( void* rhs ) const = 0;

      virtual void deinitialize() = 0;
   };

   template< typename T >
   class TEntry : public Entry
   {
   private:
      T*       m_instance;

   public:
      TEntry( T* instance );
      ~TEntry();

      bool compare( void* rhs ) const override;
      void deinitialize() override;
   };

private:
   static SingletonsManager*              s_theInstance;

   Entry*                                 m_rootEntry;

public:
   ~SingletonsManager();

   /**
   * Returns the only instance of the manager.
   */
   static SingletonsManager& getInstance();

   /**
   * Call this method when the application closes to deinitialize the manager
   * and all registered singletons.
   */
   static void deinitialize();

   /**
   * Registers a new singleton.
   *
   * @param singleton
   */
   template< typename T >
   void registerInstance( T* instance );

   /**
   * Unregisters a new singleton.
   *
   * @param singleton
   */
   template< typename T >
   void unregisterInstance( T* instance );

private:
   /**
   * Constructor.
   */
   SingletonsManager();

   /**
   * Copy constructor.
   */
   SingletonsManager( const SingletonsManager& ) {}
};

///////////////////////////////////////////////////////////////////////////////

#include "core\Singleton.inl"

///////////////////////////////////////////////////////////////////////////////

#define INIT_SINGLETON( ClassName ) template<> ClassName* TSingleton< ClassName >::s_theInstance = NULL;

#define REGULAR_SINGLETON() friend class SingletonsManager; static uint getSingletonInsertionOrder() { return 65535; }

#define PRIORITY_SINGLETON( priorityIdx ) friend class SingletonsManager; static uint getSingletonInsertionOrder() { return priorityIdx; }

///////////////////////////////////////////////////////////////////////////////

#endif // _SINGLETON_H
