/// @file   TamyEditor/ResourceDropArea.h
/// @brief  Drop area for resources
#pragma once

#include <QtWidgets\QFrame>
#include <string>
#include "core\FilePath.h"
#include "core\Array.h"


///////////////////////////////////////////////////////////////////////////////

class DropArea;

///////////////////////////////////////////////////////////////////////////////

/**
 * Drop area for resources.
 */
class ResourceDropArea : public QFrame
{
   Q_OBJECT

private:
   Array< FilePath >                m_paths;
   DropArea*                        m_resourceName;

   FilePath                         m_path;

public:
   /**
    * Constructor.
    *
    * @param parentWidget
    * @param initialPath
    */
   ResourceDropArea( QWidget* parentWidget, const FilePath& initialPath = FilePath() );

   /**
    * Returns the file path.
    */
   inline const FilePath& getFilePath() const { return m_path; }

   /**
    * Sets a new file path. No signals will be emitted in response.
    *
    * @param path
    */
   void setFilePath( const FilePath& path );

signals:
   void pathChanged( const FilePath& newPath );

public slots:
   void valChanged();
   void valErased( bool );

private:
   void refreshPropertyName();
};

///////////////////////////////////////////////////////////////////////////////
