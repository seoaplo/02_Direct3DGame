#include "SCAObject.h"

bool SCAObject::Init()
{
	return true;
}
bool SCAObject::Frame()
{
	if (m_pMatrixObjectList != nullptr)
	{
		m_pMatrixObjectList->Frame();
	}
	for (int iSkinObj = 0; iSkinObj < m_iSkinObjectListSize; iSkinObj++)
	{
		m_SkinObjectList[iSkinObj]->Frame();
	}
	return true;
}
bool SCAObject::Render(ID3D11DeviceContext* pContext)
{
	if (m_pMatrixObjectList != nullptr)
	{
		m_pMatrixObjectList->SetMatrix(&m_matWorld, &m_matView, &m_matProj);
		m_pMatrixObjectList->Render(pContext);
	}
	for (int iSkinObj = 0; iSkinObj < m_iSkinObjectListSize; iSkinObj++)
	{
		m_SkinObjectList[iSkinObj]->SetMatrix(&m_matWorld, &m_matView, &m_matProj);
		pContext->VSSetConstantBuffers(1, 1, m_pMatrixObjectList->m_pMatListConstantBuffer.GetAddressOf());
		m_SkinObjectList[iSkinObj]->Render(pContext);
	}
	return true;
}
bool SCAObject::Release()
{
	ClearSkinObjectList();
	return true;
}

bool SCAObject::SetMatrixObjectList(int iMatrixObejct)
{
	m_pMatrixObjectList = I_MatrixObjectListManager.GetMatrixObjectList(iMatrixObejct);
	return true;
}
bool SCAObject::SetSkinObject(int iSkinObejct)
{
	SSkinObject* pSkinObject = I_SkinObjectManager.GetSkinObject(iSkinObejct);
	if (pSkinObject == nullptr) return false;
	m_iSkinObjectListSize++;
	m_SkinObjectList.push_back(pSkinObject);
	return true;
}
void SCAObject::ClearSkinObjectList()
{
	m_SkinObjectList.clear();
	m_iSkinObjectListSize = 0;
}
SCAObject::SCAObject()
{
	m_iSkinObjectListSize = 0;
	m_pMatrixObjectList = nullptr;

	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_matView);
	D3DXMatrixIdentity(&m_matProj);

}


SCAObject::~SCAObject()
{
}
