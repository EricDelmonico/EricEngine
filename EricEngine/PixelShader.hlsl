#include "ShaderIncludes.hlsli"

// Based on https://learnopengl.com/PBR/Lighting

cbuffer ExternalData
{
	float3 camPosition;
	float3 tint;
	float3 sunColor;
	float3 sunDir;
	float sunIntensity;
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
	input.normal = mul(unpackedNormal, TBN);

	float3 L = normalize(-sunDir);

	float3 albedo = Albedo.Sample(BasicSampler, input.uv).rgb;
	albedo = pow(albedo, 2.2f); // Reverse gamma correct
	float3 metallic = Metalness.Sample(BasicSampler, input.uv).rrr;
	float roughness = Roughness.Sample(BasicSampler, input.uv).r;
	float3 N = input.normal;
	float3 V = normalize(camPosition - input.worldPosition);

	float3 H = normalize(V + L);

	float3 F0 = float(0.04).xxx;
	F0 = lerp(F0, albedo, metallic);
	float3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

	float NDF = DistributionGGX(N, H, roughness);
	float G = GeometrySmith(N, V, L, roughness);

	float3 numerator = NDF * G * F;
	float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
	float3 specular = numerator / denominator;

	float3 kS = F;
	float3 kD = float(1.0).rrr - kS;
	kD *= 1.0 - metallic;

	float NdotL = max(dot(N, L), 0.0);
	float3 light = (kD * albedo / PI + specular) * sunColor * NdotL * sunIntensity;

	float ao = AO.Sample(BasicSampler, input.uv).r;
	float3 ambient = float3(0.1, 0.1, 0.1) * albedo * ao;
	float3 color = ambient + light;

	// Gamma correction
	float gC = 1.0 / 2.2;
	color = pow(color, gC);

	return float4(color * tint, 1.0);
}