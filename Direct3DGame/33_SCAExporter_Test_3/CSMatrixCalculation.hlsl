StructuredBuffer<matrix> SkinInvMatrix : register(t0);
RWStructuredBuffer<matrix> CalMatrix : register(u0);

[numthreads(1, 1, 1)]
void CS(uint3 DTid : SV_DispatchThreadID)
{
	matrix Cal = mul(SkinInvMatrix[DTid.x], transpose(CalMatrix[DTid.x]));
	CalMatrix[DTid.x] = Cal;
}