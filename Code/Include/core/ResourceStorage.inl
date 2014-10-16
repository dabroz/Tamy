#ifndef _RESOURCE_STORAGE_H
#error "Must be included only from the ResourceStorage.h"
#else


///////////////////////////////////////////////////////////////////////////////

template<typename ResourceType>
ResourceStorage<ResourceType>::~ResourceStorage()
{
   for (ResourceMap::iterator it = m_resources.begin();
      it != m_resources.end(); ++it)
   {
      delete it->second;
   }
}

///////////////////////////////////////////////////////////////////////////////

template<typename ResourceType>
ResourceType& ResourceStorage<ResourceType>::get( const std::string& name )
{
   ResourceMap::iterator it = m_resources.find(name);
   if ( it != m_resources.end() ) 
   {
      return *(it->second);
   }

   ASSERT_MSG( false, "Resource doesn't exist");
}

///////////////////////////////////////////////////////////////////////////////

template<typename ResourceType>
const ResourceType& ResourceStorage<ResourceType>::get(const std::string& name) const
{
   ResourceMap::const_iterator it = m_resources.find(name);
   if ( it != m_resources.end() ) 
   {
      return *(it->second);
   }

   ASSERT_MSG( false, "Resource doesn't exist");
}

///////////////////////////////////////////////////////////////////////////////

template<typename ResourceType>
bool ResourceStorage<ResourceType>::is(const std::string& name) const
{
   ResourceMap::const_iterator it = m_resources.find(name);
   return (it != m_resources.end());
}

///////////////////////////////////////////////////////////////////////////////

template<typename ResourceType>
bool ResourceStorage<ResourceType>::add(ResourceType* resource)
{
   ASSERT_MSG (resource != NULL, "NULL pointer instead a valid resource");

   ResourceMap::iterator it = m_resources.find(resource->getName());
   if (it != m_resources.end()) 
   {
      // resource like this already exists
      delete resource;
      return false;
   } 

   m_resources.insert(std::make_pair(resource->getName(), resource));

   return true;
}

///////////////////////////////////////////////////////////////////////////////

#endif // _RESOURCE_STORAGE_H
