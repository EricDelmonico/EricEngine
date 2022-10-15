#include "ShaderIncludes.hlsli"

// Based on https://learnopengl.com/PBR/Lighting

cbuffer ExternalData
{
	float3 camPosition;
};

Texture2D		Albedo			: register(t0);
Texture2D		Normals			: register(t1);
Texture2D		Metalness		: register(t2);
Texture2D		Roughness		: register(t3);

SamplerState	BasicSampler	: register(s0);

float4 main(VertexToPixel_NormalMap input) : SV_TARGET
{
	float4 color = Albedo.Sample(BasicSampler, input.uv);
	return color;
}