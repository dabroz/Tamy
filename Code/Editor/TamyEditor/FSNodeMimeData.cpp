#include "FSNodeMimeData.h"
#include <QtCore\QMimeData>
#include <QtCore\QDataStream>


///////////////////////////////////////////////////////////////////////////////

FSNodeMimeData::FSNodeMimeData( Array< FilePath >& paths )
: m_paths( paths )
{
}

///////////////////////////////////////////////////////////////////////////////

void FSNodeMimeData::save( const Array< FilePath >& paths, QMimeData& data )
{
   QByteArray encodedData;
   QDataStream stream( &encodedData, QIODevice::WriteOnly );

   unsigned int count = paths.size();
   for ( unsigned int i = 0; i < count; ++i )
   {
      stream << QString( paths[ i ].c_str() );
   }

   data.setData( "TamyEditor/FSNodeMimeData", encodedData );
}

///////////////////////////////////////////////////////////////////////////////

void FSNodeMimeData::load( Array< FilePath >& outPaths, const QMimeData& data )
{
   QByteArray encodedData = data.data( "TamyEditor/FSNodeMimeData" );
   QDataStream stream( &encodedData, QIODevice::ReadOnly );

   while ( !stream.atEnd() )
   {
      QString text;
      stream >> text;
      outPaths.push_back( FilePath( text.toStdString() ) );
   }
}

///////////////////////////////////////////////////////////////////////////////

bool FSNodeMimeData::containsRelevantData( const QMimeData& data )
{
   QByteArray encodedData = data.data( "TamyEditor/FSNodeMimeData" );
   QDataStream stream( &encodedData, QIODevice::ReadOnly );

   return !stream.atEnd();
}

///////////////////////////////////////////////////////////////////////////////

void FSNodeMimeData::load( const QMimeData& data )
{
   load( m_paths, data );
}

///////////////////////////////////////////////////////////////////////////////

bool FSNodeMimeData::canAccept( const QMimeData& data ) const
{
   return containsRelevantData( data );
}

///////////////////////////////////////////////////////////////////////////////
