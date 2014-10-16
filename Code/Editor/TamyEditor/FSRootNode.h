#pragma 

/// @file   TamyEditor\FSRootNode.h
/// @brief  A node describing the root of the file system.

#include "FSTreeNode.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * A node describing the root of the file system.
 */
class FSRootNode : public FSTreeNode
{
private:
   std::string          m_fsNodeName;

public:
   /**
    * Constructor.
    *
    * @param parent
    * @param rootDirectory
    */
   FSRootNode( QTreeWidget* parent, const char* rootDirectory );

   // -------------------------------------------------------------------------
   // FSTreeNode implementation
   // -------------------------------------------------------------------------
   bool isContainerNode() const {
      return true;
   }
   bool compareNodeName( const std::string& name ) const;
   FilePath getRelativePath() const;
   TreeWidgetDescFactory* getDescFactory( FilesystemTree& resourcesFactory );
   void addNode( unsigned int typeIdx, FilesystemTree& resourcesFactory );
   void insertNodes( int insertionIndex, const Array< FilePath >& paths );
};

///////////////////////////////////////////////////////////////////////////////
