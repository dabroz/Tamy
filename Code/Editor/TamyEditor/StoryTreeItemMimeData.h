/// @file   TamyEditor/StoryTreeItemMimeData.h
/// @brief  story browser tree item drag&drop data encoder
#pragma once

#include "MimeDataEncoder.h"


/////////////////////////////////////////////////////////////////////////////////

class StoryBrowserTreeItem;

/////////////////////////////////////////////////////////////////////////////////

/**
 * Story chapter drag&drop data encoder.
 */
class StoryTreeItemMimeData : public MimeDataEncoder
{
private:
   StoryBrowserTreeItem*&        m_itemPtr;

public:
   /**
    * Constructor.
    *
    * @param chapterPtr      reference to a chapter pointer
    */
   StoryTreeItemMimeData( StoryBrowserTreeItem*& itemPtr );

   // -------------------------------------------------------------------------
   // MimeDataEncoder implementation
   // -------------------------------------------------------------------------
   void load( const QMimeData& data );
   bool canAccept( const QMimeData& data ) const;
};

/////////////////////////////////////////////////////////////////////////////////

class StoryTreeItemMimeDataUtil
{
public:
   /**
    * Stores the tree item ptr in a mime data stream.
    *
    * @param itemPtr
    * @param data
    */
   static void encode( const StoryBrowserTreeItem* itemPtr, QMimeData* data );

   /**
    * Deserializes a tree item ptr from the mime data stream.
    *
    * @param data
    * @return itemPtr
    */
   static StoryBrowserTreeItem* decode( const QMimeData* data );

   /**
    * Checks if the specified mime data stream carries StoryTreeItem related data.
    *
    * @param data
    */
   static bool checkDataPresence( const QMimeData* data );
};

/////////////////////////////////////////////////////////////////////////////////
