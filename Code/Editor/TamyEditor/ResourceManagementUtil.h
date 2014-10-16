/// @file   TamyEditor/ResourceManagementUtil.h
/// @brief  a couple of methods that facilitate resources management used by the ResourcesBrowser
#ifndef _RESOURCE_MANAGEMENT_UTIL_H
#define _RESOURCE_MANAGEMENT_UTIL_H

#include <string>


///////////////////////////////////////////////////////////////////////////////

class SerializableReflectionType;
class QIcon;
class QWidget;
class FilePath;

///////////////////////////////////////////////////////////////////////////////

/**
 * A couple of methods that facilitate resources management used by the ResourcesBrowser.
 */
class ResourceManagementUtil
{
public:
   /**
    * Creates a new resource of the specified type and puts it in the
    * specified parent directory.
    *
    * The browser will ask to specify the resource name etc.
    *
    * @param parentWidget     parent widget for the dialog box that's gonna be popped during the resource creation process
    * @param type
    * @param parentDir
    */
   static void createResource( QWidget* parentWidget, const SerializableReflectionType& type, const std::string& parentDir );

   /**
    * Opens a resource for edition.
    *
    * @param path          path to a resource
    * @param resourceIcon
    */
   static void editResource( const std::string& path, const QIcon& resourceIcon );

   /**
    * Checks if the specified file path represents a resource of the given type.
    *
    * @param ResourceType
    * @param path
    */
   template< typename ResourceType >
   static bool isResourceOfType( const FilePath& path );
};

///////////////////////////////////////////////////////////////////////////////

#include "ResourceManagementUtil.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _RESOURCE_MANAGEMENT_UTIL_H
