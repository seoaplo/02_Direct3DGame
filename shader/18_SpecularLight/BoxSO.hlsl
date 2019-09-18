//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D g_txDiffuse: register (t0);
SamplerState g_samLinear: register (s0);
cbuffer cb0 : register(b0)
{
	matrix	g_matWorld		: packoffset(c0);
	matrix	g_matView		: packoffset(c4);
	matrix	g_matProj		: packoffset(c8);
	float4  g_MeshColor     : packoffset(c12);
};
//--------------------------------------------------------------------------------------
//Lighting Variables
//--------------------------------------------------------------------------------------
cbuffer cb1: register(b1)
{
	matrix				g_matWorldInverse: packoffset(c0);
	float4				g_cAmbientMaterial: packoffset(c4);
	float4				g_cDiffuseMaterial: packoffset(c5);
	float4				g_cSpecularMaterial: packoffset(c6);
	float4				g_cEmissionMaterial: packoffset(c7);

	float4				g_cAmbientLightColor : packoffset(c8);
	float4				g_cDiffuseLightColor: packoffset(c9);
	float4				g_cSpecularLightColor: packoffset(c10);
	float3				g_vLightDir : packoffset(c11);	
	float			    g_fDamping : packoffset(c11.w);
	float3				g_vLightPos : packoffset(c12);
	float			    g_fRadius : packoffset(c12.w);
	float3				g_vEyeDir : packoffset(c13);
	float			    g_fIntensity : packoffset(c13.w);
	float3				g_vEyePos : packoffset(c14);
	float			    g_fEyeRadius : packoffset(c14.w);
};
//--------------------------------------------------------------------------------------
struct VS_INPUT
{
	float3 Pos : POSITION;
	float3 Nor : NORMAL;
	float4 Color : COLOR;
	float2 TexCoord : TEXCOORD;
};

struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float3 Nor : NORMAL;
	float4 Color : COLOR0;
	float2 TexCoord : TEXCOORD;
	float2 Diffuse : TEXCOORD1;
};


struct S0_VS_INPUT
{
	float3 Pos : POSITION;
	float3 Nor : NORMAL;
	float4 Color : COLOR;
	float2 TexCoord : TEXCOORD;
};
struct SO_VS_OUTPUT
{
	float3 Pos : POSITION;
	float3 Nor : NORMAL;
	float4 Color : COLOR;
	float2 TexCoord : TEXCOORD;
};

float4 Diffuse(float3 vNormal)
{
	float fIntensity = max(0, dot(vNormal, normalize(-g_vLightDir)));
	float4 diffuse = g_cAmbientMaterial * g_cAmbientLightColor +
		(g_cDiffuseMaterial * g_cDiffuseLightColor * fIntensity);
	return diffuse;
}

float4 Specular(float3 vNormal)
{
	// Specular Lighting
	float fPower = 0.0f;
#ifndef HALF_VECTOR
	// 2 * dot(g_vLightDir, vNormal) * vNormal - g_vLightDir
	float3 vReflection = reflect(g_vLightDir, vNormal);
	fPower = pow(saturate(dot(vReflection, -g_vEyeDir)), g_fIntensity);
#else
	// 
	float3 vHalf = normalize(-g_vLightDir + -g_vEyeDir);
	fPower = pow(saturate(dot(vNormal, vHalf)), g_fIntensity);
#endif
	float4 specular = g_cSpecularMaterial * g_cSpecularLightColor * fPower;
	return specular;
}

VS_OUTPUT VS(VS_INPUT vIn)
{
	VS_OUTPUT vOut = (VS_OUTPUT)0;
	// 정점 행렬 변환
	vOut.Pos = mul(float4(vIn.Pos, 1.0f), g_matWorld);
	vOut.Pos = mul(vOut.Pos, g_matView);
	vOut.Pos = mul(vOut.Pos, g_matProj);
	// normal은 정점의 scale 변환 때문에 그대로 사용할 수 없다.
	vOut.Nor = normalize(mul(vIn.Nor, (float3x3)g_matWorldInverse));
	vOut.TexCoord = vIn.TexCoord;
	vOut.Color = vIn.Color;
	return vOut;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT vIn) : SV_Target
{ 
	float4 vTexColor = g_txDiffuse.Sample( g_samLinear, vIn.TexCoord );
	float4 vFinalColor = vTexColor * (Diffuse(vIn.Nor) + Specular(vIn.Nor))* vIn.Color;
	vFinalColor.a = 1.0f;
	return vFinalColor;
}

float4 PS_Texture(VS_OUTPUT vIn) : SV_Target
{
	return g_txDiffuse.Sample(g_samLinear, vIn.TexCoord);
}

float4 PS_Color(VS_OUTPUT vIn) : SV_Target
{	
	float4 vColor = Diffuse( vIn.Nor );
	vColor.a = 1.0f;
	return vColor;
}
//--------------------------------------------------------------------------------------
// Stream Output
//--------------------------------------------------------------------------------------
SO_VS_OUTPUT VS_SO(S0_VS_INPUT input )
{
	SO_VS_OUTPUT output = (SO_VS_OUTPUT)0;
    output.Pos = input.Pos;
    output.Nor = input.Nor;
    output.Color = input.Color;
    output.TexCoord = input.TexCoord;
    return output;
}
// 로칼 정점의 크기를 정규화하여 1로 만든다.
SO_VS_OUTPUT NormalizeVertex(SO_VS_OUTPUT Vertex )
{
	SO_VS_OUTPUT newvertex;
	newvertex.Pos = normalize( Vertex.Pos.xyz );
	// 정점이 원점을 중심으로 하는 노말 벡터가 된다.
	newvertex.Nor = normalize( Vertex.Pos.xyz );
	newvertex.Color = Vertex.Color;
	newvertex.TexCoord = Vertex.TexCoord;
	return newvertex;
}
void TriAppend(SO_VS_OUTPUT V0, SO_VS_OUTPUT V1, SO_VS_OUTPUT V2, inout TriangleStream<SO_VS_OUTPUT> TriStream )
{
	TriStream.Append( NormalizeVertex(V0));
	TriStream.Append( NormalizeVertex(V1));	
	TriStream.Append( NormalizeVertex(V2));	
	// 삼각형 단위로 스트립으로 구성토록 한다.		
	TriStream.RestartStrip();
}
[maxvertexcount(12)]
void GS_SO(triangle SO_VS_OUTPUT input[3], inout TriangleStream<SO_VS_OUTPUT> TriStream)
{	
	// 페이스의 중점을 구한다.
	SO_VS_OUTPUT Center0, Center1, Center2;
    Center0.Pos = (input[0].Pos.xyz + input[1].Pos.xyz)/2.0;
    Center0.Nor = (input[0].Nor.xyz + input[1].Nor.xyz)/2.0;
    Center0.Color = (input[0].Color + input[1].Color)/2.0;  
    Center0.TexCoord = (input[0].TexCoord.xy + input[1].TexCoord.xy)/2.0;      

	Center1.Pos = (input[1].Pos.xyz + input[2].Pos.xyz)/2.0;
    Center1.Nor = (input[1].Nor.xyz + input[2].Nor.xyz)/2.0;
    Center1.Color = (input[1].Color + input[2].Color)/2.0;  
    Center1.TexCoord = (input[1].TexCoord.xy + input[2].TexCoord.xy)/2.0;      

	Center2.Pos = (input[0].Pos.xyz + input[2].Pos.xyz)/2.0;
    Center2.Nor = (input[0].Nor.xyz + input[2].Nor.xyz)/2.0;
    Center2.Color = (input[0].Color + input[2].Color)/2.0;  
    Center2.TexCoord = (input[0].TexCoord.xy + input[2].TexCoord.xy)/2.0;      

    TriAppend( input[0], Center0,Center2,  TriStream );				
	TriAppend( Center0, input[1], Center1, TriStream );		
	TriAppend( input[2], Center2, Center1, TriStream );				
	TriAppend( Center0, Center1, Center2, TriStream );		
}
