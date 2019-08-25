struct VS_INPUT
{
	float3 p : POSITION;
	float2 t : TEXCOORD;
};
struct VS_OUTPUT
{
	float4 p : SV_POSITION;
	float2 t : TEXCOORD0;
};

cbuffer cb0 : register(b0)
{
	float4 g_Color : packoffset(c0);
	float  g_fTime : packoffset(c1.x);
};
VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT pOut = (VS_OUTPUT)0;
	//pOut.p = float4(input.p,1.0f);
	//pOut.p.x = input.p.x * cos(g_fTime) -input.p.y * sin(g_fTime);
	//pOut.p.y = input.p.y * sin(g_fTime) +input.p.y * cos(g_fTime);
	//pOut.p.z = 1.0f;
	pOut.p = float4(input.p, 1.0f);
	pOut.t = input.t;
	return pOut;
}
VS_OUTPUT VSRotation(VS_INPUT input)
{
	VS_OUTPUT pOut = (VS_OUTPUT)0;
	pOut.p = float4(input.p, 1.0f);
	pOut.p.x = input.p.x * cos(g_fTime) - input.p.y * sin(g_fTime);
	pOut.p.y = input.p.x * sin(g_fTime) + input.p.y * cos(g_fTime);
	pOut.p.z = 1.0f;
	pOut.t = input.t;
	return pOut;
}