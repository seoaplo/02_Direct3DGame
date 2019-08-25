Texture2D g_txDiffuse : register(t0);
Texture2D g_txMask : register(t1);
SamplerState s0 : register(s0);
struct VS_OUTPUT
{
	float4 p : SV_POSITION;
	float3 n : NORMAL;
	float4 c : COLOR0;
	float2 t : TEXCOORD0;
};

float4 PS(VS_OUTPUT input) : SV_TARGET
{
	float4 color =
	g_txDiffuse.Sample(s0 , input.t);// *input.c;
	//float4 color = float4(1,0,0,1);
	return color;
}
//
//float4 PSMask(in float4 p : POSITION,
//	in float2 t : TEXCOORD0) : SV_TARGET
//{
//	float4 color = g_txDiffuse.Sample(s0 , t);
//	float4 alpha = g_txMask.Sample(s0, t);
//	if (alpha.r==1.0f && alpha.g == 1.0f && alpha.b)
//		discard;
//	return color;
//}
//float4 PSBlend(in float4 p : POSITION,
//	in float2 t : TEXCOORD0) : SV_TARGET
//{
//	float4 colorA = g_txDiffuse.Sample(s0 , t);
//	float4 colorB = g_txMask.Sample(s0, t);
//
//	float alpha = 1.0f;
//	if (colorB.r == 1.0f && colorB.g == 1.0f && colorB.b == 1.0f)
//		alpha = 0.0f;
//	float4 color = colorA * (1.0f- alpha)  +  colorB * alpha;
//	return color;
//}