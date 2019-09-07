#include "SDXShape.h"


bool SLine::CreateVertexBuffer(void* pData, int iNumCount, int iSize)
{
	m_dxobj.m_iVertexSize = iSize;
	m_dxobj.m_iNumVertex = iNumCount;
	m_dxobj.g_pVertexBuffer.Attach(
		DXGame::CreateVertexBuffer(m_pDevice,
			pData, iNumCount, iSize, true)
	);
	if (m_dxobj.g_pVertexBuffer.Get() == nullptr)
		return false;
	return true;
}
bool SLine::LoadShaderAndInputlayout(const CHAR* pVertexShader, const CHAR* pPixelShader)
{
	HRESULT hr = S_OK;
	m_dxobj.g_pVertexShader.Attach(
		DXGame::LoadVertexShaderFile(m_pDevice,
			L"../../shader/09_Shape/Line.hlsl",
			m_dxobj.g_pVSBlob.GetAddressOf(),
			(char*)pVertexShader));
	m_dxobj.g_pPixelShader.Attach(
		DXGame::LoadPixelShaderFile(m_pDevice,
			L"../../shader/09_Shape/Line.hlsl"));
	//input layout
	//정점버퍼의 데이터를 정점 쉐이더의 인자값으로 설정
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0  },
	};
	int iNumElement = sizeof(layout) / sizeof(layout[0]);
	m_dxobj.g_pInputlayout.Attach(
		DXGame::CreateInputlayout(m_pDevice,
			m_dxobj.g_pVSBlob->GetBufferSize(),
			m_dxobj.g_pVSBlob->GetBufferPointer(),
			layout, iNumElement));

	return S_OK;
}
bool SLine::CreateIndexBuffer(void* pData, int iNumCount, int iSize)
{
	m_dxobj.m_iNumIndex = iNumCount;
	m_dxobj.g_pIndexBuffer.Attach(
		DXGame::CreateIndexBuffer(m_pDevice,
			pData, iNumCount, iSize)
	);
	if (m_dxobj.g_pIndexBuffer.Get() == nullptr)
		return false;
	return true;
}
bool SLine::CreateConstantBuffer(
	void* pData,
	int iNumCount,
	int iSize,
	bool bDynamic)
{
	m_dxobj.g_pConstantBuffer.Attach(
		DXGame::CreateConstantBuffer(m_pDevice,
			pData, iNumCount, iSize, bDynamic)
	);
	if (m_dxobj.g_pConstantBuffer.Get() == nullptr)
		return false;
	return true;
}

bool SLine::LoadSRV(T_STR name, int iIndex)
{
	return true;
}
bool SLine::Load(ID3D11Device* pDevice)
{
	m_pDevice = pDevice;

	LoadShaderAndInputlayout("VS", "PS");
	// local coordinate
	PNCT_VERTEX vertices[2];

	vertices[0].p = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	vertices[0].n = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	vertices[0].c = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);
	vertices[0].t = D3DXVECTOR2(0.0f, 0.0f);

	vertices[1].p = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	vertices[1].n = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	vertices[1].c = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
	vertices[1].t = D3DXVECTOR2(0.0f, 0.0f);

	int iNumCount = sizeof(vertices) / sizeof(vertices[0]);
	if (CreateVertexBuffer(
		vertices,
		iNumCount,
		sizeof(PNCT_VERTEX)) == false)
	{
		return false;
	}

	DWORD indexlist[] =
	{
		0, 1
	};
	iNumCount = sizeof(indexlist) / sizeof(indexlist[0]);
	if (CreateIndexBuffer(
		indexlist, iNumCount, sizeof(DWORD)) == false) return false;

	Init();

	D3DXMatrixIdentity(&m_matWorld);

	D3DXVECTOR3 Eye(0.0f, 0.0f, 1.0f);
	D3DXVECTOR3 At(1.0f, 0.0f, 0.0f);
	D3DXVECTOR3 Up(0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(&m_matView, &Eye, &At, &Up);

	D3DXMatrixPerspectiveFovLH(
		&m_matProj, D3DX_PI / 2,
		(float)g_rtClient.right / (float)g_rtClient.bottom,
		1.0f,
		100.0f);

	D3DXMatrixTranspose(&m_matWorld, &m_matWorld);
	D3DXMatrixTranspose(&m_matView, &m_matView);
	D3DXMatrixTranspose(&m_matProj, &m_matProj);


	ZeroMemory(&m_cbData, sizeof(VS_CONSTANT_BUFFER));
	m_cbData.matWorld = m_matWorld;
	m_cbData.matView = m_matView;
	m_cbData.matProj = m_matProj;
	m_cbData.Color.x = 1.0f;
	m_cbData.Color.y = 1.0f;
	m_cbData.Color.z = 1.0f;
	m_cbData.Color.w = 1.0f;

	if (CreateConstantBuffer(
		(LPVOID)&m_cbData, 1, sizeof(VS_CONSTANT_BUFFER)) == false) return false;

	return true;
}

bool SLine::Init()
{
	return true;
}
bool SLine::Frame()
{
	return true;
}
bool SLine::Render(ID3D11DeviceContext*	pContext)
{
	D3D11_MAPPED_SUBRESOURCE msr;
	if (SUCCEEDED(m_pContext->Map(
		m_dxobj.g_pVertexBuffer.Get(), 0,
		D3D11_MAP_WRITE_DISCARD, 0, &msr)))
	{
		PNCT_VERTEX* data = (PNCT_VERTEX*)msr.pData;
		data[0].p = m_vPos;
		data[0].c = m_vColor;
		data[1].p = m_vDir;
		data[1].c = m_vColor;
		m_pContext->Unmap(m_dxobj.g_pVertexBuffer.Get(), 0);
	}

	m_pContext->UpdateSubresource(m_dxobj.g_pConstantBuffer.Get(), 0, nullptr, &m_cbData, 0, 0);

	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	m_dxobj.Render(pContext, m_dxobj.m_iVertexSize, m_dxobj.m_iNumIndex);
	return true;
}
bool SLine::Release()
{
	SModel::Release();
	return true;
}

SLine::SLine()
{
	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_matView);
	D3DXMatrixIdentity(&m_matProj);
}


SLine::~SLine()
{
}

bool SPlane::Load(ID3D11Device* pDevice, std::wstring filename, PLANE_VERTEX_LIST& VertexList)
{
	m_pDevice = pDevice;
	LoadShaderAndInputlayout("VS", "PS");
	int iNumCount = sizeof(VertexList) / sizeof(VertexList[0]);
	if (CreateVertexBuffer(
		&VertexList,
		iNumCount,
		sizeof(PNCT_VERTEX)) == false)
	{
		return false;
	}

	m_PlaneVertexList = VertexList;
	DWORD indexlist[] =
	{
		1,2,0,
		2,3,0,
	};
	iNumCount = sizeof(indexlist) / sizeof(indexlist[0]);
	if (CreateIndexBuffer(
		indexlist, iNumCount, sizeof(DWORD)) == false) return false;



	Init();

	m_iTextureList.push_back(I_TextureManager.Load(m_pDevice, filename.c_str()));
	m_dxobj.g_pTextureSRV = I_TextureManager.GetPtr(m_iTextureList[0])->m_pSRV;


	D3DXVECTOR3 Eye(0.0f, 0.0f, 1.0f);
	D3DXVECTOR3 At(1.0f, 0.0f, 0.0f);
	D3DXVECTOR3 Up(0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(&m_matView, &Eye, &At, &Up);

	D3DXMatrixPerspectiveFovLH(
		&m_matProj, D3DX_PI / 2,
		(float)g_rtClient.right / (float)g_rtClient.bottom,
		1.0f,
		100.0f);

	D3DXMatrixTranspose(&m_matWorld, &m_matWorld);
	D3DXMatrixTranspose(&m_matView, &m_matView);
	D3DXMatrixTranspose(&m_matProj, &m_matProj);

	ZeroMemory(&m_cbData, sizeof(VS_CONSTANT_BUFFER));
	m_cbData.matWorld = m_matWorld;
	m_cbData.matView = m_matView;
	m_cbData.matProj = m_matProj;
	m_cbData.Color[0] = 1.0f;
	iNumCount = 1;
	if (CreateConstantBuffer(
		(LPVOID)&m_cbData, iNumCount, sizeof(VS_CONSTANT_BUFFER)) == false) return false;


	return true;
}
bool SPlane::CreateIndexBuffer(
	void* pData, int iNumCount, int iSize)
{
	m_dxobj.m_iNumIndex = iNumCount;
	m_dxobj.g_pIndexBuffer.Attach(
		DXGame::CreateIndexBuffer(m_pDevice,
			pData, iNumCount, iSize)
	);
	if (m_dxobj.g_pIndexBuffer.Get() == nullptr)
		return false;
	return true;
}
bool SPlane::CreateConstantBuffer(
	void* pData, int iNumCount,
	int iSize,
	bool bDynamic)
{
	m_dxobj.g_pConstantBuffer.Attach(
		DXGame::CreateConstantBuffer(m_pDevice,
			pData, iNumCount, iSize, bDynamic)
	);
	if (m_dxobj.g_pConstantBuffer.Get() == nullptr)
		return false;
	return true;
}
bool SPlane::CreateVertexBuffer(
	void* pData, int iNumCount, int iSize)
{
	m_dxobj.m_iVertexSize = iSize;
	m_dxobj.m_iNumVertex = iNumCount;
	m_dxobj.g_pVertexBuffer.Attach(
		DXGame::CreateVertexBuffer(m_pDevice,
			pData, iNumCount, iSize)
	);
	if (m_dxobj.g_pVertexBuffer.Get() == nullptr)
		return false;
	return true;
}
bool SPlane::LoadShaderAndInputlayout(
	const CHAR* pVertexShader,
	const CHAR* pPixelShader)
{
	HRESULT hr = S_OK;
	m_dxobj.g_pVertexShader.Attach(
		DXGame::LoadVertexShaderFile(m_pDevice,
			L"../../shader/09_Shape/Plane.hlsl",
			m_dxobj.g_pVSBlob.GetAddressOf(),
			(char*)pVertexShader));
	m_dxobj.g_pPixelShader.Attach(
		DXGame::LoadPixelShaderFile(m_pDevice,
			L"../../shader/09_Shape/Plane.hlsl"));
	//input layout
	//정점버퍼의 데이터를 정점 쉐이더의 인자값으로 설정
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0  },
	};
	int iNumElement = sizeof(layout) / sizeof(layout[0]);
	m_dxobj.g_pInputlayout.Attach(
		DXGame::CreateInputlayout(m_pDevice,
			m_dxobj.g_pVSBlob->GetBufferSize(),
			m_dxobj.g_pVSBlob->GetBufferPointer(),
			layout, iNumElement));

	return S_OK;
}
bool SPlane::LoadSRV(T_STR name, int iIndex)
{
	return true;
}
bool SPlane::Init()
{
	return true;
}
bool SPlane::Render(ID3D11DeviceContext*	pContext)
{
	D3DXMatrixTranspose(&m_matView, &m_matView);
	D3DXMatrixTranspose(&m_matProj, &m_matProj);

	pContext->UpdateSubresource(m_dxobj.g_pConstantBuffer.Get(), 0, nullptr, &m_cbData, 0, 0);
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_dxobj.Render(pContext, m_dxobj.m_iVertexSize, m_dxobj.m_iNumIndex);

	return true;
}
bool SPlane::Release()
{
	SModel::Release();
	return true;
}
bool SPlane::Frame()
{
	return true;
}
SPlane::SPlane()
{
	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_matView);
	D3DXMatrixIdentity(&m_matProj);
}


SPlane::~SPlane()
{

}


bool SBox::CreateVertexBuffer(void* pData, int iNumCount, int iSize)
{
	m_dxobj.m_iVertexSize = iSize;
	m_dxobj.m_iNumVertex = iNumCount;
	m_dxobj.g_pVertexBuffer.Attach(
		DXGame::CreateVertexBuffer(m_pDevice,
			pData, iNumCount, iSize)
	);
	if (m_dxobj.g_pVertexBuffer.Get() == nullptr)
		return false;
	return true;
}
bool SBox::LoadShaderAndInputlayout(const CHAR* pVertexShader, const CHAR* pPixelShader)
{
	HRESULT hr = S_OK;
	m_dxobj.g_pVertexShader.Attach(
		DXGame::LoadVertexShaderFile(m_pDevice,
			L"../../shader/09_Shape/Box.hlsl",
			m_dxobj.g_pVSBlob.GetAddressOf(),
			(char*)pVertexShader));
	m_dxobj.g_pPixelShader.Attach(
		DXGame::LoadPixelShaderFile(m_pDevice,
			L"../../shader/09_Shape/Box.hlsl"));
	//input layout
	//정점버퍼의 데이터를 정점 쉐이더의 인자값으로 설정
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0  },
	};
	int iNumElement = sizeof(layout) / sizeof(layout[0]);
	m_dxobj.g_pInputlayout.Attach(
		DXGame::CreateInputlayout(m_pDevice,
			m_dxobj.g_pVSBlob->GetBufferSize(),
			m_dxobj.g_pVSBlob->GetBufferPointer(),
			layout, iNumElement));

	return hr;
}
bool SBox::CreateIndexBuffer(void* pData, int iNumCount, int iSize)
{
	m_dxobj.m_iNumIndex = iNumCount;
	m_dxobj.g_pIndexBuffer.Attach(
		DXGame::CreateIndexBuffer(m_pDevice,
			pData, iNumCount, iSize)
	);
	if (m_dxobj.g_pIndexBuffer.Get() == nullptr)
		return false;
	return true;
}
bool SBox::CreateConstantBuffer(
	void* pData,
	int iNumCount,
	int iSize,
	bool bDynamic)
{
	m_dxobj.g_pConstantBuffer.Attach(
		DXGame::CreateConstantBuffer(m_pDevice,
			pData, iNumCount, iSize, bDynamic)
	);
	if (m_dxobj.g_pConstantBuffer.Get() == nullptr)
		return false;
	return true;
}

bool SBox::LoadSRV(T_STR name, int iIndex)
{
	return true;
}
bool SBox::Load(ID3D11Device* pDevice, std::wstring filename, BOX_VERTEX_LIST& VertexList)
{
	m_pDevice = pDevice;
	LoadShaderAndInputlayout("VS", "PS");
	// local coordinate
	int iNumCount = sizeof(VertexList.Plane) / sizeof(VertexList.Plane[0]);
	if (CreateVertexBuffer(
		&VertexList,
		iNumCount,
		sizeof(PNCT_VERTEX)) == false)
	{
		return false;
	}
	
	m_BoxVertexList = VertexList;

	DWORD indexlist[] =
	{
		1,2,0,
		2,3,0,

		5,6,4,
		6,7,4,

		9,10,8,
		10,11,8,

		13, 14, 12,
		14, 15, 12,

		17, 18, 16,
		18, 19, 16,

		21, 22, 20,
		22, 23, 20
	};
	iNumCount = sizeof(indexlist) / sizeof(indexlist[0]);
	if (CreateIndexBuffer(
		indexlist, iNumCount, sizeof(DWORD)) == false) return false;

	Init();

	m_iTextureList.push_back(I_TextureManager.Load(m_pDevice, filename.c_str()));

	m_dxobj.g_pTextureSRV = I_TextureManager.GetPtr(m_iTextureList[0])->m_pSRV;


	D3DXMatrixIdentity(&m_matWorld);


	D3DXVECTOR3 Eye(0.0f, 0.0f, 1.0f);
	D3DXVECTOR3 At(1.0f, 0.0f, 0.0f);
	D3DXVECTOR3 Up(0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(&m_matView, &Eye, &At, &Up);

	D3DXMatrixPerspectiveFovLH(
		&m_matProj, D3DX_PI / 2,
		(float)g_rtClient.right / (float)g_rtClient.bottom,
		1.0f,
		100.0f);

	D3DXMatrixTranspose(&m_matWorld, &m_matWorld);
	D3DXMatrixTranspose(&m_matView, &m_matView);
	D3DXMatrixTranspose(&m_matProj, &m_matProj);


	ZeroMemory(&m_cbData, sizeof(VS_CONSTANT_BUFFER));
	m_cbData.matWorld = m_matWorld;
	m_cbData.matView = m_matView;
	m_cbData.matProj = m_matProj;
	m_cbData.Color.x = 1.0f;
	m_cbData.Color.y = 1.0f;
	m_cbData.Color.z = 1.0f;
	m_cbData.Color.w = 1.0f;

	if (CreateConstantBuffer((LPVOID)&m_cbData, 1, sizeof(VS_CONSTANT_BUFFER)) == false) return false;

	return true;
}

bool SBox::Init()
{
	return true;
}
bool SBox::Frame()
{
	return true;
}
bool SBox::Render(ID3D11DeviceContext*	pContext)
{

	pContext->UpdateSubresource(m_dxobj.g_pConstantBuffer.Get(), 0, nullptr, &m_cbData, 0, 0);
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_dxobj.Render(pContext, m_dxobj.m_iVertexSize, m_dxobj.m_iNumIndex);
	return true;
}
bool SBox::Release()
{
	SModel::Release();
	return true;
}

SBox::SBox()
{
	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_matView);
	D3DXMatrixIdentity(&m_matProj);
}


SBox::~SBox()
{
}

bool SDirection::CreateVertexBuffer(void* pData, int iNumCount, int iSize)
{
	m_dxobj.m_iVertexSize = iSize;
	m_dxobj.m_iNumVertex = iNumCount;
	m_dxobj.g_pVertexBuffer.Attach(
		DXGame::CreateVertexBuffer(m_pDevice,
			pData, iNumCount, iSize, true)
	);
	if (m_dxobj.g_pVertexBuffer.Get() == nullptr)
		return false;
	return true;
}
bool SDirection::LoadShaderAndInputlayout(const CHAR* pVertexShader, const CHAR* pPixelShader)
{
	HRESULT hr = S_OK;
	m_dxobj.g_pVertexShader.Attach(
		DXGame::LoadVertexShaderFile(m_pDevice,
			L"../../shader/09_Shape/Line.hlsl",
			m_dxobj.g_pVSBlob.GetAddressOf(),
			(char*)pVertexShader));
	m_dxobj.g_pPixelShader.Attach(
		DXGame::LoadPixelShaderFile(m_pDevice,
			L"../../shader/09_Shape/Line.hlsl"));
	//input layout
	//정점버퍼의 데이터를 정점 쉐이더의 인자값으로 설정
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0  },
	};
	int iNumElement = sizeof(layout) / sizeof(layout[0]);
	m_dxobj.g_pInputlayout.Attach(
		DXGame::CreateInputlayout(m_pDevice,
			m_dxobj.g_pVSBlob->GetBufferSize(),
			m_dxobj.g_pVSBlob->GetBufferPointer(),
			layout, iNumElement));

	return S_OK;
}
bool SDirection::CreateIndexBuffer(void* pData, int iNumCount, int iSize)
{
	m_dxobj.m_iNumIndex = iNumCount;
	m_dxobj.g_pIndexBuffer.Attach(
		DXGame::CreateIndexBuffer(m_pDevice,
			pData, iNumCount, iSize)
	);
	if (m_dxobj.g_pIndexBuffer.Get() == nullptr)
		return false;
	return true;
}
bool SDirection::CreateConstantBuffer(
	void* pData,
	int iNumCount,
	int iSize,
	bool bDynamic)
{
	m_dxobj.g_pConstantBuffer.Attach(
		DXGame::CreateConstantBuffer(m_pDevice,
			pData, iNumCount, iSize, bDynamic)
	);
	if (m_dxobj.g_pConstantBuffer.Get() == nullptr)
		return false;
	return true;
}

bool SDirection::LoadSRV(T_STR name, int iIndex)
{
	return true;
}
bool SDirection::Load(ID3D11Device* pDevice)
{
	m_pDevice = pDevice;

	LoadShaderAndInputlayout("VS", "PS");
	// local coordinate
	PNCT_VERTEX vertices[6];

	vertices[0].p = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	vertices[0].n = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	vertices[0].c = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);
	vertices[0].t = D3DXVECTOR2(0.0f, 0.0f);

	vertices[1].p = D3DXVECTOR3(1000.0f, 0.0f, 0.0f);
	vertices[1].n = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	vertices[1].c = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);
	vertices[1].t = D3DXVECTOR2(0.0f, 0.0f);

	vertices[2].p = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	vertices[2].n = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	vertices[2].c = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
	vertices[2].t = D3DXVECTOR2(0.0f, 0.0f);

	vertices[3].p = D3DXVECTOR3(0.0f, 1000.0f, 0.0f);
	vertices[3].n = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	vertices[3].c = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
	vertices[3].t = D3DXVECTOR2(0.0f, 0.0f);

	vertices[4].p = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	vertices[4].n = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	vertices[4].c = D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f);
	vertices[4].t = D3DXVECTOR2(0.0f, 0.0f);

	vertices[5].p = D3DXVECTOR3(0.0f, 0.0f, 1000.0f);
	vertices[5].n = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	vertices[5].c = D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f);
	vertices[5].t = D3DXVECTOR2(0.0f, 0.0f);

	int iNumCount = sizeof(vertices) / sizeof(vertices[0]);
	if (CreateVertexBuffer(
		vertices,
		iNumCount,
		sizeof(PNCT_VERTEX)) == false)
	{
		return false;
	}

	DWORD indexlist[] =
	{
		0, 1,
		2, 3,
		4, 5
	};
	iNumCount = sizeof(indexlist) / sizeof(indexlist[0]);
	if (CreateIndexBuffer(
		indexlist, iNumCount, sizeof(DWORD)) == false) return false;

	Init();

	D3DXMatrixIdentity(&m_matWorld);

	D3DXVECTOR3 Eye(0.0f, 0.0f, 1.0f);
	D3DXVECTOR3 At(1.0f, 0.0f, 0.0f);
	D3DXVECTOR3 Up(0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(&m_matView, &Eye, &At, &Up);

	D3DXMatrixPerspectiveFovLH(
		&m_matProj, D3DX_PI / 2,
		(float)g_rtClient.right / (float)g_rtClient.bottom,
		1.0f,
		100.0f);

	D3DXMatrixTranspose(&m_matWorld, &m_matWorld);
	D3DXMatrixTranspose(&m_matView, &m_matView);
	D3DXMatrixTranspose(&m_matProj, &m_matProj);


	ZeroMemory(&m_cbData, sizeof(VS_CONSTANT_BUFFER));
	m_cbData.matWorld = m_matWorld;
	m_cbData.matView = m_matView;
	m_cbData.matProj = m_matProj;
	m_cbData.Color.x = 1.0f;
	m_cbData.Color.y = 1.0f;
	m_cbData.Color.z = 1.0f;
	m_cbData.Color.w = 1.0f;

	if (CreateConstantBuffer(
		(LPVOID)&m_cbData, 1, sizeof(VS_CONSTANT_BUFFER)) == false) return false;

	return true;
}

bool SDirection::Init()
{
	return true;
}
bool SDirection::Frame()
{
	return true;
}
bool SDirection::Render(ID3D11DeviceContext*	pContext)
{
	m_pContext->UpdateSubresource(m_dxobj.g_pConstantBuffer.Get(), 0, nullptr, &m_cbData, 0, 0);

	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	m_dxobj.Render(pContext, m_dxobj.m_iVertexSize, m_dxobj.m_iNumIndex);
	return true;
}
bool SDirection::Release()
{
	SModel::Release();
	return true;
}

SDirection::SDirection()
{
	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_matView);
	D3DXMatrixIdentity(&m_matProj);
}


SDirection::~SDirection()
{
}