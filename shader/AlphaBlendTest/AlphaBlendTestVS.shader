//====================================================================
// Global variables Shader
//====================================================================
cbuffer cb0
{
	matrix Rotation: packoffset(c0);
	float fTime : packoffset(c4.x);
};

struct VS_INPUT
{
	float4 Position : POSITION;
	float4 Color : COLOR;
	float2 Coord : TEXCOORD;
};

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float4 Color : COLOR0;
	float2 Coord : TEXCOORD0;
};

//====================================================================
// Vertex Shader
//====================================================================
VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT Output;
	Output.Position = mul(input.Position, Rotation);
	Output.Color = input.Color;
	Output.Coord = input.Coord;
	return Output;
}