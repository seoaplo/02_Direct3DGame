Texture2D g_txDiffuse : register(t0);
Texture2D g_txMask : register(t1);
SamplerState s0 : register(s0);

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

cbuffer cb0 : register(b0)
{
	matrix g_matWorld: packoffset(c0);// �����������
	matrix g_matView : packoffset(c4);
	matrix g_matProj : packoffset(c8);
	float4 g_Color : packoffset(c12);
};

VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT pOut = (VS_OUTPUT)0;
	// world
	pOut.p = mul(float4(input.p, 1.0f), g_matWorld);// local * g_matWorld;
	pOut.p = mul(pOut.p, g_matView);// world * g_matView;
	pOut.p = mul(pOut.p, g_matProj);// view * g_matWorld;

	pOut.t = input.t;
	pOut.n = input.n;
	pOut.c = input.c;
	return pOut;
}

float4 PS(VS_OUTPUT input) : SV_TARGET
{
	float4 DiffuseColor = g_txDiffuse.Sample(s0 , input.t);
	float4 color = g_txMask.Sample(s0 , input.t);// *input.c;

	color = ((1 - color.w) * DiffuseColor) + (color * color.w);

	return color;
}