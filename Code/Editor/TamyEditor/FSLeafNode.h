/// @file   TamyEditor\FSLeafNode.h
/// @brief  A filesystem tree leaf node.
#pragma once

#include "FSTreeNode.h"


///////////////////////////////////////////////////////////////////////////////

class TreeWidgetDescFactory;

///////////////////////////////////////////////////////////////////////////////

/**
 * A filesystem tree leaf node.
 */
class FSLeafNode : public FSTreeNode
{
protected:
   std::string          m_fsNodeName;

public:
   /**
    * Constructor.
    *
    * @param parent
    * @param nodeName
    * @param itemsFactory
    */
   FSLeafNode( FSTreeNode* parent,  const std::string& nodeName, TreeWidgetDescFactory& itemsFactory );

   // -------------------------------------------------------------------------
   // FSTreeNode implementation
   // -------------------------------------------------------------------------
   bool isContainerNode() const {
      return false;
   }
   bool compareNodeName( const std::string& name ) const;
   FilePath getRelativePath() const;
   TreeWidgetDescFactory* getDescFactory( FilesystemTree& resourcesFactory );
   void addNode( unsigned int typeIdx, FilesystemTree& resourcesFactory );
   void insertNodes( int insertionIndex, const Array< FilePath >& paths );
   void openItem( FilesystemTree& hostTree );

private:
   void setEntryIcon( Filesystem& fs, TreeWidgetDescFactory& itemsFactory );
   void setEntryName( Filesystem& fs );
   void setEntrySize( Filesystem& fs );
};

///////////////////////////////////////////////////////////////////////////////
