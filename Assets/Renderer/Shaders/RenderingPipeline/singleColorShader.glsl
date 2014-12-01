// @file	/Renderer/Shaders/RenderingPipeline/selectionShader.glsl
// @brief	<put the description here>
#version 330 core

layout(location=0) out vec4 vFragColor;
uniform FragmentShaderConstants
{
	vec4 g_color;
};

void main()
{
	vFragColor = g_color;
}
