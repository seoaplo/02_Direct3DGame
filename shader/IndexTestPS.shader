struct OUTPUT
{
	float4 Pos : POSITION;
	float4 Color : COLOR0;
};

struct INPUT
{
	float4 Pos : POSITION;
	float4 Color : COLOR0;
};

OUTPUT PS(INPUT var)
{
	return var;
}