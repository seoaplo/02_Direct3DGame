#include "Sample.h"


int Sample::WindowProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	if (m_pMainCamera == nullptr) return -1;
	return m_pMainCamera->WndProc(hWnd, Message, wParam, lParam);
}

S_BoxObject::S_BoxObject()
{
	m_Box.vCenter = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_Box.vMax = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	m_Box.vMin = D3DXVECTOR3(-1.0f, -1.0f, -1.0f);

	m_vPosition = D3DXVECTOR3(25 - rand() % 50, 0, 25 - rand() % 50);
	m_vColor = D3DXVECTOR4((rand() % 256) / 255.0f, (rand() % 256) / 255.0f, (rand() % 256) / 255.0f, 1.0f);

	// g_matWorld Matrix = S * R
	D3DXMATRIX matScale, matRotation, matWorld;
	D3DXMatrixScaling(&matScale, (rand() % 256) / 255.0f * 2.0f, (rand() % 256) / 255.0f * 2.0f, (rand() % 256) / 255.0f * 2.0f);
	D3DXMatrixRotationYawPitchRoll(&matRotation, (rand() % 256) / 255.0f, (rand() % 256) / 255.0f, (rand() % 256) / 255.0f);
	D3DXMatrixMultiply(&matWorld, &matScale, &matRotation);

	// OBB 기저벡터 변환( 회전 변환 이후 정규화 )
	m_Box.vCenter = m_vPosition;
	D3DXVECTOR3 vMax, vMin, vHalf;
	D3DXVec3TransformCoord(&m_Box.vAxis[0], &D3DXVECTOR3(1.0f, 0.0f, 0.0f), &matWorld);
	D3DXVec3TransformCoord(&m_Box.vAxis[1], &D3DXVECTOR3(0.0f, 1.0f, 0.0f), &matWorld);
	D3DXVec3TransformCoord(&m_Box.vAxis[2], &D3DXVECTOR3(0.0f, 0.0f, 1.0f), &matWorld);
	D3DXVec3Normalize(&m_Box.vAxis[0], &m_Box.vAxis[0]);
	D3DXVec3Normalize(&m_Box.vAxis[1], &m_Box.vAxis[1]);
	D3DXVec3Normalize(&m_Box.vAxis[2], &m_Box.vAxis[2]);

	// g_matWorld Matrix = S * R * T
	matWorld._41 = m_vPosition.x;
	matWorld._42 = m_vPosition.y;
	matWorld._43 = m_vPosition.z;

	// OBB 기저벡터의 크기 계산
	D3DXVec3TransformCoord(&vMax, &m_Box.vMax, &matWorld);
	D3DXVec3TransformCoord(&vMin, &m_Box.vMin, &matWorld);
	vHalf = vMax - m_Box.vCenter;
	m_Box.fExtent[0] = D3DXVec3Dot(&m_Box.vAxis[0], &vHalf);
	m_Box.fExtent[1] = D3DXVec3Dot(&m_Box.vAxis[1], &vHalf);
	m_Box.fExtent[2] = D3DXVec3Dot(&m_Box.vAxis[2], &vHalf);

	// AABB
	D3DXVECTOR3 vVertex[8];
	vVertex[0] = D3DXVECTOR3(m_Box.vMin.x, m_Box.vMax.y, m_Box.vMin.z);
	vVertex[1] = D3DXVECTOR3(m_Box.vMax.x, m_Box.vMax.y, m_Box.vMin.z);
	vVertex[2] = D3DXVECTOR3(m_Box.vMax.x, m_Box.vMin.y, m_Box.vMin.z);
	vVertex[3] = D3DXVECTOR3(m_Box.vMin.x, m_Box.vMin.y, m_Box.vMin.z);

	vVertex[4] = D3DXVECTOR3(m_Box.vMin.x, m_Box.vMax.y, m_Box.vMax.z);
	vVertex[5] = D3DXVECTOR3(m_Box.vMax.x, m_Box.vMax.y, m_Box.vMax.z);
	vVertex[6] = D3DXVECTOR3(m_Box.vMax.x, m_Box.vMin.y, m_Box.vMax.z);
	vVertex[7] = D3DXVECTOR3(m_Box.vMin.x, m_Box.vMin.y, m_Box.vMax.z);

	vMin = D3DXVECTOR3(100000.0f, 100000.0f, 100000.0f);
	vMax = D3DXVECTOR3(-100000.0f, -100000.0f, -100000.0f);
	for (int iVer = 0; iVer < 8; iVer++)
	{
		D3DXVec3TransformCoord(&vVertex[iVer], &vVertex[iVer], &matWorld);
		if (vVertex[iVer].x > vMax.x) vMax.x = vVertex[iVer].x;
		if (vVertex[iVer].y > vMax.y) vMax.y = vVertex[iVer].y;
		if (vVertex[iVer].z > vMax.z) vMax.z = vVertex[iVer].z;

		if (vVertex[iVer].x < vMin.x) vMin.x = vVertex[iVer].x;
		if (vVertex[iVer].y < vMin.y) vMin.y = vVertex[iVer].y;
		if (vVertex[iVer].z < vMin.z) vMin.z = vVertex[iVer].z;
	}

	m_Box.vMin = vMin;
	m_Box.vMax = vMax;
	m_matWorld = matWorld;
}

bool Sample::Init()
{
	m_Select.Init(m_hWnd, m_nClientWidth, m_nClientHeight);
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
	m_pMainCamera->SetViewMatrix(D3DXVECTOR3(0.0f, 10.0f, -50.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f));
	m_pMainCamera->SetProjMatrix(D3DX_PI * 0.25f, (float)m_nClientWidth / (float)(m_nClientHeight), 1.0f, 1000.0f);


	m_pMainCamera->Init();
	m_Line.Create(GetDevice(), GetContext());

	HRESULT hr = S_OK;

	//--------------------------------------------------------------------------------------
	// 박스 오브젝트 생성( 100개 박스가 공유해서 사용함)
	//--------------------------------------------------------------------------------------
	m_Box.Create(GetDevice(), GetContext());
	/*SAFE_NEW_ARRAY(m_pObj, S_BoxObject, NUM_OBJECTS);*/
	//--------------------------------------------------------------------------------------
	// 카메라 프로스텀 랜더링용 박스 오브젝트 생성
	//--------------------------------------------------------------------------------------
	m_pMainCamera->CreateRenderBox(GetDevice(), m_pImmediateContext);

	//--------------------------------------------------------------------------------------
	// 미니맵 영역에 랜더링할 랜더타켓용 텍스처 생성( 기본 카메라 : 탑뷰 ) 
	//--------------------------------------------------------------------------------------
	if (!m_MiniMap.Create(GetDevice(), GetContext()))
	{
		return false;
	}
	m_MiniMap.Set(GetDevice(), 0, m_SwapChainDesc.BufferDesc.Height - 300, 300, 300);

	iColl = 4;
	iNumSize = 4;
	fDistance = 10.0f;
	bCreateMap = true;
	return true;
}

bool Sample::Frame()
{
	m_Select.SetMatrix(&m_matWorld, &m_pMainCamera->_matView, &m_pMainCamera->_matProj);
	// 2초당 1회전( 1 초 * D3DX_PI = 3.14 )
	float t = I_Timer.GetElapsedTime() * D3DX_PI;
	m_pMainCamera->Frame();
	m_matWorld = m_pMainCamera->GetWorldMatrix();

	if (I_InputManager.KeyBoardState(DIK_1) == KEY_UP)
	{
		if (++m_iDrawDepth > 7) m_iDrawDepth = -1;
		m_QuadTree.SetRenderDepth(m_iDrawDepth);
	}
	//--------------------------------------------------------------------------------------
	// 미니맵 갱신
	//--------------------------------------------------------------------------------------
	m_Line.Frame();
	D3DXMatrixIdentity(&m_matWorld);
	m_MiniMap.Frame();

	if (bSaveMap)
	{
		SaveFile();
		bSaveMap = false;
	}
	if (bLoadMap)
	{
		LoadFile();
		bLoadMap = false;
	}

	if (bCreateMap)
	{
		int iCheck = iColl * iNumSize;
		if (iCheck != (iCheck&-iCheck))
		{
			MessageBox(0, _T("Cols가 2n의 크기가 아닙니다"), _T("Fatal error"), MB_OK);
			bCreateMap = false;
			return true;
		}

		m_HeightMap.Release();
		m_QuadTree.Release();

		SMapDesc MapDesc = { iColl, iColl, iNumSize, fDistance, 1.0f, L"../../data/map/castle.jpg", L"../../shader/Shape/Plane.hlsl" };
		m_Map.Init(GetDevice(), m_pImmediateContext);
		if (FAILED(m_Map.Load(MapDesc)))
		{
			bCreateMap = false;
			return true;
		}
		//--------------------------------------------------------------------------------------
		// 쿼드트리 공간분할
		//--------------------------------------------------------------------------------------
		// 프러스텀 카메라 세팅.
		m_QuadTree.Update(GetDevice(), m_pMainCamera.get());

		m_QuadTree.Build(&m_Map, 7.0f, 10.0f);
		/*for (int iBox = 0; iBox < NUM_OBJECTS; iBox++)
		{
			m_QuadTree.AddObject(&m_pObj[iBox]);
		}*/

		bMap = true;
		bCreateMap = false;
	}
	if (bMap)
	{
		//--------------------------------------------------------------------------------------
		// 쿼드트리를 사용하여 오브젝트 컬링
		//--------------------------------------------------------------------------------------
		m_QuadTree.Frame();
		m_Map.Frame();

		if (I_InputManager.MouseButtonState(0) == KEY_HOLD
			&& m_bHeightSet)
		{
			SetHeightVertex(m_QuadTree.m_pRootNode, m_fHeightDistance, m_fHeightValue);
		}
	}
	return true;

}
bool Sample::Render()
{

	m_DefaultRT.Begin(GetContext(), D3DXVECTOR4(0.3, 0.3, 0.3, 1));
	//--------------------------------------------------------------------------------------
	// Direction 오브젝트 랜더링
	//--------------------------------------------------------------------------------------
	m_Direction.SetMatrix(&m_matWorld, &m_pMainCamera->_matView, &m_pMainCamera->_matProj);
	m_Line.SetMatrix(&m_matWorld, &m_pMainCamera->_matView, &m_pMainCamera->_matProj);
	//=================================================================================
	// 쿼드트리 깊이 단위의 라인 렌더링
	//=================================================================================
	DrawQuadLine(m_QuadTree.m_pRootNode);

	DXGame::SDxState::SetDepthStencilState(GetContext(), DebugDSSDepthEnable, 0x00);

	//=================================================================================
	//쿼드트리의 임의의 레벨 선텍 레벨링
	//=================================================================================
	//DrawSelectTreeLevel(&m_pMainCamera->_matView, &m_pMainCamera->_matProj);

	//=================================================================================
	// 박스 렌더링
	//=================================================================================
	//DrawObject();


	//=================================================================================
	// 미니맵의 렌더 타겟 텍스쳐에 탑 뷰 렌더링
	//=================================================================================
	if (bMap)
	{
		DrawPickingTile(m_QuadTree.m_pRootNode);
		m_Map.SetMatrix(&m_matWorld, &m_pMainCamera->_matView, &m_pMainCamera->_matProj);
		m_QuadTree.Render(GetContext());
		DrawMiniMap();
		DrawPickingTile(m_QuadTree.m_pRootNode);
	}
	//m_Map.Render(GetContext());
	DXGame::SDxState::SetDepthStencilState(GetContext(), DebugDSSDepthDisable, 0x00);
	GetContext()->OMSetRenderTargets(1, SDevice::GetRenderTargetViewAddress(), SDevice::GetDepthStencilView());
	m_DefaultRT.End(GetContext());
	return true;
}
bool Sample::Release()
{
	m_HeightMap.Release();
	m_QuadTree.Release();
	return true;
}

bool Sample::DrawQuadLine(SNode* pNode)
{
	/*if (pNode == NULL) return false;
	if (m_QuadTree.m_iRenderDepth < pNode->m_dwDepth) return false;

	D3DXVECTOR4 vColor = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 1.0f);
	if (pNode->m_dwDepth == 0) vColor = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);
	if (pNode->m_dwDepth == 1) vColor = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
	if (pNode->m_dwDepth == 2) vColor = D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f);
	if (pNode->m_dwDepth == 3) vColor = D3DXVECTOR4(1.0f, 0.0f, 1.0f, 1.0f);
	if (pNode->m_dwDepth == 4) vColor = D3DXVECTOR4(1.0f, 1.0f, 0.0f, 1.0f);
	if (pNode->m_dwDepth == 5) vColor = D3DXVECTOR4(0.0f, 0.5f, 0.0f, 1.0f);
	if (pNode->m_dwDepth == 6) vColor = D3DXVECTOR4(1.0f, 0.5f, 0.0f, 1.0f);
	if (pNode->m_dwDepth == 7) vColor = D3DXVECTOR4(0.5f, 0.5f, 0.5f, 1.0f);
	if (pNode->m_dwDepth == 8) vColor = D3DXVECTOR4(1.0f, 0.5f, 0.5f, 1.0f);
	if (pNode->m_dwDepth == 9) vColor = D3DXVECTOR4(1.0f, 0.5f, 1.0f, 1.0f);

	S_BOX& TargetBox = pNode->m_sBox;
	D3DXVECTOR3 vPoint[4];
	vPoint[0] = D3DXVECTOR3(TargetBox.vMin.x, TargetBox.vMax.y, TargetBox.vMax.z);
	vPoint[0].y -= 1.0f * pNode->m_dwDepth;

	vPoint[1] = D3DXVECTOR3(TargetBox.vMax.x, TargetBox.vMax.y, TargetBox.vMax.z);
	vPoint[1].y -= 1.0f * pNode->m_dwDepth;

	vPoint[2] = D3DXVECTOR3(TargetBox.vMin.x, TargetBox.vMax.y, TargetBox.vMin.z);
	vPoint[2].y -= 1.0f * pNode->m_dwDepth;

	vPoint[3] = D3DXVECTOR3(TargetBox.vMax.x, TargetBox.vMax.y, TargetBox.vMin.z);
	vPoint[3].y -= 1.0f * pNode->m_dwDepth;

	m_Line.Draw(GetContext(), vPoint[0], vPoint[1], vColor);
	m_Line.Draw(GetContext(), vPoint[1], vPoint[3], vColor);
	m_Line.Draw(GetContext(), vPoint[2], vPoint[3], vColor);
	m_Line.Draw(GetContext(), vPoint[0], vPoint[2], vColor);

	if (pNode->m_ChildList.size() != 4) return true;
	for (int iNode = 0; iNode < 4; iNode++)
	{
		DrawQuadLine(pNode->m_ChildList[iNode]);
	}*/
	return true;
}
void Sample::DrawObject()
{
	for (int iBox = 0; iBox < NUM_OBJECTS; iBox++)
	{
		m_Box.SetAmbientColor(m_pObj[iBox].m_vColor.x,
			m_pObj[iBox].m_vColor.y,
			m_pObj[iBox].m_vColor.z,
			m_pObj[iBox].m_vColor.w);
		m_Box.SetMatrix(&m_pObj[iBox].m_matWorld, &m_pMainCamera->_matView, &m_pMainCamera->_matProj);

		// OBB와 프로스텀의 제외 처리(걸쳐있어도 TRUE)
		if (m_pMainCamera->CheckOBBInPlane(&m_pObj[iBox].m_Box))
		{
			m_Box.Render(GetContext());
		}
	}
}
void Sample::DrawSelectTreeLevel(D3DXMATRIX* pView, D3DXMATRIX* pProj)
{
	for (int iObj = 0; iObj < m_QuadTree.m_DrawObjList.size(); iObj++)
	{
		S_BoxObject* pBox = dynamic_cast<S_BoxObject*>(m_QuadTree.m_DrawObjList[iObj]);
		m_Box.SetAmbientColor(pBox->m_vColor.x,
			pBox->m_vColor.y,
			pBox->m_vColor.z,
			pBox->m_vColor.w);
		m_Box.SetMatrix(&pBox->m_matWorld, &m_pMainCamera->_matView, &m_pMainCamera->_matProj);
		m_Box.Render(GetContext());
	}
}

void Sample::DrawMiniMap()
{
	ID3D11ShaderResourceView* const pSRV[1] = { NULL };
	GetContext()->PSSetShaderResources(0, 1, pSRV);
	
	D3DXMATRIX matWorld, matView, matProj;
	matWorld = m_pMainCamera->_matWorld;
	matView = m_pMainCamera->_matView;
	matProj = m_pMainCamera->_matProj;
	if (m_MiniMap.BeginRender(GetContext()))
	{
		DrawMiniMapObject(m_QuadTree.m_pRootNode);
		m_pMainCamera->SetMatrix(&m_matInitWorld, &m_MiniMap._matView, &m_MiniMap._matProj);
		m_Line.SetMatrix(&m_matWorld, &m_MiniMap._matView, &m_MiniMap._matProj);
		DrawQuadLine(m_QuadTree.m_pRootNode);
		m_QuadTree.m_pMap->SetMatrix(&m_matInitWorld, &m_MiniMap._matView, &m_MiniMap._matProj);
		m_QuadTree.Render(GetContext());
		m_pMainCamera->Render(GetContext());
		m_MiniMap.EndRender(GetContext());
	}
	//=================================================================================
	// 렌더 타겟 텍스쳐를 미니맵 영역에 랜더링
	//=================================================================================
	m_MiniMap.Render(GetContext());

	m_pMainCamera->SetMatrix(&matWorld, &matView, &matProj);
}
void Sample::DrawMiniMapNode(SNode* pNode)
{

}
void Sample::DrawMiniMapObject(SNode* pNode)
{
	if (m_pMainCamera->CheckOBBInPlane(&pNode->m_sBox))
	{
		for (int iObj = 0; iObj < pNode->m_ObjectList.size(); iObj++)
		{
			S_BoxObject* pBox = dynamic_cast<S_BoxObject*>(pNode->m_ObjectList[iObj]);
			if (pBox && m_pMainCamera->ClassifyPoint(&pBox->m_Box.vCenter))
			{
				m_Box.SetAmbientColor(pBox->m_vColor.x,
					pBox->m_vColor.y,
					pBox->m_vColor.z,
					pBox->m_vColor.w);
				m_Box.SetMatrix(&pBox->m_matWorld, &m_MiniMap._matView, &m_MiniMap._matProj);
				m_Box.Render(GetContext());
			}
			else
			{
				if (pBox && m_pMainCamera->CheckOBBInPlane(&pBox->m_Box))
				{
					m_Box.SetAmbientColor(pBox->m_vColor.x,
						pBox->m_vColor.y,
						pBox->m_vColor.z,
						pBox->m_vColor.w);
					m_Box.SetMatrix(&pBox->m_matWorld, &m_MiniMap._matView, &m_MiniMap._matProj);
					m_Box.Render(GetContext());
				}
				else
				{
					float fDistance = 0.5f;// D3DXVec3Length(&m_pMainCamera->_pBoxShape->_vCenter);
					m_Box.SetAmbientColor(fDistance, fDistance, fDistance, 1.0f);
					m_Box.SetMatrix(&pBox->m_matWorld, &m_MiniMap._matView, &m_MiniMap._matProj);
					m_Box.Render(GetContext());
				}
			}
		}
	}
	else
	{
		// 프로스텀박스의 외부에 존재하는 경우
		for (int iObj = 0; iObj < pNode->m_ObjectList.size(); iObj++)
		{
			S_BoxObject* pBox = dynamic_cast<S_BoxObject*>(pNode->m_ObjectList[iObj]);
			float fDistance = D3DXVec3Length(&m_pMainCamera->_pBoxShape->_vCenter) / 100.0f;
			m_Box.SetAmbientColor(fDistance, fDistance, fDistance, 1.0f);
			m_Box.SetMatrix(&pBox->m_matWorld, &m_MiniMap._matView, &m_MiniMap._matProj);
			m_Box.Render(GetContext());
		}
	}
	for (auto ChildeNode : pNode->m_ChildList)
	{
		DrawMiniMapObject(ChildeNode);
	}
}
void Sample::SetHeightVertex(SNode* pNode, float fDistance, float fHeight)
{
	SNode* pFindeNode = FindePickingNode(pNode);
	if (pFindeNode == nullptr) return;

	m_QuadTree.UpVectexHeight(*pFindeNode, m_Select.m_vIntersection, fDistance, fHeight);
	
}
void Sample::DrawPickingTile(SNode* pNode)
{
	SNode* pFindeNode = FindePickingNode(pNode);

	if (pFindeNode == nullptr) return;
	
	S_BOX& TargetBox = pFindeNode->m_sBox;
	D3DXVECTOR3 vPoint[4];
	vPoint[0] = D3DXVECTOR3(TargetBox.vMin.x, TargetBox.vMax.y, TargetBox.vMax.z);
	vPoint[0].y -= 1.0f * pNode->m_dwDepth;

	vPoint[1] = D3DXVECTOR3(TargetBox.vMax.x, TargetBox.vMax.y, TargetBox.vMax.z);
	vPoint[1].y -= 1.0f * pNode->m_dwDepth;

	vPoint[2] = D3DXVECTOR3(TargetBox.vMin.x, TargetBox.vMax.y, TargetBox.vMin.z);
	vPoint[2].y -= 1.0f * pNode->m_dwDepth;

	vPoint[3] = D3DXVECTOR3(TargetBox.vMax.x, TargetBox.vMax.y, TargetBox.vMin.z);
	vPoint[3].y -= 1.0f * pNode->m_dwDepth;

	D3DXVECTOR4 vColor(1, 1, 1, 1);

	m_Line.Draw(GetContext(), vPoint[0], vPoint[1], vColor);
	m_Line.Draw(GetContext(), vPoint[1], vPoint[3], vColor);
	m_Line.Draw(GetContext(), vPoint[2], vPoint[3], vColor);
	m_Line.Draw(GetContext(), vPoint[0], vPoint[2], vColor);

}

SNode*	Sample::FindePickingNode(SNode* pNode)
{
	if (m_Select.ChkOBBToRay(&pNode->m_sBox, &m_Select.m_Ray) == false) return nullptr;
	if (pNode->m_ChildList.size() != 4) return pNode;
	for (int iNode = 0; iNode < 4; iNode++)
	{
		SNode* pCheckNode = FindePickingNode(pNode->m_ChildList[iNode]);
		if (pCheckNode != nullptr) return pCheckNode;
	}
	return nullptr;
}
HRESULT Sample::ScreenViewPort(UINT iWidth, UINT iHeight)
{
	HRESULT hr = S_OK;
	UINT iRectWidth = iWidth / 3;
	UINT iRectHeight = iHeight / 3;
	//====================================================
	// 미니맵 영역 설정
	//====================================================
	m_MiniMap.Set(0, iHeight - iRectHeight, iRectWidth, iRectHeight, 0.0f, 1.0f);
	return hr;
}
Sample::Sample()
{
	bCreateMap = false;
	m_pMainCamera = nullptr;
	m_bDebugRender = true;
	bMap = false;
	m_bHeightSet = false;
	bLoadMap = false;
	bSaveMap = false;

	m_fHeightValue = 0.0f;
	m_fHeightDistance = 0.0f;
	iRow = 3;
	iColl = 3;
	iNumSize = 3;
	fDistance = 10.0f;
}
Sample::~Sample()
{

}
bool Sample::LoadFile()
{
	m_QuadTree.Release();
	m_Map.Release();

	TCHAR ProgramPath[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH, ProgramPath);
	
	TCHAR* TargetFile = SaveFileDiallog(L"stm", L"MapTool");
	::SetCurrentDirectory(ProgramPath);
	if (TargetFile == nullptr)
	{
		return false;
	}
	m_Map.Init(GetDevice(), GetContext());
	m_QuadTree.Init();
	if (m_Map.LoadFile(TargetFile))
	{
		m_QuadTree.Update(GetDevice(), m_pMainCamera.get());
		m_QuadTree.Build(&m_Map, 7.0f, 10.0f);
		return true;
	}

	return false;
}
bool Sample::SaveFile()
{
	TCHAR ProgramPath[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH, ProgramPath);

	TCHAR* TargetFile = SaveFileDiallog(L"stm", L"MapTool");
	::SetCurrentDirectory(ProgramPath);
	if (TargetFile == nullptr)
	{
		return false;
	}
	m_Map.SaveFile(TargetFile);
	return true; 
}

TCHAR* Sample::SaveFileDiallog(const TCHAR* szName, const TCHAR* szTitle)
{
	TCHAR ARRAY_TCHAR_file[MAX_PATH] = { 0, };
	TCHAR ARRAY_TCHAR_filetitle[MAX_PATH] = { 0, };
	static TCHAR* Static_Pointer_TCHAR_filter;
	OPENFILENAME OPENFILENAME_filename;

	//=======================================================================
	// Initiallize
	//=======================================================================
	ZeroMemory(&OPENFILENAME_filename, sizeof(OPENFILENAME));
	_tcscpy_s(ARRAY_TCHAR_file, _T("*."));
	_tcscat_s(ARRAY_TCHAR_file, szName);
	_tcscat_s(ARRAY_TCHAR_file, _T("\0"));

	//=======================================================================
	// Set OPENFILENAME
	//=======================================================================
	OPENFILENAME_filename.lStructSize = sizeof(OPENFILENAME);
	OPENFILENAME_filename.hwndOwner = GetActiveWindow();
	OPENFILENAME_filename.lpstrFilter = Static_Pointer_TCHAR_filter;
	OPENFILENAME_filename.lpstrCustomFilter = NULL;
	OPENFILENAME_filename.nMaxCustFilter = 0L;
	OPENFILENAME_filename.nFilterIndex = 1;
	OPENFILENAME_filename.lpstrFile = ARRAY_TCHAR_file;
	OPENFILENAME_filename.nMaxFile = sizeof(ARRAY_TCHAR_file);
	OPENFILENAME_filename.lpstrFileTitle = ARRAY_TCHAR_filetitle;
	OPENFILENAME_filename.nMaxFileTitle = sizeof(ARRAY_TCHAR_filetitle);
	OPENFILENAME_filename.lpstrInitialDir = NULL;
	OPENFILENAME_filename.lpstrTitle = szTitle;
	OPENFILENAME_filename.Flags = 0L;
	OPENFILENAME_filename.nFileOffset = 0;
	OPENFILENAME_filename.nFileExtension = 0;
	OPENFILENAME_filename.lpstrDefExt = szName;


	//=======================================================================
	// Save File
	//=======================================================================
	if (!GetOpenFileName(&OPENFILENAME_filename))
	{
		return NULL;
	}
	return ARRAY_TCHAR_file;
}

void   Sample::SetTool(HWND hWnd, HINSTANCE hInst)
{
	m_hWnd = hWnd;
	m_hInstance = hInst;
	GetClientRect(m_hWnd, &m_rcClientRect);
	GetWindowRect(m_hWnd, &m_rcWindowBounds);
	m_nClientHeight = m_rcClientRect.bottom;
	m_nClientWidth = m_rcClientRect.right;
}