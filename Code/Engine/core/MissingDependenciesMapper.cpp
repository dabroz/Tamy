#include "core.h"
#include "core\MissingDependenciesMapper.h"
#include "core\Resource.h"


///////////////////////////////////////////////////////////////////////////////

MissingDependenciesMapper::MissingDependenciesMapper()
   : ReflectionPropertiesView( false )
{
   associateAbstract< Resource*, Editor >();
}

///////////////////////////////////////////////////////////////////////////////

MissingDependenciesMapper::~MissingDependenciesMapper()
{
   m_editors.clear();
}

///////////////////////////////////////////////////////////////////////////////

void MissingDependenciesMapper::removeDependenciesOn( const FilePath& resourcePath )
{
   uint count = m_editors.size();
   for ( uint i = 0; i < count; ++i )
   {
      m_editors[i]->removeDependenciesOn( resourcePath );
   }
}

///////////////////////////////////////////////////////////////////////////////

void MissingDependenciesMapper::addEditor( Editor* editor )
{
   m_editors.push_back( editor );
}

///////////////////////////////////////////////////////////////////////////////

void MissingDependenciesMapper::reset()
{
   __super::reset();

   m_editors.clear();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

MissingDependenciesMapper::Editor::Editor( TEditableReflectionProperty< Resource* >* resource )
   : m_resource( resource )
{
}

///////////////////////////////////////////////////////////////////////////////

MissingDependenciesMapper::Editor::~Editor()
{
   delete m_resource;
   m_resource = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void MissingDependenciesMapper::Editor::initialize( ReflectionPropertiesView* parentView, ReflectionObjectEditor* parentEditor )
{
   MissingDependenciesMapper* view = static_cast< MissingDependenciesMapper* >( parentView );
   view->addEditor( this );
}

///////////////////////////////////////////////////////////////////////////////

void MissingDependenciesMapper::Editor::removeDependenciesOn( const FilePath& resourcePath )
{
   Resource* res = m_resource->get();
   if ( res && res->getFilePath() == resourcePath )
   {
      m_resource->set( NULL );
   }
}

///////////////////////////////////////////////////////////////////////////////
