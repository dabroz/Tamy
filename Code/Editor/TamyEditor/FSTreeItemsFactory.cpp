#include "FSTreeItemsFactory.h"
#include "core/Resource.h"
#include <QtGui\QIcon>


///////////////////////////////////////////////////////////////////////////////

FSTreeItemsFactory::FSTreeItemsFactory()
{
   m_resourcesFactory = new TypeDescFactory< Resource >( "unknownResourceIcon.png" );
}

///////////////////////////////////////////////////////////////////////////////

FSTreeItemsFactory::~FSTreeItemsFactory()
{
   delete m_resourcesFactory;
   m_resourcesFactory = NULL;
}

///////////////////////////////////////////////////////////////////////////////

unsigned int FSTreeItemsFactory::typesCount() const
{
   return m_resourcesFactory->typesCount() + 1;
}

///////////////////////////////////////////////////////////////////////////////

void FSTreeItemsFactory::getDesc( unsigned int idx, QString& outDesc, QString& outGroup, QIcon& outIcon ) const
{
   if ( idx < 1 )
   {
      switch( idx )
      {
         case FS_ITEM_DIR:
         {
            outDesc = "Directory";
            outGroup = "";
            outIcon = QIcon( ":/TamyEditor/Resources/Icons/Editor/dirIcon.png" );

            return;
         }
      }
   }
   else
   {
      idx -= 1;
      m_resourcesFactory->getDesc( idx, outDesc, outGroup, outIcon );
   }
}

///////////////////////////////////////////////////////////////////////////////

void FSTreeItemsFactory::getDesc( const SerializableReflectionType& type, QString& outDesc, QString& outGroup, QIcon& outIcon ) const
{
   m_resourcesFactory->getDesc( type, outDesc, outGroup, outIcon );
}

///////////////////////////////////////////////////////////////////////////////

const SerializableReflectionType* FSTreeItemsFactory::getClass( unsigned int idx ) const
{
   if ( idx < 1 )
   {
      return NULL;
   }
   else
   {
      return m_resourcesFactory->getClass( idx - 1 );
   }
}

///////////////////////////////////////////////////////////////////////////////

