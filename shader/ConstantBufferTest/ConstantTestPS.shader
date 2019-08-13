struct VS_OUTPUT
{
	float4 Position : SV_Position;
	float4 Diffuse : COLOR0;
};

//====================================================================
// Pixel Shader
//====================================================================
float4 PS(VS_OUTPUT input) : SV_Target
{
	return input.Diffuse;
}