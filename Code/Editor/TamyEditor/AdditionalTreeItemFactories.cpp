#include "AdditionalTreeItemFactories.h"
#include "core\ResourcesManager.h"
#include "core\Filesystem.h"
#include <QtGui\QIcon>


///////////////////////////////////////////////////////////////////////////////

DirectoriesFactoryWrapper::DirectoriesFactoryWrapper( TreeWidgetDescFactory* wrappedFactory )
   : m_wrappedFactory( wrappedFactory )
{
}

///////////////////////////////////////////////////////////////////////////////

DirectoriesFactoryWrapper::~DirectoriesFactoryWrapper()
{
   delete m_wrappedFactory;
   m_wrappedFactory = NULL;
}

///////////////////////////////////////////////////////////////////////////////

unsigned int DirectoriesFactoryWrapper::typesCount() const
{
   return m_wrappedFactory->typesCount() + 1;
}

///////////////////////////////////////////////////////////////////////////////

void DirectoriesFactoryWrapper::getDesc( unsigned int idx, QString& outDesc, QString& outGroup, QIcon& outIcon ) const
{
   if ( idx == 0 )
   {
      // a directory
      outDesc = "Directory";
      outGroup = "";
      outIcon = QIcon( ":/TamyEditor/Resources/Icons/Editor/dirIcon.png" );
   }
   else
   {
      // a resource
      m_wrappedFactory->getDesc( idx - 1, outDesc, outGroup, outIcon );
   }
}

///////////////////////////////////////////////////////////////////////////////

void DirectoriesFactoryWrapper::getDesc( const SerializableReflectionType& type, QString& outDesc, QString& outGroup, QIcon& outIcon ) const
{
   m_wrappedFactory->getDesc( type, outDesc, outGroup, outIcon );
}

///////////////////////////////////////////////////////////////////////////////

const SerializableReflectionType* DirectoriesFactoryWrapper::getClass( unsigned int idx ) const
{
   if ( idx == 0 )
   {
      // a directory
      return NULL;
   }
   else
   {
      // a resource
      return m_wrappedFactory->getClass( idx - 1 );
   }
}

///////////////////////////////////////////////////////////////////////////////

