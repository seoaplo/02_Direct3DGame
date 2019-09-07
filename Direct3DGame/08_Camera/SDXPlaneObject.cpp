#include "SDXPlaneObject.h"



bool SDXPlaneObject::Load(ID3D11Device* pDevice, std::wstring filename)
{
	//m_Obj[0].LoadShaderAndInputlayout("VSRotation", "PS");
	LoadShaderAndInputlayout("VS", "PS");
	// local coordinate
	PNCT_VB vertices[4];
	vertices[0].p = DXGame::SVector3(-1.0f, 1.0f, 0.0f);
	vertices[0].n = DXGame::SVector3(0.0f, 0.0f, -1.0f);
	vertices[0].c = DXGame::SVector4(1.0f, 0.0f, 0.0f, 1.0f);
	vertices[0].t = DXGame::SVector2(0.0f, 0.0f);

	vertices[1].p = DXGame::SVector3(1.0f, 1.0f, 0.0f);
	vertices[1].n = DXGame::SVector3(0.0f, 0.0f, -1.0f);
	vertices[1].c = DXGame::SVector4(0.0f, 1.0f, 0.0f, 1.0f);
	vertices[1].t = DXGame::SVector2(1.0f, 0.0f);

	vertices[2].p = DXGame::SVector3(1.0f, -1.0f, 0.0f);
	vertices[2].n = DXGame::SVector3(0.0f, 0.0f, -1.0f);
	vertices[2].c = DXGame::SVector4(0.0f, 0.0f, 1.0f, 1.0f);
	vertices[2].t = DXGame::SVector2(1.0f, 1.0f);

	vertices[3].p = DXGame::SVector3(-1.0f, -1.0f, 0.0f);
	vertices[3].n = DXGame::SVector3(0.0f, 0.0f, -1.0f);
	vertices[3].c = DXGame::SVector4(1.0f, 1.0f, 1.0f, 1.0f);
	vertices[3].t = DXGame::SVector2(0.0f, 1.0f);

	int iNumCount = sizeof(vertices) / sizeof(vertices[0]);
	if (CreateVertexBuffer(
		vertices,
		iNumCount,
		sizeof(PNCT_VB)) == false)
	{
		return false;
	}

	DWORD indexlist[] =
	{
		1,2,0,
		2,3,0,
	};
	iNumCount = sizeof(indexlist) / sizeof(indexlist[0]);
	if (CreateIndexBuffer(
		indexlist, iNumCount, sizeof(DWORD)) == false) return false;



	Init();

	m_iTextureList.push_back(I_TextureManager.Load(m_pDevice, L"../../data/Image/Attacker3.png"));

	m_dxobj.g_pTextureSRV= I_TextureManager.GetPtr(m_iTextureList[0])->m_pSRV;


	//D3DXMatrixIdentity(&m_matWorld);
	DXGame::SMatrix matWorld;
	/*DXGame::SVector3 Eye(0.0f, 0.0f, -2.0f);
	DXGame::SVector3 At(0.0f, 0.0f, 0.0f);
	DXGame::SVector3 Up(0.0f, 1.0f, 0.0f);

	matWorld.VeiwLookAt(&Eye, &At, &Up);*/

	
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


	VS_CB vc;
	ZeroMemory(&vc, sizeof(VS_CB));
	vc.matWorld = matWorld;
	vc.matView = m_matView;
	vc.matProj = m_matProj;
	vc.color[0] = 1.0f;
	vc.etc[0] = 0.0f;
	iNumCount = 1;
	if (CreateConstantBuffer(
		(LPVOID)&vc, iNumCount, sizeof(VS_CB), true) == false) return false;


	return true;
}
bool SDXPlaneObject::CreateIndexBuffer(
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
bool SDXPlaneObject::CreateConstantBuffer(
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
bool SDXPlaneObject::CreateVertexBuffer(
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
bool SDXPlaneObject::LoadShaderAndInputlayout(
	const CHAR* pVertexShader,
	const CHAR* pPixelShader)
{
	HRESULT hr;
	m_dxobj.g_pVertexShader.Attach(
		DXGame::LoadVertexShaderFile(m_pDevice,
			L"../../shader/08_Camera/vertex.hlsl",
			m_dxobj.g_pVSBlob.GetAddressOf(),
			(char*)pVertexShader));
	m_dxobj.g_pPixelShader.Attach(
		DXGame::LoadPixelShaderFile(m_pDevice,
			L"../../shader/08_Camera/pixel.hlsl"));
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

	return true;
}
bool SDXPlaneObject::LoadSRV(T_STR name, int iIndex)
{
	/*xTexture tex;
	tex.Load(m_pDevice, name.c_str());
	m_Tex.push_back(tex);*/
	return true;
}
bool SDXPlaneObject::Init()
{
	//HRESULT hr;
	//if (CreateIndexBuffer() == false) return false;
	//if (CreateConstantBuffer() == false) return false;
	//SetBlendState();

	return true;
}
bool SDXPlaneObject::Render(ID3D11DeviceContext*	pContext)
{
	float fTime = I_Timer.GetElapsedTime();

	DXGame::SMatrix matWorld;
	matWorld.Identity();
	matWorld.TRanspose();

	D3DXMatrixTranspose(&m_matView, &m_matView);
	D3DXMatrixTranspose(&m_matProj, &m_matProj);

	D3D11_MAPPED_SUBRESOURCE msr;
	if (SUCCEEDED(m_pContext->Map(
		m_dxobj.g_pConstantBuffer.Get(), 0,
		D3D11_MAP_WRITE_DISCARD, 0, &msr)))
	{
		VS_CB* data = (VS_CB*)msr.pData;
		data->matWorld = matWorld;
		data->matView = m_matView;
		data->matProj = m_matProj;
		data->color[0] = cosf(fTime);
		data->color[1] = sinf(fTime);
		data->color[2] = 1 - cosf(fTime);
		data->color[3] = 1.0f;
		data->etc[0] = fTime;
		m_pContext->Unmap(m_dxobj.g_pConstantBuffer.Get(), 0);
	}

	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_dxobj.Render(pContext, sizeof(PNCT_VB), 6);
	return true;
}
bool SDXPlaneObject::Release()
{
	SModel::Release();
	return true;
}
bool SDXPlaneObject::Frame()
{
	return true;
}
SDXPlaneObject::SDXPlaneObject()
{

}


SDXPlaneObject::~SDXPlaneObject()
{

}
