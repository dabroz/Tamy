/// @file   TamyEditor\MimeDataEncoder.h
/// @brief  Common interface describing a mime data encoder used by the DropArea widget
#pragma once


///////////////////////////////////////////////////////////////////////////////

class QMimeData;

///////////////////////////////////////////////////////////////////////////////

class MimeDataEncoder
{
public:
   virtual ~MimeDataEncoder() {}

   /**
   * Restores the specified paths from the underlying mime data stream.
    *
    * @param data
    */
   virtual void load( const QMimeData& data ) = 0;

   /**
    * Checks if the contents of the dragged/dropped data can be accepted.
    *
    * @param data
    */
   virtual bool canAccept( const QMimeData& data ) const = 0;
};

///////////////////////////////////////////////////////////////////////////////
