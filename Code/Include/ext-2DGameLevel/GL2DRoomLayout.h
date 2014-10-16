/// @file   ext-2DGameLevel\GL2DRoomLayout.h
/// @brief  a layout of rooms in a single section
#pragma once 

#include "core\MemoryRouter.h"
#include "core\Array.h"


///////////////////////////////////////////////////////////////////////////////

class GL2DRoom;
class GL2DCorridor;
class GL2DRoomTheme;
struct Point;
class GL2DBounds;
class GL2DLevelGenerator;

///////////////////////////////////////////////////////////////////////////////

enum GL2DRoomSize
{
   GL2D_Room_Small,        //  1 exit
   GL2D_Room_Medium,       //  2 exits
   GL2D_Room_Large,        //  3 exits

   GL2D_Room_Sizes_Count
};

///////////////////////////////////////////////////////////////////////////////

class GL2DRoomLayout
{
   DECLARE_ALLOCATOR( GL2DRoomLayout, AM_DEFAULT );

public:
   virtual ~GL2DRoomLayout() {}

   /**
    * Collects rooms and corridors for generation.
    *
    * @param outRooms
    */
   virtual void build( GL2DLevelGenerator& generator ) = 0;

   /**
    * Translates all rooms the layout contains.
    *
    * @param offset
    */
   virtual void translate( const Point& offset ) = 0;

   /**
    * Returns the bounds of the layout.
    *
    * @param outBounds
    */
   virtual void getBounds( GL2DBounds& outBounds ) const = 0;
};

///////////////////////////////////////////////////////////////////////////////

class GL2DRoomLayoutLeaf : public GL2DRoomLayout
{
   DECLARE_ALLOCATOR( GL2DRoomLayoutLeaf, AM_DEFAULT );

private:
   Array< GL2DRoom* >         m_rooms;
   GL2DCorridor*              m_corridor;

public:
   /**
   * Constructor.
   *
   * @param theme
   * @param sizeHint
   */
   GL2DRoomLayoutLeaf( GL2DRoomTheme* theme, GL2DRoomSize sizeHint );
   ~GL2DRoomLayoutLeaf();

   /**
   * Returns a room from the layout.
   */
   GL2DRoom* getRoom( uint idx ) const;

   // -------------------------------------------------------------------------
   // GL2DRoomLayout implementation
   // -------------------------------------------------------------------------
   void build( GL2DLevelGenerator& generator );
   void translate( const Point& offset );
   void getBounds( GL2DBounds& outBounds ) const;
};

///////////////////////////////////////////////////////////////////////////////


class GL2DRoomLayoutComposite : public GL2DRoomLayout
{
   DECLARE_ALLOCATOR( GL2DRoomLayoutComposite, AM_DEFAULT );

private:
   Array< GL2DRoomLayout* >         m_layout;
   GL2DCorridor*                    m_corridor;

public:
   /**
    * Constructor.
    *
    * @param theme
    */
   GL2DRoomLayoutComposite( GL2DRoomTheme* theme );
   ~GL2DRoomLayoutComposite();

   /**
    * Adds a child layout.
    *
    * @param layout
    */
   void addLayout( GL2DRoomLayout* layout );

   // -------------------------------------------------------------------------
   // GL2DRoomLayout implementation
   // -------------------------------------------------------------------------
   void build( GL2DLevelGenerator& generator );
   void translate( const Point& offset );
   void getBounds( GL2DBounds& outBounds ) const;
};

///////////////////////////////////////////////////////////////////////////////
