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
	// fPosX / fPosZ�� ��ġ�� �ش��ϴ� ���̸ʼ��� ã�´�.
	// m_iNumCols�� m_iNumRows�� ����/������ ���� ũ�� ����
	float fCellX = (float)((m_iNumSellCols - 1) * m_fSellDistance / 2.0f + fPosX);
	float fCellZ = (float)((m_iNumSellRows - 1) * m_fSellDistance / 2.0f - fPosZ);

	// ���� ũ��� ������ 1������ ������ �ٲپ� ���̸� �迭�� �����Ѵ�.
	fCellX /= (float)m_fSellDistance;
	fCellZ /= (float)m_fSellDistance;

	// fCellX, fCellZ ������ �۰ų� ���� �ִ� ����( �Ҽ��κ��� �߶󳽴�.)
	float fVertexCol = ::floorf(fCellX);
	float fVertexRow = ::floorf(fCellZ);

	// ���̸� ������ ����� ������ �ʱ�ȭ �Ѵ�.
	if (fVertexCol < 0.0f) fVertexCol = 0.0f;
	if (fVertexRow < 0.0f) fVertexRow = 0.0f;
	if ((float)(m_iNumSellCols - 2) < fVertexCol) fVertexCol = (float)(m_iNumSellCols - 2);
	if ((float)(fVertexRow - 2) < fVertexRow) fVertexRow = (float)(fVertexRow - 2);

	// ���� ���� �÷��� �����ϴ� 4�� ������ ���̰��� ã�´�.
	//  A   B
	//  *---*
	//  | / |
	//  *---*  
	//  C   D
	float A = GetHeightmap((int)fVertexRow, (int)fVertexCol);
	float B = GetHeightmap((int)fVertexRow, (int)fVertexCol + 1);
	float C = GetHeightmap((int)fVertexRow + 1, (int)fVertexCol);
	float D = GetHeightmap((int)fVertexRow + 1, (int)fVertexCol + 1);

	// A������ ��ġ���� ������ ��(������)�� ����Ѵ�.
	float fDeltaX = fCellX - fVertexCol;
	float fDeltaZ = fCellZ - fVertexRow;
	// �����۾��� ���� ���� ���̽��� ã�´�.
	float fHeight = 0.0f;
	// �����̽��� �������� �����Ѵ�.
	if (fDeltaZ < (1.0f - fDeltaX))
	{
		float uy = B - A; // A -> B
		float vy = C - A; // A -> C
						  // �� ������ ���̰��� ���̸� ���Ͽ� ��ŸX�� ���� ���� �������� ã�´�.
		fHeight = A + Lerp(0.0f, uy, fDeltaX) + Lerp(0.0f, vy, fDeltaZ);
	}
	// �Ʒ����̽��� �������� �����Ѵ�.
	else // DCB
	{
		float uy = C - D; // D -> C
		float vy = B - D; // D -> B
						  // �� ������ ���̰��� ���̸� ���Ͽ� ��ŸZ�� ���� ���� �������� ã�´�.
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
	// �븻 ���͸� ��� ���� ��� �ﰢ���� ��������
	// DirectX�� �޼� ��ǥ�踦 ����ϱ� ������ �ð�������� ������ �����Ͽ��� �Ѵ�.
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

	// Map�� ������ �Ǵ� ����
	int iHalfCols = m_iNumSellCols / 2;
	int iHalfRows = m_iNumSellRows / 2;
	// ������ ���� ������ �ؽ��� ��ǥ ������
	float ftxOffsetU = 1.0f / (m_iNumSellCols - 1);
	float ftxOffsetV = 1.0f / (m_iNumSellRows - 1);

	for (int iRow = 0; iRow < m_iNumSellRows; iRow++)
	{
		for (int iCol = 0; iCol < m_iNumSellCols; iCol++)
		{
			int iVertexIndex = iRow * m_iNumSellCols + iCol;
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
	for (int iRow = 0; iRow < m_iNumSellRows - 1; iRow++)
	{
		for (int iCol = 0; iCol < m_iNumSellCols - 1; iCol++)
		{
			//0 1	  4
			//2		3 5
			int iNextRow = iRow + 1;
			int iNextCol = iCol + 1;
			m_IndexList[iCurIndex + 0] = iRow * m_iNumSellCols + iCol;
			m_IndexList[iCurIndex + 1] = iRow * m_iNumSellCols + iNextCol;
			m_IndexList[iCurIndex + 2] = iNextRow * m_iNumSellCols + iCol;
			m_IndexList[iCurIndex + 3] = m_IndexList[iCurIndex + 2];
			m_IndexList[iCurIndex + 4] = m_IndexList[iCurIndex + 1];
			m_IndexList[iCurIndex + 5] = iNextRow * m_iNumSellCols + iNextCol;

			iCurIndex += 6;
		}
	}
	return true;
}
HRESULT SMap::CreateConstantBuffer()
{
	SModel::CreateConstantBuffer();
	SCBTile cbBuff;
	ZeroMemory(&cbBuff, sizeof(SCBTile));
	m_CBTileNum.Attach(DXGame::CreateConstantBuffer(m_pDevice, &cbBuff, 1, sizeof(SCBTile)));
	return S_OK;
}
void SMap::UpdateConstantBuffer(ID3D11DeviceContext* pContext, SModel* pParent)
{
	SModel::UpdateConstantBuffer( pContext, pParent);
	ID3D11Buffer* CSBuffers[2] = {
		m_dxobj.g_pConstantBuffer.Get(),
		m_CBTileNum.Get()
	};
	pContext->UpdateSubresource(m_CBTileNum.Get(), 0, NULL, &m_iNumRows, 0, 0);
	pContext->VSSetConstantBuffers(0, 2, CSBuffers);
	pContext->PSSetConstantBuffers(0, 2, CSBuffers);
}

bool SMap::UpdateBuffer()
{
	//=========================================================================================
	// ���̽� �븻 ��� �� �̿� ���̽� �ε��� �����Ͽ� ���� �븻 ���
	//=========================================================================================
	InitFaceNormals();
	GenNormalLookupTable();
	CalcPerVertexNormalsFastLookup();
	return true;
}
bool SMap::CreateMap(SMapDesc& MapDesc)
{
	// ��뷮�� ����� �����Ѵ�. (pow(2.0f, 10.0f) + 1 �̻�)
	if (MapDesc.iNumCols > 1025 || MapDesc.iNumRows > 1025)
	{
		MapDesc.iNumCols = 1025; // �������� 105����,
		MapDesc.iNumRows = 1025; // ���̽� ���� 209����
	}
	m_MapDesc = MapDesc;
	m_iNumRows = MapDesc.iNumCols;
	m_iNumCols = MapDesc.iNumRows;
	m_iSellNum = MapDesc.iNumSell;

	m_iNumSellRows = m_iNumRows * m_iSellNum + 1;
	m_iNumSellCols = m_iNumCols * m_iSellNum + 1;

	m_iNumVertices = m_iNumSellRows * m_iNumSellCols;
	m_iNumFace = (m_iNumSellRows - 1) * (m_iNumSellCols - 1) * 2;
	m_fSellDistance = MapDesc.fSellDistance;

	return true;
}

float SMap::GetHeightmap(int row, int col)
{
	return m_VertexList[row * m_iNumSellRows + col].p.y * m_MapDesc.fScaleHeight;
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
	// ���̽� �븻 ��� �� �̿� ���̽� �ε��� �����Ͽ� ���� �븻 ���
	//=================================================================================
	for (int iRow = 0; iRow < m_iNumSellRows; iRow++)
	{
		for (int iCol = 0; iCol < m_iNumSellCols; iCol++)
		{
			//<vLightDir = 0, -1.0f, 0>
			int iVertexIndex = iRow * m_iNumSellCols + iCol;
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
	RenderSetTexture(pContext);
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
void SMap::SetTexture(ID3D11Device* pDevice, T_STR strBaseTexture, T_STR strAlphaTex0, T_STR strAlphaTex1, T_STR strAlphaTex2, ID3D11ShaderResourceView* pAlpha)
{
	if (pDevice == nullptr) return;
	int key = I_TextureManager.Load(pDevice, strAlphaTex0);
	if (key >= 0) m_pSplatTexture0 = I_TextureManager.GetPtr(key);

	key = I_TextureManager.Load(pDevice, strAlphaTex1);
	if (key >= 0) m_pSplatTexture1 = I_TextureManager.GetPtr(key);

	key = I_TextureManager.Load(pDevice, strAlphaTex2);
	if (key >= 0) m_pSplatTexture2 = I_TextureManager.GetPtr(key);

	key = I_TextureManager.Load(pDevice, strBaseTexture);
	if (key >= 0)
	{
		m_pBaseTexture = I_TextureManager.GetPtr(key);
		m_dxobj.g_pTextureSRV = m_pBaseTexture->m_pSRV;
	}
	if(pAlpha) m_pAlphaSRV = pAlpha;
}
void SMap::RenderSetTexture(ID3D11DeviceContext*  pContext)
{
	ID3D11ShaderResourceView* pSRVArray[5] = { nullptr, };

	pSRVArray[0] = m_dxobj.g_pTextureSRV;
	pSRVArray[1] = m_pAlphaSRV;
	if (m_pSplatTexture0) pSRVArray[2] = m_pSplatTexture0->m_pSRV;
	if (m_pSplatTexture1) pSRVArray[3] = m_pSplatTexture1->m_pSRV;
	if (m_pSplatTexture2) pSRVArray[4] = m_pSplatTexture2->m_pSRV;

	pContext->PSSetShaderResources(0, 5, pSRVArray);
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
// �� ���̽��� ������ �ִ� �븻 ���͸� ���ϴ� �Լ�
//==================================================================================
void SMap::CalcFaceNormals()
{
	// ��� ���̽��� ��ȯ�Ѵ�.
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

	// 1���� ������ �����ϴ� ���̽����� �ִ� 6���� ������ �� �ִ�.
	// ���� ������� �� ������ ������ 6�� ���Ѹ�ŭ�� ũ��� ������
	// �� ������ �����ϴ� ���̽��� ������ �� �ִ�.
	int buffersize = m_iNumSellRows * m_iNumSellCols * 6;

	m_pNormalLookupTable = (int*)malloc(sizeof(void*) * buffersize);

	// �ش���� �ʴ� ���̽��� -1�� ǥ���� ���̱� ������ -1�� �ʱ�ȭ ��Ų��.
	memset(m_pNormalLookupTable, -1, sizeof(void*) * buffersize);

	
	// ���̽��� ���� * �ش� ���̽��� ���� * ������ �����ϴ� ���̽��� ����
	for (int iFaceCount = 0; iFaceCount < m_iNumFace; iFaceCount++)
	{
		for (int iFaceVertex = 0; iFaceVertex < 3; iFaceVertex++)
		{
			for (int iVetexFace = 0; iVetexFace < 6; iVetexFace++)
			{
				int vertex = m_IndexList[iFaceCount * 3 + iFaceVertex];
				if (m_pNormalLookupTable[vertex * 6 + iVetexFace] == -1)
				// �ش� ������ ������ ���̽��� ���� �� ���� ���� ���
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
	// �� ���̽��� �븻 ���͸� ���Ѵ�.
	CalcFaceNormals();

	int iVertexFaceNum = 0;
	// �������� ��ȯ�Ѵ�.
	for (int iVertexNum = 0; iVertexNum < m_iNumVertices; iVertexNum++)
	{
		// ��� �븻�� ���ϱ� ���� ����
		D3DXVECTOR3 avgNormal;
		avgNormal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

		// �� ������ ������ ���̽��� �븻 ���͸� Ȯ���Ѵ�.
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
		// ������ ������ ���̽� ���� ��ŭ ������ ���� �븻�� ���Ѵ�.
		_ASSERT(iVertexFaceNum > 0);
		avgNormal.x /= (float)iVertexFaceNum;
		avgNormal.y /= (float)iVertexFaceNum;
		avgNormal.z /= (float)iVertexFaceNum;
		D3DXVec3Normalize(&avgNormal, &avgNormal);

		// �ش� ������ �븻 ���͸� ������Ʈ ��Ų��.
		m_VertexList[iVertexNum].n.x = avgNormal.x;
		m_VertexList[iVertexNum].n.y = avgNormal.y;
		m_VertexList[iVertexNum].n.z = avgNormal.z;
	}
	//==================================================================================
	// ���̽� �븻 ��� ��  �̿� ���̽� �ε��� �����Ͽ� ���� �븻 ���
	//==================================================================================
	if (m_bStaticLight) CalcVertexColor(m_vLightDir);
}

bool SMap::LoadFile(T_STR FileString)
{
	bool bCheck;
	if (m_pDevice == nullptr) return false;
	if (m_pContext == nullptr) return false;

	static TCHAR String[MAX_PATH];
	ZeroMemory(String, _countof(String));

	m_Paser.OpenStream(FileString.c_str());

	bCheck = m_Paser.GetDataFromFileNext(L"MapExporter100");
	if (bCheck == false) return false;

	bCheck = m_Paser.GetDataFromFileNext(L"#HEADERINFO");
	if (bCheck == false) return false;

	_stscanf_s(m_Paser.m_pBuffer, _T("\n%s %d %d %d %d %d %f"), String, MAX_PATH,
		&m_iNumRows,
		&m_iNumCols,
		&m_iSellNum,
		&m_iNumSellRows,
		&m_iNumSellCols,
		&m_fSellDistance);
	ZeroMemory(String, _countof(String));
	m_iNumFace = (m_iNumSellRows - 1) * (m_iNumSellCols - 1) * 2;

	bCheck = m_Paser.GetDataFromFileNext(L"#TEXTURE", String, STRING_DATA);
	if (bCheck == false) return false;
	m_MapDesc.strTextureFile = String;

	LoadTextures(m_pDevice, m_MapDesc.strTextureFile.c_str());

	ZeroMemory(String, _countof(String));

	bCheck = m_Paser.GetDataFromFileNext(L"#VERTEX", &m_iNumVertices, INT_DATA);
	if (bCheck == false) return false;
	m_VertexList.resize(m_iNumVertices);


	for (int iVertex = 0; iVertex < m_iNumVertices; iVertex++)
	{
		PNCT_VERTEX& Vertex = m_VertexList[iVertex];
		m_Paser.GetNextLine();
		_stscanf_s(m_Paser.m_pBuffer, _T("%f %f %f %f %f %f %f %f %f %f %f %f"),
			&Vertex.p.x,
			&Vertex.p.y,
			&Vertex.p.z,
			&Vertex.n.x,
			&Vertex.n.y,
			&Vertex.n.z,
			&Vertex.c.x,
			&Vertex.c.y,
			&Vertex.c.z,
			&Vertex.c.w,
			&Vertex.t.x,
			&Vertex.t.y);
	}
	ZeroMemory(String, _countof(String));

	int iMaxIndex = 0;
	bCheck = m_Paser.GetDataFromFileNext(L"#INDEX", &iMaxIndex, INT_DATA);
	if (bCheck == false) return false;
	ZeroMemory(String, _countof(String));

	m_IndexList.resize(iMaxIndex);
	for (int iIndex = 0; iIndex < iMaxIndex; iIndex += 3)
	{
		m_Paser.GetNextLine();
		_stscanf_s(m_Paser.m_pBuffer, _T("%d %d %d"),
			&m_IndexList[iIndex + 0],
			&m_IndexList[iIndex + 1],
			&m_IndexList[iIndex + 2]);
	}

	CreateIndexBuffer();
	CreateVertexBuffer();
	CreateConstantBuffer();
	UpdateBuffer();
	m_Paser.CloseStream();

	MessageBox(GetActiveWindow(), FileString.c_str(),
		_T("Load Succeed!"), MB_OK);
	return true;
}
bool SMap::SaveFile(T_STR FileString)
{
	FILE* pStream = nullptr;
	if (pStream == nullptr) return false;

	_ftprintf(pStream, _T("\n%s %d %d %d %d %d %f"),
		L"#HEADERINFO",
		m_iNumRows,
		m_iNumCols,
		m_iSellNum,
		m_iNumSellRows,
		m_iNumSellCols,
		m_fSellDistance);

	_ftprintf(pStream, _T("\n%s %s"),
		L"#TEXTURE",
		m_MapDesc.strTextureFile.c_str());

	_ftprintf(pStream, _T("\n%s %d"),
		L"#VERTEX",
		m_iNumVertices);
	for (auto& Vertex : m_VertexList)
	{
		_ftprintf(pStream, _T("\n%10.4f %10.4f %10.4f"),
			Vertex.p.x,
			Vertex.p.y,
			Vertex.p.z);
		_ftprintf(pStream, _T("%10.4f %10.4f %10.4f"),
			Vertex.n.x,
			Vertex.n.y,
			Vertex.n.z);
		_ftprintf(pStream, _T("%10.4f %10.4f %10.4f %10.4f"),
			Vertex.c.x,
			Vertex.c.y,
			Vertex.c.z,
			Vertex.c.w);
		_ftprintf(pStream, _T("%10.4f %10.4f"),
			Vertex.t.x,
			Vertex.t.y);
	}

	_ftprintf(pStream, _T("\n%s %d"),
		L"#INDEX",
		m_IndexList.size());

	for (int iIndex = 0; iIndex < m_IndexList.size(); iIndex += 3)
	{
		_ftprintf(pStream, _T("\n%d %d %d"),
			m_IndexList[iIndex + 0],
			m_IndexList[iIndex + 1],
			m_IndexList[iIndex + 2]);
	}

	return true;
}
SMap::SMap()
{
	m_pSplatTexture0 = nullptr;
	m_pSplatTexture1 = nullptr;
	m_pSplatTexture2 = nullptr;
	m_pAlphaSRV = nullptr;

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
