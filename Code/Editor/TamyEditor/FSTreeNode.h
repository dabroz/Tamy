/// @file   TamyEditor\FSTreeNode.h
/// @brief  Resource browser;'s filesystem tree widget node
#pragma once

#include <QtWidgets\QTreeWidgetItem>
#include "core\Array.h"
#include <string>


///////////////////////////////////////////////////////////////////////////////

class FilesystemTree;
class Filesystem;
class TreeWidgetDescFactory;
class FilePath;

///////////////////////////////////////////////////////////////////////////////

/**
 * An item describing an fs node in the tree structure
 */
class FSTreeNode : public QTreeWidgetItem 
{
public:
   virtual ~FSTreeNode() {}


   /**
    * Searches for a child entry with the specified name.
    *
    * @param nodeName
    */
   FSTreeNode* find( const std::string& nodeName );

   /**
    * Opens the item in its own specific way.
    *
    * @param hostTree    host tree
    */
   virtual void openItem( FilesystemTree& hostTree ) {}

   /**
    * Returns a relative path leading to this fs node.
    */
   virtual FilePath getRelativePath() const = 0;

   /**
    * This is a call marshaling method - a node can marshal it or not, thus allowing
    * for new items creation or not.
    * 
    * @param resourcesFactory    factory capable of creating the actual resources
    */
   virtual TreeWidgetDescFactory* getDescFactory( FilesystemTree& resourcesFactory ) = 0;

   /**
    * This is a call marshaling method - a node can marshal it or not, thus allowing
    * for new items to be added or not.
    *
    * @param typeIdx
    * @param resourcesFactory    factory capable of creating the actual resources
    */
   virtual void addNode( unsigned int typeIdx, FilesystemTree& resourcesFactory ) = 0;

   /**
    * Compares the node name.
    *
    * @param name
    */
   virtual bool compareNodeName( const std::string& name ) const = 0;

   /**
    * Assigns the node a new name.
    *
    * @param newName
    */
   void setName( const QString& newName );

   /**
    * Removes a child node, deleting the corresponding entry from
    * the file system.
    *
    * @param child
    */
   void removeNode( FSTreeNode* child );

   /**
    * Removes all node's children, physically deleting the corresponding
    * entries from the file system.
    */
   void clearNodes();

   /**
    * Can this node store other nodes.
    */
   virtual bool isContainerNode() const = 0;

   /**
    * Moves the specified files to this node.
    *
    * @param insertionIndex
    * @param paths
    */
   virtual void insertNodes( int insertionIndex, const Array< FilePath >& paths ) = 0;

   /**
    * Removes all node's children, but doesn't touch their physical
    * file system versions.
    */
   void clear();

protected:
   /**
    * Constructor for common nodes.
    *
    * @param parent
    */
   FSTreeNode( FSTreeNode* parent );

   /**
    * Constructor for the root node.
    *
    * @param parent
    */
   FSTreeNode( QTreeWidget* parent );
};

///////////////////////////////////////////////////////////////////////////////
