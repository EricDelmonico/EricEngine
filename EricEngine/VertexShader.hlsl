#include "ShaderIncludes.hlsli"

cbuffer ExternalData
{
	matrix model;
	matrix modelInvTranspose;
	matrix view;
	matrix projection;
};

struct VsInput 
{
	float3 position	: POSITION;
	float3 normal	: NORMAL;
	float3 tangent	: TANGENT;
	float2 uv		: TEXCOORD;
};

VertexToPixel_NormalMap main(VsInput input)
{
	VertexToPixel_NormalMap output;
	
	matrix mvp = mul(projection, mul(view, model));
	output.position = mul(mvp, float4(input.position, 1.0f));

	output.normal = mul((float3x3)modelInvTranspose, input.normal);
	output.tangent = mul((float3x3)modelInvTranspose, input.tangent);
	
	output.uv = input.uv;

	return output;
}