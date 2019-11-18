#include "SQuadTreeMap.h"

bool SQuadTreeMap::Build(SMap* pMap, int   iMaxDepth, float fMinSize)
{
	m_dwWidth = pMap->m_iNumSellCols;
	m_dwHeight = pMap->m_iNumSellRows;
	m_iSellNum = pMap->m_iSellNum;
	m_pMap = pMap;

	m_pRootNode = CreateNode(nullptr,
		0, m_dwWidth - 1,
		m_dwWidth * (m_dwHeight - 1),
		m_dwWidth * m_dwHeight - 1);

	m_iMaxDepthLimit = log2(m_dwWidth - 1);
	m_fMinDivideSize = (pMap->m_fSellDistance * pMap->m_iSellNum) / 2;
	return BuildTree(m_pRootNode);
}
SNode* SQuadTreeMap::CreateNode(SNode* pParentNode, float fLeft, float fRight, float fBottom, float fTop)
{
	SNode* pNode = new SNode;
	pNode->m_ChildList.reserve(4);
	pNode->m_IsLeaf = FALSE;
	pNode->m_CornerIndex.push_back((DWORD)fLeft);
	pNode->m_CornerIndex.push_back((DWORD)fRight);
	pNode->m_CornerIndex.push_back((DWORD)fBottom);
	pNode->m_CornerIndex.push_back((DWORD)fTop);

	ComputeBoundingBox(pNode);


	pNode->m_dwDepth = 0;
	if (pParentNode != nullptr)
	{
		pNode->m_dwDepth = pParentNode->m_dwDepth + 1;
	}
	return pNode;
}
bool SQuadTreeMap::SubDivide(SNode* pNode)
{
	if (m_iMaxDepthLimit <= pNode->m_dwDepth)
	{
		pNode->m_IsLeaf = TRUE;
		CreateBuffer(pNode);
		return false;
	}
	float fWidthSplit = (pNode->m_sBox.vMax.x - pNode->m_sBox.vMin.x) / 2;
	float fHeightSplit = (pNode->m_sBox.vMax.z - pNode->m_sBox.vMin.z) / 2;
	if (m_fMinDivideSize >= fWidthSplit ||
		m_fMinDivideSize >= fHeightSplit)
	{
		pNode->m_IsLeaf = TRUE;
		CreateBuffer(pNode);
		return false;
	}
	DWORD dwEdgeCenter[4];
	DWORD dwCenter =
		(pNode->m_CornerIndex[0] + pNode->m_CornerIndex[3]) / 2;


	dwEdgeCenter[0] =
		(pNode->m_CornerIndex[0] + pNode->m_CornerIndex[1]) / 2;
	dwEdgeCenter[1] =
		(pNode->m_CornerIndex[1] + pNode->m_CornerIndex[3]) / 2;
	dwEdgeCenter[2] =
		(pNode->m_CornerIndex[2] + pNode->m_CornerIndex[3]) / 2;
	dwEdgeCenter[3] =
		(pNode->m_CornerIndex[0] + pNode->m_CornerIndex[2]) / 2;

	SNode* pChildNode = CreateNode(
		pNode,
		pNode->m_CornerIndex[0],
		dwEdgeCenter[0],
		dwEdgeCenter[3],
		dwCenter);
	pNode->m_ChildList.push_back(pChildNode);
	pChildNode = CreateNode(
		pNode,
		dwEdgeCenter[0],
		pNode->m_CornerIndex[1],
		dwCenter,
		dwEdgeCenter[1]);
	pNode->m_ChildList.push_back(pChildNode);

	pChildNode = CreateNode(
		pNode,
		dwCenter,
		dwEdgeCenter[1],
		dwEdgeCenter[2],
		pNode->m_CornerIndex[3]);
	pNode->m_ChildList.push_back(pChildNode);

	pChildNode = CreateNode(
		pNode,
		dwEdgeCenter[3],
		dwCenter,
		pNode->m_CornerIndex[2],
		dwEdgeCenter[2]);
	pNode->m_ChildList.push_back(pChildNode);
	return true;
}
void SQuadTreeMap::ComputeBoundingBox(SNode* pNode)
{
	D3DXVECTOR2 vHeight =
		GetHeightFromNode(
			pNode->m_CornerIndex[0],
			pNode->m_CornerIndex[1],
			pNode->m_CornerIndex[2],
			pNode->m_CornerIndex[3]);

	D3DXVECTOR3 v0 = m_pMap->m_VertexList[pNode->m_CornerIndex[0]].p;
	D3DXVECTOR3 v1 = m_pMap->m_VertexList[pNode->m_CornerIndex[3]].p;

	pNode->m_sBox.vMax.x = v1.x;
	pNode->m_sBox.vMax.y = vHeight.y;
	pNode->m_sBox.vMax.z = v0.z;
	pNode->m_sBox.vMin.x = v0.x;
	pNode->m_sBox.vMin.y = vHeight.x;
	pNode->m_sBox.vMin.z = v1.z;
	pNode->m_sBox.vCenter = (pNode->m_sBox.vMax +
		pNode->m_sBox.vMin) / 2;
	pNode->m_sBox.vAxis[0] = D3DXVECTOR3(1, 0, 0);
	pNode->m_sBox.vAxis[1] = D3DXVECTOR3(0, 1, 0);
	pNode->m_sBox.vAxis[2] = D3DXVECTOR3(0, 0, 1);
	pNode->m_sBox.fExtent[0] =
		(pNode->m_sBox.vMax.x - pNode->m_sBox.vMin.x) / 2;
	pNode->m_sBox.fExtent[1] =
		(pNode->m_sBox.vMax.y - pNode->m_sBox.vMin.y) / 2;
	pNode->m_sBox.fExtent[2] =
		(pNode->m_sBox.vMax.z - pNode->m_sBox.vMin.z) / 2;
}
D3DXVECTOR2 SQuadTreeMap::GetHeightFromNode(DWORD nLeft, DWORD nRight, DWORD nBottom, DWORD nTop)
{
	float fMin = 99999.0f;
	float fMax = -99999.0f;

	DWORD dwStartRow = nLeft / m_dwWidth;
	DWORD dwEndRow = nBottom / m_dwWidth;
	DWORD dwStartCol = nLeft % m_dwWidth;
	DWORD dwEndCol = nRight % m_dwWidth;
	for (DWORD dwRow = dwStartRow;
		dwRow < dwEndRow;
		dwRow++)
	{
		for (DWORD dwCol = dwStartCol;
			dwCol < dwEndCol;
			dwCol++)
		{
			DWORD dwIndex = dwRow * m_dwWidth + dwCol;
			if (m_pMap->m_VertexList[dwIndex].p.y < fMin)
			{
				fMin = m_pMap->m_VertexList[dwIndex].p.y;
			}
			if (m_pMap->m_VertexList[dwIndex].p.y > fMax)
			{
				fMax = m_pMap->m_VertexList[dwIndex].p.y;
			}
		}
	}
	if (fMin > 99998.0f || fMax < -99998.0f)
	{
		fMin = 0;
	}
	return D3DXVECTOR2(fMin, fMax);
}
void SQuadTreeMap::CreateBuffer(SNode* pNode)
{
	if (pNode == nullptr) return;

	CreateIndexBuffer(pNode);
}
void SQuadTreeMap::CreateIndexBuffer(SNode* pNode)
{
	DWORD dwStartRow = pNode->m_CornerIndex[0] / m_dwWidth;
	DWORD dwEndRow = (pNode->m_CornerIndex[2] / m_dwWidth);
	DWORD dwStartCol = pNode->m_CornerIndex[0] % m_dwWidth;
	DWORD dwEndCol = (pNode->m_CornerIndex[1] % m_dwWidth);

	pNode->m_IndexData.resize(
		(dwEndRow - dwStartRow)*(dwEndCol - dwStartCol) * 2 * 3);

	int iIndex = 0;
	for (DWORD iRow = dwStartRow;	iRow < dwEndRow;
		iRow++)
	{
		for (DWORD iCol = dwStartCol; iCol < dwEndCol;
			iCol++)
		{
			DWORD dwIndex = iRow * m_dwWidth + iCol;
			int iNextRow = iRow + 1;
			int iNextCol = iCol + 1;
			pNode->m_IndexData[iIndex + 0] = iRow * m_dwWidth  + iCol;
			pNode->m_IndexData[iIndex + 1] = iRow * m_dwWidth  + iNextCol;
			pNode->m_IndexData[iIndex + 2] = iNextRow * m_dwWidth  + iCol;

			pNode->m_IndexData[iIndex + 3] = pNode->m_IndexData[iIndex + 2];
			pNode->m_IndexData[iIndex + 4] = pNode->m_IndexData[iIndex + 1];
			pNode->m_IndexData[iIndex + 5] = iNextRow * m_dwWidth + iNextCol;
			iIndex += 6;
		}
	}

	pNode->m_IndexBuffer.Attach(
		DXGame::CreateIndexBuffer(m_pDevice,
			&pNode->m_IndexData.at(0),
			pNode->m_IndexData.size(), sizeof(DWORD)));
}
bool  SQuadTreeMap::Render(ID3D11DeviceContext*	pContext)
{
	pContext->UpdateSubresource(
		m_pMap->_dxobj.g_pConstantBuffer.Get(),
		0, NULL, &m_pMap->_cbData, 0, 0);

	m_pMap->PreRender(pContext);

	for (int iNode = 0; iNode < m_DrawNodeList.size(); iNode++)
	{
		SNode* pNode = m_DrawNodeList[iNode];
		pContext->IASetIndexBuffer(pNode->m_IndexBuffer.Get(),
			DXGI_FORMAT_R32_UINT, 0);
		pContext->DrawIndexed(pNode->m_IndexData.size(), 0, 0);
	}
	return true;
}
void SQuadTreeMap::UpVectexHeight(SNode& pNode, D3DXVECTOR3& vIntersection, float fDistance, float fHeight)
{
	std::vector<PNCT_VERTEX*> RetVertexList;

	const float& fSellDistance = m_pMap->m_fSellDistance;

	int iHalfCol = m_pMap->m_iNumSellCols / 2;
	int iHalfRow = m_pMap->m_iNumSellRows / 2;


	float fLeft = (vIntersection.x - fDistance) / fSellDistance;
	float fTop = -(vIntersection.z - fDistance) / fSellDistance;
	float fRight = (vIntersection.x + fDistance) / fSellDistance;
	float fBottom = -(vIntersection.z + fDistance) / fSellDistance;


	fLeft = fLeft + iHalfCol > 0  ? fLeft + iHalfCol : 0;
	fBottom = fBottom + iHalfRow > 0 ? fBottom + iHalfRow : 0;

	fRight = fRight + iHalfCol < m_pMap->m_iNumSellCols
			? fRight + iHalfCol : m_pMap->m_iNumSellCols;
	fTop = fTop + iHalfRow < m_pMap->m_iNumSellRows
			? fTop + iHalfRow : m_pMap->m_iNumSellRows;



	DWORD dwStartRow = floorf(fBottom);
	DWORD dwEndRow = ceilf(fTop);
	DWORD dwStartCol = floorf(fLeft);
	DWORD dwEndCol = ceilf(fRight);

	for (DWORD iRow = dwStartRow;	iRow < dwEndRow;
		iRow++)
	{
		for (DWORD iCol = dwStartCol; iCol < dwEndCol;
			iCol++)
		{
			DWORD Index = iRow * m_dwWidth + iCol;
			if (Index < 0 || Index >= m_pMap->m_VertexList.size()) continue;

			PNCT_VERTEX* TargetVertex = &m_pMap->m_VertexList[Index];

			D3DXVECTOR2 LengthVector;
			LengthVector.x = TargetVertex->p.x - vIntersection.x;
			LengthVector.y = TargetVertex->p.z - vIntersection.z;

			float fTargetHeight = (fDistance - D3DXVec2Length(&LengthVector)) * fHeight;
			TargetVertex->p.y += max(fTargetHeight, 0.0f) * I_Timer.GetSPF();
		}
	}

	m_pMap->ReLoadVBuffer();
	UpdateNode(m_pRootNode);
}

void SQuadTreeMap::UpdateNode(SNode* pNode)
{
	assert(pNode);
	if (pNode->m_IsLeaf)
	{
		ComputeBoundingBox(pNode);
	}
	for (auto& pChilde : pNode->m_ChildList)
	{
		UpdateNode(pChilde);
	}
}
SQuadTreeMap::SQuadTreeMap()
{
}


SQuadTreeMap::~SQuadTreeMap()
{
}
