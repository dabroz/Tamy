/// @file   core-Renderer/MaterialFunction.h
/// @brief  a material shader function definition
#ifndef _MATERIAL_FUNCTION_H
#define _MATERIAL_FUNCTION_H

#include "core\MemoryRouter.h"
#include "core\Array.h"
#include "core\List.h"
#include <string>
#include "core\GraphBuilderSockets.h"


///////////////////////////////////////////////////////////////////////////////

class MaterialDataStruct;
class MaterialShaderBuilder;
class MaterialNode;
typedef GBNodeInput< MaterialNode > MNInput;
typedef GBNodeOutput< MaterialNode > MNOutput;
class MaterialCodeLine;
class MaterialCodeNode;

///////////////////////////////////////////////////////////////////////////////

/**
 * A material shader function definition.
 */
class MaterialFunction
{
   DECLARE_ALLOCATOR( MaterialFunction, AM_DEFAULT );

protected:
   MaterialShaderBuilder&        m_shaderBuilder;

   std::string                   m_functionName;

   MaterialDataStruct*           m_inputStruct;
   std::string                   m_outputType;

public:
   /**
    * Constructor.
    *
    * @param shaderBuilder
    * @param functionName
    * @param inputStruct
    */
   MaterialFunction( MaterialShaderBuilder& shaderBuilder );
   ~MaterialFunction();

   /**
    * Assigns the function a new input structure.
    *
    * @param inputStruct
    */
   inline void setInputStruct( MaterialDataStruct* inputStruct ) {
      m_inputStruct = inputStruct;
   }

   /**
    * Returns the name of the function.
    */
   inline const char* getName() const {
      return m_functionName.c_str();
   }

   /**
    * Returns the function's input structure.
    */
   const MaterialDataStruct& getInputStruct() const {
      return *m_inputStruct;
   };

   /**
    * Compiles function code.
    *
    * @param language
    * @param outCode
    */
   virtual void compile( std::string& outCode ) = 0;
};

///////////////////////////////////////////////////////////////////////////////

class MatFuncPrecompiled : public MaterialFunction
{
   DECLARE_ALLOCATOR( MatFuncPrecompiled, AM_DEFAULT );

private:
   std::string m_code;
   
public:
   /**
    * Constructor.
    *
    * @param shaderBuilder
    * @param descFileContents
     */
   MatFuncPrecompiled( MaterialShaderBuilder& shaderBuilder, const std::string& descFileContents );

   // -------------------------------------------------------------------------
   // MaterialFunction implementation
   // -------------------------------------------------------------------------
   void compile( std::string& outCode ) override;
};

///////////////////////////////////////////////////////////////////////////////

class MatFuncUserDefined : public MaterialFunction
{
   DECLARE_ALLOCATOR( MatFuncUserDefined, AM_DEFAULT );

private:
   List< MaterialCodeNode* >     m_code;

public:
   /**
    * Constructor.
    *
    * @param shaderBuilder
    */
   MatFuncUserDefined( MaterialShaderBuilder& shaderBuilder );
   ~MatFuncUserDefined();

   /**
    * Assigns the function a new name.
    */
   inline void setFunctionName( const std::string& name ) {
      m_functionName = name;
   }

   /**
    * Adds a function parameter and binds the specified input to it.
    *
    * @param input
    * @param outVarName    a name that can be used to access the parameter
    */
   void addInputParam( MNInput* input, std::string& outVarName );

   /**
    * Adds an output parameter.
    *
    * @param output
    */
   void setReturnType( MNOutput* output );

   /**
    * Adds a line of code.
    *
    * @param code
    * @param var args
    */
   void addCodeLine( const char* code, ... );

   /**
    * Adds a custom line of code. Call this method if the HLSL version differs from the GLSL version.
    */
   MaterialCodeLine* addCodeLine();

   // -------------------------------------------------------------------------
   // MaterialFunction implementation
   // -------------------------------------------------------------------------
   void compile( std::string& outCode ) override;
};

///////////////////////////////////////////////////////////////////////////////

#include "core-Renderer\MaterialFunction.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _MATERIAL_FUNCTION_H
