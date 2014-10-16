/// @file   core-Renderer\MaterialCodeNode.h
/// @brief  a single code node
#ifndef _MATERIAL_CODE_NODE_H
#define _MATERIAL_CODE_NODE_H

#include "core\MemoryRouter.h"
#include "core\List.h"
#include "core\GraphBuilderSockets.h"
#include <string>


///////////////////////////////////////////////////////////////////////////////

class MaterialFunction;
class MaterialShaderBuilder;
class MaterialNode;
typedef GBNodeOutput< MaterialNode > MNOutput;

///////////////////////////////////////////////////////////////////////////////

class MaterialCodeNode
{
   DECLARE_ALLOCATOR( MaterialCodeNode, AM_DEFAULT );

public:
   virtual ~MaterialCodeNode() {}

   /**
    * Compiles the node to a code that will be appended to the specified string.
    *
    * @param language
    * @param outCode
    */
   virtual void compile( std::string& outCode ) const = 0;
};

///////////////////////////////////////////////////////////////////////////////

class MaterialCodeLine : public MaterialCodeNode
{
   DECLARE_ALLOCATOR( MaterialCodeLine, AM_DEFAULT );

private:
   std::string       m_codeLine;

public:
   /**
    * Constructor.
    *
    * @param codeLine
    */
   MaterialCodeLine( const char* codeLine = "" );

   /**
    * Creates a custom code line for HLSL.
    *
    * @param code
    */
   void set( const char* code, ... );

   // -------------------------------------------------------------------------
   // MaterialCodeNode implementation
   // -------------------------------------------------------------------------
   void compile( std::string& outCode ) const;
};

///////////////////////////////////////////////////////////////////////////////

class MaterialFunctionCall : public MaterialCodeNode
{
   DECLARE_ALLOCATOR( MaterialFunctionCall, AM_DEFAULT );

private:
   MaterialFunction*                                  m_function;
   MaterialShaderBuilder&                             m_builder;

   List< GBNodeInput< MaterialNode >* >               m_params;
   GBNodeOutput< MaterialNode >*                      m_output;

public:
   /**
   * Constructor.
   *
   * @param function
   * @param builder
   */
   MaterialFunctionCall( MaterialFunction* function, MaterialShaderBuilder& builder );

   /**
    * Binds an input socket to a function param.
    * The order of binding should match the order of function param definitions.
    *
    * @param input
    */
   void bindInput( GBNodeInput< MaterialNode >* input );

   /**
   * Binds an output socket to function's return value.
   *
   * @param output
   */
   void bindOutput( GBNodeOutput< MaterialNode >* output );

   // -------------------------------------------------------------------------
   // MaterialCodeNode implementation
   // -------------------------------------------------------------------------
   void compile( std::string& outCode ) const;

private:
   static void addLine( std::string& outCode, const char* formatStr, ... );
   void compileHLSL( std::string& outCode ) const;
   void compileGLSL( std::string& outCode ) const;
};

///////////////////////////////////////////////////////////////////////////////

template< typename T >
class MaterialConstant : public MaterialCodeNode
{
   DECLARE_ALLOCATOR( MaterialConstant, AM_DEFAULT );

private:
   std::string             m_typeId;
   std::string             m_varName;

   T                       m_value;

public:
   /**
   * Constructor.
   *
   * @param output
   * @param varName
   * @param value
   */
   MaterialConstant( const MNOutput* output, const char* varName, const T& value );

   // -------------------------------------------------------------------------
   // MaterialCodeNode implementation
   // -------------------------------------------------------------------------
   void compile( std::string& outCode ) const;
};

///////////////////////////////////////////////////////////////////////////////

#include "core-Renderer\MaterialCodeNode.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _MATERIAL_CODE_NODE_H
