Texture2D g_txDiffuse : register(t0);
Texture2D g_txBrush : register(t1);
Texture2D g_txMask : register(t2);
Texture2D g_txSplat0 : register(t3);
Texture2D g_txSplat1 : register(t4);
Texture2D g_txSplat2 : register(t5);
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
	float2 TileTex : TEXCOORD1;
	float3 vEye			: TEXCOORD2;
	float3 vHalf		: TEXCOORD3;
	float3 vLightDir	: TEXCOORD4;
};

cbuffer cb0 : register(b0)
{
	matrix g_matWorld: packoffset(c0);// 상수레지스터
	matrix g_matView : packoffset(c4);
	matrix g_matProj : packoffset(c8);
	float4 g_Color : packoffset(c12);
};
cbuffer cb0 : register(b1)
{
	dword	TileNum		: packoffset(c0.x);
}

cbuffer cbObjectChangesEveryFrame : register(b2)
{
	// Global
	matrix				g_matNormal;
	float3				cb_vLightVector;
	float3				cb_vEyePos;
	float3				cb_vEyeDir;
};

cbuffer cbObjectNeverChanges: register(b3)
{
	// Global
	float4				cb_AmbientLightColor: packoffset(c0);
	float4				cb_DiffuseLightColor: packoffset(c1);
	float3				cb_SpecularLightColor: packoffset(c2);
	float				cb_SpecularPower : packoffset(c2.w);
};

VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT pOut = (VS_OUTPUT)0;
	// world
	pOut.p = mul(float4(input.p, 1.0f), g_matWorld);// local * g_matWorld;
	pOut.p = mul(pOut.p, g_matView);// world * g_matView;
	pOut.p = mul(pOut.p, g_matProj);// view * g_matWorld;

	pOut.t = input.t;
	pOut.c = input.c;
	pOut.TileTex = input.t * TileNum;

	pOut.n = normalize(mul(input.n, (float3x3)g_matNormal));
	pOut.vEye = normalize(cb_vEyePos - pOut.p.xyz);
	pOut.vLightDir = normalize(cb_vLightVector.xyz);
	pOut.vHalf = normalize(-pOut.vLightDir + -cb_vEyePos);

	pOut.vEye = normalize(mul(pOut.vEye, (float3x3)g_matNormal));
	pOut.vLightDir = normalize(mul(pOut.vLightDir, (float3x3)g_matNormal));
	pOut.vHalf = normalize(-pOut.vLightDir + -cb_vEyeDir);

	return pOut;
}

float4 PS(VS_OUTPUT input) : SV_TARGET
{
	float4 DiffuseColor = g_txDiffuse.Sample(s0 , input.t);
	float4 color = g_txMask.Sample(s0 , input.t);
	float4 BrushColor = g_txBrush.Sample(s0, input.t);

	float4 AlphaTexColor0 = g_txSplat0.Sample(s0 , input.TileTex); 
	float4 AlphaTexColor1 = g_txSplat1.Sample(s0 , input.TileTex); 
	float4 AlphaTexColor2 = g_txSplat2.Sample(s0 , input.TileTex); 
	float4 AlphaTexColor3 = float4(0.0f, 0.0f, 0.0f, color.w);

	float4 RenderColor = (color.w * DiffuseColor) +
		(AlphaTexColor0 * color.x ) +
		(AlphaTexColor1 * color.y ) +
		(AlphaTexColor2 * color.z );

	// 디퓨즈 조명 
	float  fDot = saturate(dot(input.n.xyz, input.vLightDir));
	float3 LightColor = cb_DiffuseLightColor.rgb * fDot;

	// 스펙큘러 조명 		
	float3 R = reflect(input.n.xyz, input.vHalf);
	float3 SpecColor = cb_SpecularLightColor.rgb * pow(saturate(dot(R, -cb_vEyeDir)), cb_SpecularPower);

	// 전체 컬러 조합  	

	float4 vFinalColor = RenderColor *  float4(LightColor + SpecColor, 1.0f);

	if (BrushColor.w > 0.0f)
	{
		vFinalColor = ( (1.0f - BrushColor.w) * vFinalColor) +
			float4(BrushColor.x, 0.0f, 0.0f, BrushColor.w) +
			float4( 0.0f, BrushColor.y, 0.0f, BrushColor.w) +
			float4(0.0f, 0.0f, BrushColor.z, BrushColor.w);
	}

	return vFinalColor;
}
