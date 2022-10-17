#include "ShaderIncludes.hlsli"

cbuffer ExternalData
{
	matrix model;
	matrix modelInvTranspose;
	matrix view;
	matrix projection;
};

VertexToPixel_NormalMap main(VsInput input)
{
	VertexToPixel_NormalMap output;
	
	matrix mvp = mul(projection, mul(view, model));
	output.position = mul(mvp, float4(input.position, 1.0f));

	output.worldPosition = mul(model, float4(input.position, 1.0f)).xyz;

	output.normal = mul((float3x3)modelInvTranspose, input.normal);
	output.tangent = mul((float3x3)modelInvTranspose, input.tangent);
	
	output.uv = input.uv;

	return output;
}