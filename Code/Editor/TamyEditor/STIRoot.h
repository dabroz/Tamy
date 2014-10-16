/// @file   TamyEditor/STIRoot.h
/// @brief  story editor node that represents the tree root node

#include "STIChapter.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * Story editor node that represents the tree root.
 */
class STIRoot : public STIChapter
{
private:
   Story&            m_story;

public:
   /**
    * Constructor.
    *
    * @param parentTreeWidget
    * @param story
    */
   STIRoot( QTreeWidget* parentTreeWidget, Story& story );

};

///////////////////////////////////////////////////////////////////////////////
