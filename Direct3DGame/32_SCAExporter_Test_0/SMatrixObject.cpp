#include "SMatrixObject.h"


SMatrixMesh* SMatrixObject::CreateMesh()
{
	m_MeshList.push_back(SMatrixMesh());
	return &m_MeshList[m_MeshList.size() - 1];
}
bool SMatrixObject::Init()
{
	return true;
}
bool SMatrixObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const TCHAR* pShaderFile)
{

	for (auto& TargetMesh : m_MeshList)
	{
		TargetMesh.Create(pDevice, pContext, nullptr, pShaderFile);
	}

	m_matAnimSelf = m_matAnim;
	D3DXMatrixInverse(
		&m_matAnimInv, NULL,
		&m_matAnim);

	D3DXMatrixDecompose(
		&m_vScaleTrack,
		&m_qRotTrack,
		&m_vPosTrack,
		&m_matAnimSelf);

	D3DXMatrixTranslation(&m_matPos,
		m_vPosTrack.x,
		m_vPosTrack.y,
		m_vPosTrack.z);
	D3DXMatrixScaling(&m_matScl,
		m_vScaleTrack.x,
		m_vScaleTrack.y,
		m_vScaleTrack.z);
	D3DXMatrixRotationQuaternion(
		&m_matRot,
		&m_qRotTrack);

	return true;
}
bool SMatrixObject::Frame(float fAnimElapseTime)
{
	Interpolate(fAnimElapseTime);
	return true;
}
bool SMatrixObject::Render(ID3D11DeviceContext* pContext)
{
	D3DXMATRIX matWorld = m_matAnimInv * m_matCalculation * m_matWorld;
	for (auto& TargetMesh : m_MeshList)
	{
		TargetMesh.SetMatrix(&matWorld, &m_matView, &m_matProj);
		TargetMesh.Render(pContext);
	}
	return true;
}
bool SMatrixObject::Release()
{
	return true;
}


void SMatrixObject::Interpolate( float fElapseTime)
{
	D3DXMATRIX matAnimScale, matAnimRot, matAnimPos;
	matAnimScale = m_matScl;
	matAnimRot = m_matRot;
	matAnimPos = m_matPos;

	D3DXMatrixIdentity(&m_matCalculation);


	SAnimTrack StartTrack, EndTrack;

	ZeroMemory(&StartTrack, sizeof(StartTrack));
	ZeroMemory(&EndTrack, sizeof(EndTrack));
	D3DXQUATERNION qRotatin;
	D3DXQUATERNION qScale;
	D3DXQuaternionRotationMatrix(&qRotatin, &matAnimRot);
	D3DXQuaternionRotationMatrix(&qScale, &matAnimScale);

	if (m_RotAnimList.size())
	{
		if (GetAnimationTrack(
			fElapseTime,
			m_RotAnimList,
			StartTrack,
			EndTrack))
		{
			float t = (fElapseTime - StartTrack.iTick) / (EndTrack.iTick - StartTrack.iTick);
			D3DXQuaternionSlerp(&qRotatin,
				&StartTrack.Quaternion,
				&EndTrack.Quaternion, t);
		}
		D3DXMatrixRotationQuaternion(&matAnimRot, &qRotatin);
	}
	ZeroMemory(&StartTrack, sizeof(StartTrack));
	ZeroMemory(&EndTrack, sizeof(EndTrack));

	D3DXMATRIX matScaleRotation, matScaleRotInverse, matScaleVector;
	D3DXVECTOR3 vTrans(matAnimPos._41, matAnimPos._42, matAnimPos._43);
	if (m_ScaleAnimList.size())
	{
		if (GetAnimationTrack(
			fElapseTime,
			m_ScaleAnimList,
			StartTrack,
			EndTrack))
		{
			float t = (fElapseTime - StartTrack.iTick) / (EndTrack.iTick - StartTrack.iTick);
			D3DXQuaternionSlerp(&qRotatin,
				&StartTrack.Quaternion,
				&EndTrack.Quaternion, t);
			D3DXVec3Lerp(&vTrans,
				&StartTrack.Position,
				&EndTrack.Position, t);
		}
		D3DXMatrixScaling(&matScaleVector, vTrans.x, vTrans.y, vTrans.z);
		D3DXMatrixRotationQuaternion(&matScaleRotation, &qRotatin);
		D3DXMatrixInverse(&matScaleRotInverse, NULL, &matScaleRotation);
		matAnimScale = matScaleRotInverse * matScaleVector * matScaleRotation;
	}

	ZeroMemory(&StartTrack, sizeof(StartTrack));
	ZeroMemory(&EndTrack, sizeof(EndTrack));
	if (m_PosAnimList.size())
	{
		if (GetAnimationTrack(
			fElapseTime, m_PosAnimList, StartTrack, EndTrack))
		{
			float t = (fElapseTime - StartTrack.iTick) / (EndTrack.iTick - StartTrack.iTick);
			D3DXVec3Lerp(&vTrans,
				&StartTrack.Position,
				&EndTrack.Position, t);
		}
		D3DXMatrixTranslation(&matAnimPos, vTrans.x,
			vTrans.y, vTrans.z);
	}

	D3DXMATRIX matAnim;
	D3DXMatrixIdentity(&matAnim);
	matAnim = matAnimScale * matAnimRot;
	matAnim._41 = matAnimPos._41;
	matAnim._42 = matAnimPos._42;
	matAnim._43 = matAnimPos._43;

	D3DXMatrixMultiply(&m_matCalculation, &m_matAnimInv, &matAnim);
	return;
}


bool SMatrixObject::GetAnimationTrack(
	float fElapseTime,
	std::vector<SAnimTrack>& trackList,
	SAnimTrack& StartTrack,
	SAnimTrack& EndTrack)
{
	for (auto& track : trackList)
	{
		if (track.iTick > fElapseTime)
		{
			EndTrack = track;
			break;
		}
		StartTrack = track;
	}
	return true;
}
bool SMatrixObject::SetMatrix(D3DXMATRIX* pWorld, D3DXMATRIX* pView, D3DXMATRIX* pProj)
{
	if (pWorld != NULL)
	{
		m_matWorld = *pWorld;
	}
	if (pView != NULL)
	{
		m_matView = *pView;
	}
	if (pProj != NULL)
	{
		m_matProj = *pProj;
	}
	return true;
}
SMatrixObject::SMatrixObject()
{
	m_iIndex = -1;
	m_ObjectName = L"none";

	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_matView);
	D3DXMatrixIdentity(&m_matProj);
}
SMatrixObject::~SMatrixObject()
{
	m_MeshList.clear();
}
bool SMatrixObjectList::Init()
{
	m_fAnimationSpeed = 1.0f;
	m_fAnimElapseTime = 0.0f;
	return true;
}
bool SMatrixObjectList::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	HRESULT hr = S_OK;
	ID3D11Buffer* pBuffer = nullptr;
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0;

	bd.ByteWidth = sizeof(D3DXMATRIX) * 255;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	InitData.pSysMem = m_MatrixList;
	hr = pDevice->CreateBuffer(&bd, &InitData, &pBuffer);
	if (FAILED(hr))
	{
		HRESULT_FAILE_MESSAGE(hr);
		return false;
	}
	m_pMatListConstantBuffer = pBuffer;
	return true;
}
bool SMatrixObjectList::Frame()
{
	m_fAnimElapseTime += m_fAnimationSpeed * I_Timer.GetSPF() * m_Scene.iFrameSpeed * m_Scene.iTickPerFrame;

	float fEndTime = m_Scene.iLastFrame * m_Scene.iTickPerFrame;
	if (m_fAnimElapseTime >= fEndTime)
	{
		m_fAnimElapseTime = 0.0f;
	}
	for(int iMesh = 0; iMesh < m_MatrixObjectList.size(); iMesh++)
	{
		m_MatrixObjectList[iMesh].Frame(m_fAnimElapseTime);
		D3DXMatrixTranspose(&m_MatrixList[iMesh], &m_MatrixObjectList[iMesh].m_matCalculation);
	}
	return true;
}
bool SMatrixObjectList::Render(ID3D11DeviceContext* pContext)
{
	if (bDebug)
	{
		for (auto& TargetObject : m_MatrixObjectList)
		{
			TargetObject.SetMatrix(&m_matWorld, &m_matView, &m_matProj);
			TargetObject.Render(pContext);
		}
	}
	pContext->UpdateSubresource(m_pMatListConstantBuffer.Get(), 0, nullptr, m_MatrixList, 0, 0);
	pContext->VSSetConstantBuffers(1, 1, m_pMatListConstantBuffer.GetAddressOf());
	return true;
}
bool SMatrixObjectList::Release()
{
	return true;
}
bool SMatrixObjectList::SetMatrix(D3DXMATRIX* pWorld, D3DXMATRIX* pView, D3DXMATRIX* pProj)
{
	if (pWorld != NULL)
	{
		m_matWorld = *pWorld;
	}
	if (pView != NULL)
	{
		m_matView = *pView;
	}
	if (pProj != NULL)
	{
		m_matProj = *pProj;
	}
	return true;
}
SMatrixObjectList::SMatrixObjectList()
{
	m_iIndex = -1;
	m_ObjectName = L"none";
	m_fAnimationSpeed = 1.0f;
	m_fAnimElapseTime = 0.0f;
	bDebug = false;

	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_matView);
	D3DXMatrixIdentity(&m_matProj);
	for (int iCount = 0; iCount < 255; iCount++)
	{
		D3DXMatrixIdentity(&m_MatrixList[iCount]);
	}
}


SMatrixObjectList::~SMatrixObjectList()
{
	m_MatrixObjectList.clear();
}

SMatrixObjectList* SMatrixObjectListManager::Create()
{
	m_MatrixObjectLists.push_back(SMatrixObjectList());
	m_MatrixObjectLists[m_MatrixObjectLists.size() - 1].m_iIndex = m_MatrixObjectLists.size() - 1;
	m_MatrixObjectLists[m_MatrixObjectLists.size() - 1].Create(m_pDevice, m_pContext);
	return &m_MatrixObjectLists[m_MatrixObjectLists.size() - 1];
}

SMatrixObjectList* SMatrixObjectListManager::GetMatrixObjectList(T_STR MatrixObjectListName)
{
	for (auto& Target : m_MatrixObjectLists)
	{
		if (Target.m_ObjectName == MatrixObjectListName)
		{
			return &Target;
		}
	}
	return nullptr;
}
SMatrixObjectList* SMatrixObjectListManager::GetMatrixObjectList(int	iIndex)
{
	if (iIndex >= m_MatrixObjectLists.size()) return nullptr;
	else return &m_MatrixObjectLists[iIndex];
}
int	SMatrixObjectListManager::GetSize()
{
	return m_MatrixObjectLists.size();
}
