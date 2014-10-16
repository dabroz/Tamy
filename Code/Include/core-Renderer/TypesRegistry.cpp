#include "core/TypeRegistrationMacros.h"
#include "core-Renderer.h"


///////////////////////////////////////////////////////////////////////////////

REGISTER_EXTERNAL_TYPE( ShaderConstantDesc );
REGISTER_ABSTRACT_TYPE( RenderResource );
REGISTER_ENUM_TYPE( TextureUsage );

///////////////////////////////////////////////////////////////////////////////

REGISTER_TYPE( GeometryComponent );
REGISTER_ABSTRACT_TYPE( GeometryResource );
REGISTER_TYPE( StaticGeometry );
REGISTER_EXTERNAL_TYPE( LitVertex );
REGISTER_EXTERNAL_TYPE( VertexWeight );
REGISTER_EXTERNAL_TYPE( Face );
REGISTER_TYPE( TriangleMesh );

///////////////////////////////////////////////////////////////////////////////

REGISTER_ABSTRACT_TYPE( ShaderTexture );
REGISTER_TYPE( RenderState );
REGISTER_TYPE( Texture );
REGISTER_TYPE( CubeTexture );
REGISTER_ABSTRACT_TYPE( RenderTarget2D );
REGISTER_ABSTRACT_TYPE( RenderTarget2DStatic );
REGISTER_ABSTRACT_TYPE( RenderTarget2DDynamic );
REGISTER_ABSTRACT_TYPE( RenderTargetCube );

///////////////////////////////////////////////////////////////////////////////

REGISTER_TYPE( Material );
REGISTER_TYPE( MaterialProfile );
REGISTER_ABSTRACT_TYPE( MaterialNode );
DEFINE_TYPE_ID( GBNodeOutput<MaterialNode> );
MAP_TYPE( GBNodeInput<class MaterialNode>, ReflectionObject );

REGISTER_TYPE( MNColor );
REGISTER_TYPE( MNCamera );
REGISTER_TYPE( MNFloat );
REGISTER_TYPE( MNTexture );
REGISTER_TYPE( MNCubeTexture );
REGISTER_TYPE( MNVec4 );
REGISTER_TYPE( MNBool );
REGISTER_TYPE( MNTimer );
REGISTER_TYPE( MNTransform );
REGISTER_TYPE( MNTransformNormal );
REGISTER_TYPE( MNTransformVector );
REGISTER_TYPE( MNTransformSkinVector );
REGISTER_TYPE( MNTransformSkinNormal );
REGISTER_TYPE( MNNormalTangentToView );
REGISTER_TYPE( MNColorToNormal );
REGISTER_TYPE( MNInputStruct );
REGISTER_TYPE( MNOutputStruct );
REGISTER_TYPE( MNMixRGBs );
REGISTER_TYPE( MNSplitColor );

REGISTER_ABSTRACT_TYPE( MatrixArray );

REGISTER_ENUM_TYPE( MNRGBOperation );
REGISTER_ENUM_TYPE( TransformationType );

REGISTER_EXTERNAL_TYPE( MSVec2Type );
REGISTER_EXTERNAL_TYPE( MSVec3Type );
REGISTER_TYPE( MSFloatInput );
REGISTER_TYPE( MSFloatOutput );
REGISTER_TYPE( MSMatrixInput );
REGISTER_TYPE( MSMatrixOutput );
REGISTER_TYPE( MSMatrixArrInput );
REGISTER_TYPE( MSMatrixArrOutput );
REGISTER_TYPE( MSColorInput );
REGISTER_TYPE( MSColorOutput );
REGISTER_TYPE( MSVec2Input );
REGISTER_TYPE( MSVec2Output );
REGISTER_TYPE( MSVec3Input );
REGISTER_TYPE( MSVec3Output );
REGISTER_TYPE( MSVec4Input );
REGISTER_TYPE( MSVec4Output );
REGISTER_TYPE( MSBoolInput );
REGISTER_TYPE( MSBoolOutput );
REGISTER_TYPE( MSIntInput );
REGISTER_TYPE( MSIntOutput );

///////////////////////////////////////////////////////////////////////////////

REGISTER_ABSTRACT_TYPE( Light );
REGISTER_TYPE( DirectionalLight );
REGISTER_TYPE( PointLight );
REGISTER_TYPE( SpotLight );

///////////////////////////////////////////////////////////////////////////////

REGISTER_TYPE( ShaderProfile );
REGISTER_TYPE( Shader );
REGISTER_ABSTRACT_TYPE( ShaderDataBuffer );
REGISTER_EXTERNAL_TYPE( TextureSamplerSettings );

///////////////////////////////////////////////////////////////////////////////
