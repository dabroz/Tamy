/// @file   core/MissingDependenciesMapper.h
/// @brief  this editor will zero all dependencies on a resource with a specific path
#pragma once

#include <vector>
#include "core/ReflectionPropertiesView.h"
#include "core/ReflectionProperty.h"

///////////////////////////////////////////////////////////////////////////////

class MissingDependencyEditor;
class FilePath;
class Resource;

///////////////////////////////////////////////////////////////////////////////

/**
 * This editor will zero all dependencies on a resource with a specific path.
 */
class MissingDependenciesMapper : public ReflectionPropertiesView
{
public:
   class Editor : public ReflectionPropertyEditor
   {
   private:
      TEditableReflectionProperty< Resource* >*     m_resource;

   public:
      /**
       * Constructor.
       */
      Editor( TEditableReflectionProperty< Resource* >* resource );
      ~Editor();

      /**
       * Edits a resource dependency.
       *
       * @param resourcePath
       */
      void removeDependenciesOn( const FilePath& path );

      // -------------------------------------------------------------------------
      // ReflectionPropertyEditor implementation
      // -------------------------------------------------------------------------
      void initialize( ReflectionPropertiesView* parentView, ReflectionObjectEditor* parentEditor );
      void deinitialize( ReflectionObjectEditor* parentEditor ) {}
      void onPropertyChanged() {}
   };

private:
   std::vector< Editor* >     m_editors;

public:
   /**
    * Constructor.
    */
   MissingDependenciesMapper();
   ~MissingDependenciesMapper();

   /**
    * Edits all registered resource dependencies.
    *
    * @param resourcePath
    */
   void removeDependenciesOn( const FilePath& resourcePath );

   // -------------------------------------------------------------------------
   // ReflectionPropertiesView implementation
   // -------------------------------------------------------------------------
   void reset();

private:
   /**
    * Registers a new dependency editor.
    *
    * @param editor
    */
   void addEditor( Editor* editor );
};

///////////////////////////////////////////////////////////////////////////////
