struct VertexToPixel_NormalMap
{
	float4 position			: SV_POSITION;
	float2 uv				: TEXCOORD;
	float3 normal			: NORMAL;
	float3 worldPosition	: POSITION;
	float3 tangent			: TANGENT;
};