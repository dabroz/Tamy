/// @file   TamyEditor\MaterialRoutine.h
/// @brief  a library resource that stores a single material routine graph
#pragma once

#include "core\Resource.h"
#include "core\Vector.h"
#include <vector>


///////////////////////////////////////////////////////////////////////////////

class MaterialNode;
class MaterialProfile;

///////////////////////////////////////////////////////////////////////////////

/**
 * A library resource that stores a single material routine graph.
 */
class MaterialRoutine : public Resource
{
   DECLARE_ALLOCATOR( MaterialRoutine, AM_DEFAULT );
   DECLARE_RESOURCE();

private:
   std::vector< MaterialNode* >        m_nodes;

public:
   /**
    * Constructor.
    *
    * @param filePath
    */
   MaterialRoutine( const FilePath& path = FilePath() );
   ~MaterialRoutine();

   /**
    * Removes the previous contents of the library and initializes it with the specified
    * contents.
    *
    * @param profile
    * @param routineIdx
    */
   void grabSnapshot( const MaterialProfile* profile, uint routineIdx );

   /**
    * Copies the snapshot contained in the library to the specified routine
    *
    * @param profile
    * @param routineIdx
    * @param pastingOffset
    */
   void pasteSnapshot( MaterialProfile* profile, uint routineIdx, const Vector& pastingOffset = Vector_ZERO ) const;

   // -------------------------------------------------------------------------
   // Resource implementation
   // -------------------------------------------------------------------------
   void replaceContents( Resource& rhs );
};

///////////////////////////////////////////////////////////////////////////////
