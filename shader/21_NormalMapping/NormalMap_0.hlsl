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

struct PNCT2_VS_INPUT
{
    float4 vPos			: POSITION;
    float3 vNormal		: NORMAL;
    float4 vColor		: COLOR0;
    float2 vTexCoord	: TEXCOORD0;
    float3 Tan			: TANGENT;    
};

struct PCT4_PS_INPUT
{
    float4 vPos			: SV_POSITION;
    float4 vColor		: COLOR0;
    float2 vTexCoord	: TEXCOORD0;
    float3 vEye			: TEXCOORD1;
    float3 vHalf		: TEXCOORD2;
    float3 vLightDir	: TEXCOORD3;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PCT4_PS_INPUT VS( PNCT2_VS_INPUT input )
{
    PCT4_PS_INPUT output = (PCT4_PS_INPUT)0;
    float4 vWorldPos	= mul( input.vPos,  g_matWorld );
    float4 vViewPos		= mul( vWorldPos, g_matView );
    output.vPos			= mul( vViewPos, g_matProj );
    
    output.vTexCoord			= input.vTexCoord;
    output.vColor			= input.vColor;
    
    float3 vNormal		= normalize( mul( input.vNormal, (float3x3)g_matNormal ) ) ;
    output.vEye			= normalize( cb_vEyePos-vWorldPos.xyz );
 	
	float3 T			= normalize( mul( input.Tan, (float3x3)g_matNormal ));	
	float3 B			= normalize( cross( vNormal, T) );                   

	float3x3 tanMat		= { T.x, B.x, vNormal.x,
							T.y, B.y, vNormal.y,
							T.z, B.z, vNormal.z };
	float3 vLightDir = normalize(cb_vLightVector.xyz - vWorldPos);
	output.vHalf	 = normalize( mul( normalize(vLightDir + output.vEye), tanMat )  );
	output.vLightDir = normalize( mul(vLightDir, tanMat) );
	output.vEye	     = normalize( mul( output.vEye, tanMat ) );		
    return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( PCT4_PS_INPUT input ) : SV_Target
{
	// 기본 텍스쳐 컬러 
/*	float4 normal		=	g_txNormalMap.Sample( g_samLinear, input.vTexCoord );
	float1 x			=	input.vTexCoord.x+normal.x*0.1f*cb_vLightVector.z;	
	float1 y			=	input.vTexCoord.y+normal.y*0.1f*cb_vLightVector.x;	
	float2 uv			=	float2(input.vTexCoord.y, y);
*/	
	float4 DiffuseColor =	g_txDiffuse.Sample( g_samLinear, input.vTexCoord );
	
	// 디퓨즈 조명 
	float4 normal		=	g_txNormalMap.Sample( g_samLinear, input.vTexCoord );
		   normal		=	normalize( (normal - 0.5f) * 2.0f );			
	float  fDot			=	saturate( dot( normal.xyz, input.vLightDir ));
	float3 LightColor	=	cb_DiffuseLightColor.rgb * fDot;	
	
	// 스펙큘러 조명 		
	float3 R		    = reflect( -input.vLightDir, normal.xyz); 
	float3 SpecColor	=	cb_SpecularLightColor.rgb * pow( saturate(dot( R, input.vEye )), cb_SpecularPower );
	//float3 SpecColor	=	cb_SpecularLightColor.rgb * pow( saturate(dot( input.vHalf, normal.xyz )), cb_SpecularPower );
	
	// 전체 컬러 조합  	
    float4 vFinalColor	= DiffuseColor * float4( LightColor + SpecColor, 1.0f);
    //float4 vFinalColor = DiffuseColor;
	return vFinalColor;
}
float4 DEFAULT_PS( PCT4_PS_INPUT input ) : SV_Target
{
    return g_txDiffuse.Sample( g_samLinear, input.vTexCoord );	
}

[maxvertexcount(12)]
void GS(triangle SO_VS_OUTPUT input[3], inout TriangleStream<SO_VS_OUTPUT> TriStream)
{
	
}
