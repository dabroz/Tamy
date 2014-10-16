/// @file   core-Renderer/MaterialSockets.h
/// @brief  material graph sockets
#pragma once

#include "core\GraphBuilderSockets.h"
#include "core\Matrix.h"
#include "core\Vector.h"
#include "core\Color.h"
#include "core-Renderer\MaterialDataProxies.h"


///////////////////////////////////////////////////////////////////////////////

class MaterialNode;
class ShaderTexture;
typedef GBNodeOutput< MaterialNode > MSOutput;

///////////////////////////////////////////////////////////////////////////////

/**
 * We need 2 fake structures to fill in for the non-existing Vec2 & Vec3 types.
 */
struct MSVec2Type
{
};

struct MSVec3Type
{
};

///////////////////////////////////////////////////////////////////////////////

class MSFloatInput : public TGBNodeInput< MaterialNode, float >
{
   DECLARE_ALLOCATOR( MSFloatInput, AM_ALIGNED_16 );
   DECLARE_CLASS()

public:
   /**
    * Constructor.
    */
   MSFloatInput( const std::string& name = "" ) : TGBNodeInput< MaterialNode, float >( name ) {}

   GBNodeOutput< MaterialNode >* createOutput( const char* name ) const;
};

class MSFloatOutput : public TGBNodeOutput< MaterialNode, float >
{
   DECLARE_ALLOCATOR( MSFloatOutput, AM_ALIGNED_16 );
   DECLARE_CLASS()

public:
   /**
    * Constructor.
    */
   MSFloatOutput( const std::string& name = "" ) : TGBNodeOutput< MaterialNode, float >( name ) {}

   GBNodeInput< MaterialNode >* createInput( const char* name ) const;
};

///////////////////////////////////////////////////////////////////////////////

class MSMatrixInput : public TGBNodeInput< MaterialNode, Matrix >
{
   DECLARE_ALLOCATOR( MSMatrixInput, AM_ALIGNED_16 );
   DECLARE_CLASS()

public:
   /**
    * Constructor.
    */
   MSMatrixInput( const std::string& name = "" ) : TGBNodeInput< MaterialNode, Matrix >( name ) {}

   GBNodeOutput< MaterialNode >* createOutput( const char* name ) const;
};

class MSMatrixOutput : public TGBNodeOutput< MaterialNode, Matrix >
{
   DECLARE_ALLOCATOR( MSMatrixOutput, AM_ALIGNED_16 );
   DECLARE_CLASS()

public:
   /**
    * Constructor.
    */
   MSMatrixOutput( const std::string& name = "" ) : TGBNodeOutput< MaterialNode, Matrix >( name ) {}

   GBNodeInput< MaterialNode >* createInput( const char* name ) const;
};

///////////////////////////////////////////////////////////////////////////////

class MSMatrixArrInput : public TGBNodePtrInput< MaterialNode, MatrixArray >
{
   DECLARE_ALLOCATOR( MSMatrixArrInput, AM_ALIGNED_16 );
   DECLARE_CLASS()

public:
   /**
   * Constructor.
   */
   MSMatrixArrInput( const std::string& name = "" ) : TGBNodePtrInput< MaterialNode, MatrixArray >( name ) {}

   GBNodeOutput< MaterialNode >* createOutput( const char* name ) const;
};

class MSMatrixArrOutput : public TGBNodePtrOutput< MaterialNode, MatrixArray >
{
   DECLARE_ALLOCATOR( MSMatrixArrOutput, AM_ALIGNED_16 );
   DECLARE_CLASS()

public:
   /**
   * Constructor.
   */
   MSMatrixArrOutput( const std::string& name = "" ) : TGBNodePtrOutput< MaterialNode, MatrixArray >( name ) {}

   GBNodeInput< MaterialNode >* createInput( const char* name ) const;
};

///////////////////////////////////////////////////////////////////////////////

class MSColorInput : public TGBNodeInput< MaterialNode, Color >
{
   DECLARE_ALLOCATOR( MSColorInput, AM_ALIGNED_16 );
   DECLARE_CLASS()

public:
   /**
    * Constructor.
    */
   MSColorInput( const std::string& name = "" ) : TGBNodeInput< MaterialNode, Color >( name ) {}

   GBNodeOutput< MaterialNode >* createOutput( const char* name ) const;
};

class MSColorOutput : public TGBNodeOutput< MaterialNode, Color >
{
   DECLARE_ALLOCATOR( MSColorOutput, AM_ALIGNED_16 );
   DECLARE_CLASS()

public:
   /**
    * Constructor.
    */
   MSColorOutput( const std::string& name = "" ) : TGBNodeOutput< MaterialNode, Color >( name ) {}

   GBNodeInput< MaterialNode >* createInput( const char* name ) const;
};

///////////////////////////////////////////////////////////////////////////////

class MSVec2Input : public TGBNodeInput< MaterialNode, MSVec2Type >
{
   DECLARE_ALLOCATOR( MSVec2Input, AM_ALIGNED_16 );
   DECLARE_CLASS()

public:
   /**
   * Constructor.
   */
   MSVec2Input( const std::string& name = "" ) : TGBNodeInput< MaterialNode, MSVec2Type >( name ) {}

   GBNodeOutput< MaterialNode >* createOutput( const char* name ) const;
};

class MSVec2Output : public TGBNodeOutput< MaterialNode, MSVec2Type >
{
   DECLARE_ALLOCATOR( MSVec2Output, AM_ALIGNED_16 );
   DECLARE_CLASS()

public:
   /**
   * Constructor.
   */
   MSVec2Output( const std::string& name = "" ) : TGBNodeOutput< MaterialNode, MSVec2Type >( name ) {}

   GBNodeInput< MaterialNode >* createInput( const char* name ) const;
};

///////////////////////////////////////////////////////////////////////////////

class MSVec3Input : public TGBNodeInput< MaterialNode, MSVec3Type >
{
   DECLARE_ALLOCATOR( MSVec3Input, AM_ALIGNED_16 );
   DECLARE_CLASS()

public:
   /**
   * Constructor.
   */
   MSVec3Input( const std::string& name = "" ) : TGBNodeInput< MaterialNode, MSVec3Type >( name ) {}

   GBNodeOutput< MaterialNode >* createOutput( const char* name ) const;
};

class MSVec3Output : public TGBNodeOutput< MaterialNode, MSVec3Type >
{
   DECLARE_ALLOCATOR( MSVec3Output, AM_ALIGNED_16 );
   DECLARE_CLASS()

public:
   /**
   * Constructor.
   */
   MSVec3Output( const std::string& name = "" ) : TGBNodeOutput< MaterialNode, MSVec3Type >( name ) {}

   GBNodeInput< MaterialNode >* createInput( const char* name ) const;
};

///////////////////////////////////////////////////////////////////////////////

class MSVec4Input : public TGBNodeInput< MaterialNode, Vector >
{
   DECLARE_ALLOCATOR( MSVec4Input, AM_ALIGNED_16 );
   DECLARE_CLASS()

public:
   /**
    * Constructor.
    */
   MSVec4Input( const std::string& name = "" ) : TGBNodeInput< MaterialNode, Vector >( name ) {}

   GBNodeOutput< MaterialNode >* createOutput( const char* name ) const;
};

class MSVec4Output : public TGBNodeOutput< MaterialNode, Vector >
{
   DECLARE_ALLOCATOR( MSVec4Output, AM_ALIGNED_16 );
   DECLARE_CLASS()

public:
   /**
    * Constructor.
    */
   MSVec4Output( const std::string& name = "" ) : TGBNodeOutput< MaterialNode, Vector >( name ) {}

   GBNodeInput< MaterialNode >* createInput( const char* name ) const;
};

///////////////////////////////////////////////////////////////////////////////

class MSBoolInput : public TGBNodeInput< MaterialNode, bool >
{
   DECLARE_ALLOCATOR( MSBoolInput, AM_ALIGNED_16 );
   DECLARE_CLASS()

public:
   /**
    * Constructor.
    */
   MSBoolInput( const std::string& name = "" ) : TGBNodeInput< MaterialNode, bool >( name ) {}

   GBNodeOutput< MaterialNode >* createOutput( const char* name ) const;
};

class MSBoolOutput : public TGBNodeOutput< MaterialNode, bool >
{
   DECLARE_ALLOCATOR( MSBoolOutput, AM_ALIGNED_16 );
   DECLARE_CLASS()

public:
   /**
    * Constructor.
    */
   MSBoolOutput( const std::string& name = "" ) : TGBNodeOutput< MaterialNode, bool >( name ) {}

   GBNodeInput< MaterialNode >* createInput( const char* name ) const;
};

///////////////////////////////////////////////////////////////////////////////

class MSIntInput : public TGBNodeInput< MaterialNode, int >
{
   DECLARE_ALLOCATOR( MSIntInput, AM_ALIGNED_16 );
   DECLARE_CLASS()

public:
   /**
    * Constructor.
    */
   MSIntInput( const std::string& name = "" ) : TGBNodeInput< MaterialNode, int >( name ) {}

   GBNodeOutput< MaterialNode >* createOutput( const char* name ) const;
};

class MSIntOutput : public TGBNodeOutput< MaterialNode, int >
{
   DECLARE_ALLOCATOR( MSIntOutput, AM_ALIGNED_16 );
   DECLARE_CLASS()

public:
   /**
    * Constructor.
    */
   MSIntOutput( const std::string& name = "" ) : TGBNodeOutput< MaterialNode, int >( name ) {}

   GBNodeInput< MaterialNode >* createInput( const char* name ) const;
};

///////////////////////////////////////////////////////////////////////////////
