// Input/Output structs

float4x4 gWorldViewProj : WorldViewProjection;
float4x4 gWorldMatrix : WORLD;
float4x4 gViewInverseMatrix : VIEWINVERSE;

Texture2D gDiffuseMap : DiffuseMap;
Texture2D gNormalMap : NormalMap;
Texture2D gSpecularMap : SpecularMap;
Texture2D gGlossinessMap : GlosinessMap;

float3 gLightDirection : LightDirection;

float gPI = 3.14159f;
float gLightIntensity = 7.0f;
float gShininess = 25.0f;

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


RasterizerState gRasterizerState
{
	CullMode = back;
	FrontCounterClockwise = false;
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
	output.Position = mul(float4(input.Position, 1.f), gWorldViewProj);
	output.WorldPosition = mul(float4(input.Position, 1.f), gWorldMatrix);
	output.Normal = mul(normalize(input.Normal), (float3x3)gWorldMatrix);
	output.Tangent = mul(normalize(input.Tangent), (float3x3)gWorldMatrix);
	output.TexCoord = input.TexCoord;
	return output;
}

// Pixel Shader
float4 PS(VS_OUTPUT input) : SV_TARGET
{
	// Get normalmap transform matrix
	float3 binormal = cross(input.Tangent, input.Normal);
	float3x3 tangentSpaceAxis = float3x3( input.Tangent, binormal, input.Normal );
	
	SamplerState samplerState = pointSampler;
	float4 uvColor = gNormalMap.Sample(samplerState, input.TexCoord);
    
	float3 normal = ( 2.f * uvColor - 1.f).xyz;
	normal = normalize(mul(normal, tangentSpaceAxis));
	
	// Lambert cosine law
	float observedArea = saturate(dot(-normal, gLightDirection) / gPI);
	// sample diffuse texture
	float4 diffuseMapColor = gDiffuseMap.Sample(samplerState, input.TexCoord);
	
	//Phong
	float3 phongReflect = reflect(-gLightDirection, normal);
	float3 viewDirection = normalize(input.WorldPosition.xyz - gViewInverseMatrix[3].xyz);
	float cosa = saturate(dot(viewDirection, phongReflect));
	
	float4 phongExponent = gGlossinessMap.Sample(samplerState, input.TexCoord);
	float4 phongReflectance = gSpecularMap.Sample(samplerState, input.TexCoord);
	
	float4 phongSpecularReflection = (phongReflectance * (pow(cosa, phongExponent.x * gShininess)));
	
	float4 diffuse =  gLightIntensity * diffuseMapColor * observedArea;
	
	float4 finalColor = diffuse + phongSpecularReflection;
	return finalColor;
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
};