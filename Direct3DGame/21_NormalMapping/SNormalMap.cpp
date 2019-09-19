#include "SNormalMap.h"

DWORD SNormalMap::EncodeVectorAsDWORDColor(D3DXVECTOR3* vVector)
{
	DWORD dwRed = (DWORD)(127 * vVector->x + 128);
	DWORD dwGreen = (DWORD)(127 * vVector->y + 128);
	DWORD dwBlue = (DWORD)(127 * vVector->z + 128);
	return (DWORD)(0xff000000 + (dwRed << 16) + (dwGreen << 8) + dwBlue);
}

void SNormalMap::CreateTangentSpaceVectors(D3DXVECTOR3 *v0,
	D3DXVECTOR3 *v1, D3DXVECTOR3 *v2, D3DXVECTOR2 uv0,
	D3DXVECTOR2 uv1, D3DXVECTOR2 uv2,
	D3DXVECTOR3 *vTangent, D3DXVECTOR3 *vBiNormal, D3DXVECTOR3 *vNormal)
{
	//  Vertex Edge Vector
	D3DXVECTOR3 vEdge1 = *v1 - *v0;
	D3DXVECTOR3 vEdge2 = *v2 - *v0;
	D3DXVec3Normalize(&vEdge1, &vEdge1);
	D3DXVec3Normalize(&vEdge2, &vEdge2);
	// UV Vector
	D3DXVECTOR2 deltaUV1 = uv1 - uv0;
	D3DXVECTOR2 deltaUV2 = uv2 - uv0;
	D3DXVec2Normalize(&deltaUV1, &deltaUV1);
	D3DXVec2Normalize(&deltaUV2, &deltaUV2);


	D3DXVECTOR3 biNormal;
	// Tangent, BiNormal 벡터를 구하기 위한 역행렬의 행렬식
	float fDet = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
	if (fabsf(fDet) < 1e-6f)
	{
		*vTangent = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
		biNormal = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	}
	else
	{
		*vTangent = (vEdge1 * deltaUV2.y - vEdge2 * deltaUV1.y)*fDet;
		biNormal = (vEdge2 * deltaUV1.x - vEdge1 * deltaUV2.x)*fDet;
	}
	D3DXVec3Normalize(vTangent, vTangent);
	D3DXVec3Normalize(&biNormal, &biNormal);

	D3DXVec3Cross(vBiNormal, vNormal, vTangent);
	// 벡터가 뒤집혔다면 본래 방향으로 돌려놓는다.
	float crossinv = (D3DXVec3Dot(vBiNormal, &biNormal) < 0.0f) ? -1.0f : 1.0f;
	*vBiNormal *= crossinv;
}

void SNormalMap::CreateTangentSpaceVectors(D3DXVECTOR3 *v1,
	D3DXVECTOR3 *v2,
	D3DXVECTOR3 *v3,
	float v1u, float v1v,
	float v2u, float v2v,
	float v3u, float v3v,
	D3DXVECTOR3 *vTangent,
	D3DXVECTOR3 *vBiNormal,
	D3DXVECTOR3 *vNormal)
{
	//  Vertex Edge Vector
	D3DXVECTOR3 vEdge1 = *v2 - *v1;
	D3DXVECTOR3 vEdge2 = *v3 - *v1;
	// UV Vector
	float fEdge1_U = v2u - v1u;
	float fEdge1_V = v2v - v1v;
	float fEdge2_U = v3u - v1u;
	float fEdge2_V = v3v - v1v;

	// 역행렬을 구하기 위한 행렬식
	float fDenominator = fEdge1_U * fEdge2_V - fEdge2_U * fEdge1_V;

	// 0에 근접하면
	if (fDenominator < 0.0001f && fDenominator > -0.0001f)
	{
		*vTangent = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
		*vBiNormal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		*vNormal = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	}
	else
	{
		// 정점 간의 Edge Vector
		D3DXVECTOR3 vEdge1 = *v2 - *v1;
		D3DXVECTOR3 vEdge2 = *v3 - *v1;
		// 텍스쳐 좌표 상에서의 Edge UV 값
		float fEdge1_U = v2u - v1u;
		float fEdge1_V = v2v - v1v;
		float fEdge2_U = v3u - v1u;
		float fEdge2_V = v3v - v1v;

		// 역행렬의 행렬식
		float fDenominator = fEdge1_U * fEdge2_V - fEdge2_U * fEdge1_V;

		// 0과 같을 경우 단위 벡터로 넘긴다.
		if (fDenominator < 0.0001f && fDenominator > -0.0001f)
		{
			*vTangent = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
			*vBiNormal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
			*vNormal = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
		}
		else
		{
			float fScale1 = 1.0f / fDenominator;

			D3DXVECTOR3 T;
			D3DXVECTOR3 B;
			D3DXVECTOR3 N;

			*vTangent = D3DXVECTOR3((fEdge2_V * vEdge1.x - fEdge1_V * vEdge2.x) * fScale1,
				(fEdge2_V * vEdge1.y - fEdge1_V * vEdge2.y) * fScale1,
				(fEdge2_V * vEdge1.z - fEdge1_V * vEdge2.z) * fScale1);

			*vBiNormal = D3DXVECTOR3((-fEdge2_U * vEdge1.x + fEdge1_U * vEdge2.x) * fScale1,
				(-fEdge2_U * vEdge1.y + fEdge1_U * vEdge2.y) * fScale1,
				(-fEdge2_U * vEdge1.z + fEdge1_U * vEdge2.z) * fScale1);

			D3DXVec3Cross(vNormal, vTangent, vBiNormal);
		}
	}
}
SNormalMap::SNormalMap()
{
}


SNormalMap::~SNormalMap()
{
}
