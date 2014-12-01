// @file	/Renderer/Shaders/RenderingPipeline/selectionShader.glsl
// @brief	<put the description here>
#version 330 core

smooth in vec4 positionVS;

layout(location=0) out float vFragColor;

void main()
{
	vFragColor = length(positionVS);
}
