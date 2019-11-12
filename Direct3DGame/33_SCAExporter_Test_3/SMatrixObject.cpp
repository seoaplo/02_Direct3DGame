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
	D3DXMATRIX matWorld = m_matCalculation * m_matWorld;
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

	m_matCalculation = matAnim;
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

//--------------------------------------------------------------------------------------
// Create Structured Buffer
//--------------------------------------------------------------------------------------
HRESULT SMatrixObjectList::CreateStructuredBuffer(ID3D11Device* pDevice, UINT uElementSize, UINT uCount, VOID* pInitData, ID3D11Buffer** ppBufOut)
{
	*ppBufOut = NULL;

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	desc.ByteWidth = uElementSize * uCount;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = uElementSize;

	if (pInitData)
	{
		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = pInitData;
		return pDevice->CreateBuffer(&desc, &InitData, ppBufOut);
	}
	else
		return pDevice->CreateBuffer(&desc, NULL, ppBufOut);
}

//--------------------------------------------------------------------------------------
// Create Shader Resource View for Structured or Raw Buffers
//--------------------------------------------------------------------------------------
HRESULT SMatrixObjectList::CreateBufferSRV(ID3D11Device* pDevice, ID3D11Buffer* pBuffer, ID3D11ShaderResourceView** ppSRVOut)
{
	D3D11_BUFFER_DESC descBuf;
	ZeroMemory(&descBuf, sizeof(descBuf));
	pBuffer->GetDesc(&descBuf);

	D3D11_SHADER_RESOURCE_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	desc.BufferEx.FirstElement = 0;

	if (descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS)
	{
		// This is a Raw Buffer

		desc.Format = DXGI_FORMAT_R32_TYPELESS;
		desc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
		desc.BufferEx.NumElements = descBuf.ByteWidth / 4;
	}
	else
		if (descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED)
		{
			// This is a Structured Buffer

			desc.Format = DXGI_FORMAT_UNKNOWN;
			desc.BufferEx.NumElements = descBuf.ByteWidth / descBuf.StructureByteStride;
		}
		else
		{
			return E_INVALIDARG;
		}

	return pDevice->CreateShaderResourceView(pBuffer, &desc, ppSRVOut);
}

//--------------------------------------------------------------------------------------
// Create Unordered Access View for Structured or Raw Buffers
//-------------------------------------------------------------------------------------- 
HRESULT SMatrixObjectList::CreateBufferUAV(ID3D11Device* pDevice, ID3D11Buffer* pBuffer, ID3D11UnorderedAccessView** ppUAVOut)
{
	D3D11_BUFFER_DESC descBuf;
	ZeroMemory(&descBuf, sizeof(descBuf));
	pBuffer->GetDesc(&descBuf);

	D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	desc.Buffer.FirstElement = 0;

	if (descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS)
	{
		// This is a Raw Buffer

		desc.Format = DXGI_FORMAT_R32_TYPELESS; // Format must be DXGI_FORMAT_R32_TYPELESS, when creating Raw Unordered Access View
		desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
		desc.Buffer.NumElements = descBuf.ByteWidth / 4;
	}
	else
		if (descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED)
		{
			// This is a Structured Buffer

			desc.Format = DXGI_FORMAT_UNKNOWN;      // Format must be must be DXGI_FORMAT_UNKNOWN, when creating a View of a Structured Buffer
			desc.Buffer.NumElements = descBuf.ByteWidth / descBuf.StructureByteStride;
		}
		else
		{
			return E_INVALIDARG;
		}

	return pDevice->CreateUnorderedAccessView(pBuffer, &desc, ppUAVOut);
}

//--------------------------------------------------------------------------------------
// Create a CPU accessible buffer and download the content of a GPU buffer into it
// This function is very useful for debugging CS programs
//-------------------------------------------------------------------------------------- 
ID3D11Buffer* SMatrixObjectList::CreateAndCopyToBuf(ID3D11Device* pDevice, ID3D11DeviceContext* pd3dImmediateContext, ID3D11Buffer* pBuffer)
{
	ID3D11Buffer* debugbuf = NULL;

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	pBuffer->GetDesc(&desc);
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.Usage = D3D11_USAGE_STAGING;
	desc.BindFlags = 0;
	desc.MiscFlags = 0;
	if (SUCCEEDED(pDevice->CreateBuffer(&desc, NULL, &debugbuf)))
	{
		pd3dImmediateContext->CopyResource(debugbuf, pBuffer);
	}

	return debugbuf;
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
	
	m_pDevice = pDevice;
	m_pContext = pContext;

	CreateStructuredBuffer(pDevice, sizeof(D3DXMATRIX),
		AnimationMatrixList,
		m_MatrixList,
		m_pMatrixSRVBuffer.GetAddressOf());

	CreateStructuredBuffer(pDevice, sizeof(D3DXMATRIX),
		AnimationMatrixList,
		m_MatrixList,
		m_pMatrixUAVBuffer.GetAddressOf());

	CreateBufferUAV(pDevice, m_pMatrixUAVBuffer.Get(), m_pMatrixUAV.GetAddressOf());
	CreateBufferSRV(pDevice, m_pMatrixSRVBuffer.Get(), m_pMatrixSRV.GetAddressOf());
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
		m_MatrixList[iMesh] = m_MatrixObjectList[iMesh].m_matCalculation;
	}
	m_pContext->UpdateSubresource(m_pMatrixUAVBuffer.Get(), 0, nullptr, m_MatrixList, 0, 0);
	return true;
}
bool SMatrixObjectList::Render(ID3D11DeviceContext* pContext)
{
	m_pContext->CopyResource(m_pMatrixSRVBuffer.Get(), m_pMatrixUAVBuffer.Get());
	if (bDebug)
	{
		for (auto& TargetObject : m_MatrixObjectList)
		{
			TargetObject.SetMatrix(&m_matWorld, &m_matView, &m_matProj);
			TargetObject.Render(pContext);
		}
	}
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
	for (int iCount = 0; iCount < AnimationMatrixList; iCount++)
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
	SMatrixObjectList* NewMatrixList = new SMatrixObjectList();
	m_MatrixObjectLists.push_back(NewMatrixList);
	m_MatrixObjectLists[m_MatrixObjectLists.size() - 1]->m_iIndex = m_MatrixObjectLists.size() - 1;
	m_MatrixObjectLists[m_MatrixObjectLists.size() - 1]->Create(m_pDevice, m_pContext);
	return m_MatrixObjectLists[m_MatrixObjectLists.size() - 1];
}

SMatrixObjectList* SMatrixObjectListManager::GetMatrixObjectList(T_STR MatrixObjectListName)
{
	for (auto& Target : m_MatrixObjectLists)
	{
		if (Target->m_ObjectName == MatrixObjectListName)
		{
			return Target;
		}
	}
	return nullptr;
}
SMatrixObjectList* SMatrixObjectListManager::GetMatrixObjectList(int	iIndex)
{
	if (iIndex >= m_MatrixObjectLists.size()) return nullptr;
	else return m_MatrixObjectLists[iIndex];
}
int	SMatrixObjectListManager::GetSize()
{
	return m_MatrixObjectLists.size();
}
