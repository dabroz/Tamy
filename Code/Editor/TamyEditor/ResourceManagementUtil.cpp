#include "ResourceManagementUtil.h"
#include "core.h"
#include <QtWidgets\QInputDialog>
#include "progressdialog.h"
#include <string>
#include "tamyeditor.h"


///////////////////////////////////////////////////////////////////////////////

void ResourceManagementUtil::createResource( QWidget* parentWidget, const SerializableReflectionType& type, const std::string& parentDir )
{
   Resource* newResource = type.instantiate< Resource >();

   // learn the new file's name
   ResourcesManager& resMgr = TSingleton< ResourcesManager >::getInstance();
   const Filesystem& fs = resMgr.getFilesystem();
   std::string extension = newResource->getVirtualExtension();

   bool ok = false;
   QString fullFileName = QInputDialog::getText( parentWidget, "New resource", "Resource name:", QLineEdit::Normal, "", &ok );
   if ( !ok ) 
   {
      // no file was selected or user pressed 'cancel'
      return;
   }

   // once the file is open, extract the directory name
   std::string fileName = parentDir + fullFileName.toStdString() + "." + extension;

   // create & save the resource
   newResource->setFilePath( fileName );
   resMgr.addResource( newResource );
   newResource->saveResource();
}

///////////////////////////////////////////////////////////////////////////////

void ResourceManagementUtil::editResource( const std::string& path, const QIcon& resourceIcon )
{
   ResourcesManager& resMgr = TSingleton< ResourcesManager >::getInstance();
   Resource* resource = resMgr.create( path );

   if ( resource )
   {
      TamyEditor& tamyEd = TamyEditor::getInstance();
      if ( tamyEd.activateResourceEditor( resource ) == false )
      {
         ResourceEditor* resourceEd = tamyEd.createResourceEditor( resource, resourceIcon );
         tamyEd.addResourceEditor( resourceEd );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////
