#include "SObject.h"

bool SObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const TCHAR* pLoadShaderFile)
{
	if (pDevice == nullptr)
	{
		MessageBox(0, _T("pDevice가 nullptr입니다"), _T("Fatal error"), MB_OK);
		return false;
	}

	m_pDevice = pDevice;
	m_pContext = pContext;

	if (FAILED(LoadShaderFile(pDevice, pLoadShaderFile)))
	{
		MessageBox(0, _T("LoadShaderFile 실패"), _T("Fatal error"), MB_OK);
		return false;
	}
	if (FAILED(SetInputLayout()))
	{
		MessageBox(0, _T("SetInputLayout 실패"), _T("Fatal error"), MB_OK);
		return false;
	}
	if (!CreateVertexData())
	{
		MessageBox(0, _T("CreateVertexData 실패"), _T("Fatal error"), MB_OK);
		return false;
	}
	if (!CreateIndexData())
	{
		MessageBox(0, _T("CreateIndexData 실패"), _T("Fatal error"), MB_OK);
		return false;
	}
	if (FAILED(CreateVertexBuffer()))
	{
		MessageBox(0, _T("CreateVertexBuffer 실패"), _T("Fatal error"), MB_OK);
		return false;
	}
	if (FAILED(CreateIndexBuffer()))
	{
		MessageBox(0, _T("CreateIndexBuffer 실패"), _T("Fatal error"), MB_OK);
		return false;
	}
	if (FAILED(CreateConstantBuffer()))
	{
		MessageBox(0, _T("CreateConstantBuffer 실패"), _T("Fatal error"), MB_OK);
		return false;
	}
	if (FAILED(LoadTextures(pDevice)))
	{
		MessageBox(0, _T("LoadTextures 실패"), _T("Fatal error"), MB_OK);
		return false;
	}
	if (!UpdateBuffer())
	{
		MessageBox(0, _T("UpdateBuffer 실패"), _T("Fatal error"), MB_OK);
		return false;
	}
	if (FAILED(CreateResource()))
	{
		MessageBox(0, _T("CreateResource 실패"), _T("Fatal error"), MB_OK);
		return false;
	}

	m_matWorld = m_pMesh->m_matWorld;
	return Init();
}
HRESULT SObject::LoadTextures(ID3D11Device* pDevice)
{

	HRESULT hr = S_OK;
	
	for (int iSubMesh = 0; iSubMesh < m_pMesh->iSubMeshNum; iSubMesh++)
	{
		if (m_pMaterial == nullptr)
		{
			m_pMesh->m_dxobjList[iSubMesh].g_pTextureSRV = nullptr;
		}
		else if (m_pMaterial->SubMaterial.size() <= 0
			|| m_pMaterial->SubMaterial[iSubMesh].TextrueMapList.size() <= 0
			|| m_pMesh->m_SubMaterialNum.size() <= 0)
		{
			m_pMesh->m_dxobjList[iSubMesh].g_pTextureSRV = nullptr;
		}
		else
		{
			int iSubMaterial = m_pMesh->m_SubMaterialNum[iSubMesh];
			m_pMesh->m_dxobjList[iSubMesh].g_pTextureSRV = m_pMaterial->SubMaterial[iSubMesh].TextrueMapList[0].STexture->m_pSRV;
		}
	}

	return hr;
}
HRESULT SObject::LoadShaderFile(ID3D11Device* pDevice, const TCHAR* pLoadShaderFile)
{
	for (int iSubMesh = 0; iSubMesh < m_pMesh->iSubMeshNum; iSubMesh++)
	{
		DXGame::SDxHelperEX& dxobj = m_pMesh->m_dxobjList[iSubMesh];
		dxobj.g_pVertexShader.Attach(DXGame::LoadVertexShaderFile(pDevice, pLoadShaderFile, dxobj.g_pVSBlob.GetAddressOf()));;
		dxobj.g_pPixelShader.Attach(DXGame::LoadPixelShaderFile(pDevice, pLoadShaderFile));;
		dxobj.g_pGeometryShader.Attach(DXGame::LoadGeometryShaderFile(pDevice, pLoadShaderFile, dxobj.g_pGSBlob.GetAddressOf()));;
		dxobj.g_pHullShader.Attach(DXGame::LoadHullShaderFile(pDevice, pLoadShaderFile, dxobj.g_pHSBlob.GetAddressOf()));;
		dxobj.g_pDomainShader.Attach(DXGame::LoadDomainShaderFile(pDevice, pLoadShaderFile, dxobj.g_pDSBlob.GetAddressOf()));;
		dxobj.g_pComputeShader.Attach(DXGame::LoadComputeShaderFile(pDevice, pLoadShaderFile, dxobj.g_pCSBlob.GetAddressOf()));;
	}
	return S_OK;
}
HRESULT SObject::CreateVertexBuffer()
{
	for (int iSubMesh = 0; iSubMesh < m_pMesh->iSubMeshNum; iSubMesh++)
	{
		DXGame::SDxHelperEX& dxobj = m_pMesh->m_dxobjList[iSubMesh];
		dxobj.m_iNumVertex = m_pMesh->m_VertexList[iSubMesh].size();
		dxobj.m_iVertexSize = sizeof(PNCT_VERTEX);

		if (dxobj.m_iNumVertex <= 0) continue;
		void** pData = nullptr;
		if (m_pMesh->m_VertexList[iSubMesh].size() > 0) pData = (void**)&m_pMesh->m_VertexList[iSubMesh].at(0);

		
		dxobj.g_pVertexBuffer.Attach(DXGame::CreateVertexBuffer(m_pDevice,
			pData,
			dxobj.m_iNumVertex,
			dxobj.m_iVertexSize));
	}

	return S_OK;
}
HRESULT	SObject::CreateIndexBuffer()
{

	for (int iSubMesh = 0; iSubMesh < m_pMesh->iSubMeshNum; iSubMesh++)
	{
		DXGame::SDxHelperEX& dxobj = m_pMesh->m_dxobjList[iSubMesh];
		dxobj.m_iNumIndex = m_pMesh->m_IndexList[iSubMesh].size();
		dxobj.m_iIndexSize = sizeof(DWORD);

		if (dxobj.m_iNumIndex <= 0) continue;
		void** pData = nullptr;
		if (m_pMesh->m_IndexList[iSubMesh].size() > 0) pData = (void**)&m_pMesh->m_IndexList[iSubMesh].at(0);

		dxobj.g_pIndexBuffer.Attach(DXGame::CreateIndexBuffer(m_pDevice,
			pData,
			dxobj.m_iNumIndex,
			dxobj.m_iIndexSize));
	}

	return S_OK;
}
HRESULT SObject::CreateConstantBuffer()
{
	for (int iSubMesh = 0; iSubMesh < m_pMesh->iSubMeshNum; iSubMesh++)
	{
		DXGame::SDxHelperEX& dxobj = m_pMesh->m_dxobjList[iSubMesh];
		dxobj.g_pConstantBuffer.Attach(DXGame::CreateConstantBuffer(m_pDevice, &m_cbData, 1, sizeof(VS_CONSTANT_BUFFER)));
	}
	return S_OK;
}
bool SObject::Draw(ID3D11DeviceContext* pContext, D3DXVECTOR3 vStart, D3DXVECTOR3 vPost, D3DXVECTOR4 vColor) { return true; }
HRESULT SObject::SetInputLayout()
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
	for (int iSubMesh = 0; iSubMesh < m_pMesh->iSubMeshNum; iSubMesh++)
	{
		DXGame::SDxHelperEX& dxobj = m_pMesh->m_dxobjList[iSubMesh];
		dxobj.g_pInputlayout.Attach(DXGame::CreateInputlayout(m_pDevice, dxobj.g_pVSBlob.Get()->GetBufferSize(),
			dxobj.g_pVSBlob.Get()->GetBufferPointer(), layout, numElements));
	}
	return hr;
}
bool SObject::GetAnimationTrack(
	float fElapseTime,
	std::vector<SAnimTrack>& trackList,
	SAnimTrack& StartTrack,
	SAnimTrack& EndTrack)
{
	for (auto& track : trackList)
	{
		if (track.i > fElapseTime)
		{
			EndTrack = track;
			break;
		}
		StartTrack = track;
	}
	return true;
}

void SObject::Interpolate(
	SMesh& sMesh,
	D3DXMATRIX &matParent,
	float fElapseTime)
{
	D3DXMATRIX matAnimScale, matAnimRot, matAnimPos;
	matAnimScale = sMesh.m_matScl;
	matAnimRot = sMesh.m_matRot;
	matAnimPos = sMesh.m_matPos;

	D3DXMatrixIdentity(&sMesh.m_matCalculation);

	SAnimTrack StartTrack, EndTrack;

	ZeroMemory(&StartTrack, sizeof(StartTrack));
	ZeroMemory(&EndTrack, sizeof(EndTrack));
	D3DXQUATERNION qRotatin;
	D3DXQUATERNION qScale;
	D3DXQuaternionRotationMatrix(&qRotatin, &matAnimRot);
	D3DXQuaternionRotationMatrix(&qScale, &matAnimScale);

	if (sMesh.m_RotAnimList.size())
	{
		if (GetAnimationTrack(
			fElapseTime,
			sMesh.m_RotAnimList,
			StartTrack,
			EndTrack))
		{
			float t = (fElapseTime - StartTrack.i) / (EndTrack.i - StartTrack.i);
			D3DXQuaternionSlerp(&qRotatin,
				&StartTrack.q,
				&EndTrack.q, t);
		}
		D3DXMatrixRotationQuaternion(&matAnimRot, &qRotatin);
	}
	ZeroMemory(&StartTrack, sizeof(StartTrack));
	ZeroMemory(&EndTrack, sizeof(EndTrack));

	D3DXMATRIX matScaleRotation, matScaleRotInverse, matScaleVector;
	D3DXVECTOR3 vTrans(matAnimPos._41, matAnimPos._42, matAnimPos._43);
	if (sMesh.m_ScaleAnimList.size())
	{
		if (GetAnimationTrack(
			fElapseTime,
			sMesh.m_ScaleAnimList,
			StartTrack,
			EndTrack))
		{
			float t = (fElapseTime - StartTrack.i) / (EndTrack.i - StartTrack.i);
			D3DXQuaternionSlerp(&qRotatin,
				&StartTrack.q,
				&EndTrack.q, t);
			D3DXVec3Lerp(&vTrans,
				&StartTrack.p,
				&EndTrack.p, t);
		}
		D3DXMatrixScaling(&matScaleVector, vTrans.x, vTrans.y, vTrans.z);
		D3DXMatrixRotationQuaternion(&matScaleRotation, &qRotatin);
		D3DXMatrixInverse(&matScaleRotInverse, NULL, &matScaleRotation);
		matAnimScale = matScaleRotInverse * matScaleVector * matScaleRotation;
	}

	ZeroMemory(&StartTrack, sizeof(StartTrack));
	ZeroMemory(&EndTrack, sizeof(EndTrack));
	if (sMesh.m_PosAnimList.size())
	{
		if (GetAnimationTrack(
			fElapseTime, sMesh.m_PosAnimList, StartTrack, EndTrack))
		{
			float t = (fElapseTime - StartTrack.i) / (EndTrack.i - StartTrack.i);
			D3DXVec3Lerp(&vTrans,
				&StartTrack.p,
				&EndTrack.p, t);
		}
		D3DXMatrixTranslation(&matAnimPos, vTrans.x,
			vTrans.y, vTrans.z);
	}

	D3DXMATRIX matAnim;
	D3DXMatrixIdentity(&matAnim);
	matAnim = matAnimScale * matAnimRot;
	//matAnim = matAnimRot;
	matAnim._41 = matAnimPos._41;
	matAnim._42 = matAnimPos._42;
	matAnim._43 = matAnimPos._43;

	D3DXMATRIX matinvParent;
	D3DXMatrixIdentity(&matinvParent);
	if (sMesh.m_pParent != nullptr)
	{
		//matinvParent = sMesh.m_pParent->m_matCalculationBasic;
		D3DXMatrixInverse(&matinvParent, nullptr, &sMesh.m_matWorld);
	}

	sMesh.m_matCalculation =  matAnim * matParent;
	return;
}
bool SObject::UpdateBuffer() { return true; }
bool SObject::Init() { return true; }
bool SObject::Frame() 
{
	m_fElapseTime += 1.00f *  I_Timer.GetSPF() *
		m_pMesh->m_Scene.iFrameSpeed *
		m_pMesh->m_Scene.iTickPerFrame;

	float fEndTime = m_pMesh->m_Scene.iLastFrame *
		m_pMesh->m_Scene.iTickPerFrame;
	if (m_fElapseTime >= fEndTime)
	{
		m_fElapseTime = 0.0f;
	}
	D3DXMATRIX matParent;
	D3DXMatrixIdentity(&matParent);

	if (m_pMesh->m_pParent != nullptr)
	{
		Interpolate(*m_pMesh,
			m_pMesh->m_pParent->m_matCalculation,
			m_fElapseTime);
	}
	else
	{
		Interpolate(*m_pMesh,
			matParent,
			m_fElapseTime);
	}

	return true;
}
bool SObject::Release() { return true; }
bool SObject::PreRender(ID3D11DeviceContext* pContext)
{
	pContext->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)m_dxobj.m_iPrimitiveType);
	for (int iSubMesh = 0; iSubMesh < m_pMesh->iSubMeshNum; iSubMesh++)
	{
		DXGame::SDxHelperEX& dxobj = m_pMesh->m_dxobjList[iSubMesh];
		dxobj.PreRender(pContext, dxobj.m_iVertexSize);
	}
	return true;
}
void SObject::UpdateConstantBuffer(ID3D11DeviceContext* pContext, SObject* pParent)
{
	for (int iSubMesh = 0; iSubMesh < m_pMesh->iSubMeshNum; iSubMesh++)
	{
		DXGame::SDxHelperEX& dxobj = m_pMesh->m_dxobjList[iSubMesh];
		if (pParent != nullptr)
		{
			pContext->UpdateSubresource(pParent->m_dxobj.g_pConstantBuffer.Get(), 0, nullptr, &pParent->m_cbData, 0, 0);
		}
		else
		{
			pContext->UpdateSubresource(dxobj.g_pConstantBuffer.Get(), 0, nullptr, &m_cbData, 0, 0);
		}
	}
}
bool SObject::PostRender(ID3D11DeviceContext* pContext)
{
	UpdateConstantBuffer(pContext);
	for (int iSubMesh = 0; iSubMesh < m_pMesh->iSubMeshNum; iSubMesh++)
	{
		DXGame::SDxHelperEX& dxobj = m_pMesh->m_dxobjList[iSubMesh];
		dxobj.PostRender(pContext, dxobj.m_iNumIndex);
	}
	return true;
}
bool SObject::Render(ID3D11DeviceContext* pContext)
{
	pContext->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)m_dxobj.m_iPrimitiveType);
	for (int iSubMesh = 0; iSubMesh < m_pMesh->iSubMeshNum; iSubMesh++)
	{
		DXGame::SDxHelperEX& dxobj = m_pMesh->m_dxobjList[iSubMesh];
		
		D3DXMATRIX matWorld = m_pMesh->m_matInvWorld * m_pMesh->m_matCalculation;

		D3DXMatrixTranspose(&m_cbData.matWorld,
			&matWorld);

		float fTime = I_Timer.GetElapsedTime();
		m_cbData.Color[0] = cosf(fTime);
		m_cbData.Color[1] = sinf(fTime);
		m_cbData.Color[2] = 1 - cosf(fTime);
		m_cbData.Color[3] = 1.0f;
		pContext->UpdateSubresource(
			dxobj.g_pConstantBuffer.Get(),
			0, NULL, &m_cbData, 0, 0);
		dxobj.PreRender(pContext, dxobj.m_iVertexSize);

		pContext->VSSetConstantBuffers(0, 1, dxobj.g_pConstantBuffer.GetAddressOf());

		dxobj.PostRender(pContext, dxobj.m_iNumIndex);
	}

	return true;
}
HRESULT SObject::CreateResource()
{
	m_dxobj.m_iPrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	return S_OK;
}
HRESULT SObject::DeleteResource()
{
	return S_OK;
}
SObject::SObject()
{
	m_pDevice = nullptr;
	m_cbData.Color = D3DXVECTOR4(1, 1, 1, 1);
	m_vCenter = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXMatrixIdentity(&m_matView);
	D3DXMatrixIdentity(&m_matProj);
	D3DXMatrixIdentity(&m_matWorld);
	m_fElapseTime = 0.0f;

}

SObject::~SObject()
{
}
