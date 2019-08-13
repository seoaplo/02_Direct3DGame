//====================================================================
// Global variables Shader
//====================================================================
cbuffer cb0
{
	matrix Rotation: packoffset(c0);
	float fTime : packoffset(c4.x);
};

struct VS_OUTPUT
{
	float4 Position : SV_Position;
	float4 Diffuse	: COLOR0;
};

//====================================================================
// Vertex Shader
//====================================================================
VS_OUTPUT VS(float4 p: POSITION)
{
	VS_OUTPUT Output;
	float3 vPos = p * fTime;
	Output.Position = mul(p, Rotation);
	Output.Diffuse = float4(fTime, fTime, fTime, 1.0f);
	return Output;
}