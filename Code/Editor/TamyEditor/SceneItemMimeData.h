/// @file   TamyEditor/SceneItemMimeData.h
/// @brief  scene tree item drag&drop data encoder
#pragma once

#include "MimeDataEncoder.h"


/////////////////////////////////////////////////////////////////////////////////

class SceneTreeItem;

/////////////////////////////////////////////////////////////////////////////////

/**
 * Scene drag&drop data encoder.
 */
class SceneItemMimeData : public MimeDataEncoder
{
private:
   SceneTreeItem*&        m_itemPtr;

public:
   /**
    * Constructor.
    *
    * @param chapterPtr      reference to a chapter pointer
    */
   SceneItemMimeData( SceneTreeItem*& itemPtr );

   // -------------------------------------------------------------------------
   // MimeDataEncoder implementation
   // -------------------------------------------------------------------------
   void load( const QMimeData& data );
   bool canAccept( const QMimeData& data ) const;
};

/////////////////////////////////////////////////////////////////////////////////

class SceneItemMimeDataUtil
{
public:
   /**
    * Stores the tree item ptr in a mime data stream.
    *
    * @param itemPtr
    * @param data
    */
   static void encode( const SceneTreeItem* itemPtr, QMimeData* data );

   /**
    * Deserializes a tree item ptr from the mime data stream.
    *
    * @param data
    * @return itemPtr
    */
   static SceneTreeItem* decode( const QMimeData* data );

   /**
    * Checks if the specified mime data stream carries StoryTreeItem related data.
    *
    * @param data
    */
   static bool checkDataPresence( const QMimeData* data );
};

/////////////////////////////////////////////////////////////////////////////////
