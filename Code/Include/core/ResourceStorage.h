/// @file   core\ResourceStorage.h
/// @brief  and implementation of IWFScene
#ifndef _RESOURCE_STORAGE_H
#define _RESOURCE_STORAGE_H

#include <map>
#include <string>
#include "core\MemoryRouter.h"
#include "core\Assert.h"


///////////////////////////////////////////////////////////////////////////////

/** 
 * A generic storage capable of managing objects of a certain type.
 * All registered object MUST implement a 'const std::string& getName() const'
 * method.
 */
template<typename ResourceType>
class ResourceStorage
{
   DECLARE_ALLOCATOR( ResourceStorage, AM_DEFAULT );

private:
   typedef std::map<std::string, ResourceType*> ResourceMap;
   ResourceMap m_resources;

public:
   ~ResourceStorage();

   /** 
    * Retrieves a reference to a resource from the storage.
    *
    * @param name    resource id
    * @return        reference to the resource
    */ 
   ResourceType& get(const std::string& name);

   /** 
    * Retrieves a reference to a resource from the storage.
    * (const version)
    *
    * @param name    resource id
    * @return        reference to the resource
    */
   const ResourceType& get(const std::string& name) const;

   /**
    * Checks if the resource exists.
    *
    * @param name    resource id
    * @return        'true' if it exists, 'false' otherwise
    */
   bool is(const std::string& name) const;

   /**
    * Adds a new resource to the storage.
    *
    * @param resource   resource to add
    * @return           'true' if the resource was successfully added
    *                   (no other resource with same name was found),
    *                   'false' otherwise
    */
   bool add(ResourceType* resource);
};

///////////////////////////////////////////////////////////////////////////////

#include "core\ResourceStorage.inl"

///////////////////////////////////////////////////////////////////////////////

#endif _RESOURCE_STORAGE_H
