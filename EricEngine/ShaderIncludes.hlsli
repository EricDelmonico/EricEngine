#define MAX_LIGHTS 10

struct VsInput
{
	float3 position	: POSITION;
	float3 normal	: NORMAL;
	float3 tangent	: TANGENT;
	float2 uv		: TEXCOORD;
};

struct VertexToPixel_NormalMap
{
	float4 position			: SV_POSITION;
	float2 uv				: TEXCOORD;
	float3 normal			: NORMAL;
	float3 worldPosition	: POSITION;
	float3 tangent			: TANGENT;
};

struct VertexToPixel_Sky
{
	float4 position			: SV_POSITION;
	float3 sampleDir		: DIRECTION;
};

struct Light
{
	float3 dir;
	float intensity;
	float3 color;
	int lightType;
	float3 pos;
	float range;
};

#define PI 3.14159265359

// Fresnel values
static const float F0_NON_METAL = 0.04f;
static const float F0_CHROME = 0.6f;

float3 fresnelSchlick(float cosTheta, float3 F0)
{
	return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float DistributionGGX(float3 N, float3 H, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;

	float num = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;
	return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r * r) / 8.0;

	float num = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return num / denom;
}

float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}

float Attenuate(Light light, float3 worldPos)
{
	float dist = distance(light.pos, worldPos);
	float att = saturate(1.0f - (dist * dist / (light.range * light.range)));
	return att * att;
}

// N is the normal at this pixel, L is the direction to the light source
float DiffusePBR(float3 N, float3 L)
{
	float nDotL = max(dot(N, L), 0.0f);
	return saturate(nDotL);
}

float3 EnergyConserve(float3 diffuse, float3 specular, float3 metalness)
{
	return diffuse * ((1 - saturate(specular)) * (1 - metalness));
}

float3 MicrofacetBRDF(float3 N, float3 L, float3 V, float roughness, float3 specColor, float3 H, float3 F)
{
	float3 NDF = DistributionGGX(N, H, roughness);
	float G = GeometrySmith(N, V, L, roughness);

	float3 numerator = NDF * G * F;
	float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
	return numerator / denominator;
}

float3 DirLightPBR(Light light, float3 N, float3 camPos, float3 worldPos, float roughness, float3 specColor, float metalness, float3 surfaceColor)
{
	float3 L = normalize(-light.dir);
	float3 V = normalize(camPos - worldPos);

	float3 H = normalize(V + L);
	float3 F = fresnelSchlick(max(dot(H, V), 0.0), specColor);

	float diffuse = DiffusePBR(N, L);
	float3 specular = MicrofacetBRDF(N, L, V, roughness, specColor, H, F);

	// Calculate diffuse accounting for conservation of energy
	// (Reflected light should not get diffused
	float3 balancedDiffuse = EnergyConserve(diffuse, specColor, metalness);
	
	float3 kS = F;
	float3 kD = float(1.0).rrr - kS;
	kD *= 1.0 - metalness;

	return (kD * surfaceColor / PI + specular) * diffuse * light.intensity * light.color;
}

float3 PointLightPBR(Light light, float3 N, float3 camPos, float3 worldPos, float roughness, float3 specColor, float metalness, float3 surfaceColor)
{
	float3 L = normalize(light.pos - worldPos);
	float3 V = normalize(camPos - worldPos);

	float3 H = normalize(V + L);
	float3 F = fresnelSchlick(max(dot(H, V), 0.0), specColor);

	float diffuse = DiffusePBR(N, L);
	float3 specular = MicrofacetBRDF(N, L, V, roughness, specColor, H, F);
	float attenuation = Attenuate(light, worldPos);

	// Calculate diffuse accounting for conservation of energy
	// (Reflected light should not get diffused
	float3 balancedDiffuse = EnergyConserve(diffuse, specColor, metalness);

	float3 kS = F;
	float3 kD = float(1.0).rrr - kS;
	kD *= 1.0 - metalness;

	return (kD * surfaceColor / PI + specular) * diffuse * light.intensity * light.color * attenuation;
}

float3 LightingLoop(
	Light lights[MAX_LIGHTS],
	float3 normal,
	float3 worldPosition,
	float3 cameraPos,
	float roughness,
	float3 surfaceColor,
	float metalness)
{
	float3 specColor = F0_NON_METAL.xxx;
	specColor = lerp(specColor, surfaceColor, metalness);

	float3 lightResult = 0.0f;
	for (int i = 0; i < MAX_LIGHTS; i++)
	{
		if (lights[i].intensity <= 0) continue;

		int type = lights[i].lightType;
		if (type == 0) // directional
		{
			lightResult += DirLightPBR(lights[i], normal, cameraPos, worldPosition, roughness, specColor, metalness, surfaceColor);
		}
		if (type == 1) // point
		{
			lightResult += PointLightPBR(lights[i], normal, cameraPos, worldPosition, roughness, specColor, metalness, surfaceColor);
		}
	}

	return lightResult;
}

//float3 L = normalize(-sunDir);
//
//float3 N = input.normal;
//float3 V = normalize(camPosition - input.worldPosition);
//
//float3 H = normalize(V + L);
//
//float3 F0 = float(0.04).xxx;
//F0 = lerp(F0, albedo, metallic);
//float3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
//
//float NDF = DistributionGGX(N, H, roughness);
//float G = GeometrySmith(N, V, L, roughness);
//
//float3 numerator = NDF * G * F;
//float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
//float3 specular = numerator / denominator;
//
//float3 kS = F;
//float3 kD = float(1.0).rrr - kS;
//kD *= 1.0 - metallic;
//
//float NdotL = max(dot(N, L), 0.0);
//float3 light = (kD * albedo / PI + specular) * sunColor * NdotL * sunIntensity;