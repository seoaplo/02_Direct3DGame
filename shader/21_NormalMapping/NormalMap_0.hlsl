//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D	g_txDiffuse : register (t0);
Texture2D	g_txNormalMap	: register (t1);
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

struct PNCT_VS_OUTPUT
{
	float4 vPos			: SV_POSITION;
	float4 vLocalPos	: POSITION0;
	float4 vWorldPos	: POSITION1;
	float3 vNormal		: NORMAL;
	float4 vColor		: COLOR0;
	float2 vTexCoord	: TEXCOORD0;
};


struct PCT4_PS_INPUT
{
	float4 vPos			: SV_POSITION;
	float4 vColor		: COLOR0;
	float3 vNormal		: NORMAL0;
	float3 vBiNormal	: NORMAL1;
	float3 vTangent		: TANGENT;
	float2 vTexCoord	: TEXCOORD0;
	float3 vEye			: TEXCOORD1;
	float3 vHalf		: TEXCOORD2;
	float3 vLightDir	: TEXCOORD3;
};



//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PNCT_VS_OUTPUT VS(PNCT_VS_INPUT input )
{
	PNCT_VS_OUTPUT output = (PNCT_VS_OUTPUT)0;
	output.vLocalPos	= float4(input.vPos, 1.0f);
	output.vWorldPos	= mul(output.vLocalPos,  g_matWorld );
    float4 vViewPos		= mul(output.vWorldPos, g_matView );
    output.vPos			= mul( vViewPos, g_matProj );
    
	output.vNormal		= input.vNormal;
    output.vTexCoord	= input.vTexCoord;
    output.vColor		= input.vColor;

    return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PCT4_PS_INPUT input ) : SV_Target
{
	normalize(input.vTangent); 
	normalize(input.vBiNormal);
	normalize(input.vNormal);
	normalize(input.vLightDir);

	float4 TextureColor = g_txDiffuse.Sample(g_samLinear, input.vTexCoord);
	
	/*float3x3 tanMat = { input.vTangent, input.vBiNormal, input.vNormal};
	transpose(tanMat);*/

	// 디퓨즈 조명 
	float3 normal		=	g_txNormalMap.Sample( g_samLinear, input.vTexCoord  * 10);
		   normal		=	normalize( (normal - 0.5f) * 2.0f );
		   //normal		=	normalize(mul(tanMat, normal));

	float  fDot			=	saturate( dot(normal.xyz, input.vLightDir ));
	float3 LightColor	=	cb_DiffuseLightColor.rgb * fDot;	
	
	// 스펙큘러 조명 		
	float3 R = reflect(-input.vLightDir, normal.xyz);
	float3 SpecColor	=	cb_SpecularLightColor.rgb * pow( saturate(dot( R, input.vEye )), cb_SpecularPower );
	
	// 전체 컬러 조합  	
    float4 vFinalColor	= TextureColor * float4( LightColor + SpecColor, 1.0f);
	return vFinalColor;
}
float4 DEFAULT_PS( PCT4_PS_INPUT input ) : SV_Target
{
    return g_txDiffuse.Sample( g_samLinear, input.vTexCoord );	
}


float3 CreateTangent(float3 v0, float3 v1, float3 v2, float2 uv0, float2 uv1, float2 uv2)
{
	float3 vTangent;
	float3 vBiNormal;
	float3 vNormal;

	float3 vEdge1 = v1 - v0;
	float3 vEdge2 = v2 - v0;
	vEdge1 = normalize(vEdge1);
	vEdge2 = normalize(vEdge2);
	// UV delta
	float2 deltaUV1 = uv1 - uv0;
	float2 deltaUV2 = uv2 - uv0;
	deltaUV1 = normalize(deltaUV1);
	deltaUV2 = normalize(deltaUV2);

	float3 biNormal;
	float fDet = deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x;

	if (fDet < 0.0001f && fDet > -0.0001f)
	{
		vTangent = float3(1.0f, 0.0f, 0.0f);
	}
	else
	{
		fDet = 1.0f / fDet;
		vTangent = (vEdge1 * deltaUV2.y - vEdge2 * deltaUV1.y)*fDet;
	}
	vTangent = normalize(vTangent);
	return vTangent;
}

[maxvertexcount(36)]
void GS(triangle PNCT_VS_OUTPUT input[3], inout TriangleStream<PCT4_PS_INPUT> TriStream)
{
	PCT4_PS_INPUT output;
	float3 vTangent;
	for (int iCount = 0; iCount < 3; iCount++)
	{
		output = (PCT4_PS_INPUT)0;

		output.vPos			=	input[iCount].vPos;
		output.vColor		=	input[iCount].vColor;
		output.vNormal		=	input[iCount].vNormal;
		output.vTexCoord	=	input[iCount].vTexCoord;

		int i0 = iCount % 3;
		int i1 = (iCount + 1) % 3;
		int i2 = (iCount + 2) % 3;
		vTangent = float4(CreateTangent(input[i0].vLocalPos.xyz, input[i1].vLocalPos.xyz, input[i2].vLocalPos.xyz,
										input[i0].vTexCoord, input[i1].vTexCoord, input[i2].vTexCoord), 1.0f).xyz;

		output.vNormal =	normalize(mul(input[iCount].vNormal, (float3x3)g_matNormal));
		output.vEye =		normalize(cb_vEyePos - input[iCount].vWorldPos.xyz);
		output.vLightDir =	normalize(cb_vLightVector.xyz - input[iCount].vWorldPos);
		output.vHalf =		normalize(output.vLightDir + output.vEye);

		output.vEye = normalize(mul(output.vEye, (float3x3)g_matNormal));
		output.vLightDir = normalize(mul(output.vLightDir, (float3x3)g_matNormal));
		output.vHalf = normalize(mul(output.vLightDir, (float3x3)g_matNormal));

		output.vTangent = normalize(mul(vTangent, (float3x3)g_matNormal));
		output.vBiNormal = normalize(cross(output.vNormal, output.vTangent));

		float3x3 tanMat = { output.vTangent.x, output.vBiNormal.x, output.vNormal.x,
							output.vTangent.y, output.vBiNormal.y, output.vNormal.y,
							output.vTangent.z, output.vBiNormal.z, output.vNormal.z };

		output.vHalf = normalize(mul(normalize(output.vLightDir + output.vEye), tanMat));
		output.vLightDir = normalize(mul(output.vLightDir, tanMat));
		output.vEye = normalize(mul(output.vEye, tanMat));

		

		TriStream.Append(output);
	}

	TriStream.RestartStrip();
}