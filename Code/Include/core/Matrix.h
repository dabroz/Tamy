/// @file   core\Matrix.h
/// @brief  matrix representation
#ifndef _MATRIX_H
#define _MATRIX_H

#include <iostream>
#include "core/EngineDefines.h"
#include "core/MemoryRouter.h"
#include "core/types.h"
#include "core/Vector.h"


///////////////////////////////////////////////////////////////////////////////

struct Vector;
struct EulerAngles;
struct Plane;
struct Quaternion;
class OutStream;
class InStream;

///////////////////////////////////////////////////////////////////////////////

/**
 * Mathematical matrix - expressed in left-handed Cartesian coordinate system.
 */
ALIGN_16 struct Matrix
{
   DECLARE_ALLOCATOR( Matrix, AM_ALIGNED_16 );
   
   QuadStorage    m_rows[4];
   static Matrix  IDENTITY;

   /**
    * Default constructor.
    */
   inline Matrix();

   /**
   * DCreates an array for 16 floats.
   */
   inline explicit Matrix( float _11, float _12, float _13, float _14,
                           float _21, float _22, float _23, float _24,
                           float _31, float _32, float _33, float _34,
                           float _41, float _42, float _43, float _44 );

   /**
    * Sets an identity matrix.
    */
   inline void setIdentity();

   /**
    * Constructor.
    *
    * @param values  an array of 16 values ( 16-bytes aligned ) in Row-Major format
    */
   inline void set( const float* values );

   /**
    * Constructor.
    *
    * @param 16 float values in Row-Major format
    */
   inline void set(  float _11, float _12, float _13, float _14, 
                     float _21, float _22, float _23, float _24, 
                     float _31, float _32, float _33, float _34,
                     float _41, float _42, float _43, float _44 );

   /**
    * Composes the matrix from the specified row vectors
    *
    * @param sideVec
    * @param upVec
    * @param forwardVec
    * @param pos
    */
   inline void setRows( const Vector& sideVec, const Vector& upVec, const Vector& forwardVec, const Vector& pos = Quad_0001 );

   /**
    * Sets the specified column.
    *
    * @param colIdx        column index
    * @param vec           column vector
    */
   inline void setColumn( int colIdx, const Vector& vec );

   /**
    * Sets a matrix based on the specified rotation around the specified axis and a position.
    *
    * @param axis
    * @param angle         in radians
    * @param pos
    */
   void setAxisAnglePos( const Vector& axis, const FastFloat& angle, const Vector& pos );

   // -------------------------------------------------------------------------
   // Operators
   // -------------------------------------------------------------------------
   inline void operator=( const Matrix& rhs );
   inline bool operator==( const Matrix& rhs ) const;
   inline bool operator!=( const Matrix& rhs ) const;

   /**
    * An operator that gives access to individual matrix values.
    *
    * @param row
    * @param col
    */
   inline float operator()( int row, int col ) const;
   inline float& operator()( int row, int col );

   // -------------------------------------------------------------------------
   // Operations
   // -------------------------------------------------------------------------

   /**
    * Creates a translation matrix with translation equal to the specified vector.
    *
    * @param translationVec
    */
   void setTranslation( const Vector& translationVec );

   /**
    * Creates a rotation matrix with rotation equal to the specified quaternion.
    *
    * @param rotationQuat
    */
   void setRotation( const Quaternion& rotationQuat );

   /**
    * Creates a rotation matrix with rotation corresponding to the specified Euler angle value
    *
    * @param angles
    */
   void setRotation( const EulerAngles& angles );

   /**
    * Returns the rotation this matrix describes in the form of a quaternion.
    *
    * @param outRotationQuat
    */
   void getRotation( Quaternion& outRotationQuat ) const;

   /**
    * Returns the rotation this matrix describes in the form of Euler angles.
    *
    * @param outAngles
    */
   void getRotation( EulerAngles& outAngles ) const;

   /**
    * this = a * b
    *
    * @param a
    * @param b
    */
   inline void setMul( const Matrix& a, const Matrix& b );

   /**
    * this = this * a
    *
    * CAUTION - it allocates temporary data on stack, thus it's slower than setMul.
    *
    * @param a
    */
   void mul( const Matrix& a );

   /**
    * this = a * this
    *
    * CAUTION - it allocates temporary data on stack, thus it's slower than setMul.
    *
    * @param a
    */
   void preMul( const Matrix& a );

   /**
    * this = a * t
    *
    * @param a
    * @param t
    */
   inline void setMul( const Matrix& a, const FastFloat& t );

   /**
    * this *= t
    *
    * @param t
    */
   inline void mul( const FastFloat& t );

   /**
    * Sets an inverse of the specified matrix.
    *
    * @param rhs
    */
   inline void setInverse( const Matrix& rhs );

   /**
    * Inverts the matrix.
    */
   void invert();

   /**
    * Sets a transposed version of the specified matrix.
    *
    * @param rhs
    */
   inline void setTransposed( const Matrix& rhs );

   /**
    * Transposes the matrix.
    */
   void transpose();

   /**
    * Sets an inverse transpose of the specified matrix.
    *
    *  this = ( rhs^-1 )^t
    *
    * @param rhs
    */
   void setInverseTranspose( const Matrix& rhs );

   /**
    * Returns the matrix side vector.
    */
   inline const Vector& sideVec() const;

   /**
    * Returns the matrix up vector.
    */
   inline const Vector& upVec() const;

   /**
    * Returns the matrix forward vector.
    */
   inline const Vector& forwardVec() const;

   /**
    * Returns the matrix position.
    */
   inline const Vector& position() const;

   /**
    * Returns the specified row vector.
    *
    * @param axisIdx    values in range <0..3> ONLY
    */
   inline const Vector& getRow( byte rowIdx ) const;

   /**
    * Sets the new value for the row that represents the side axis.
    *
    * @param vec
    */
   template< int N >
   void setSideVec( const Vector& vec );

   /**
    * Sets the new value for the row that represents the up axis.
    *
    * @param vec
    */
   template< int N >
   void setUpVec( const Vector& vec );

   /**
    * Sets the new value for the row that represents the forward axis.
    *
    * @param vec
    */
   template< int N >
   void setForwardVec( const Vector& vec );

   /**
    * Sets the new value for the row that represents the position.
    *
    * @param pos
    */
   template< int N >
   void setPosition( const Vector& pos );

   /**
    * Returns the coordinate system axes and the position vector.
    *
    * @param outSideVec
    * @param outUpVec
    * @param outForwardVec
    * @param outPos
    */
   inline void getVectors( Vector& outSideVec, Vector& outUpVec, Vector& outForwardVec, Vector& outPos ) const;

   /**
    * Returns the scale the matrix describes.
    *
    * @param outScaleVec
    */
   inline void getScale( Vector& outScaleVec ) const;

   /**
    * Applies a uniform scale to this matrix ( it doesn't override any coefficients, but is multiplied by them ).
    *
    * @param scale
    */
   inline void scaleUniform( const FastFloat& scale );

   /**
    * Applies a non-uniform scale to this matrix ( it doesn't override any coefficients, but is multiplied by them ).
    *
    * @param scaleVec
    */
   inline void scale( const Vector& scaleVec );

   /**
    * Transforms the specified vector.
    *
    * @param inVec     vector to be transformed.
    * @param outVec  transformed vector
    */
   inline void transform( const Vector& inVec, Vector& outVec ) const;

   /**
    * Transforms the specified normal vector
    *
    * @param inNorm        normal vector to be transformed.
    * @param outNorm    transformed normal
    */
   inline void transformNorm( const Vector& inNorm, Vector& outNorm ) const;

   /**
    * Transforms the specified plane.
    *
    * @param inPlane     plane to be transformed.
    * @param outPlane  transformed plane
    */
   void transform( const Plane& inPlane, Plane& outPlane ) const;

   /**
    * Transforms a vector taking its four elements into account, 
    * not homogenizing it back to 3 dimensions afterwards.
    *
    * @param rhs
    * @param outVec
    */
   inline void transform4( const Vector& rhs, Vector& outVec ) const;

   /**
    * Returns the matrix in row-major format.
    *
    * @param outRowMajorMtx
    */
   inline void getRowMajor( float* __restrict outRowMajorMtx ) const;

   /**
    * Returns the matrix in column-major format.
    *
    * @param outColumnMajorMtx
    */
   inline void getColumnMajor( float* __restrict outColumnMajorMtx ) const;

   /**
    * Returns the matrix in a right-handed format ( OpenGL compatible )
    *
    * @param outRightHandedMtx
    */
   inline void getRightHanded( float* __restrict outRowMajorMtx ) const;

   /**
    * This method writes the contents of the orientation to the specified 
    * output stream.
    *
    * @param stream     output stream
    * @param rhs        orientation description of which we want to output
    */
   friend std::ostream& operator<<(std::ostream& stream, const Matrix& rhs);

   // -------------------------------------------------------------------------
   // Serialization support
   // -------------------------------------------------------------------------
   friend OutStream& operator<<( OutStream& serializer, const Matrix& rhs );
   friend InStream& operator>>( InStream& serializer, Matrix& rhs );

};

///////////////////////////////////////////////////////////////////////////////

#ifdef _USE_SIMD
   #include "core/MatrixSimd.inl"
#else
   #include "core/MatrixFpu.inl"
#endif

///////////////////////////////////////////////////////////////////////////////

#endif // _MATRIX_H
