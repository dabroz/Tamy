/// @file   core\Resource.h
/// @brief  interface marking all resources
#ifndef _RESOURCE_H
#define _RESOURCE_H

#include "core\MemoryRouter.h"
#include "core\ReflectionObject.h"
#include "core\FilePath.h"
#include "core\Array.h"
#include "core\ResourceHandle.h"


///////////////////////////////////////////////////////////////////////////////

class ResourcesManager;
class ResourcesDB;
class ReflectionType;

///////////////////////////////////////////////////////////////////////////////

/**
 * This interface marks all resources that can be loaded using 
 * the resources manager.
 */
class Resource : public ReflectionObject
{
   DECLARE_ALLOCATOR( Resource, AM_DEFAULT );
   DECLARE_CLASS()

public:
   enum AccessMode
   {
      AM_BINARY,
      AM_TEXT
   };

private:
   FilePath                         m_filePath;
   ResourcesDB*                     m_host;

   // an array of handles that reference the resource
   Array< ResourceHandle* >         m_referencingHandles;

public:
   /**
    * Constructor.
    *
    * @param filePath   path to the file where we want to save the resource.
    */
   Resource( const FilePath& filePath = FilePath() );
   virtual ~Resource();

   /**
    * Returns the path to the file holding this resource.
    */
   inline const FilePath& getFilePath() const { return m_filePath; }

   /**
    * Sets a new path to the resource.
    *
    * @param path
    * @param host    host manager performing the operation
    */
   void setFilePath( const FilePath& path, ResourcesDB* host = NULL );

   /**
    * Saves the resource to an associated file.
    */
   void saveResource();

   /**
    * Tells whether the resource is managed by a resources manager.
    */
   inline bool isManaged() const { return m_host != NULL; }

   /**
    * Returns an extension of this resource instance.
    */
   virtual const char* getVirtualExtension() { return ""; }

   /**
    * Called after the resource was successfully loaded from an archive and all its internal and external
    * dependencies were resolved.
    *
    * Calls the onResourceLoaded method and registers all active components with the resource.
    */
   void finalizeResourceLoading();

   // -------------------------------------------------------------------------
   // Resource types management
   // -------------------------------------------------------------------------

   /**
    * Tells whether the specified extension can be mapped to a specific resource.
    *
    * @param extension     resource file extension
    */
   static bool isResource( const std::string& extension );

   /**
    * A method telling how each resource file should be opened
    * (using binary or text access mode).
    *
    * @param extension     resource file extension
    */
   static std::ios_base::openmode getFileAccessMode( const std::string& extension );

   /**
    * Searches for a resource type matching the specified extension.
    *
    * @param extension
    */
   static const SerializableReflectionType* findResourceClass( const std::string& extension );

   /**
    * Registers a new resource type.
    *
    * @param extension     resource file extension
    * @param accessMode    resource file access mode
    * @param typeName      resource type name
    */
   static void registerResource( const std::string& extension, AccessMode accessMode, const std::string& typeName );

   // -------------------------------------------------------------------------
   // Notifications
   // -------------------------------------------------------------------------

   /**
    * Replaces contents of the resource with another after the resource was reloaded.
    *
    * @param rhs
    */
   virtual void replaceContents( Resource& rhs ) {}

   /**
    * The method is called by the resources manager once
    * before the resource gets saved to give it a chance to perform pre-save state caching etc.
    */
    virtual void onResourceSave( ResourcesManager& mgr ) {}

protected:

   // -------------------------------------------------------------------------
   // Notifications
   // -------------------------------------------------------------------------
   /**
    * This is the place to kick off all post-load activities that involve resolution of inter-object
    * dependencies.
    * It's guaranteed that the method will be called AFTER all loaded objects receive an onObjectLoaded method call.
    * What's not guaranteed is the ordered in which the onResourceLoaded method will be called on multiple resources
    * loaded at the same time.
    */
    virtual void onResourceLoaded( ResourcesManager& mgr ) {}

   // -------------------------------------------------------------------------
   // ResourceHandle API
   // -------------------------------------------------------------------------
   friend class ResourceHandle;
   
   /**
    * Attaches a resource handle as a lifecycle listener.
    *
    * @param handle
    */
   void attachHandleListener( ResourceHandle& handle );

   /**
    * Attaches a resource handle as a lifecycle listener.
    *
    * @param handle
    */
   void detachHandleListener( ResourceHandle& handle );

   // -------------------------------------------------------------------------
   // ResourcesDB API
   // -------------------------------------------------------------------------
   friend class ResourcesDB;

   /**
    * Informs the resource that from now on it's managed by a resources manager.
    *
    * @param hostDB
    */
   void setResourcesManager( ResourcesDB& hostDB );

   /**
    * Informs the resource that it's been removed from the resources manager.
    */
   void resetResourcesManager();
};

///////////////////////////////////////////////////////////////////////////////

// Resource declaration
#define DECLARE_RESOURCE()                                                    \
   DECLARE_CLASS()                                                            \
   public:                                                                    \
      static const char* getExtension();                                      \
      virtual const char* getVirtualExtension();                              \
   private:

///////////////////////////////////////////////////////////////////////////////

/**
 * Declares a specific resource without implicit parent declaration ( BEGIN_RESOURCE
 * will declare Resource as a parent )
 */
#define BEGIN_RESOURCE_NO_PARENT( ClassName, Extension, AccessMode )          \
   class RegisterResource##Extension                                          \
   {                                                                          \
   public:                                                                    \
      RegisterResource##Extension()                                           \
      {                                                                       \
         ReflectionTypeID< ClassName > type;                                  \
         Resource::registerResource( #Extension, Resource::AccessMode, type.m_name ); \
      }                                                                       \
   };                                                                         \
   RegisterResource##Extension resourceTypeRegistryFor_##Extension;           \
   const char* ClassName::getExtension() { return #Extension; }               \
   const char* ClassName::getVirtualExtension() { return ClassName::getExtension(); } \
   BEGIN_OBJECT( ClassName );

///////////////////////////////////////////////////////////////////////////////

/**
 * Declares a specific resource, implicitly declaring Resource as the parent.
 */ 
#define BEGIN_RESOURCE( ClassName, Extension, AccessMode )                    \
      BEGIN_RESOURCE_NO_PARENT( ClassName, Extension, AccessMode );           \
      PARENT( Resource );

///////////////////////////////////////////////////////////////////////////////

#define END_RESOURCE() END_OBJECT()

///////////////////////////////////////////////////////////////////////////////

#endif // _RESOURCE_H 
