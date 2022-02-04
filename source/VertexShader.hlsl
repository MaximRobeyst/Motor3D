cbuffer MatrixBuffer :  register(b0)
{
	float4x4 gWorldViewProj;
};

struct VS_INPUT
{
	float3 Position : POSITION;
};

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
};
	
VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
	
	output.Position = mul(float4(input.Position, 1.f), gWorldViewProj);
	
	return output;
}