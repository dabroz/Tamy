/// @file   ext-2DGameLevel\GL2DProceduralLevel.h
/// @brief  a utility component that allows us to generate level geometry from the scene tree
#pragma once

#include "core-MVC\Component.h"
#include "core\FilePath.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * A utility component that allows us to generate level geometry from the scene tree.
 */
class GL2DProceduralLevel : public Component
{
   DECLARE_ALLOCATOR( GL2DProceduralLevel, AM_DEFAULT );
   DECLARE_CLASS();

public:
   // static data
   FilePath             m_geometryDir;
   FilePath             m_backgroundPrefabsDir;
   float                m_levelLenght;

public:
   /**
    * Default constructor.
    *
    * @param name
    */
   GL2DProceduralLevel( const char* name = "GL2DProceduralLevel" );

   /**
    * Copy constructor
    *
    * @param rhs
    */
   GL2DProceduralLevel( const GL2DProceduralLevel& rhs );

};

///////////////////////////////////////////////////////////////////////////////
