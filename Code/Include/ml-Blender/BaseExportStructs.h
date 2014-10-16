/// @file   ml-Blender/BaseExportStructs.h
/// @brief  base export structures
#pragma once


///////////////////////////////////////////////////////////////////////////////

struct Matrix;
struct Vector;
struct Quaternion;

///////////////////////////////////////////////////////////////////////////////

struct ExportMatrix
{
   float                coords[16];

   /**
    * Stores the matrix in an engine matrix.
    */
   void store( Matrix& mtx ) const;
};

///////////////////////////////////////////////////////////////////////////////

struct ExportVector
{
   float                coords[4];

   /**
    * Stores the vector in an engine vector.
    */
   void store( Vector& vec ) const;

   /**
    * Stores the vector in an engine quaternion.
    */
   void store( Quaternion& quat ) const;
};

///////////////////////////////////////////////////////////////////////////////
