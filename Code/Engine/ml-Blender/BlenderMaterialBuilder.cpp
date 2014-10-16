#include "ml-Blender\BlenderMaterialBuilder.h"
#include "core-Renderer\Material.h"
#include "core-Renderer\MaterialProfile.h"
#include "ml-Blender\TamyMaterial.h"
#include "ml-Blender\BlenderMaterialNodesLayout.h"

// material nodes
#include "core-Renderer\MNInputStruct.h"
#include "core-Renderer\MNOutputStruct.h"
#include "core-Renderer\MNColor.h"
#include "core-Renderer\MNTransform.h"
#include "core-Renderer\MNTransformVector.h"
#include "core-Renderer\MNTransformNormal.h"
#include "core-Renderer\MNColor.h"
#include "core-Renderer\MNMixRGBs.h"
#include "core-Renderer\MNTexture.h"
#include "core-Renderer\MNNormalTangentToView.h"
#include "core-Renderer\MNColorToNormal.h"
#include "ext-RenderingPipeline\MNEntityPtr.h"
#include "ext-RenderingPipeline\MNPhongBlinn.h"
#include "ext-RenderingPipeline\DeferredRenderingMechanism.h"


///////////////////////////////////////////////////////////////////////////////

const float  X_OFFSET = 300;
const float  Y_OFFSET = 150;

///////////////////////////////////////////////////////////////////////////////

BlenderMaterialBuilder::BlenderMaterialBuilder( const std::vector< Texture* >& textures )
   : m_textures( textures )
{
}

///////////////////////////////////////////////////////////////////////////////

void BlenderMaterialBuilder::buildMaterial( const TamyMaterial& exportedMaterial, Material* outMaterial )
{
   // clear all profiles
   MaterialProfile* profile = outMaterial->accessProfile();
   profile->clear();

   m_exportedMaterial = &exportedMaterial;

   // setup the profiles
   setupDefaultProfile( outMaterial->accessProfile() );
}

///////////////////////////////////////////////////////////////////////////////

void BlenderMaterialBuilder::setupDefaultProfile( MaterialProfile* outMaterialProfile ) const
{
   // vertex shader
   {
      GraphBuilderTransaction< MaterialNode > transaction( *outMaterialProfile->getGraph( SHADER_VERTEX ) );

      // define nodes
      
      MNInputStruct* vsInput = new MNInputStruct( "VSInput" );
      {
         std::vector< GBNodeOutput< MaterialNode >* > outputs;
         outputs.push_back( new MSVec4Output( "vPosition" ) );
         outputs.push_back( new MSVec3Output( "vNormal" ) );
         outputs.push_back( new MSVec3Output( "vTangent" ) );
         outputs.push_back( new MSVec2Output( "vUV0" ) );
         vsInput->defineOutputs( outputs );

         vsInput->setPosition( Vector( 0, 0, 0 ) );

         transaction.addNode( vsInput );
      }

      MNTransform* model2World = new MNTransform( "model 2 world", SPACE_Model_To_Projection );
      {
         model2World->setPosition( Vector( X_OFFSET, -Y_OFFSET, 0 ) );

         transaction.addNode( model2World );
      }

      MNTransform* model2View = new MNTransform( "model 2 view", SPACE_Model_to_View );
      {
         model2View->setPosition( Vector( X_OFFSET * 2, -Y_OFFSET, 0 ) );

         transaction.addNode( model2View );
      }

      MNTransformVector* transformPosCS = new MNTransformVector( "pos CS" );
      {
         transformPosCS->setPosition( Vector( X_OFFSET, 0, 0 ) );

         transaction.addNode( transformPosCS );
      }

      MNTransformNormal* transformNormalVS = new MNTransformNormal( "normal VS" );
      {
         transformNormalVS->setPosition( Vector( X_OFFSET * 2, Y_OFFSET, 0 ) );

         transaction.addNode( transformNormalVS );
      }

      MNTransformNormal* transformTangentVS = new MNTransformNormal( "tan VS" );
      {
         transformTangentVS->setPosition( Vector( X_OFFSET * 2, Y_OFFSET * 2, 0 ) );

         transaction.addNode( transformTangentVS );
      }

      MNOutputStruct* vsOutput = new MNOutputStruct( "VSOutput" );
      {
         std::vector< GBNodeInput< MaterialNode >* > inputs;
         inputs.push_back( new MSVec4Input( "positionCS" ) );
         inputs.push_back( new MSVec3Input( "normalVS" ) );
         inputs.push_back( new MSVec3Input( "tangentVS" ) );
         inputs.push_back( new MSVec2Input( "uv0" ) );
         vsOutput->defineInputs( inputs );

         vsOutput->setPosition( Vector( X_OFFSET * 3, 0, 0 ) );

         transaction.addNode( vsOutput );
      }

      // connect nodes
      {
         transaction.connectNodes( vsInput, "vPosition", transformPosCS, "Input" );
         transaction.connectNodes( model2World, "Transform", transformPosCS, "Transform" );
         transaction.connectNodes( transformPosCS, "Output", vsOutput, "positionCS" );

         transaction.connectNodes( vsInput, "vNormal", transformNormalVS, "Input" );
         transaction.connectNodes( model2View, "Transform", transformNormalVS, "Transform" );
         transaction.connectNodes( transformNormalVS, "Output", vsOutput, "normalVS" );

         transaction.connectNodes( vsInput, "vTangent", transformTangentVS, "Input" );
         transaction.connectNodes( model2View, "Transform", transformTangentVS, "Transform" );
         transaction.connectNodes( transformTangentVS, "Output", vsOutput, "tangentVS" );

         transaction.connectNodes( vsInput, "vUV0", vsOutput, "uv0" );
      }

      // commit results
      transaction.commit();

      // arrange the nodes
      {
         BMNHorizLayout* mainLayout = new BMNHorizLayout();
         mainLayout->add( vsInput );

         BMNVertLayout* projTransformLayout = new BMNVertLayout();
         {
            mainLayout->add( projTransformLayout );
            projTransformLayout->add( model2World );
            projTransformLayout->add( transformPosCS );
         }

         BMNVertLayout* viewTransformLayout = new BMNVertLayout();
         {
            mainLayout->add( viewTransformLayout );
            projTransformLayout->add( model2View );
            projTransformLayout->add( transformNormalVS );
            projTransformLayout->add( transformTangentVS );
         }

         mainLayout->add( vsOutput );
         mainLayout->arrangeNodes();
      }
      
   }

   // fragment shader
   {
      GraphBuilderTransaction< MaterialNode > transaction( *outMaterialProfile->getGraph( SHADER_FRAGMENT ) );

      // define nodes
      MNInputStruct* fsInput = new MNInputStruct( "FSInput" );
      {
         std::vector< GBNodeOutput< MaterialNode >* > outputs;
         outputs.push_back( new MSVec4Output( "positionVS" ) );
         outputs.push_back( new MSVec3Output( "normalVS" ) );
         outputs.push_back( new MSVec3Output( "tangentVS" ) );
         outputs.push_back( new MSVec2Output( "uv0" ) );
         fsInput->defineOutputs( outputs );

         transaction.addNode( fsInput );
      }

      MNOutputStruct* fsOutput = new MNOutputStruct( "FSOutput" );
      {
         DeferredRenderingMechanism::defineFragmentShaderOutput( fsOutput );
         transaction.addNode( fsOutput );
      }

      MNPhongBlinn* lighting = new MNPhongBlinn( "Lighting" );
      {
         transaction.addNode( lighting );
      }

      BMNCompositeItem* diffuseSectionLayout = buildDiffuseColorProvider( fsInput, fsOutput, transaction );

      BMNCompositeItem* normalSectionLayout = buildNormalMapProvider( fsInput, lighting, transaction );

      BMNCompositeItem* specularSectionLayout = buildSpecularColorProvider( fsInput, fsOutput, transaction );

      // connect nodes
      {
         transaction.connectNodes( lighting, "GBuf_BRDF", fsOutput, "gBuf_BRDF" );
         transaction.connectNodes( lighting, "GBuf_Normals", fsOutput, "gBuf_Normals" );
      }

      // commit results
      transaction.commit();

      // arrange the nodes
      {
         BMNHorizLayout* mainLayout = new BMNHorizLayout();
         mainLayout->add( fsInput );
         mainLayout->add( diffuseSectionLayout );
         mainLayout->add( normalSectionLayout );
         mainLayout->add( specularSectionLayout );
         mainLayout->add( lighting );
         mainLayout->add( fsOutput );
         mainLayout->arrangeNodes();
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

BMNCompositeItem* BlenderMaterialBuilder::buildDiffuseColorProvider( MaterialNode* inputStruct, MaterialNode* output, GraphBuilderTransaction< MaterialNode >& outTransaction ) const
{
   BMNHorizLayout* mainLayout = new BMNHorizLayout();

   // check if at any point a color override operation is defined - start from the end, 'cause we want to start queuing textures from that point.
   // A color override happens if you use Mix mode with the factor value set to 1.0
   int firstActiveStage = m_exportedMaterial->diffuseTexturesCount - 1;
   for ( ; firstActiveStage >= 0; --firstActiveStage )
   {
      MNRGBOperation textureOperation = ( MNRGBOperation ) m_exportedMaterial->diffuseTexturesOperations[firstActiveStage];
      float factor = m_exportedMaterial->diffuseTexturesOperationFactors[firstActiveStage];

      if ( textureOperation == RGBO_Mix && factor >= 1.0f )
      {
         // found it
         break;
      }
   }

   MaterialNode* outputDiffuseNode = NULL;
   if ( firstActiveStage < 0 )
   {
      // we're gonna go with the whole thing, starting with the color
      MNColor* diffuseColor = new MNColor( "Base color", m_exportedMaterial->diffuseColor );
      outTransaction.addNode( diffuseColor );
      mainLayout->add( diffuseColor );
      outputDiffuseNode = diffuseColor;

      firstActiveStage = 0;
   }

   for ( int i = firstActiveStage; i < m_exportedMaterial->diffuseTexturesCount; ++i )
   {
      int diffuseTextureIdx = m_exportedMaterial->diffuseTexturesIndices[i];
      Texture* diffuseTexture = m_textures[diffuseTextureIdx];

      BMNVertLayout* localLayout = new BMNVertLayout();
      mainLayout->add( localLayout );

      MNTexture* diffuseTexNode = new MNTexture( "Texture", diffuseTexture );
      outTransaction.connectNodes( inputStruct, "uv0", diffuseTexNode, "UV" );
      outTransaction.addNode( diffuseTexNode );
      localLayout->add( diffuseTexNode );

      if ( outputDiffuseNode != NULL )
      {
         MNRGBOperation textureOperation = ( MNRGBOperation ) m_exportedMaterial->diffuseTexturesOperations[i];
         float factor = m_exportedMaterial->diffuseTexturesOperationFactors[i];

         MNMixRGBs* diffuseMixer = new MNMixRGBs( "Blend colors", textureOperation, factor );
         outTransaction.addNode( diffuseMixer );
         localLayout->add( diffuseMixer );

         outTransaction.connectNodes( outputDiffuseNode, "Color", diffuseMixer, "In1" );
         outTransaction.connectNodes( diffuseTexNode, "Color", diffuseMixer, "In2" );
         outputDiffuseNode = diffuseMixer;
      }
      else
      {
         // there's no
         outputDiffuseNode = diffuseTexNode;
      }
   }

   // connect the final stage to the lighting node
   outTransaction.connectNodes( outputDiffuseNode, "Color", output, "gBuf_Albedo" );

   return mainLayout;
}

///////////////////////////////////////////////////////////////////////////////

BMNCompositeItem* BlenderMaterialBuilder::buildSpecularColorProvider( MaterialNode* inputStruct, MaterialNode* output, GraphBuilderTransaction< MaterialNode >& outTransaction ) const
{
   BMNHorizLayout* mainLayout = new BMNHorizLayout();

   if ( m_exportedMaterial->specularTextureIndex >= 0 && m_exportedMaterial->specularTextureIndex < ( int ) m_textures.size() )
   {
      Texture* specularTexture = m_textures[m_exportedMaterial->specularTextureIndex];
      MNTexture* specularNode = new MNTexture( "Specular map", specularTexture );
      outTransaction.addNode( specularNode );
      mainLayout->add( specularNode );

      // connect the node
      outTransaction.connectNodes( inputStruct, "uv0", specularNode, "UV" );
      outTransaction.connectNodes( specularNode, "Color", output, "gBuf_Specular" );

      // Specular intensity
      // Specular hardness
   }


   return mainLayout;
}

///////////////////////////////////////////////////////////////////////////////

BMNCompositeItem* BlenderMaterialBuilder::buildNormalMapProvider( MaterialNode* inputStruct, MaterialNode* lighting, GraphBuilderTransaction< MaterialNode >& outTransaction ) const
{
   BMNVertLayout* mainLayout = new BMNVertLayout();

   if ( m_exportedMaterial->normalTextureIndex >= 0 && m_exportedMaterial->normalTextureIndex < ( int ) m_textures.size() )
   {
      // read the normals from the normal map
      Texture* normalTexture = m_textures[m_exportedMaterial->normalTextureIndex];

      MNTexture* normalTexNode = new MNTexture( "Normal map", normalTexture );
      MNNormalTangentToView* normalsMapper = new MNNormalTangentToView( "to tangent space" );
      MNColorToNormal* colorToNormal = new MNColorToNormal( "cast to normal" );

      mainLayout->add( normalTexNode );
      mainLayout->add( colorToNormal );
      mainLayout->add( normalsMapper );

      outTransaction.addNode( normalTexNode );
      outTransaction.addNode( colorToNormal );
      outTransaction.addNode( normalsMapper );

      // connect nodes
      outTransaction.connectNodes( inputStruct, "uv0", normalTexNode, "UV" );

      outTransaction.connectNodes( normalTexNode, "Color", colorToNormal, "Color" );

      outTransaction.connectNodes( colorToNormal, "Normal", normalsMapper, "NormalTS" );
      outTransaction.connectNodes( inputStruct, "normalVS", normalsMapper, "SurfNormal" );
      outTransaction.connectNodes( inputStruct, "tangentVS", normalsMapper, "SurfTangent" );

      outTransaction.connectNodes( normalsMapper, "Normal", lighting, "NormalsVS" );
   }
   else
   {
      // pass the vertex normals
      outTransaction.connectNodes( inputStruct, "normalVS", lighting, "NormalsVS" );
   }

   return mainLayout;
}

///////////////////////////////////////////////////////////////////////////////
