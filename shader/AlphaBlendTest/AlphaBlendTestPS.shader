Texture2D g_txDiffuse : register (t0);
SamplerState sample0 : register (s0);

struct PS_INPUT
{
	float4 p : SV_POSITION;
	float4 c : COLOR0;
	float2 t : TEXCOORD0;
};

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_Target
{
	float4 ColorSample = g_txDiffuse.Sample(sample0, input.t);
	ColorSample = ColorSample * input.c;
	return  ColorSample;
}