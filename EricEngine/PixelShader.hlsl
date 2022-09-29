#include "ShaderIncludes.hlsli"

Texture2D		Albedo			: register(t0);
SamplerState	BasicSampler	: register(s0);

float4 main(VertexToPixel_NormalMap input) : SV_TARGET
{
	float4 color = Albedo.Sample(BasicSampler, input.uv);
	return color;
}