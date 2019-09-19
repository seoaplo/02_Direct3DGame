#define HALF_VECTOR
//==========================================================================================
// Constant Buffer Variables
//==========================================================================================
Texture2D g_txDiffuse : register (t0);
SamplerState g_samLinear : register (s0);

cbuffer cb0: register(b0)
{
	matrix g_matWorld		: packoffset(c0);
	matrix g_matView		: packoffset(c4);
	matrix g_matProj		: packoffset(c8);
	float4 g_MeshColor		: packoffset(c12);
};

//==========================================================================================
// Lighting Variagles
//==========================================================================================
cbuffer cb1: register(b1)
{
	matrix			g_matWorldInverse : packoffset(c0);
	float4			g_AmbientMaterial : packoffset(c4);
	float4			g_DiffuseMaterial : packoffset(c5);
	float4			g_cSpecularMaterial : packoffset(c6);
	float4			g_cEmissionMaterial : packoffset(c7);

	float4			g_AmbientLightColor : packoffset(c8);
	float4			g_DiffuseLightColor : packoffset(c9);
	float4			g_cSpecularLightColor : packoffset(c10);
	float3			g_vLightDir : packoffset(c11);
	float			g_fDamping : packoffset(c11.w);
	float3			g_vLightPos : packoffset(c12);
	float			g_fRadius : packoffset(c12.w);
	float3			g_EyeDir : packoffset(c13);
	float			g_fIntensity : packoffset(c13.w);
	float3			g_vEyePos : packoffset(c14);
	float			g_fEyeRadius : packoffset(c14.w);
};

struct VS_INPUT
{
	float4 Pos : POSITION;
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

float4 Diffuse(float3 vNormal)
{
	float fIntensity = max(0, dot(vNormal, normalize(-g_vLightDir)));
	float4 diffuse = g_AmbientMaterial * g_AmbientLightColor + (g_DiffuseMaterial * g_DiffuseLightColor * fIntensity);
	return diffuse;
}

float4 Specular(float3 vNormal)
{
	// Specular Lighting
	float fPower = 0.0f;
#ifndef HALF_VECTOR
	// 2 * dot(g_vLightDir, vNormal) * vNormal - g_vLightDir
	float3 vReflection = reflect(g_vLightDir, vNormal);
	fPower = pow(saturate(dot(vReflection, -g_EyeDir)), g_fIntensity);
#else
	// 
	float3 vHalf = normalize(-g_vLightDir + -g_EyeDir);
	fPower = pow(saturate(dot(vNormal, vHalf)), g_fIntensity);
#endif
	float4 specular = g_cSpecularMaterial * g_cSpecularLightColor * fPower;
	return specular;
}

float4 Rim(VS_OUTPUT vIn)
{
	float fRimLightcolor = smoothstep(1.0f - vIn.Pos.w, 1.0f, 1.0f - max(0, dot(vIn.Nor, vIn.Pos.xyz)));
	float4 vFinalColor = float4(fRimLightcolor, fRimLightcolor, fRimLightcolor, 1.0f);
	return vFinalColor;
}
float Geom(VS_OUTPUT vIn)
{
	float3 vReflect = normalize(-g_vLightDir + -g_EyeDir);
	float fDot = pow(saturate(dot(vIn.Nor, vReflect)), 1);

	float NdotL = saturate(dot(vIn.Nor, -g_vLightDir));
	float NdotH = saturate(dot(vIn.Nor, vReflect));
	float NdotV = saturate(dot(vIn.Nor, -g_EyeDir));
	float VdotH = saturate(dot(-g_EyeDir, vReflect));

	float Geom = min(min(2 * (NdotH*NdotV / VdotH), 2 * (NdotH*NdotL / VdotH)), 1.0f) / max(0.1f, NdotV);
	return Geom;
}


//==========================================================================================
// Vertex Shader
//==========================================================================================
VS_OUTPUT VS(VS_INPUT vIn)
{
	VS_OUTPUT vOut = (VS_OUTPUT)0;
	// 정점 행렬 변환
	vOut.Pos = mul(vIn.Pos, g_matWorld);
	vOut.Pos = mul(vOut.Pos, g_matView);
	vOut.Pos = mul(vOut.Pos, g_matProj);
	// normal은 정점의 scale 변환 때문에 그대로 사용할 수 없다.
	vOut.Nor = normalize(mul(vIn.Nor, (float3x3)g_matWorldInverse));
	// 정점과 정점 사이의 텍스쳐 밀도를 높인다.
	vOut.TexCoord = vIn.TexCoord *10;
	vOut.Color = vIn.Color * g_MeshColor;
	return vOut;
}
//==========================================================================================
// Pixel Shader
//==========================================================================================
float4 PS(VS_OUTPUT vIn) : SV_Target
{
	float4 vTexColor = g_txDiffuse.Sample(g_samLinear, vIn.TexCoord);
	float4 vFinalColor = vTexColor * Diffuse(vIn.Nor) * vIn.Color;
	vFinalColor.a = 1.0f;
	return vFinalColor;
}

float4 PS_Texture(VS_OUTPUT vIn) : SV_Target
{
	return g_txDiffuse.Sample(g_samLinear, vIn.TexCoord);
}

float4 PS_Color(VS_OUTPUT vIn) : SV_Target
{
	return vIn.Color;
}