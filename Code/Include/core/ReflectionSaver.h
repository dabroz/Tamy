/// @file   core/ReflectionSaver.h
/// @brief  serialization tool for saving reflection objects
#ifndef _REFLECTION_SAVER_H
#define _REFLECTION_SAVER_H

#include <vector>
#include "core\MemoryRouter.h"
#include "core\types.h"
#include "core\Array.h"
#include "core\FilePath.h"
#include "core\ReflectionDependenciesCallback.h"


///////////////////////////////////////////////////////////////////////////////

class ReflectionObject;
class OutStream;
class Resource;

///////////////////////////////////////////////////////////////////////////////

/**
 * Serialization tool for saving reflection objects.
 *
 * The tool writes the serialized data to a single stream, so if this stream
 * is a file, it can contain multiple resources.
 *
 * Since each resource is identified by a FilePath. But this path is NOT
 * an actual path of a file on the hard drive - it's a path in our mapped file system.
 * 
 * A single serialized archive can contain multiple files and, when loaded by the resource
 * manager, can add mappings to multiple new file system files.
 *
 *
 * NOTE: If the layout of the saved resource changes, be sure to update the ResourceRenameScanner class as well,
 * which performs direct IO operations on the resources using the said resource layout.
 */
class ReflectionSaver : public ReflectionDependencyMapperCallback
{
   DECLARE_ALLOCATOR( ReflectionSaver, AM_DEFAULT );

private:
   OutStream&                                m_outStream;

   // saved dependencies
   std::vector< const ReflectionObject* >    m_dependencies;

   // indices in the m_dependencies array of the objects that were passed
   // as an argument of the 'save' method
   std::vector< uint >                       m_serializedObjectsIndices;

   // external dependencies that should be saved along with the saved objects
   std::vector< const Resource* >            m_externalDependencies;

   // a temporary array that contains objects added as the dependencies, but the dependencies of which have not yet been mapped
   std::vector< const ReflectionObject* >    m_objectsToMap;

public:
   /**
    * Constructor.
    *
    * @param outStream         where should the saver save the serialized objects
    */
   ReflectionSaver( OutStream& outStream );
   ~ReflectionSaver();

   /**
    * Serializes the specified object to the stream.
    * The object has to have an RTTI information.
    *
    * Keep in mind that you can serialize several objects using this method,
    * and if you do that, the pointers they might contain will be mapped
    * to the same objects. This allows you to serialize large number of objects
    * without loosing track of their dependencies.
    *
    * However, the method on its own doesn't flush the data to the output stream
    * specified in the constructor. You either have to call the 'flush' method
    * to do that ( the instance will start remapping the dependencies from the start then )
    * or destroy the instance of the class - destructor flushes the serialized data automatically.
    *
    * @param object
    */
   void save( const ReflectionObject* object );

   /**
    * Flushes the serialized data out to the output stream, thus finishing
    * the serialization process.
    */
   void flush();

   /**
    * Collects all mapped external dependencies.
    *
    * @param outDependencies
    */
   void collectExternalDependencies( std::vector< const Resource* >& outDependencies ) const;

   // -------------------------------------------------------------------------
   // ReflectionDependencyMapperCallback implementation
   // -------------------------------------------------------------------------
   void addDependency( const ReflectionObject* dependency );
   uint findDependency( const ReflectionObject* dependency ) const;

private:
   /**
    * Maps the dependencies on the objects the specified object references.
    *
    * @param object              mapped object
    */
   void mapDependencies( const ReflectionObject* object );

   void addExternalDependency( const ReflectionObject* dependency );
   void addInternalDependency( const ReflectionObject* dependency );
   void saveExternalDependencies( OutStream& outStream );
   void saveInternalDependencies( OutStream& outStream );
};

///////////////////////////////////////////////////////////////////////////////

#include "core/ReflectionSaver.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _REFLECTION_SAVER_H
