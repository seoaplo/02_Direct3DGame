#include "SDXShape.h"

#pragma region SDXShape
void SDXShape::CreateOBBBox(float fExtentX, float fExtentY, float fExtentZ,
	D3DXVECTOR3 vCenter,
	D3DXVECTOR3 vDirX,
	D3DXVECTOR3 vDirY,
	D3DXVECTOR3 vDirZ)
{
	// OBB Size Set
	m_Box.fExtent[0] = fExtentX;
	m_Box.fExtent[1] = fExtentY;
	m_Box.fExtent[2] = fExtentZ;
	
	m_Box.vCenter = vCenter;

	// OBB Axis Set
	m_Box.vAxis[0] = m_Box.fExtent[0] * vDirX;
	m_Box.vAxis[1] = m_Box.fExtent[1] * vDirY;
	m_Box.vAxis[2] = m_Box.fExtent[2] * vDirZ;

	// AABB Position Set
	m_Box.vPos[0] = vCenter - m_Box.vAxis[0] - m_Box.vAxis[1] - m_Box.vAxis[2];
	m_Box.vPos[1] = vCenter - m_Box.vAxis[0] + m_Box.vAxis[1] - m_Box.vAxis[2];
	m_Box.vPos[2] = vCenter + m_Box.vAxis[0] + m_Box.vAxis[1] - m_Box.vAxis[2];
	m_Box.vPos[3] = vCenter + m_Box.vAxis[0] - m_Box.vAxis[1] - m_Box.vAxis[2];
	m_Box.vPos[4] = vCenter - m_Box.vAxis[0] - m_Box.vAxis[1] + m_Box.vAxis[2];
	m_Box.vPos[5] = vCenter - m_Box.vAxis[0] + m_Box.vAxis[1] + m_Box.vAxis[2];
	m_Box.vPos[6] = vCenter + m_Box.vAxis[0] + m_Box.vAxis[1] + m_Box.vAxis[2];
	m_Box.vPos[7] = vCenter + m_Box.vAxis[0] - m_Box.vAxis[1] + m_Box.vAxis[2];

	// AABB Min, Max Set
	m_Box.vMax = m_Box.vPos[0];
	m_Box.vMin = m_Box.vPos[0];

	for (int iPoint = 1; iPoint < 8; iPoint++)
	{
		if (m_Box.vMax.x < m_Box.vPos[iPoint].x)
		{
			m_Box.vMax.x = m_Box.vPos[iPoint].x;
		}
		if (m_Box.vMax.y < m_Box.vPos[iPoint].y)
		{
			m_Box.vMax.y = m_Box.vPos[iPoint].y;
		}
		if (m_Box.vMax.z < m_Box.vPos[iPoint].z)
		{
			m_Box.vMax.z = m_Box.vPos[iPoint].z;
		}

		if (m_Box.vMin.x > m_Box.vPos[iPoint].x)
		{
			m_Box.vMin.x = m_Box.vPos[iPoint].x;
		}
		if (m_Box.vMin.y > m_Box.vPos[iPoint].y)
		{
			m_Box.vMin.y = m_Box.vPos[iPoint].y;
		}
		if (m_Box.vMin.z > m_Box.vPos[iPoint].z)
		{
			m_Box.vMin.z = m_Box.vPos[iPoint].z;
		}
	}

	// OBB Axis Nomalize
	D3DXVec3Normalize(&m_Box.vAxis[0], &m_Box.vAxis[0]);
	D3DXVec3Normalize(&m_Box.vAxis[1], &m_Box.vAxis[1]);
	D3DXVec3Normalize(&m_Box.vAxis[2], &m_Box.vAxis[2]);
}

void SDXShape::CreateAABBBox(D3DXVECTOR3 max, D3DXVECTOR3 min)
{
	// AABB Min, Max Set
	m_Box.vMax = max;
	m_Box.vMin = min;

	// AABB Position Set
	m_Box.vPos[0] = D3DXVECTOR3(min.x, min.y, min.z);
	m_Box.vPos[1] = D3DXVECTOR3(min.x, max.y, min.z);
	m_Box.vPos[2] = D3DXVECTOR3(max.x, max.y, min.z);
	m_Box.vPos[3] = D3DXVECTOR3(max.x, min.y, min.z);

	m_Box.vPos[4] = D3DXVECTOR3(min.x, min.y, max.z);
	m_Box.vPos[5] = D3DXVECTOR3(min.x, max.y, max.z);
	m_Box.vPos[6] = D3DXVECTOR3(max.x, max.y, max.z);
	m_Box.vPos[7] = D3DXVECTOR3(max.x, min.y, max.z);

	// OBB Set
	m_Box.vCenter = (max + min) / 2.0f;
	m_Box.vAxis[0] = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	m_Box.vAxis[1] = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	m_Box.vAxis[2] = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
}
#pragma endregion

#pragma region SPoint
HRESULT SPoint::SetInputLayout()
{
	HRESULT hr = S_OK;

	// 
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = sizeof(layout) / sizeof(layout[0]);
	m_dxobj.g_pInputlayout.Attach(DXGame::CreateInputlayout(m_pDevice, m_dxobj.g_pVSBlob.Get()->GetBufferSize(),
		m_dxobj.g_pVSBlob.Get()->GetBufferPointer(), layout, numElements));
	return hr;
}
bool SPoint::CreateVertexData()
{
	m_vPosition.p = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_vPosition.c = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);

	m_dxobj.m_iVertexSize = sizeof(PC_VERTEX);
	m_dxobj.m_iNumVertex = 1;
	return true;
}
bool SPoint::CreateIndexData()
{
	m_IndexList.resize(1);
	m_IndexList[0] = 0;

	m_dxobj.m_iNumIndex = m_IndexList.size();
	m_dxobj.m_iIndexSize = sizeof(DWORD);
	return true;
}
HRESULT SPoint::CreateVertexBuffer()
{
	if (m_dxobj.m_iNumVertex <= 0) return S_OK;
	m_dxobj.g_pVertexBuffer.Attach(DXGame::CreateVertexBuffer(m_pDevice,
		&m_vPosition,
		m_dxobj.m_iNumVertex,
		m_dxobj.m_iVertexSize));
	if (m_dxobj.g_pVertexBuffer == nullptr) return S_FALSE;
	return S_OK;
}
HRESULT SPoint::LoadShaderFile(ID3D11Device* pDevice, const TCHAR* pLoadShaderFile)
{
	T_STR ShaderFile;
	if (pLoadShaderFile == nullptr) ShaderFile = L"../../shader/Shape/Line.hlsl";
	else							ShaderFile = pLoadShaderFile;

	m_dxobj.g_pVertexShader.Attach(DXGame::LoadVertexShaderFile(pDevice, ShaderFile.c_str(), m_dxobj.g_pVSBlob.GetAddressOf()));
	m_dxobj.g_pPixelShader.Attach(DXGame::LoadPixelShaderFile(pDevice, ShaderFile.c_str()));
	m_dxobj.g_pGeometryShader.Attach(DXGame::LoadGeometryShaderFile(pDevice, ShaderFile.c_str(), m_dxobj.g_pGSBlob.GetAddressOf()));
	m_dxobj.g_pHullShader.Attach(DXGame::LoadHullShaderFile(pDevice, ShaderFile.c_str(), m_dxobj.g_pHSBlob.GetAddressOf()));
	m_dxobj.g_pDomainShader.Attach(DXGame::LoadDomainShaderFile(pDevice, ShaderFile.c_str(), m_dxobj.g_pDSBlob.GetAddressOf()));
	m_dxobj.g_pComputeShader.Attach(DXGame::LoadComputeShaderFile(pDevice, ShaderFile.c_str(), m_dxobj.g_pCSBlob.GetAddressOf()));
	return S_OK;
}

bool SPoint::Draw(ID3D11DeviceContext* pContext, D3DXVECTOR3 vPosition, D3DXVECTOR4 vColor)
{
	PC_VERTEX vertices;
	vertices.p = vPosition;
	vertices.c = vColor;
	// 동적 리소스 갱신 방법 1 : D3D11_USAGE_DEFAULT 사용
	pContext->UpdateSubresource(GetVB(), 0, NULL, &vertices, 0, 0);
	return Render(pContext);
}
//--------------------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------------------
HRESULT SPoint::CreateResource()
{
	m_dxobj.m_iPrimitiveType = D3D10_PRIMITIVE_TOPOLOGY_POINTLIST;
	return S_OK;
}
SPoint::SPoint()
{

}
SPoint::~SPoint()
{

}
#pragma endregion

#pragma region SLine
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                            TLineShape
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT SLine::SetInputLayout()
{
	HRESULT hr = S_OK;

	// 
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = sizeof(layout) / sizeof(layout[0]);
	m_dxobj.g_pInputlayout.Attach(DXGame::CreateInputlayout(m_pDevice, m_dxobj.g_pVSBlob.Get()->GetBufferSize(),
		m_dxobj.g_pVSBlob.Get()->GetBufferPointer(), layout, numElements));
	return hr;
}
bool SLine::CreateVertexData()
{
	m_LineVertexList.resize(2);
	m_LineVertexList[0].p = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_LineVertexList[0].c = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);
	m_LineVertexList[1].p = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	m_LineVertexList[1].c = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);

	m_dxobj.m_iVertexSize = sizeof(PC_VERTEX);
	m_dxobj.m_iNumVertex = m_LineVertexList.size();
	return true;
}
bool SLine::CreateIndexData()
{
	m_IndexList.resize(2);
	m_IndexList[0] = 0;
	m_IndexList[1] = 1;

	m_dxobj.m_iNumIndex = m_IndexList.size();
	m_dxobj.m_iIndexSize = sizeof(DWORD);
	return true;
}
HRESULT SLine::CreateVertexBuffer()
{
	if (m_dxobj.m_iNumVertex <= 0) return S_OK;
	m_dxobj.g_pVertexBuffer.Attach(DXGame::CreateVertexBuffer(m_pDevice,
		&m_LineVertexList.at(0),
		m_dxobj.m_iNumVertex,
		m_dxobj.m_iVertexSize));
	return S_OK;
}
HRESULT SLine::LoadShaderFile(ID3D11Device* pDevice, const TCHAR* pLoadShaderFile)
{
	T_STR ShaderFile;
	if (pLoadShaderFile == nullptr) ShaderFile = L"../../shader/Shape/Line.hlsl";
	else							ShaderFile = pLoadShaderFile;

	m_dxobj.g_pVertexShader.Attach(DXGame::LoadVertexShaderFile(pDevice, ShaderFile.c_str(), m_dxobj.g_pVSBlob.GetAddressOf()));
	m_dxobj.g_pPixelShader.Attach(DXGame::LoadPixelShaderFile(pDevice, ShaderFile.c_str()));
	m_dxobj.g_pGeometryShader.Attach(DXGame::LoadGeometryShaderFile(pDevice, ShaderFile.c_str(), m_dxobj.g_pGSBlob.GetAddressOf()));
	m_dxobj.g_pHullShader.Attach(DXGame::LoadHullShaderFile(pDevice, ShaderFile.c_str(), m_dxobj.g_pHSBlob.GetAddressOf()));
	m_dxobj.g_pDomainShader.Attach(DXGame::LoadDomainShaderFile(pDevice, ShaderFile.c_str(), m_dxobj.g_pDSBlob.GetAddressOf()));
	m_dxobj.g_pComputeShader.Attach(DXGame::LoadComputeShaderFile(pDevice, ShaderFile.c_str(), m_dxobj.g_pCSBlob.GetAddressOf()));
	return S_OK;
}

bool SLine::Draw(ID3D11DeviceContext* pContext, D3DXVECTOR3 vStart, D3DXVECTOR3 vEnd, D3DXVECTOR4 vColor)
{
	PC_VERTEX vertices[2];
	vertices[0].p = vStart;
	vertices[0].c = vColor;
	vertices[1].p = vEnd;
	vertices[1].c = vColor;
	// 동적 리소스 갱신 방법 1 : D3D11_USAGE_DEFAULT 사용
	pContext->UpdateSubresource(GetVB(), 0, NULL, &vertices, 0, 0);
	return Render(pContext);
}
//--------------------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------------------
HRESULT SLine::CreateResource()
{
	m_dxobj.m_iPrimitiveType = D3D10_PRIMITIVE_TOPOLOGY_LINELIST;
	return S_OK;
}
SLine::SLine(void)
{
}

SLine::~SLine(void)
{
}
#pragma endregion

#pragma region SDirection

HRESULT SDirection::SetInputLayout()
{
	HRESULT hr = S_OK;

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = sizeof(layout) / sizeof(layout[0]);
	m_dxobj.g_pInputlayout.Attach(DXGame::CreateInputlayout(m_pDevice, m_dxobj.g_pVSBlob.Get()->GetBufferSize(),
		m_dxobj.g_pVSBlob.Get()->GetBufferPointer(), layout, numElements));
	return hr;
}
bool SDirection::CreateVertexData()
{
	m_LineVertexList.resize(6);
	m_LineVertexList[0] = PC_VERTEX(D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f));
	m_LineVertexList[1] = PC_VERTEX(D3DXVECTOR3(1000.0f, 0.0f, 0.0f), D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f));
	m_LineVertexList[2] = PC_VERTEX(D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f));
	m_LineVertexList[3] = PC_VERTEX(D3DXVECTOR3(0.0f, 1000.0f, 0.0f), D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f));
	m_LineVertexList[4] = PC_VERTEX(D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f));
	m_LineVertexList[5] = PC_VERTEX(D3DXVECTOR3(0.0f, 0.0f, 1000.0f), D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f));

	m_dxobj.m_iVertexSize = sizeof(PC_VERTEX);
	m_dxobj.m_iNumVertex = m_LineVertexList.size();
	return true;
}
bool SDirection::CreateIndexData()
{
	m_IndexList.resize(6);
	int iIndex = 0;
	m_IndexList[iIndex++] = 0; 	m_IndexList[iIndex++] = 1; 	m_IndexList[iIndex++] = 2;
	m_IndexList[iIndex++] = 3;	m_IndexList[iIndex++] = 4; 	m_IndexList[iIndex++] = 5;

	m_dxobj.m_iNumIndex = m_IndexList.size();
	m_dxobj.m_iIndexSize = sizeof(DWORD);
	return true;
}
HRESULT SDirection::CreateVertexBuffer()
{
	if (m_dxobj.m_iNumVertex <= 0) return S_OK;
	m_dxobj.g_pVertexBuffer.Attach(DXGame::CreateVertexBuffer(m_pDevice,
		&m_LineVertexList.at(0),
		m_dxobj.m_iNumVertex,
		m_dxobj.m_iVertexSize));
	return S_OK;
}

HRESULT SDirection::LoadShaderFile(ID3D11Device* pDevice, const TCHAR* pLoadShaderFile)
{
	T_STR ShaderFile;
	if (pLoadShaderFile == nullptr) ShaderFile = L"../../shader/Shape/Line.hlsl";
	else							ShaderFile = pLoadShaderFile;

	m_dxobj.g_pVertexShader.Attach(DXGame::LoadVertexShaderFile(pDevice, ShaderFile.c_str(), m_dxobj.g_pVSBlob.GetAddressOf()));
	m_dxobj.g_pPixelShader.Attach(DXGame::LoadPixelShaderFile(pDevice, ShaderFile.c_str()));
	m_dxobj.g_pGeometryShader.Attach(DXGame::LoadGeometryShaderFile(pDevice, ShaderFile.c_str(), m_dxobj.g_pGSBlob.GetAddressOf()));
	m_dxobj.g_pHullShader.Attach(DXGame::LoadHullShaderFile(pDevice, ShaderFile.c_str(), m_dxobj.g_pHSBlob.GetAddressOf()));
	m_dxobj.g_pDomainShader.Attach(DXGame::LoadDomainShaderFile(pDevice, ShaderFile.c_str(), m_dxobj.g_pDSBlob.GetAddressOf()));
	m_dxobj.g_pComputeShader.Attach(DXGame::LoadComputeShaderFile(pDevice, ShaderFile.c_str(), m_dxobj.g_pCSBlob.GetAddressOf()));
	return S_OK;
}
HRESULT SDirection::CreateResource()
{
	m_dxobj.m_iPrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
	return S_OK;
}

SDirection::SDirection()
{
}
SDirection::~SDirection()
{
}
#pragma endregion

#pragma region SPlane
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                            SPlane
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool SPlane::SetScreenVertex(float x,
	float y,
	float w,
	float h,
	D3DXVECTOR2 vScreen)
{
	if (m_VertexList.size() != 4) m_VertexList.resize(4);

	D3DXVECTOR2 vPoint;
	// 0 ~ 1
	vPoint.x = x / vScreen.x;
	vPoint.y = y / vScreen.y;
	// 0~1 => -1 ~ +1
	vPoint.x = vPoint.x *  2.0f - 1.0f;
	vPoint.y = -(vPoint.y *  2.0f - 1.0f);

	D3DXVECTOR2 vOffset;
	// 0 ~ 1
	vOffset.x = (x + w) / vScreen.x;
	vOffset.y = (y + h) / vScreen.y;
	// 0~1 => -1 ~ +1
	vOffset.x = (vOffset.x *  2.0f - 1.0f) - vPoint.x;
	vOffset.y = vPoint.y + (vOffset.y *  2.0f - 1.0f);

	m_VertexList[0].p.x = vPoint.x;
	m_VertexList[0].p.y = vPoint.y;
	m_VertexList[0].p.z = 0.5f;

	m_VertexList[1].p.x = vPoint.x + vOffset.x;
	m_VertexList[1].p.y = vPoint.y;
	m_VertexList[1].p.z = 0.5f;

	m_VertexList[2].p.x = vPoint.x + vOffset.x;
	m_VertexList[2].p.y = vPoint.y - vOffset.y;
	m_VertexList[2].p.z = 0.5f;

	m_VertexList[3].p.x = vPoint.x;
	m_VertexList[3].p.y = vPoint.y - vOffset.y;
	m_VertexList[3].p.z = 0.5f;

	m_pContext->UpdateSubresource(m_dxobj.g_pVertexBuffer.Get(), 0, nullptr, m_VertexList.data(), 0, 0);
	return true;
}
void SPlane::UpdateVertexData(PLANE_PNCTLIST& VertexDataList)
{
	if(m_VertexList.size() != 4) m_VertexList.resize(4);

	m_VertexList[0] = VertexDataList[0];
	m_VertexList[1] = VertexDataList[1];
	m_VertexList[2] = VertexDataList[2];
	m_VertexList[3] = VertexDataList[3];

	m_pContext->UpdateSubresource(m_dxobj.g_pVertexBuffer.Get(), 0, nullptr, m_VertexList.data(), 0, 0);
}
bool SPlane::CreateVertexData()
{
	if (m_VertexList.size() <= 0)
	{
		m_VertexList.resize(4);
		m_VertexList[0] = PNCT_VERTEX(D3DXVECTOR3(-1.0f, 1.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f));
		m_VertexList[1] = PNCT_VERTEX(D3DXVECTOR3(1.0f, 1.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(1.0f, 0.0f));
		m_VertexList[2] = PNCT_VERTEX(D3DXVECTOR3(1.0f, -1.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(1.0f, 1.0f));
		m_VertexList[3] = PNCT_VERTEX(D3DXVECTOR3(-1.0f, -1.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 1.0f));
	}
	else
	{
		m_VertexList[0] = PNCT_VERTEX(m_VertexList[0].p, D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f));
		m_VertexList[1] = PNCT_VERTEX(m_VertexList[1].p, D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(1.0f, 0.0f));
		m_VertexList[2] = PNCT_VERTEX(m_VertexList[2].p, D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(1.0f, 1.0f));
		m_VertexList[3] = PNCT_VERTEX(m_VertexList[3].p, D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 1.0f));

	}
	m_dxobj.m_iVertexSize = sizeof(PNCT_VERTEX);
	m_dxobj.m_iNumVertex = m_VertexList.size();
	return true;
}
bool SPlane::CreateIndexData()
{
	m_IndexList.resize(6);
	m_IndexList[0] = 0;
	m_IndexList[1] = 1;
	m_IndexList[2] = 2;
	m_IndexList[3] = 0;
	m_IndexList[4] = 2;
	m_IndexList[5] = 3;

	m_dxobj.m_iNumIndex = m_IndexList.size();
	m_dxobj.m_iIndexSize = sizeof(DWORD);
	return true;
}
HRESULT SPlane::CreateResource()
{
	m_dxobj.m_iPrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	return S_OK;
}

HRESULT SPlane::LoadShaderFile(ID3D11Device* pDevice, const TCHAR* pLoadShaderFile)
{
	T_STR ShaderFile;
	if (pLoadShaderFile == nullptr) ShaderFile = L"../../shader/Shape/Plane.hlsl";
	else							ShaderFile = pLoadShaderFile;

	m_dxobj.g_pVertexShader.Attach(DXGame::LoadVertexShaderFile(pDevice, ShaderFile.c_str(), m_dxobj.g_pVSBlob.GetAddressOf()));
	m_dxobj.g_pPixelShader.Attach(DXGame::LoadPixelShaderFile(pDevice, ShaderFile.c_str()));
	m_dxobj.g_pGeometryShader.Attach(DXGame::LoadGeometryShaderFile(pDevice, ShaderFile.c_str(), m_dxobj.g_pGSBlob.GetAddressOf()));
	m_dxobj.g_pHullShader.Attach(DXGame::LoadHullShaderFile(pDevice, ShaderFile.c_str(), m_dxobj.g_pHSBlob.GetAddressOf()));
	m_dxobj.g_pDomainShader.Attach(DXGame::LoadDomainShaderFile(pDevice, ShaderFile.c_str(), m_dxobj.g_pDSBlob.GetAddressOf()));
	m_dxobj.g_pComputeShader.Attach(DXGame::LoadComputeShaderFile(pDevice, ShaderFile.c_str(), m_dxobj.g_pCSBlob.GetAddressOf()));
	return S_OK;

}
SPlane::SPlane()
{
}
SPlane::~SPlane()
{

}
#pragma endregion SPlane


#pragma region SBox
bool SBox::CreateVertexData()
{
	// 상단
	// 5    6
	// 1    2
	// 하단
	// 4    7
	// 0    3  
	// 앞면
	m_VertexList.resize(24);
	m_VertexList[0] = PNCT_VERTEX(D3DXVECTOR3(-1.0f, 1.0f, -1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f));
	m_VertexList[1] = PNCT_VERTEX(D3DXVECTOR3(1.0f, 1.0f, -1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f), D3DXVECTOR2(1.0f, 0.0f));
	m_VertexList[2] = PNCT_VERTEX(D3DXVECTOR3(1.0f, -1.0f, -1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f), D3DXVECTOR2(1.0f, 1.0f));
	m_VertexList[3] = PNCT_VERTEX(D3DXVECTOR3(-1.0f, -1.0f, -1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f), D3DXVECTOR2(0.0f, 1.0f));
	// 뒷면
	m_VertexList[4] = PNCT_VERTEX(D3DXVECTOR3(1.0f, 1.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), D3DXVECTOR4(0.0f, 0.0f, 0.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f));
	m_VertexList[5] = PNCT_VERTEX(D3DXVECTOR3(-1.0f, 1.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f), D3DXVECTOR2(1.0f, 0.0f));
	m_VertexList[6] = PNCT_VERTEX(D3DXVECTOR3(-1.0f, -1.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f), D3DXVECTOR2(1.0f, 1.0f));
	m_VertexList[7] = PNCT_VERTEX(D3DXVECTOR3(1.0f, -1.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f), D3DXVECTOR2(0.0f, 1.0f));

	// 오른쪽
	m_VertexList[8] = PNCT_VERTEX(D3DXVECTOR3(1.0f, 1.0f, -1.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f));
	m_VertexList[9] = PNCT_VERTEX(D3DXVECTOR3(1.0f, 1.0f, 1.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f), D3DXVECTOR2(1.0f, 0.0f));
	m_VertexList[10] = PNCT_VERTEX(D3DXVECTOR3(1.0f, -1.0f, 1.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f), D3DXVECTOR2(1.0f, 1.0f));
	m_VertexList[11] = PNCT_VERTEX(D3DXVECTOR3(1.0f, -1.0f, -1.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 1.0f));

	// 왼쪽
	m_VertexList[12] = PNCT_VERTEX(D3DXVECTOR3(-1.0f, 1.0f, 1.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), D3DXVECTOR4(1.0f, 1.0f, 0.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f));
	m_VertexList[13] = PNCT_VERTEX(D3DXVECTOR3(-1.0f, 1.0f, -1.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), D3DXVECTOR4(1.0f, 1.0f, 0.0f, 1.0f), D3DXVECTOR2(1.0f, 0.0f));
	m_VertexList[14] = PNCT_VERTEX(D3DXVECTOR3(-1.0f, -1.0f, -1.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), D3DXVECTOR4(1.0f, 1.0f, 0.0f, 1.0f), D3DXVECTOR2(1.0f, 1.0f));
	m_VertexList[15] = PNCT_VERTEX(D3DXVECTOR3(-1.0f, -1.0f, 1.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), D3DXVECTOR4(1.0f, 1.0f, 0.0f, 1.0f), D3DXVECTOR2(0.0f, 1.0f));

	// 윗면
	m_VertexList[16] = PNCT_VERTEX(D3DXVECTOR3(-1.0f, 1.0f, 1.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR4(1.0f, 0.5f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f));
	m_VertexList[17] = PNCT_VERTEX(D3DXVECTOR3(1.0f, 1.0f, 1.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR4(1.0f, 0.5f, 1.0f, 1.0f), D3DXVECTOR2(1.0f, 0.0f));
	m_VertexList[18] = PNCT_VERTEX(D3DXVECTOR3(1.0f, 1.0f, -1.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR4(1.0f, 0.5f, 1.0f, 1.0f), D3DXVECTOR2(1.0f, 1.0f));
	m_VertexList[19] = PNCT_VERTEX(D3DXVECTOR3(-1.0f, 1.0f, -1.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR4(1.0f, 0.5f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 1.0f));

	// 아랫면
	m_VertexList[20] = PNCT_VERTEX(D3DXVECTOR3(-1.0f, -1.0f, -1.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR4(0.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f));
	m_VertexList[21] = PNCT_VERTEX(D3DXVECTOR3(1.0f, -1.0f, -1.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR4(0.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(1.0f, 0.0f));
	m_VertexList[22] = PNCT_VERTEX(D3DXVECTOR3(1.0f, -1.0f, 1.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR4(0.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(1.0f, 1.0f));
	m_VertexList[23] = PNCT_VERTEX(D3DXVECTOR3(-1.0f, -1.0f, 1.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR4(0.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 1.0f));

	m_dxobj.m_iVertexSize = sizeof(PNCT_VERTEX);
	m_dxobj.m_iNumVertex = m_VertexList.size();
	return true;
}
bool SBox::CreateIndexData()
{
	m_IndexList.resize(36);
	int iIndex = 0;
	m_IndexList[iIndex++] = 0; 	m_IndexList[iIndex++] = 1; 	m_IndexList[iIndex++] = 2; 	m_IndexList[iIndex++] = 0;	m_IndexList[iIndex++] = 2; 	m_IndexList[iIndex++] = 3;
	m_IndexList[iIndex++] = 4; 	m_IndexList[iIndex++] = 5; 	m_IndexList[iIndex++] = 6; 	m_IndexList[iIndex++] = 4;	m_IndexList[iIndex++] = 6; 	m_IndexList[iIndex++] = 7;
	m_IndexList[iIndex++] = 8; 	m_IndexList[iIndex++] = 9; 	m_IndexList[iIndex++] = 10; m_IndexList[iIndex++] = 8;	m_IndexList[iIndex++] = 10; m_IndexList[iIndex++] = 11;
	m_IndexList[iIndex++] = 12; m_IndexList[iIndex++] = 13; m_IndexList[iIndex++] = 14; m_IndexList[iIndex++] = 12;	m_IndexList[iIndex++] = 14; m_IndexList[iIndex++] = 15;
	m_IndexList[iIndex++] = 16; m_IndexList[iIndex++] = 17; m_IndexList[iIndex++] = 18; m_IndexList[iIndex++] = 16;	m_IndexList[iIndex++] = 18; m_IndexList[iIndex++] = 19;
	m_IndexList[iIndex++] = 20; m_IndexList[iIndex++] = 21; m_IndexList[iIndex++] = 22; m_IndexList[iIndex++] = 20;	m_IndexList[iIndex++] = 22; m_IndexList[iIndex++] = 23;

	m_dxobj.m_iNumIndex = m_IndexList.size();
	m_dxobj.m_iIndexSize = sizeof(DWORD);
	return true;
}

HRESULT SBox::CreateResource()
{
	m_dxobj.m_iPrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	return S_OK;
}

void SBox::UpdateVertexData(BOX_VERTEX_LIST& VertexDataList)
{
	if (m_VertexList.size() != 24) m_VertexList.resize(24);

	m_VertexList[0] = VertexDataList[0];
	m_VertexList[1] = VertexDataList[1];
	m_VertexList[2] = VertexDataList[2];
	m_VertexList[3] = VertexDataList[3];

	m_VertexList[4] = VertexDataList[4];
	m_VertexList[5] = VertexDataList[5];
	m_VertexList[6] = VertexDataList[6];
	m_VertexList[7] = VertexDataList[7];

	m_VertexList[8] = VertexDataList[8];
	m_VertexList[9] = VertexDataList[9];
	m_VertexList[10] = VertexDataList[10];
	m_VertexList[11] = VertexDataList[11];

	m_VertexList[12] = VertexDataList[12];
	m_VertexList[13] = VertexDataList[13];
	m_VertexList[14] = VertexDataList[14];
	m_VertexList[15] = VertexDataList[15];

	m_VertexList[16] = VertexDataList[16];
	m_VertexList[17] = VertexDataList[17];
	m_VertexList[18] = VertexDataList[18];
	m_VertexList[19] = VertexDataList[19];

	m_VertexList[20] = VertexDataList[20];
	m_VertexList[21] = VertexDataList[21];
	m_VertexList[22] = VertexDataList[22];
	m_VertexList[23] = VertexDataList[23];

	m_pContext->UpdateSubresource(m_dxobj.g_pVertexBuffer.Get(), 0, nullptr, m_VertexList.data(), 0, 0);
}

HRESULT SBox::LoadShaderFile(ID3D11Device* pDevice, const TCHAR* pLoadShaderFile)
{
	T_STR ShaderFile;
	if (pLoadShaderFile == nullptr) ShaderFile = L"../../shader/Shape/Box.hlsl";
	else							ShaderFile = pLoadShaderFile;

	if (m_dxobj.g_pTextureSRV == nullptr)
	{
		m_dxobj.g_pPixelShader.Attach(DXGame::LoadPixelShaderFile(pDevice, ShaderFile.c_str(), "PS_Color"));
	}
	else
	{
		m_dxobj.g_pPixelShader.Attach(DXGame::LoadPixelShaderFile(pDevice, ShaderFile.c_str()));
	}

	m_dxobj.g_pVertexShader.Attach(DXGame::LoadVertexShaderFile(pDevice, ShaderFile.c_str(), m_dxobj.g_pVSBlob.GetAddressOf()));
	m_dxobj.g_pGeometryShader.Attach(DXGame::LoadGeometryShaderFile(pDevice, ShaderFile.c_str(), m_dxobj.g_pGSBlob.GetAddressOf()));
	m_dxobj.g_pHullShader.Attach(DXGame::LoadHullShaderFile(pDevice, ShaderFile.c_str(), m_dxobj.g_pHSBlob.GetAddressOf()));
	m_dxobj.g_pDomainShader.Attach(DXGame::LoadDomainShaderFile(pDevice, ShaderFile.c_str(), m_dxobj.g_pDSBlob.GetAddressOf()));
	m_dxobj.g_pComputeShader.Attach(DXGame::LoadComputeShaderFile(pDevice, ShaderFile.c_str(), m_dxobj.g_pCSBlob.GetAddressOf()));
	return S_OK;

}
SBox::SBox()
{

}
SBox::~SBox()
{

}

#pragma endregion SBox

#pragma region Sphere
#pragma endregion Sphere