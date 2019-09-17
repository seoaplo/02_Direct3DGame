#include "SMap.h"

DXGI_FORMAT SMap::MAKE_TYPELESS(DXGI_FORMAT format)
{
	switch (format)
	{
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
	case DXGI_FORMAT_R8G8B8A8_UNORM:
	case DXGI_FORMAT_R8G8B8A8_UINT:
	case DXGI_FORMAT_R8G8B8A8_SNORM:
	case DXGI_FORMAT_R8G8B8A8_SINT:
		return DXGI_FORMAT_R8G8B8A8_TYPELESS;
	case DXGI_FORMAT_BC1_UNORM_SRGB:
	case DXGI_FORMAT_BC1_UNORM:
		return DXGI_FORMAT_BC1_TYPELESS;
	case DXGI_FORMAT_BC2_UNORM_SRGB:
	case DXGI_FORMAT_BC2_UNORM:
		return DXGI_FORMAT_BC2_TYPELESS;
	case DXGI_FORMAT_BC3_UNORM_SRGB:
	case DXGI_FORMAT_BC3_UNORM:
		return DXGI_FORMAT_BC3_TYPELESS;
	}
	return format;
}
DXGI_FORMAT SMap::MAKE_SRGB(DXGI_FORMAT format)
{
	switch (format)
	{
	case DXGI_FORMAT_R8G8B8A8_TYPELESS:
	case DXGI_FORMAT_R8G8B8A8_UNORM:
	case DXGI_FORMAT_R8G8B8A8_UINT:
	case DXGI_FORMAT_R8G8B8A8_SNORM:
	case DXGI_FORMAT_R8G8B8A8_SINT:
		return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	case DXGI_FORMAT_BC1_TYPELESS:
	case DXGI_FORMAT_BC1_UNORM:
		return DXGI_FORMAT_BC1_UNORM_SRGB;
	case DXGI_FORMAT_BC2_TYPELESS:
	case DXGI_FORMAT_BC2_UNORM:
		return DXGI_FORMAT_BC2_UNORM_SRGB;
	case DXGI_FORMAT_BC3_TYPELESS:
	case DXGI_FORMAT_BC3_UNORM:
		return DXGI_FORMAT_BC3_UNORM_SRGB;

	};
	return format;
}
float SMap::GetHeight(float fPosX, float fPosZ)
{
	// fPosX / fPosZ의 위치에 해당하는 높이맵셀을 찾는다.
	// m_iNumCols와 m_iNumRows는 가로/세로의 실제 크기 값임
	float fCellX = (float)(m_iNumSellCols * m_fSellDistance / 2.0f + fPosX);
	float fCellZ = (float)(m_iNumSellRows * m_fSellDistance / 2.0f - fPosZ);

	// 셀의 크기로 나누어 1단위의 값으로 바꾸어 높이맵 배열에 접근한다.
	fCellX /= (float)m_fSellDistance;
	fCellZ /= (float)m_fSellDistance;

	// fCellX, fCellZ 값보다 작거나 같은 최대 정수( 소수부분을 잘라낸다.)
	float fVertexCol = ::floorf(fCellX);
	float fVertexRow = ::floorf(fCellZ);

	// 높이맵 범위를 벗어나면 강제로 초기화 한다.
	if (fVertexCol < 0.0f) fVertexCol = 0.0f;
	if (fVertexRow < 0.0f) fVertexRow = 0.0f;
	if ((float)(m_iNumCols - 2) < fVertexCol) fVertexCol = (float)(m_iNumCols - 2);
	if ((float)(fVertexRow - 2) < fVertexRow) fVertexRow = (float)(fVertexRow - 2);

	// 계산된 셀의 플랜을 구성하는 4개 정점의 높이값을 찾는다.
	//  A   B
	//  *---*
	//  | / |
	//  *---*  
	//  C   D
	float A = GetHeightmap((int)fVertexRow, (int)fVertexCol);
	float B = GetHeightmap((int)fVertexRow, (int)fVertexCol + 1);
	float C = GetHeightmap((int)fVertexRow + 1, (int)fVertexCol);
	float D = GetHeightmap((int)fVertexRow + 1, (int)fVertexCol + 1);

	// A정점의 위치에서 떨어진 값(변위값)을 계산한다.
	float fDeltaX = fCellX - fVertexCol;
	float fDeltaZ = fCellZ - fVertexRow;
	// 보간작업을 위한 기준 페이스를 찾는다.
	float fHeight = 0.0f;
	// 윗페이스를 기준으로 보간한다.
	if (fDeltaZ < (1.0f - fDeltaX))
	{
		float uy = B - A; // A -> B
		float vy = C - A; // A -> C
						  // 두 정점의 높이값의 차이를 비교하여 델타X의 값에 따라 보간값을 찾는다.
		fHeight = A + Lerp(0.0f, uy, fDeltaX) + Lerp(0.0f, vy, fDeltaZ);
	}
	// 아래페이스를 기준으로 보간한다.
	else // DCB
	{
		float uy = C - D; // D -> C
		float vy = B - D; // D -> B
						  // 두 정점의 높이값의 차이를 비교하여 델타Z의 값에 따라 보간값을 찾는다.
		fHeight = D + Lerp(0.0f, uy, 1.0f - fDeltaX) + Lerp(0.0f, vy, 1.0 - fDeltaZ);
	}
	return fHeight;
}
float SMap::Lerp(float fStart, float fEnd, float fTangent)
{
	return fStart - (fStart * fTangent) + (fEnd * fTangent);
}
D3DXVECTOR3 SMap::ComputeFaceNormal(DWORD dwIndex0, DWORD dwIndex1, DWORD dwIndex2)
{
	D3DXVECTOR3 vNormal;
	// 노말 벡터를 얻기 위해 얻는 삼각형의 엣지벡터
	// DirectX는 왼손 좌표계를 사용하기 때문에 시계방향으로 외적을 진행하여야 한다.
	D3DXVECTOR3 v0 = m_VertexList[dwIndex1].p - m_VertexList[dwIndex0].p;
	D3DXVECTOR3 v1 = m_VertexList[dwIndex2].p - m_VertexList[dwIndex0].p;

	D3DXVec3Cross(&vNormal, &v0, &v1);
	D3DXVec3Normalize(&vNormal, &vNormal);
	return vNormal;
}
HRESULT SMap::SetInputLayout()
{
	HRESULT hr = S_OK;
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = sizeof(layout) / sizeof(layout[0]);
	m_dxobj.g_pInputlayout.Attach(DXGame::CreateInputlayout(m_pDevice, m_dxobj.g_pVSBlob.Get()->GetBufferSize(),
		m_dxobj.g_pVSBlob.Get()->GetBufferPointer(), layout, numElements));
	return hr;
}
HRESULT SMap::CreateVertexBuffer()
{
	HRESULT hr = S_OK;
	m_dxobj.m_iVertexSize = sizeof(PNCT_VERTEX);
	m_dxobj.m_iNumVertex = m_iNumVertices;
	m_dxobj.g_pVertexBuffer.Attach(DXGame::CreateVertexBuffer(m_pDevice, &m_VertexList.at(0), m_dxobj.m_iNumVertex, m_dxobj.m_iVertexSize));
	return hr;
}
HRESULT SMap::CreateIndexBuffer()
{
	HRESULT hr = S_OK;
	m_dxobj.m_iNumIndex = m_iNumFace * 3;//(m_iNumRows - 1)*(m_iNumCols - 1) * 2 * 3
	m_dxobj.g_pIndexBuffer.Attach(DXGame::CreateIndexBuffer(m_pDevice, &m_IndexList.at(0), m_dxobj.m_iNumIndex, sizeof(DWORD)));
	return hr;
}
bool SMap::CreateVertexData()
{
	m_VertexList.resize(m_iNumVertices);

	// Map의 원점이 되는 정점
	int iHalfCols = m_iNumCols / 2;
	int iHalfRows = m_iNumRows / 2;
	// 정점과 정점 사이의 텍스쳐 좌표 증가값
	float ftxOffsetU = 1.0f / (m_iNumCols - 1);
	float ftxOffsetV = 1.0f / (m_iNumRows - 1);

	for (int iRow = 0; iRow < m_iNumRows; iRow++)
	{
		for (int iCol = 0; iCol < m_iNumCols; iCol++)
		{
			int iVertexIndex = iRow * m_iNumCols + iCol;
			m_VertexList[iVertexIndex].p.x = (iCol - iHalfCols)		* m_fSellDistance;
			m_VertexList[iVertexIndex].p.z = -(( iRow - iHalfRows)	* m_fSellDistance);
			m_VertexList[iVertexIndex].p.y = GetHeightOfVertex(iVertexIndex);
			m_VertexList[iVertexIndex].n = GetNormalOfVertex(iVertexIndex);
			m_VertexList[iVertexIndex].c = GetColorOfVertex(iVertexIndex);
			m_VertexList[iVertexIndex].t = GetTextureOfVertex( ftxOffsetU * iCol, ftxOffsetV * iRow);
		}
	}
	return true;
}
bool SMap::CreateIndexData()
{
	m_IndexList.resize(m_iNumFace * 3);

	int iCurIndex = 0;
	for (int iRow = 0; iRow < m_iNumSellRows; iRow++)
	{
		for (int iCol = 0; iCol < m_iNumSellCols; iCol++)
		{
			//0 1	  4
			//2		3 5
			int iNextRow = iRow + 1;
			int iNextCol = iCol + 1;
			m_IndexList[iCurIndex + 0] = iRow * m_iNumCols + iCol;
			m_IndexList[iCurIndex + 1] = iRow * m_iNumCols + iNextCol;
			m_IndexList[iCurIndex + 2] = iNextRow * m_iNumCols + iCol;
			m_IndexList[iCurIndex + 3] = m_IndexList[iCurIndex + 2];
			m_IndexList[iCurIndex + 4] = m_IndexList[iCurIndex + 1];
			m_IndexList[iCurIndex + 5] = iNextRow * m_iNumCols + iNextCol;

			iCurIndex += 6;
		}
	}
	return true;
}
bool SMap::UpdateBuffer()
{
	//=========================================================================================
	// 페이스 노말 계산 및 이웃 페이스 인덱스 저장하여 정점 노말 계산
	//=========================================================================================
	InitFaceNormals();
	GenNormalLookupTable();
	CalcPerVertexNormalsFastLookup();
	return true;
}
bool SMap::CreateMap(SMapDesc& MapDesc)
{
	// 대용량의 사이즈를 지정한다. (pow(2.0f, 10.0f) + 1 이상)
	if (MapDesc.iNumCols > 1025 || MapDesc.iNumRows > 1025)
	{
		MapDesc.iNumCols = 1025; // 정점개수 105만개,
		MapDesc.iNumRows = 1025; // 페이스 개수 209만개
	}
	m_MapDesc = MapDesc;
	m_iNumRows = MapDesc.iNumCols;
	m_iNumCols = MapDesc.iNumRows;
	m_iNumSellRows = m_iNumRows - 1;
	m_iNumSellCols = m_iNumCols - 1;
	m_iNumVertices = m_iNumRows * m_iNumCols;
	m_iNumFace = m_iNumSellRows * m_iNumSellCols * 2;
	m_fSellDistance = MapDesc.fSellDistance;

	return true;
}

float SMap::GetHeightmap(int row, int col)
{
	return m_VertexList[row * m_iNumRows + col].p.y * m_MapDesc.fScaleHeight;
}
float SMap::GetHeightOfVertex(UINT Index)
{
	return 0.0f;
};
D3DXVECTOR3 SMap::GetNormalOfVertex(UINT Index)
{
	return D3DXVECTOR3(0.0f, 1.0f, 0.0f);
};
D3DXVECTOR4 SMap::GetColorOfVertex(UINT Index)
{
	return D3DXVECTOR4(1, 1, 1, 1.0f);
};
D3DXVECTOR2 SMap::GetTextureOfVertex(float fOffsetX, float fOffsetY)
{
	return D3DXVECTOR2(fOffsetX, fOffsetY);
};
void SMap::CalcVertexColor(D3DXVECTOR3 vLightDir)
{
	//=================================================================================
	// 페이스 노말 계산 및 이웃 페이스 인덱스 저장하여 정점 노말 계산
	//=================================================================================
	for (int iRow = 0; iRow < m_iNumRows; iRow++)
	{
		for (int iCol = 0; iCol < m_iNumCols; iCol++)
		{
			//<vLightDir = 0, -1.0f, 0>
			int iVertexIndex = iRow * m_iNumCols + iCol;
			float fDot =
				D3DXVec3Dot(&-vLightDir, &m_VertexList[iVertexIndex].n);
			m_VertexList[iVertexIndex].c *= fDot;
			m_VertexList[iVertexIndex].c.w = 1.0f;
		}
	}
}
bool SMap::ReLoadVBuffer()
{
	CalcPerVertexNormalsFastLookup();
	m_pContext->UpdateSubresource(m_dxobj.g_pVertexBuffer.Get(), 0, nullptr, m_VertexList.data(), 0, 0);
	return true;
}
bool SMap::ReLoadIBuffer()
{
	m_pContext->UpdateSubresource(m_dxobj.g_pIndexBuffer.Get(), 0, nullptr, m_IndexList.data(), 0, 0);
	return true;
}
void SMap::UpdateIndexBuffer(ID3D11DeviceContext*  pContext, DWORD* pdwIndexArray, int iFaceCount)
{
	assert(pdwIndexArray);
	m_iNumFace = iFaceCount;
	m_dxobj.m_iNumIndex = m_iNumFace * 3;
	pContext->UpdateSubresource(m_dxobj.g_pIndexBuffer.Get(), 0, nullptr, pdwIndexArray, 0, 0);
}
bool SMap::Init(ID3D11Device* pDevice, ID3D11DeviceContext*  pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;
	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_matView);
	D3DXMatrixIdentity(&m_matProj);
	return true;
}
bool SMap::Frame()
{
	return true;
}
bool SMap::Render(ID3D11DeviceContext*  pContext)
{
	PreRender(pContext);
	PostRender(pContext);
	return true;
}
bool SMap::Release()
{
	if (m_pNormalLookupTable)
	{
		free(m_pNormalLookupTable);
		m_pNormalLookupTable = nullptr;
	}
	if (m_pFaceNormals != nullptr)
	{
		delete m_pFaceNormals;
		m_pFaceNormals = nullptr;
	}
	return true;
}
void SMap::InitFaceNormals()
{
	m_pFaceNormals = new D3DXVECTOR3[m_iNumFace];

	for (int iCount = 0; iCount < m_iNumFace; iCount++)
	{
		m_pFaceNormals[iCount] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	}
}
bool SMap::Load(SMapDesc& MapDesc)
{
	if (!CreateMap(MapDesc))
	{
		return false;
	}

	if (!SModel::Create(m_pDevice, m_pContext,  MapDesc.strTextureFile.c_str(), MapDesc.strShaderFile.c_str()))
	{
		return false;
	}

	return true;
}
//==================================================================================
// 각 페이스가 가지고 있는 노말 벡터를 구하는 함수
//==================================================================================
void SMap::CalcFaceNormals()
{
	// 모든 페이스를 순환한다.
	int iFaceNomalNum = 0;
	for (int iIndexCount = 0; iIndexCount < m_iNumFace * 3; iIndexCount += 3)
	{
		DWORD i0 = m_IndexList[iIndexCount];
		DWORD i1 = m_IndexList[iIndexCount + 1];
		DWORD i2 = m_IndexList[iIndexCount + 2];
		m_pFaceNormals[iFaceNomalNum] = ComputeFaceNormal(i0, i1, i2);
		iFaceNomalNum++;
	}
}
//==================================================================================
// Create a face normal lookup table
//==================================================================================
void SMap::GenNormalLookupTable()
{

	if (m_pNormalLookupTable != nullptr)
	{
		free(m_pNormalLookupTable);
		m_pNormalLookupTable = nullptr;
	}

	// 1개의 정점을 공유하는 페이스들은 최대 6개를 공유할 수 있다.
	// 버퍼 사이즈는 각 정점의 갯수에 6을 곱한만큼의 크기로 만들어야
	// 각 정점에 인접하는 페이스를 저장할 수 있다.
	int buffersize = m_iNumRows * m_iNumCols * 6;

	m_pNormalLookupTable = (int*)malloc(sizeof(void*) * buffersize);

	// 해당되지 않는 페이스는 -1로 표기할 것이기 때문에 -1로 초기화 시킨다.
	memset(m_pNormalLookupTable, -1, sizeof(void*) * buffersize);

	
	// 페이스의 개수 * 해당 페이스의 정점 * 정점에 인접하는 페이스의 개수
	for (int iFaceCount = 0; iFaceCount < m_iNumFace; iFaceCount++)
	{
		for (int iFaceVertex = 0; iFaceVertex < 3; iFaceVertex++)
		{
			for (int iVetexFace = 0; iVetexFace < 6; iVetexFace++)
			{
				int vertex = m_IndexList[iFaceCount * 3 + iFaceVertex];
				if (m_pNormalLookupTable[vertex * 6 + iVetexFace] == -1)
				// 해당 정점에 인접한 페이스에 값이 들어가 있지 않을 경우
				{
					m_pNormalLookupTable[vertex * 6 + iVetexFace] = iFaceCount;
					break;
				}
			}
		}
	}
}
//==================================================================================
// Compute vertex normals from the fast normal lookup table
//==================================================================================
void SMap::CalcPerVertexNormalsFastLookup()
{
	// 각 페이스의 노말 벡터를 구한다.
	CalcFaceNormals();

	int iVertexFaceNum = 0;
	// 정점들을 순환한다.
	for (int iVertexNum = 0; iVertexNum < m_iNumVertices; iVertexNum++)
	{
		// 평균 노말을 구하기 위한 벡터
		D3DXVECTOR3 avgNormal;
		avgNormal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

		// 각 정점에 인접한 페이스의 노말 벡터를 확인한다.
		for (iVertexFaceNum = 0; iVertexFaceNum < 6; iVertexFaceNum++)
		{
			int iTriangleIndex;
			iTriangleIndex = m_pNormalLookupTable[iVertexNum * 6 + iVertexFaceNum];

			// If the triangle index is valid, get the normal and average it in.
			if (iTriangleIndex != -1)
			{
				avgNormal += m_pFaceNormals[iTriangleIndex];
			}
			else
				break;
		}
		// 정점에 인접한 페이스 개수 만큼 나누어 정점 노말을 구한다.
		_ASSERT(iVertexFaceNum > 0);
		avgNormal.x /= (float)iVertexFaceNum;
		avgNormal.y /= (float)iVertexFaceNum;
		avgNormal.z /= (float)iVertexFaceNum;
		D3DXVec3Normalize(&avgNormal, &avgNormal);

		// 해당 정점의 노말 벡터를 업데이트 시킨다.
		m_VertexList[iVertexNum].n.x = avgNormal.x;
		m_VertexList[iVertexNum].n.y = avgNormal.y;
		m_VertexList[iVertexNum].n.z = avgNormal.z;
	}
	//==================================================================================
	// 페이스 노말 계산 및  이웃 페이스 인덱스 저장하여 정점 노말 계산
	//==================================================================================
	if (m_bStaticLight) CalcVertexColor(m_vLightDir);
}
SMap::SMap()
{
	m_iNumFace = 0;
	m_iDiffuseTex = 0;
	m_iNumCols = 0;
	m_iNumRows = 0;
	m_pDevice = nullptr;
	m_pNormalLookupTable = nullptr;
	m_pFaceNormals = nullptr;
	m_vLightDir = D3DXVECTOR3(0, -1, 0);
	m_bStaticLight = false;
}
SMap::~SMap()
{
}
