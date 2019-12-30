Texture2D g_txDiffuse : register(t0);
SamplerState g_samLinear: register (s0);

cbuffer cb0 : register(b0)
{
	matrix matProj : packoffset(c0);
	float fDistance : packoffset(c4.x);
	float fSmoothSize : packoffset(c4.y);
	uint  dwWidth : packoffset(c4.z);
	uint  dwHeight : packoffset(c4.w);
};
//--------------------------------------------------------------------------------------
struct VS_INPUT
{
	float3 p : POSITION;
	float4 c : COLOR;
};
struct VS_OUTPUT
{
	float4 p : SV_POSITION;
	float4 c : COLOR0;
};

VS_OUTPUT VS(VS_INPUT vIn)
{
	VS_OUTPUT vOut = (VS_OUTPUT)0;

	vOut.p = float4(vIn.p.xz, 0.0f, 1.0f);
	vOut.c = vIn.c;
	return vOut;
}


struct PS_INPUT
{
	float4 vPos : SV_POSITION;
	float4 vColor : COLOR0;
	float2 vTexCoord : TEXCOORD0;
	float2 vOrigin : TEXCOORD1;
};

[maxvertexcount(4)]
void GS(
	point VS_OUTPUT input[1],
	inout TriangleStream< PS_INPUT > output
)
{

	const float3 g_positions[4] =
	{
		float3(-1.0f, 1.0f, 0.0f),
		float3(1.0f, 1.0f, 0.0f),
		float3(-1.0f, -1.0f, 0.0f),
		float3(1.0f, -1.0f, 0.0f),
	};
	const float2 g_texcoords[4] =
	{
		float2(0.0f, 0.0f),
		float2(1.0f, 0.0f),
		float2(0.0f, 1.0f),
		float2(1.0f, 1.0f),
	};
	PS_INPUT vertex = (PS_INPUT)0;
	for (int i = 0; i < 4; i++)
	{
		vertex.vPos = float4(input[0].p.xyz + (g_positions[i] * fDistance), 1.0f);
		vertex.vPos = mul(vertex.vPos, matProj);
		vertex.vOrigin.x = (vertex.vPos.x + 1.0f) * 0.5f;
		vertex.vOrigin.y = 1.0f - ((vertex.vPos.y + 1.0f) * 0.5f);
		vertex.vColor = input[0].c;
		vertex.vTexCoord = float2(g_texcoords[i].x * fDistance, g_texcoords[i].y * fDistance);
		output.Append(vertex);
	}
	output.RestartStrip();
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT vIn) : SV_Target
{
	float xPos = (vIn.vTexCoord.x * 2.0f) - fDistance;
	float yPos = (vIn.vTexCoord.y * 2.0f) - fDistance;

	float fCicleDistance = sqrt(float((xPos * xPos) + (yPos * yPos)));

	float4 RenderColor = g_txDiffuse.Sample(g_samLinear, vIn.vOrigin);
	if (fCicleDistance < fDistance)
	{
		float fAlpha = fCicleDistance - fSmoothSize;
		if (fAlpha < 0.0f) fAlpha = 1.0f;
		else fAlpha = (fDistance - fSmoothSize - fAlpha) / (fDistance - fSmoothSize);
		float4 SourceColor = vIn.vColor;
		RenderColor = ((1 - fAlpha) *  RenderColor)
		+ fAlpha * SourceColor;
	}
	return RenderColor;
}