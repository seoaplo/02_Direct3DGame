struct OUTPUT
{
	float4 Pos : POSITION;
	float4 Color : COLOR0;
};

OUTPUT VS(float4 Pos : POSITION) : SV_POSITION
{
	OUTPUT ret;
	ret.Pos = Pos;
	ret.Color = float4(1, 1, 0, 1);
	return ret;
}