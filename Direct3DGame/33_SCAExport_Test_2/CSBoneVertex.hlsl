struct Vertex
{
	float3 p;
	float3 n;
	float4 c;
	float2 t;
};

struct IW
{
	float4 i1;
	float4 w1;
	float4 i2;
	float4 w2;
};

StructuredBuffer<Vertex> SkinVertex : register(t0);
StructuredBuffer<IW> SkinVertexIW : register(t1);
StructuredBuffer<matrix> SkinInvMatrix : register(t2);
RWStructuredBuffer<Vertex> BoneVertex : register(u0);

[numthreads(1, 1, 1)]
void CS( uint3 DTid : SV_DispatchThreadID )
{
	float4x4  matMatrix;
	float4 vLocal = float4(SkinVertex[DTid.x].p, 1.0f);
	float4 vAnim = float4(0.0f, 0.0f, 0.0f, 0.0f);
	int iBone;
	float fWeight;

	for (int ibiped = 8; ibiped > 0; ibiped--)
	{
		if (ibiped < 4)
		{
			iBone = SkinVertexIW[DTid.x].i1[ibiped];
			fWeight = SkinVertexIW[DTid.x].w1[ibiped];
		}
		else
		{
			iBone = SkinVertexIW[DTid.x].i2[ibiped - 4];
			fWeight = SkinVertexIW[DTid.x].w2[ibiped - 4];
		}
		//if (iBone < 0 || fWeight < 0) break;
		if (iBone < 0) continue;

		matMatrix = SkinInvMatrix[iBone];
		vAnim += fWeight * mul(vLocal, matMatrix);
	}
	vAnim.w = 1.0f;
	BoneVertex[DTid.x] = SkinVertex[DTid.x];
	BoneVertex[DTid.x].p = vAnim.xyz;
}