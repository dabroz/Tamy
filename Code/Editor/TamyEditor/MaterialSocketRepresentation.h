/// @file   TamyEditor/MaterialSocketRepresentation.h
/// @brief  material pipeline's socket representation
#pragma once

#include "GraphBlockSocket.h"
#include "core-Renderer/MaterialNode.h"


///////////////////////////////////////////////////////////////////////////////

DECLARE_PIPELINE_SOCKET( MSSVec2, MaterialNode, 100, 100, 120 );
DECLARE_PIPELINE_SOCKET( MSSVec3, MaterialNode, 100, 100, 160 );
DECLARE_PIPELINE_SOCKET( MSSVec4, MaterialNode, 100, 100, 200 );
DECLARE_PIPELINE_SOCKET( MSSTexture, MaterialNode, 186, 166, 221 );
DECLARE_PIPELINE_SOCKET( MSSMatrix, MaterialNode, 159, 211, 55 );
DECLARE_PIPELINE_SOCKET( MSSMatrixArr, MaterialNode, 159, 211, 80 );
DECLARE_PIPELINE_SOCKET( MSSFloat, MaterialNode, 163, 255, 247 );
DECLARE_PIPELINE_SOCKET( MSSBool, MaterialNode, 186, 211, 255 );
DECLARE_PIPELINE_SOCKET( MSSInt, MaterialNode, 255, 198, 237 );
DECLARE_PIPELINE_SOCKET( MSSColor, MaterialNode, 199, 199,41 );
DECLARE_PIPELINE_SOCKET( MSSBSDF, MaterialNode, 99, 199, 99 );

///////////////////////////////////////////////////////////////////////////////
