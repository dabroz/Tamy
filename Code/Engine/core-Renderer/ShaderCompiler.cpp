#include "core-Renderer\ShaderCompiler.h"
#include "core-Renderer\Shader.h"
#include "core\StringParser.h"


///////////////////////////////////////////////////////////////////////////////

bool ShaderCompiler::parseConstants( const Shader& shader, Array< ShaderConstantDesc >& outConstants )
{
   // reset error messages
   m_errorMsg = "";

   // parse the technique markers located in the script
   std::string shaderCode;
   for ( uint routine = 0; routine < LAST_SHADER_ROUTINE; ++routine )
   {
      shader.getScriptPreprocessed( ( ShaderRoutine ) routine, shaderCode );
      if ( shaderCode.empty() )
      {
         continue;
      }

      StringParser parser( &shaderCode );
      parser.addDelimiter( ";" );

      std::string varTypeStr, varName;
      while ( parser.findNext( "uniform" ) )
      {

         if ( !parser.getNextWord( varTypeStr ) || !parser.getNextWord( varName ) )
         {
            m_errorMsg += "Invalid constant declaration found\n";
            return false;
         }

         ShaderConstantDesc::Type varType = ShaderConstantDesc::CT_Float;

         if ( varTypeStr == "mat4" )                  varType = ShaderConstantDesc::CT_Matrix;
         else if ( varTypeStr == "vec4" )             varType = ShaderConstantDesc::CT_Vec4;
         else if ( varTypeStr == "bool" )             varType = ShaderConstantDesc::CT_Bool;
         else if ( varTypeStr == "int" )              varType = ShaderConstantDesc::CT_Int;
         else if ( varTypeStr == "float" )            varType = ShaderConstantDesc::CT_Float;
         else if ( varTypeStr == "sampler2D" )        varType = ShaderConstantDesc::CT_Texture;
         else if ( varTypeStr == "sampler2DShadow" )  varType = ShaderConstantDesc::CT_Texture;
         else if ( varTypeStr == "samplerCube" )      varType = ShaderConstantDesc::CT_Texture;
         else if ( varName == "{" )
         {
            varType = ShaderConstantDesc::CT_DataBuffer;
            varName = varTypeStr;
         }

         // check the constant for uniqueness
         bool isUnique = true;
         const uint constantsCount = outConstants.size();
         for ( uint i = 0; i < constantsCount; ++i )
         {
            if ( outConstants[i].m_name == varName )
            {
               isUnique = false;
               break;
            }
         }

         if ( isUnique )
         {
            outConstants.push_back( ShaderConstantDesc( varType, varName.c_str() ) );
         }
      }
   }

   return true;
}

///////////////////////////////////////////////////////////////////////////////
