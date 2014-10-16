/// @file   core-Renderer/Face.h
/// @brief  triangle face definition
#pragma once

#include <windows.h>
#include <iostream>
#include "core\InStream.h"
#include "core\OutStream.h"


///////////////////////////////////////////////////////////////////////////////

struct Face
{
   DECLARE_ALLOCATOR( Face, AM_DEFAULT );

   USHORT idx[3];

   Face()
   {
      this->idx[0] = this->idx[1] = this->idx[2] = -1;
   }

   Face(USHORT i0, USHORT i1, USHORT i2)
   {
      this->idx[0] = i0; this->idx[1] = i1; this->idx[2] = i2;
   }

   friend OutStream& operator<<( OutStream& stream, const Face& face )
   {
      stream << face.idx[0];
      stream << face.idx[1];
      stream << face.idx[2];
      return stream;
   }

   friend InStream& operator>>( InStream& stream, Face& face )
   {
      stream >> face.idx[0];
      stream >> face.idx[1];
      stream >> face.idx[2];
      return stream;
   }
};

///////////////////////////////////////////////////////////////////////////////
