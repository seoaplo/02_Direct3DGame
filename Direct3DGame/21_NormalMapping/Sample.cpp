#include "Sample.h"
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	Sample g_Sample;
	RECT rc{ 0, 0, 1024, 768 };
	if (!g_Sample.InitWindow(hInstance, nCmdShow, const_cast<TCHAR*>(L"TextureTest"), rc, false))
	{
		MessageBox(0, _T("Sample  초기화 실패"), _T("Fatal error"), MB_OK);
		return 0;
	}
	ShowCursor(TRUE);
	g_Sample.Run();
	return 1;
}
int Sample::WindowProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	if(m_pMainCamera == nullptr) return Message;
		return m_pMainCamera->WndProc(hWnd, Message, wParam, lParam);
}
HRESULT Sample::CreateConstantBuffer()
{
	HRESULT hr;
	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(VS_CBChangesEveryFrame);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	hr = GetDevice()->CreateBuffer(&cbDesc, NULL, m_pConstantBuffer.GetAddressOf());
	if (FAILED(hr))
		return hr;

	cbDesc.ByteWidth = sizeof(VS_CBNeverChanges);
	m_cbNeverChanges.cb_AmbientLightColor = D3DXVECTOR4(0.1f, 0.1f, 0.1f, 1);
	m_cbNeverChanges.cb_DiffuseLightColor = D3DXVECTOR4(1, 1, 1, 1);
	m_cbNeverChanges.cb_SpecularLightColor = D3DXVECTOR4(1, 1, 1, 10.0f);

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = &m_cbNeverChanges;
	hr = GetDevice()->CreateBuffer(&cbDesc, &InitData, m_pCBNeverChanges.GetAddressOf());
	if (FAILED(hr))
		return hr;

	return hr;
}

bool Sample::Init()
{
	m_pSunLight = I_Light.GetPtr(I_Light.Add(D3DXVECTOR3(0.0f, 10.0f, 0.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), L"SBasisSunLight"));
	if (!m_pSunLight->Init()) return false;

	m_pBoxObj = make_shared<SSampleObj>();
	m_pBoxObj->m_strNormalMapName = _T("../../data/NormalMap/test_normal_map.bmp");
	if (m_pBoxObj->Create(GetDevice(), GetContext(), L"../../data/cube/grassenvmap1024.dds", L"../../shader/21_NormalMapping/HeightMap_2.hlsl") == false)
	{
		MessageBox(0, _T("m_pBoxObj 실패"), _T("Fatal error"), MB_OK);
		return 0;
	}

	/*m_pBoxObj = make_shared<SSampleObj>();
	m_pBoxObj->m_strNormalMapName = _T("../../data/NormalMap/test_normal_map.bmp");
	if (m_pBoxObj->Create(GetDevice(), GetContext(), L"../../data/NormalMap/tileA.jpg", L"../../shader/21_NormalMapping/NormalMap_0.hlsl") == false)
	{
		MessageBox(0, _T("m_pBoxObj 실패"), _T("Fatal error"), MB_OK);
		return 0;
	}*/

	//--------------------------------------------------------------------------------------
	// 박스 오브젝트를 구 오브젝트로 변환(기하 쉐이더 및 스트림 아웃 처리)
	//--------------------------------------------------------------------------------------
	if (FAILED(m_SphereObj.Create(GetDevice(), GetContext(), L"../../data/cube/grassenvmap1024.dds", L"../../shader/21_NormalMapping/HeightMap_2.hlsl")))
	{
		MessageBox(0, _T("m_SphereObj 실패"), _T("Fatal error"), MB_OK);
		return 0;
	}
	m_SphereObj.SetSO(GetDevice(), GetContext());


	// L"../../data/baseColor.jpg"
	CreateConstantBuffer();

	//--------------------------------------------------------------------------------------
	// 높이맵 생성
	//--------------------------------------------------------------------------------------
	m_HeightMap.Init(GetDevice(), m_pImmediateContext);
	if (m_HeightMap.CreateHeightMap(L"../../data/HEIGHT_CASTLE.bmp") == false)
	{
		return false;
	}
	//  L"../../data/cube/grassenvmap1024.dds"
	// L"../../data/castle.jpg"
	SMapDesc MapDesc = { m_HeightMap.m_iNumRows,
							m_HeightMap.m_iNumCols,
							1.0f, 1.0f,
							L"../../data/castle.jpg",
							L"../../shader/21_NormalMapping/NormalMap_0.hlsl" };
	if (!m_HeightMap.Load(MapDesc))
	{
		return false;
	}
	//--------------------------------------------------------------------------------------
	// 월드  행렬
	//--------------------------------------------------------------------------------------
	D3DXMATRIX matScale, matTrans;
	D3DXMatrixScaling(&m_matInitWorld, 1.0f, 1.0f, 1.0f);
	//--------------------------------------------------------------------------------------
	// 카메라  행렬 
	//--------------------------------------------------------------------------------------	
	m_pMainCamera = make_shared<SCamera>();

	float fAspectRatio = m_nWindowWidth / (FLOAT)m_nWindowHeight;
	m_pMainCamera->SetViewMatrix(D3DXVECTOR3(0.0f, 0.0f, -10.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	m_pMainCamera->SetProjMatrix(D3DX_PI * 0.25f,
		m_SwapChainDesc.BufferDesc.Width / (float)(m_SwapChainDesc.BufferDesc.Height),
		1.0f, 1000.0f);

	m_pMainCamera->Init();

	m_vInitLightPos = D3DXVECTOR3(100.0f, 100.0f, 0.0f);
	return true;
}

bool Sample::Frame()
{

	// 2초당 1회전( 1 초 * D3DX_PI = 3.14 )
	float t = I_Timer.GetElapsedTime() * D3DX_PI;
	m_pMainCamera->Frame();
	//D3DXMatrixMultiply(&m_matWorld, &m_matInitWorld, &m_pMainCamera->_matWorld);

	D3DXMatrixIdentity(&m_matWorld);

	//--------------------------------------------------------------------------------------
	// 라이트 벡터 계산
	//--------------------------------------------------------------------------------------
	D3DXMATRIX mTranslate, mRotation;
	D3DXMatrixTranslation(&mTranslate, m_vInitLightPos.x, m_vInitLightPos.y, m_vInitLightPos.z);
	D3DXMatrixRotationY(&mRotation, t * 0.0f);
	D3DXMatrixMultiply(&m_mLightWorld, &mTranslate, &mRotation);

	// 추가
	m_pSunLight->Update(&m_mLightWorld);

	D3D11_MAPPED_SUBRESOURCE MappedFaceDest;
	if (SUCCEEDED(m_pImmediateContext->Map((ID3D11Resource*)m_pConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedFaceDest)))
	{
		VS_CBChangesEveryFrame* pConstData = (VS_CBChangesEveryFrame*)MappedFaceDest.pData;

		pConstData->matNormal = m_pBoxObj->m_matNormal;
		pConstData->vLightPos = m_pSunLight->m_vPosition;
		pConstData->vEye = m_pMainCamera->_vPos;
		pConstData->vEyeDir = m_pMainCamera->_vLook;
		m_pImmediateContext->Unmap(m_pConstantBuffer.Get(), 0);
	}
	m_HeightMap.Frame();
	m_SphereObj.Frame();

	return true;
}
bool Sample::Render()
{
	m_Direction.SetMatrix(&m_matWorld, &m_pMainCamera->_matView, &m_pMainCamera->_matProj);
	if (I_InputManager.KeyBoardState(DIK_P))
	{
		m_pBoxObj->SetMatrix(&m_matWorld, &m_pMainCamera->_matView, &m_pMainCamera->_matProj);
		m_pBoxObj->PreRender(m_pImmediateContext);
		m_pImmediateContext->PSSetShaderResources(1, 1, &m_pBoxObj->m_pNormalTexture->m_pSRV);
		m_pImmediateContext->VSSetConstantBuffers(1, 1, m_pConstantBuffer.GetAddressOf());
		m_pImmediateContext->VSSetConstantBuffers(2, 1, m_pCBNeverChanges.GetAddressOf());
		m_pImmediateContext->GSSetConstantBuffers(1, 1, m_pConstantBuffer.GetAddressOf());
		m_pImmediateContext->GSSetConstantBuffers(2, 1, m_pCBNeverChanges.GetAddressOf());
		m_pImmediateContext->PSSetConstantBuffers(1, 1, m_pConstantBuffer.GetAddressOf());
		m_pImmediateContext->PSSetConstantBuffers(2, 1, m_pCBNeverChanges.GetAddressOf());
		m_pBoxObj->PostRender(m_pImmediateContext);

	}
	

	if (I_InputManager.KeyBoardState(DIK_O))
	{
		m_HeightMap.SetMatrix(&m_matWorld, &m_pMainCamera->_matView, &m_pMainCamera->_matProj);
		m_HeightMap.PreRender(m_pImmediateContext);
		m_pImmediateContext->PSSetShaderResources(1, 1, &m_pBoxObj->m_pNormalTexture->m_pSRV);
		m_pImmediateContext->VSSetConstantBuffers(1, 1, m_pConstantBuffer.GetAddressOf());
		m_pImmediateContext->VSSetConstantBuffers(2, 1, m_pCBNeverChanges.GetAddressOf());
		m_pImmediateContext->GSSetConstantBuffers(1, 1, m_pConstantBuffer.GetAddressOf());
		m_pImmediateContext->GSSetConstantBuffers(2, 1, m_pCBNeverChanges.GetAddressOf());
		m_pImmediateContext->PSSetConstantBuffers(1, 1, m_pConstantBuffer.GetAddressOf());
		m_pImmediateContext->PSSetConstantBuffers(2, 1, m_pCBNeverChanges.GetAddressOf());
		m_HeightMap.PostRender(m_pImmediateContext);
	}
	
	

	m_SphereObj.SetMatrix(&m_matWorld, &m_pMainCamera->_matView, &m_pMainCamera->_matProj);
	m_SphereObj.PreRender(m_pImmediateContext);
	m_pImmediateContext->PSSetShaderResources(0, 1, &m_SphereObj._dxobj.g_pTextureSRV);
	m_pImmediateContext->VSSetConstantBuffers(1, 1, m_pConstantBuffer.GetAddressOf());
	m_pImmediateContext->VSSetConstantBuffers(2, 1, m_pCBNeverChanges.GetAddressOf());
	m_pImmediateContext->PSSetConstantBuffers(1, 1, m_pConstantBuffer.GetAddressOf());
	m_pImmediateContext->PSSetConstantBuffers(2, 1, m_pCBNeverChanges.GetAddressOf());
	m_SphereObj.PostRender(m_pImmediateContext);
	return true;
}
bool Sample::Release()
{
	return true;
}
HRESULT Sample::CreateResource()
{
	HRESULT hr = S_OK;
	//float fAspectRatio = m_SwapChainDesc.BufferDesc.Width / (float)m_SwapChainDesc.BufferDesc.Height;
	//m_pMainCamera->SetProjMatrix(D3DX_PI / 4, fAspectRatio, 0.1f, 500.0f);

	return S_OK;
}
HRESULT Sample::DeleteResource()
{
	HRESULT hr = S_OK;
	if (m_pImmediateContext) m_pImmediateContext->ClearState();
	return S_OK;
}

Sample::Sample()
{
	m_pMainCamera = nullptr;
	m_bDebugRender = true;
	m_bNormalMapRender = true;
}
Sample::~Sample()
{

}
