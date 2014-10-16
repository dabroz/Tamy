/// @file   core-MVC\Component.h
/// @brief  an entity component
#pragma once

#include "core-MVC\SceneNode.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * An entity can contain multiple components that define its various aspects.
 */
class Component : public SceneNode
{
   DECLARE_ALLOCATOR( Component, AM_DEFAULT );
   DECLARE_CLASS();

public:
   /**
    * Constructor.
    *
    * @param name
    */
   Component( const char* name = "" );

   /**
    * Copy constructor.
    */
   Component( const Component& rhs );
   virtual ~Component();

   // -------------------------------------------------------------------------
   // SceneNode implementation
   // -------------------------------------------------------------------------
   const Matrix& getGlobalMtx() const;

};

///////////////////////////////////////////////////////////////////////////////
