/// @file   TamyEditor\FSNodeMimeData.h
/// @brief  Filesystem node drag&drop related data

#pragma once

#include "MimeDataEncoder.h"
#include "core\Array.h"
#include "core\FilePath.h"
#include <string>


///////////////////////////////////////////////////////////////////////////////

/**
 * This class manages the data used during a during a drag&drop operation
 * to describe nodes in a file system.
 */
class FSNodeMimeData : public MimeDataEncoder
{
private:
   Array< FilePath >&   m_paths;

public:
   /**
    * Constructor.
    *
    * @param paths      paths we want to persist
    */
   FSNodeMimeData( Array< FilePath >& paths );

   // -------------------------------------------------------------------------
   // static API
   // -------------------------------------------------------------------------
   static void save( const Array< FilePath >& paths, QMimeData& data );
   static void load( Array< FilePath >& outPaths, const QMimeData& data );
   static bool containsRelevantData( const QMimeData& data );

   // -------------------------------------------------------------------------
   // MimeDataEncoder implementation
   // -------------------------------------------------------------------------
   void load( const QMimeData& data );
   bool canAccept( const QMimeData& data ) const;
};

///////////////////////////////////////////////////////////////////////////////
