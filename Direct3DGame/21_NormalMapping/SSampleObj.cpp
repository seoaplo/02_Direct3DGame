#include "SSampleObj.h"

void SSampleObj::SetMatrix(const D3DXMATRIX* pWorld, const D3DXMATRIX* pView, const D3DXMATRIX* pProj)
{
	if (pWorld != NULL)
	{
		m_matWorld = *pWorld;
		m_vCenter.x = pWorld->_41;
		m_vCenter.y = pWorld->_42;
		m_vCenter.z = pWorld->_43;
	}
	D3DXMatrixInverse(&m_matNormal, 0, &m_matWorld);
	if (pView)
	{
		m_matView = *pView;
	}
	if (pProj)
	{
		m_matProj = *pProj;
	}
	D3DXMatrixTranspose(&m_cbData.matWorld, &m_matWorld);
	D3DXMatrixTranspose(&m_cbData.matView, &m_matView);
	D3DXMatrixTranspose(&m_cbData.matProj, &m_matProj);
}
HRESULT SSampleObj::LoadTextures(ID3D11Device* pDevice, const TCHAR* pLoadTextureString)
{
	HRESULT hr = S_OK;
	m_dxobj.g_pTextureSRV.Attach(DXGame::CreateShaderResourceView(pDevice, pLoadTextureString));

	m_itxNormalMap = I_TextureManager.Load(pDevice, const_cast<TCHAR*>(m_strNormalMapName.c_str()));
	m_pNormalTexture = I_TextureManager.GetPtr(m_itxNormalMap);
	return hr;
}
HRESULT SSampleObj::SetInputLayout()
{
	HRESULT hr;

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 12,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,       0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT",	0, DXGI_FORMAT_R32G32B32_FLOAT,	   0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT numElements = sizeof(layout) / sizeof(layout[0]);
	m_dxobj.g_pInputlayout.Attach(DXGame::CreateInputlayout(m_pDevice, m_dxobj.g_pVSBlob.Get()->GetBufferSize(),
		m_dxobj.g_pVSBlob.Get()->GetBufferPointer(), layout, numElements));
	return S_OK;
}

HRESULT SSampleObj::CreateVertexBuffer()
{
	HRESULT hr;
	// »ó´Ü
	// 5    6
	// 1    2
	// ÇÏ´Ü
	// 4    7
	// 0    3  
	// ¾Õ¸é
	m_VertexList.resize(24);
	m_VertexList[0] = PNCT2_VERTEX(D3DXVECTOR3(-1.0f, 1.0f, -1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	m_VertexList[1] = PNCT2_VERTEX(D3DXVECTOR3(1.0f, 1.0f, -1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f), D3DXVECTOR2(1.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	m_VertexList[2] = PNCT2_VERTEX(D3DXVECTOR3(1.0f, -1.0f, -1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f), D3DXVECTOR2(1.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	m_VertexList[3] = PNCT2_VERTEX(D3DXVECTOR3(-1.0f, -1.0f, -1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f), D3DXVECTOR2(0.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	// µÞ¸é
	m_VertexList[4] = PNCT2_VERTEX(D3DXVECTOR3(1.0f, 1.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), D3DXVECTOR4(0.0f, 0.0f, 0.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	m_VertexList[5] = PNCT2_VERTEX(D3DXVECTOR3(-1.0f, 1.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f), D3DXVECTOR2(1.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	m_VertexList[6] = PNCT2_VERTEX(D3DXVECTOR3(-1.0f, -1.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f), D3DXVECTOR2(1.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	m_VertexList[7] = PNCT2_VERTEX(D3DXVECTOR3(1.0f, -1.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f), D3DXVECTOR2(0.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	// ¿À¸¥ÂÊ
	m_VertexList[8] = PNCT2_VERTEX(D3DXVECTOR3(1.0f, 1.0f, -1.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	m_VertexList[9] = PNCT2_VERTEX(D3DXVECTOR3(1.0f, 1.0f, 1.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f), D3DXVECTOR2(1.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	m_VertexList[10] = PNCT2_VERTEX(D3DXVECTOR3(1.0f, -1.0f, 1.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f), D3DXVECTOR2(1.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	m_VertexList[11] = PNCT2_VERTEX(D3DXVECTOR3(1.0f, -1.0f, -1.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	// ¿ÞÂÊ
	m_VertexList[12] = PNCT2_VERTEX(D3DXVECTOR3(-1.0f, 1.0f, 1.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), D3DXVECTOR4(1.0f, 1.0f, 0.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	m_VertexList[13] = PNCT2_VERTEX(D3DXVECTOR3(-1.0f, 1.0f, -1.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), D3DXVECTOR4(1.0f, 1.0f, 0.0f, 1.0f), D3DXVECTOR2(1.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	m_VertexList[14] = PNCT2_VERTEX(D3DXVECTOR3(-1.0f, -1.0f, -1.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), D3DXVECTOR4(1.0f, 1.0f, 0.0f, 1.0f), D3DXVECTOR2(1.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	m_VertexList[15] = PNCT2_VERTEX(D3DXVECTOR3(-1.0f, -1.0f, 1.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), D3DXVECTOR4(1.0f, 1.0f, 0.0f, 1.0f), D3DXVECTOR2(0.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	// À­¸é
	m_VertexList[16] = PNCT2_VERTEX(D3DXVECTOR3(-1.0f, 1.0f, 1.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR4(1.0f, 0.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	m_VertexList[17] = PNCT2_VERTEX(D3DXVECTOR3(1.0f, 1.0f, 1.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR4(1.0f, 0.0f, 1.0f, 1.0f), D3DXVECTOR2(1.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	m_VertexList[18] = PNCT2_VERTEX(D3DXVECTOR3(1.0f, 1.0f, -1.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR4(1.0f, 0.0f, 1.0f, 1.0f), D3DXVECTOR2(1.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	m_VertexList[19] = PNCT2_VERTEX(D3DXVECTOR3(-1.0f, 1.0f, -1.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR4(1.0f, 0.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	// ¾Æ·§¸é
	m_VertexList[20] = PNCT2_VERTEX(D3DXVECTOR3(-1.0f, -1.0f, -1.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR4(0.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	m_VertexList[21] = PNCT2_VERTEX(D3DXVECTOR3(1.0f, -1.0f, -1.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR4(0.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(1.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	m_VertexList[22] = PNCT2_VERTEX(D3DXVECTOR3(1.0f, -1.0f, 1.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR4(0.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(1.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	m_VertexList[23] = PNCT2_VERTEX(D3DXVECTOR3(-1.0f, -1.0f, 1.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR4(0.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	UINT iNumVertex = m_VertexList.size();
	m_dxobj.m_iVertexSize = sizeof(PNCT2_VERTEX);
	m_dxobj.m_iNumVertex = iNumVertex;
	return S_OK;
}
HRESULT SSampleObj::CreateIndexBuffer()
{
	HRESULT hr = S_OK;
	m_IndexList.resize(36);
	int iIndex = 0;
	m_IndexList[iIndex++] = 0; 	m_IndexList[iIndex++] = 1; 	m_IndexList[iIndex++] = 2; 	m_IndexList[iIndex++] = 0;	m_IndexList[iIndex++] = 2; 	m_IndexList[iIndex++] = 3;
	m_IndexList[iIndex++] = 4; 	m_IndexList[iIndex++] = 5; 	m_IndexList[iIndex++] = 6; 	m_IndexList[iIndex++] = 4;	m_IndexList[iIndex++] = 6; 	m_IndexList[iIndex++] = 7;
	m_IndexList[iIndex++] = 8; 	m_IndexList[iIndex++] = 9; 	m_IndexList[iIndex++] = 10; m_IndexList[iIndex++] = 8;	m_IndexList[iIndex++] = 10; m_IndexList[iIndex++] = 11;
	m_IndexList[iIndex++] = 12; m_IndexList[iIndex++] = 13; m_IndexList[iIndex++] = 14; m_IndexList[iIndex++] = 12;	m_IndexList[iIndex++] = 14; m_IndexList[iIndex++] = 15;
	m_IndexList[iIndex++] = 16; m_IndexList[iIndex++] = 17; m_IndexList[iIndex++] = 18; m_IndexList[iIndex++] = 16;	m_IndexList[iIndex++] = 18; m_IndexList[iIndex++] = 19;
	m_IndexList[iIndex++] = 20; m_IndexList[iIndex++] = 21; m_IndexList[iIndex++] = 22; m_IndexList[iIndex++] = 20;	m_IndexList[iIndex++] = 22; m_IndexList[iIndex++] = 23;

	UINT iNumIndex = m_IndexList.size();
	m_dxobj.m_iNumIndex = iNumIndex;
	m_dxobj.g_pIndexBuffer.Attach(DXGame::CreateIndexBuffer(m_pDevice, &m_IndexList.at(0), m_dxobj.m_iNumIndex, sizeof(DWORD)));
	return hr;
}
//--------------------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------------------
HRESULT SSampleObj::CreateResource()
{
	m_dxobj.m_iPrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	return S_OK;
}
bool SSampleObj::UpdateBuffer()
{
	//--------------------------------------------------------------------------------------
	// Á¢¼±º¤ÅÍ °è»ê
	//--------------------------------------------------------------------------------------	
	D3DXVECTOR3 vTangent, vBiNormal, vNormal;
	int iIndex = 0;
	int i0, i1, i2, i3, i4, i5;

	for (int i = 0; i < m_dxobj.m_iNumVertex; i += 4)
	{
		i0 = m_IndexList[iIndex + 0];
		i1 = m_IndexList[iIndex + 1];
		i2 = m_IndexList[iIndex + 2];
		i3 = m_IndexList[iIndex + 3];
		i4 = m_IndexList[iIndex + 4];
		i5 = m_IndexList[iIndex + 5];
		m_NormalMap.CreateTangentSpaceVectors(&m_VertexList[i0].p, &m_VertexList[i1].p, &m_VertexList[i2].p,
			m_VertexList[i0].t.x, m_VertexList[i0].t.y,
			m_VertexList[i1].t.x, m_VertexList[i1].t.y,
			m_VertexList[i2].t.x, m_VertexList[i2].t.y,
			&vTangent, &vBiNormal, &vNormal);
		//m_VertexList[i+0].n  = vNormal;
		m_VertexList[i + 0].vTangent = vTangent;

		m_NormalMap.CreateTangentSpaceVectors(&m_VertexList[i1].p, &m_VertexList[i2].p, &m_VertexList[i0].p,
			m_VertexList[i1].t.x, m_VertexList[i1].t.y,
			m_VertexList[i2].t.x, m_VertexList[i2].t.y,
			m_VertexList[i0].t.x, m_VertexList[i0].t.y,
			&vTangent, &vBiNormal, &vNormal);
		//vertices[i+1].n  = vNormal;
		m_VertexList[i + 1].vTangent = vTangent;
		m_NormalMap.CreateTangentSpaceVectors(&m_VertexList[i2].p, &m_VertexList[i0].p, &m_VertexList[i1].p,
			m_VertexList[i2].t.x, m_VertexList[m_IndexList[2]].t.y,
			m_VertexList[i0].t.x, m_VertexList[m_IndexList[0]].t.y,
			m_VertexList[i1].t.x, m_VertexList[m_IndexList[1]].t.y,
			&vTangent, &vBiNormal, &vNormal);
		//m_VertexList[i+2].n  = vNormal;
		m_VertexList[i + 2].vTangent = vTangent;

		m_NormalMap.CreateTangentSpaceVectors(&m_VertexList[i5].p, &m_VertexList[i3].p, &m_VertexList[i4].p,
			m_VertexList[i5].t.x, m_VertexList[i5].t.y,
			m_VertexList[i3].t.x, m_VertexList[i3].t.y,
			m_VertexList[i4].t.x, m_VertexList[i4].t.y,
			&vTangent, &vBiNormal, &vNormal);
		//m_VertexList[i+3].n  = vNormal;
		m_VertexList[i + 3].vTangent = vTangent;

		iIndex += 6;
	}


	m_dxobj.g_pVertexBuffer.Attach(DXGame::CreateVertexBuffer(m_pDevice, &m_VertexList.at(0),
		m_dxobj.m_iNumVertex, m_dxobj.m_iVertexSize));

	return true;
}

bool SSampleObj::Render(ID3D11DeviceContext*  pContext)
{
	PreRender(pContext);
	PostRender(pContext);
	return true;
}

SSampleObj::SSampleObj()
{
}


SSampleObj::~SSampleObj()
{
}
