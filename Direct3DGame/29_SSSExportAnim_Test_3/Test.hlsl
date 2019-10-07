Texture2D	g_txDiffuse : register (t0);
SamplerState g_samLinear: register (s0);

cbuffer cb0 : register(b0)
{
	matrix	g_matWorld		: packoffset(c0);
	matrix	g_matView		: packoffset(c4);
	matrix	g_matProj		: packoffset(c8);
	float4  g_MeshColor     : packoffset(c12);
};
//--------------------------------------------------------------------------------------
struct VS_INPUT
{
	float3 p : POSITION;
	float3 n : NORMAL;
	float4 c : COLOR;
	float2 t : TEXCOORD;
};
struct VS_OUTPUT
{
	float4 p : SV_POSITION;
	float3 n : NORMAL;
	float4 c : COLOR0;
	float2 t : TEXCOORD0;
};

VS_OUTPUT VS(VS_INPUT vIn)
{
	VS_OUTPUT vOut = (VS_OUTPUT)0;

	vOut.p = float4(vIn.p.xyz, 1.0f);
	vOut.p = mul(vOut.p, g_matWorld);
	vOut.p = mul(vOut.p, g_matView);
	vOut.p = mul(vOut.p, g_matProj);
	vOut.c = vIn.c;
	vOut.t = vIn.t;
	vOut.n = vIn.n;

	return vOut;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT vIn) : SV_Target
{
	float4 TextureColor =  g_txDiffuse.Sample(g_samLinear, vIn.t);
	return TextureColor * float4(vIn.c.xyz, 1);
	//return float4(vIn.c.xyz, 1);
	//return TextureColor;

}