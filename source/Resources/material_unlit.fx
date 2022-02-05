float4x4 gWorldViewProj : WorldViewProjection;

Texture2D gDiffuseMap : DiffuseMap;

struct VS_INPUT
{
	float3 Position : POSITION;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
	float2 TexCoord : TEXCOORD;
};

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float4 WorldPosition : COLOR;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
	float2 TexCoord : TEXCOORD;
};

BlendState gBlendState
{
	BlendEnable[0] = false;
	SrcBlend = src_alpha;
	DestBlend = inv_src_alpha;
	SrcBlendAlpha = zero;
	DestBlendAlpha = zero;
	BlendOpAlpha = add;
	RenderTargetWriteMask[0] = 0x0F;
};

RasterizerState gRasterizerState
{
	CullMode = none;
	FrontCounterClockwise = false;
};

DepthStencilState gDepthStencilState
{
	DepthEnable = true;
	DepthWriteMask = 1;
	DepthFunc = less;
	StencilEnable = false;

	StencilReadMask = 0x0F;
	StencilWriteMask = 0x0F;

	FrontFaceStencilFunc = always;
	BackFaceStencilFunc = always;

	FrontFaceStencilDepthFail = keep;
	BackFaceStencilDepthFail = keep;
	
	FrontFaceStencilPass = keep;
	BackFaceStencilPass = keep;

	FrontFaceStencilFail = keep;
	BackFaceStencilFail = keep;
};

SamplerState pointSampler
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Border;// or Mirror or Clamp or Border or Wrap
	AddressV = Clamp;// or Mirror or Clamp or Border or Wrap
	BorderColor = float4(0.0f, 0.0f, 1.0f, 1.0f);
};

// Vertex Shader
VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.Position = mul(float4(input.Position, 1.0f), gWorldViewProj);
	output.WorldPosition = float4(input.Position,1.0f); //mul(float4(input.Position, 1.0f), gWorldMatrix);
	output.Normal = input.Normal; //mul(normalize(input.Normal), (float3x3)gWorldMatrix);
	output.Tangent = input.Tangent; //mul(normalize(input.Tangent), (float3x3)gWorldMatrix);
	output.TexCoord = input.TexCoord;
	return output;
}

// Pixel Shader
float4 PS(VS_OUTPUT input) : SV_TARGET
{
	return gDiffuseMap.Sample(pointSampler, input.TexCoord);
	
	//float4 finalColor = diffuse;
	//return (finalColor);
}

// Technique
technique11 DefaultTechnique
{
	pass P0
	{
		SetRasterizerState(gRasterizerState);
		SetDepthStencilState(gDepthStencilState, 0);
		SetBlendState(gBlendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetVertexShader( CompileShader( vs_5_0, VS() ));
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_5_0, PS() ));
	}
}
