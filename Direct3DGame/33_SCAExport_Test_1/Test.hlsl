#define MAX_BONE_MATRIX 250
Texture2D	g_txDiffuse : register (t0);
SamplerState g_samLinear: register (s0);

cbuffer cb0 : register(b0)
{
	matrix	g_matWorld		: packoffset(c0);
	matrix	g_matView		: packoffset(c4);
	matrix	g_matProj		: packoffset(c8);
	float4  g_MeshColor     : packoffset(c12);
};
cbuffer cb1 : register(b1)
{
	matrix g_matAnim[MAX_BONE_MATRIX];
};
//--------------------------------------------------------------------------------------
struct VS_INPUT
{
	float3 p : POSITION;
	float3 n : NORMAL;
	float4 c : COLOR;
	float2 t : TEXCOORD0;

	float4 i1 : TEXCOORD1;
	float4 w1 : TEXCOORD2;
	float4 i2 : TEXCOORD3;
	float4 w2 : TEXCOORD4;
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

	float4x4  matMatrix;
	float4 vLocal = float4(vIn.p, 1.0f);
	float4 vAnim = float4(0.0f, 0.0f, 0.0f, 1.0f);
	int		iBone;
	float fWeight;

	for (int ibiped = 0; ibiped < 8; ibiped++)
	{
		if (ibiped < 4)
		{
			iBone = vIn.i1[ibiped];
			fWeight = vIn.w1[ibiped];
		}
		else
		{
			iBone = vIn.i2[ibiped - 4];
			fWeight = vIn.w2[ibiped - 4];
		}
		//if (iBone < 0 || fWeight < 0) break;
		if (iBone < 0) break;

		matMatrix = g_matAnim[iBone];
		vAnim += fWeight * mul(vLocal, matMatrix);
		vAnim.w = 1.0f;
		//vOut.n += fWeight * mul(vLocal, matMatrix);
	}

	vAnim.w = 1.0f;
	vOut.p = mul(vAnim, g_matWorld);
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
	return TextureColor * float4(vIn.c.xyz, 1.0f);
	//return float4(1.0f, 1.0f, 0.0f, 1.0f);
	//return TextureColor;

}