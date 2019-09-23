//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
TextureCube	g_txDiffuse : register (t0);
SamplerState g_samLinear: register (s0);

cbuffer cb0 : register(b0)
{
	matrix	g_matWorld		: packoffset(c0);
	matrix	g_matView		: packoffset(c4);
	matrix	g_matProj		: packoffset(c8);
	float4  g_MeshColor     : packoffset(c12);
};

//-----------------------------------------------------------------------------
//상수 버퍼
//-----------------------------------------------------------------------------
cbuffer cbObjectChangesEveryFrame: register(b1)
{
	// Global
	matrix				g_matNormal;
	float3				cb_vLightVector;
	float3				cb_vEyePos;
	float3				cb_vEyeDir;
};

cbuffer cbObjectNeverChanges: register(b2)
{
	// Global
	float4				cb_AmbientLightColor: packoffset(c0);
	float4				cb_DiffuseLightColor: packoffset(c1);
	float3				cb_SpecularLightColor: packoffset(c2);
	float				cb_SpecularPower : packoffset(c2.w);
};

struct PNCT_VS_INPUT
{
	float3 vPos			: POSITION;
	float3 vNormal		: NORMAL;
	float4 vColor		: COLOR0;
	float2 vTexCoord	: TEXCOORD0;
};

struct PCT4_PS_INPUT
{
	float4 vPos			: SV_POSITION;
	float4 vColor		: COLOR0;
	float3 vNormal		: NORMAL;
	float4 vWorldPos	: POSITION;
	float2 vTexCoord	: TEXCOORD0;
	float3 vEye			: TEXCOORD1;
	float3 vHalf		: TEXCOORD2;
	float3 vLightDir	: TEXCOORD3;
};



//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PCT4_PS_INPUT VS(PNCT_VS_INPUT input)
{
	PCT4_PS_INPUT output = (PCT4_PS_INPUT)0;
	output.vWorldPos = mul(float4(input.vPos, 1.0f), g_matWorld);
	float4 vViewPos = mul(output.vWorldPos, g_matView);
	output.vPos = mul(vViewPos, g_matProj);

	output.vNormal = normalize(mul(input.vNormal, (float3x3)g_matNormal));
	output.vTexCoord = input.vTexCoord;
	output.vColor = input.vColor;

	output.vLightDir = normalize(cb_vLightVector.xyz - output.vWorldPos.xyz);
	output.vEye = normalize(cb_vEyePos - output.vWorldPos.xyz);
	output.vHalf = normalize(output.vLightDir + output.vEye);

	/*output.vLightDir = normalize(mul(cb_vLightVector.xyz - output.vWorldPos.xyz, (float3x3)g_matNormal));
	output.vEye = normalize(mul(output.vEye, (float3x3)g_matNormal));
	output.vHalf = normalize(mul(output.vLightDir + output.vEye, (float3x3)g_matNormal));*/

	return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PCT4_PS_INPUT input) : SV_Target
{
		// 기본 텍스쳐 컬러 
	/*	float4 normal		=	g_txNormalMap.Sample( g_samLinear, input.vTexCoord );
		float1 x			=	input.vTexCoord.x+normal.x*0.1f*cb_vLightVector.z;
		float1 y			=	input.vTexCoord.y+normal.y*0.1f*cb_vLightVector.x;
		float2 uv			=	float2(input.vTexCoord.y, y);
	*/
	
	// 디퓨즈 조명 
	float  fDot = saturate(dot(input.vNormal.xyz, -input.vLightDir));
	float3 LightColor = cb_DiffuseLightColor.rgb * fDot;
	
	input.vEye = normalize(input.vEye);
	input.vNormal = normalize(input.vNormal);
	// 스펙큘러 조명 
	float3 R = reflect(-input.vEye, input.vNormal);
	float3 SpecColor = cb_SpecularLightColor.rgb * pow(saturate(dot(R, input.vEye)), cb_SpecularPower);
	
	float4 DiffuseColor = g_txDiffuse.Sample(g_samLinear, R);
	//float3 SpecColor	=	cb_SpecularLightColor.rgb * pow( saturate(dot( input.vHalf, normal.xyz )), cb_SpecularPower );
	
	// 전체 컬러 조합  	
	//float4 vFinalColor = DiffuseColor * float4(LightColor + SpecColor, 1.0f);
	float4 vFinalColor = DiffuseColor;
	return DiffuseColor;
}

//--------------------------------------------------------------------------------------
// Stream Output
//--------------------------------------------------------------------------------------


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


SO_VS_OUTPUT VS_SO(S0_VS_INPUT input)
{
	SO_VS_OUTPUT output = (SO_VS_OUTPUT)0;
	output.Pos = input.Pos;
	output.Nor = input.Nor;
	output.Color = input.Color;
	output.TexCoord = input.TexCoord;
	return output;
}
// 로칼 정점의 크기를 정규화하여 1로 만든다.
SO_VS_OUTPUT NormalizeVertex(SO_VS_OUTPUT Vertex)
{
	SO_VS_OUTPUT newvertex;
	newvertex.Pos = normalize(Vertex.Pos.xyz);
	// 정점이 원점을 중심으로 하는 노말 벡터가 된다.
	newvertex.Nor = normalize(Vertex.Pos.xyz);
	newvertex.Color = Vertex.Color;
	newvertex.TexCoord = Vertex.TexCoord;
	return newvertex;
}
void TriAppend(SO_VS_OUTPUT V0, SO_VS_OUTPUT V1, SO_VS_OUTPUT V2, inout TriangleStream<SO_VS_OUTPUT> TriStream)
{
	TriStream.Append(NormalizeVertex(V0));
	TriStream.Append(NormalizeVertex(V1));
	TriStream.Append(NormalizeVertex(V2));
	// 삼각형 단위로 스트립으로 구성토록 한다.		
	TriStream.RestartStrip();
}
[maxvertexcount(12)]
void GS_SO(triangle SO_VS_OUTPUT input[3], inout TriangleStream<SO_VS_OUTPUT> TriStream)
{
	// 페이스의 중점을 구한다.
	SO_VS_OUTPUT Center0, Center1, Center2;
	Center0.Pos = (input[0].Pos.xyz + input[1].Pos.xyz) / 2.0;
	Center0.Nor = (input[0].Nor.xyz + input[1].Nor.xyz) / 2.0;
	Center0.Color = (input[0].Color + input[1].Color) / 2.0;
	Center0.TexCoord = (input[0].TexCoord.xy + input[1].TexCoord.xy) / 2.0;

	Center1.Pos = (input[1].Pos.xyz + input[2].Pos.xyz) / 2.0;
	Center1.Nor = (input[1].Nor.xyz + input[2].Nor.xyz) / 2.0;
	Center1.Color = (input[1].Color + input[2].Color) / 2.0;
	Center1.TexCoord = (input[1].TexCoord.xy + input[2].TexCoord.xy) / 2.0;

	Center2.Pos = (input[0].Pos.xyz + input[2].Pos.xyz) / 2.0;
	Center2.Nor = (input[0].Nor.xyz + input[2].Nor.xyz) / 2.0;
	Center2.Color = (input[0].Color + input[2].Color) / 2.0;
	Center2.TexCoord = (input[0].TexCoord.xy + input[2].TexCoord.xy) / 2.0;

	TriAppend(input[0], Center0, Center2, TriStream);
	TriAppend(Center0, input[1], Center1, TriStream);
	TriAppend(input[2], Center2, Center1, TriStream);
	TriAppend(Center0, Center1, Center2, TriStream);
}
