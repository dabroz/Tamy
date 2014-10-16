/// @file   TamyEditor\EntitiesClipboard.h
/// @brief  a clipboard for copy-pasting entities
#pragma once

#include "core\MemoryRouter.h"
#include "core\List.h"
#include "core\Vector.h"


///////////////////////////////////////////////////////////////////////////////

class Entity;
class SceneNode;

///////////////////////////////////////////////////////////////////////////////

class EntitiesClipboard
{
   DECLARE_ALLOCATOR( EntitiesClipboard, AM_DEFAULT );

private:
   List< Entity* >                           m_clipboard;

public:
   ~EntitiesClipboard();

   /**
   * This method copies all currently selected entities.
   *
   * @param rootEntity
   */
   void copySelectedEntities( const List< SceneNode* >& selectedNodes, Entity* rootEntity );

   /**
    * This method pastes copied entities.
    *
    * @param instantiationPos
    * @param rootEntity
    */
   void pasteSelectedEntities( const Vector& instantiationPos, Entity* rootEntity );
};

///////////////////////////////////////////////////////////////////////////////
