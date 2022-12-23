#include "ShaderIncludes.hlsli"

// Based on https://learnopengl.com/PBR/Lighting

cbuffer ExternalData
{
	float3 camPosition;
	float3 tint;
	Light lights[MAX_LIGHTS];
};

Texture2D		Albedo			: register(t0);
Texture2D		Normals			: register(t1);
Texture2D		Metalness		: register(t2);
Texture2D		Roughness		: register(t3);
Texture2D		AO				: register(t4);

SamplerState	BasicSampler	: register(s0);

float4 main(VertexToPixel_NormalMap input) : SV_TARGET
{
	input.normal = normalize(input.normal);
	input.tangent = normalize(input.tangent);

	input.tangent = normalize(input.tangent - input.normal * dot(input.tangent, input.normal)); // Gram-Schmidt assumes T&N are normalized!
	float3 B = normalize(cross(input.tangent, input.normal));
	float3x3 TBN = float3x3(input.tangent, B, input.normal);
	float3 unpackedNormal = Normals.Sample(BasicSampler, input.uv).rgb * 2.0f - 1.0f;
	input.normal = normalize(mul(unpackedNormal, TBN));
	float3 albedo = Albedo.Sample(BasicSampler, input.uv).rgb;
	albedo = pow(albedo, 2.2f); // Reverse gamma correct
	float3 metalness = Metalness.Sample(BasicSampler, input.uv).rrr;
	float roughness = Roughness.Sample(BasicSampler, input.uv).r;

	float3 light = LightingLoop(lights, input.normal, input.worldPosition, camPosition, roughness, albedo, metalness);

	float ao = AO.Sample(BasicSampler, input.uv).r;
	float3 ambient = float3(0.1, 0.1, 0.1) * albedo * ao;

	float3 color = ambient + light;

	// Gamma correction
	float gC = 1.0 / 2.2;
	color = pow(color, gC);

	return float4(color * tint, 1.0);
}