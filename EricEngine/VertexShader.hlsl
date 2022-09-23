cbuffer ExternalData
{
	matrix model;
	matrix view;
	matrix projection;
};

struct VsInput 
{
	float3 position	: POSITION;
	float3 color	: COLOR0;
};

struct VsOutput
{
	float4 position	: SV_POSITION;
	float4 color	: COLOR0;
};

VsOutput main(VsInput input)
{
	VsOutput output;
	matrix mvp = mul(projection, mul(view, model));
	output.position = mul(mvp, float4(input.position, 1.0f));
	output.color = float4(input.color, 1.0f);
	return output;
}