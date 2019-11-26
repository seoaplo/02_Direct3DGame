#include "SQuadTreeArray.h"


bool SQuadTreeArray::Init()
{
	return true;
}
bool SQuadTreeArray::Frame()
{
	m_DrawObjList.clear();
	m_DrawNodeList.clear();
	if (!m_pCamera) return false;
	DrawFindNode(&m_NodeList[0]);
	return true;
}
bool  SQuadTreeArray::Render(ID3D11DeviceContext*	pContext)
{
	pContext->UpdateSubresource(
		m_pMap->_dxobj.g_pConstantBuffer.Get(),
		0, NULL, &m_pMap->_cbData, 0, 0);

	m_pMap->PreRender(pContext);
	m_pMap->UpdateConstantBuffer(pContext, nullptr);
	m_pMap->RenderSetTexture(pContext);
	if (m_pAlphaTexture != nullptr)
	{
		pContext->PSSetShaderResources(1, 1, &m_pAlphaTexture);
	}
	pContext->IASetIndexBuffer(m_pIndexBuffer.Get(),
		DXGI_FORMAT_R32_UINT, 0);

	for (int iNode = 0; iNode < m_DrawNodeList.size(); iNode++)
	{
		SNode* pNode = m_DrawNodeList[iNode];


		int iNodeIndexSize = pow(4, m_iMaxDepthLimit - pNode->m_dwDepth) * m_dwTileIndexBufferSize;
		int iStartIndex = pNode->m_dwDepth - 1;//pNode->m_dwIndex - pow(4, pNode->m_dwDepth - 1);
		int iDepthPow = 0;

		for (int iCount = 1; iCount <= iStartIndex; iCount++)
		{
			iDepthPow += pow(4, iCount);
		}

		/*iStartIndex = iStartIndex > 0 ?
			pNode->m_dwIndex  - (iStartIndex * iDepthPow) -  1
			: max(pNode->m_dwIndex - 1, 0);*/

		if (iStartIndex = iStartIndex > 0)
		{
			iStartIndex = pNode->m_dwIndex - iDepthPow - 1;
		}
		else
		{
			iStartIndex = pNode->m_dwIndex - 1;
			iStartIndex = max(iStartIndex, 0);
		}

		iStartIndex *= iNodeIndexSize;
		pContext->DrawIndexed(iNodeIndexSize, iStartIndex, 0);
	}
	return true;
}
bool SQuadTreeArray::Release()
{
	SQuadTreeMap::Release();
	m_dwTileIndexBufferSize = 0;
	m_IndexBufferData.clear();
	return true;
}
bool SQuadTreeArray::DeleteNode(SNode* pNode)
{
	m_NodeList.clear();
	m_IndexBufferData.clear();
	return true;
}
void SQuadTreeArray::Update(ID3D11Device* pDevice, SCamera* pCamera)
{
	m_pDevice = pDevice;
	m_pCamera = pCamera;
}

bool SQuadTreeArray::Build(float fWidth, float fHeight)
{
	return false;
}

bool SQuadTreeArray::Build(SMap* pMap, int   iMaxDepth, float fMinSize)
{

	m_dwWidth = pMap->m_iNumSellCols;
	m_dwHeight = pMap->m_iNumSellRows;
	m_iSellNum = pMap->m_iSellNum;
	m_pMap = pMap;
	m_dwTileIndexBufferSize = m_iSellNum * m_iSellNum * 6;

	m_iMaxDepthLimit = log2(pMap->m_iNumCols);
	int NodeMaxNum = 0;
	for (int iCount = 0; iCount <= m_iMaxDepthLimit; iCount++)
	{
		NodeMaxNum += pow(4, iCount);
	}
	m_NodeList.resize(NodeMaxNum);
	m_IndexBufferData.reserve(pMap->_IndexList.size());

	m_fMinDivideSize = (pMap->m_fSellDistance * pMap->m_iSellNum) / 2;


	m_pRootNode = CreateNode(0, 0,
		0, m_dwWidth - 1,
		m_dwWidth * (m_dwHeight - 1),
		m_dwWidth * m_dwHeight - 1);

	return BuildTree(m_pRootNode);
}
SNode* SQuadTreeArray::CreateNode(SNode* pNode, float fLeft, float fRight, float fBottom, float fTop)
{
	return false;
}
SNode* SQuadTreeArray::CreateNode(DWORD iNodeNum, DWORD dwDepth, float fLeft, float fRight, float fBottom, float fTop)
{
	if (iNodeNum > m_NodeList.size()) return nullptr;

	SNode& TargetNode = m_NodeList[iNodeNum];
	TargetNode.m_dwIndex = iNodeNum;
	TargetNode.m_ChildList.reserve(4);
	TargetNode.m_IsLeaf = FALSE;
	TargetNode.m_CornerIndex.push_back((DWORD)fLeft);
	TargetNode.m_CornerIndex.push_back((DWORD)fRight);
	TargetNode.m_CornerIndex.push_back((DWORD)fBottom);
	TargetNode.m_CornerIndex.push_back((DWORD)fTop);

	ComputeBoundingBox(&TargetNode);
	TargetNode.m_dwDepth = dwDepth;

	return &TargetNode;
}
bool SQuadTreeArray::SubDivide(SNode* pNode)
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

	int iChildNum = (pNode->m_dwIndex * 4) + 1;

	SNode* pChildNode = CreateNode(
		iChildNum,
		pNode->m_dwDepth + 1,
		pNode->m_CornerIndex[0],
		dwEdgeCenter[0],
		dwEdgeCenter[3],
		dwCenter);
	pNode->m_ChildList.push_back(pChildNode);

	iChildNum += 1;
	pChildNode = CreateNode(
		iChildNum,
		pNode->m_dwDepth + 1,
		dwEdgeCenter[0],
		pNode->m_CornerIndex[1],
		dwCenter,
		dwEdgeCenter[1]);
	pNode->m_ChildList.push_back(pChildNode);

	iChildNum += 1;
	pChildNode = CreateNode(
		iChildNum,
		pNode->m_dwDepth + 1,
		dwCenter,
		dwEdgeCenter[1],
		dwEdgeCenter[2],
		pNode->m_CornerIndex[3]);
	pNode->m_ChildList.push_back(pChildNode);

	iChildNum += 1;
	pChildNode = CreateNode(
		iChildNum,
		pNode->m_dwDepth + 1,
		dwEdgeCenter[3],
		dwCenter,
		pNode->m_CornerIndex[2],
		dwEdgeCenter[2]);
	pNode->m_ChildList.push_back(pChildNode);
	return true;
}

bool SQuadTreeArray::BuildTree(SNode* pNode)
{
	//if (SubDivide(pNode))
	//{
	//	for (int iNode = 0; iNode < 4; iNode++)
	//	{
	//		BuildTree(pNode->m_ChildList[iNode]);
	//	}
	//}

	std::queue<SNode*> IndexArray0;
	std::queue<SNode*> IndexArray1;
	IndexArray0.push(&m_NodeList[0]);
	m_NodeList[0].m_dwIndex = 0;
	m_NodeList[0].m_dwDepth = 0;
	DWORD dwDepth = 0;
	SNode* pTargetNode = nullptr;

	while (dwDepth <= m_iMaxDepthLimit)
	{
		std::queue<SNode*>& TargetNodeQueue = dwDepth % 2 == 0 ? IndexArray0 : IndexArray1;
		std::queue<SNode*>& TargetNodeChildQueue = dwDepth % 2 == 0 ? IndexArray1 : IndexArray0;

		while (TargetNodeQueue.empty() == false)
		{
			pTargetNode = TargetNodeQueue.front();
			TargetNodeQueue.pop();

			SubDivide(pTargetNode);
			for (int iCount = 0; iCount < pTargetNode->m_ChildList.size(); iCount++)
			{
				TargetNodeChildQueue.push(pTargetNode->m_ChildList[iCount]);
			}
		}
		if (TargetNodeChildQueue.empty())
		{
			m_iMaxDepthLimit = dwDepth;
			break;
		}
		dwDepth++;
	}

	m_pIndexBuffer.Attach(
		DXGame::CreateIndexBuffer(m_pDevice,
			&m_IndexBufferData.at(0),
			m_IndexBufferData.size(), sizeof(DWORD)));
	return true;
}

SNode* SQuadTreeArray::FindNode(SNode* pNode, SBaseObj* pObj)
{
	assert(pNode);
	do
	{
		// 4개의 자식 중에서 찾는다
		for (int iChild = 0; iChild < pNode->m_ChildList.size(); iChild++)
		{
			if (pNode->m_ChildList[iChild] && CheckRect(pNode->m_ChildList[iChild], pObj))
			{
				m_QuadTreeQueue.push(pNode->m_ChildList[iChild]);
				break;
			}
		}
		// 큐에 들어 있는 리스트가 없다면 pNode가 완전히 포함하는 노드가 된다.
		if (m_QuadTreeQueue.empty()) break;
		// 완전히 오브젝트가 포함된 부모노드를 꺼내온다
		pNode = m_QuadTreeQueue.front();
		m_QuadTreeQueue.pop();
	} while (pNode);
	return pNode;
}

void SQuadTreeArray::DrawFindNode(SNode* pNode)
{
	assert(pNode);

	S_POSITION s_Pos = m_pCamera->CheckPoitionOBBInPlane(&pNode->m_sBox);
	if (pNode->m_dwDepth > (DWORD)m_iRenderDepth) return;

	// 리프노드 일 경우는 완전히 제외되지 않았다면(걸쳐 있거나 완전 포함)추가한다.
	if (pNode->m_IsLeaf &&  s_Pos != P_BACK)
	{
		VisibleNode(pNode);
		return;
	}
	// 완전히 포함되어 있으면 자식을 탐색하지 않고 노드를 추가한다.
	else if (s_Pos == P_FRONT)
	{
		VisibleNode(pNode);
		return;
	}
	// 걸쳐져 있는 노드에 포함된 오브젝트 체크
	else if (s_Pos == P_SPANNING)
	{
		VisibleObject(pNode);
	}

	for (int iNode = 0; iNode < pNode->m_ChildList.size(); iNode++)
	{
		DrawFindNode(pNode->m_ChildList[iNode]);
	}
}

void SQuadTreeArray::VisibleNode(SNode* pNode)
{
	assert(m_pCamera);
	if (pNode->m_dwDepth > m_iRenderDepth) return;

	m_DrawNodeList.push_back(pNode);
	VisibleObject(pNode);

	if (m_pCamera->CheckOBBInPlane(&pNode->m_sBox))
	{

		/*for (auto& ChildNode : pNode->m_ChildList)
		{
			VisibleNode(ChildNode);
		}*/
	}
}
void SQuadTreeArray::VisibleObject(SNode* pNode)
{
	for (auto& Object : pNode->m_ObjectList)
	{
		S_BOX& CheckBox = Object->m_Box;
		if (m_pCamera->CheckOBBInPlane(&CheckBox))
		{
			m_DrawObjList.push_back(Object);
		}
	}
}
int	SQuadTreeArray::AddObject(SBaseObj* pObj)
{
	if (CheckRect(m_pRootNode, pObj))
	{
		SNode* pNode = FindNode(m_pRootNode, pObj);
		if (pNode)
		{
			pNode->m_ObjectList.push_back(pObj);
		}
		return 1;
	}
	return 0;
}
int	SQuadTreeArray::CheckRect(SNode* pNode, SBaseObj* pObj)
{
	if (pNode == nullptr || pObj == nullptr) return -1;

	S_BOX& NodeBox = pNode->m_sBox;
	S_BOX& ObjectBox = pObj->m_Box;

	if (NodeBox.vMin.x <= ObjectBox.vMin.x && NodeBox.vMax.x >= ObjectBox.vMax.x)
	{
		if (NodeBox.vMin.z <= ObjectBox.vMin.z && NodeBox.vMax.z >= ObjectBox.vMax.z)
		{
			return 1;
		}
	}
	return 0;
}

D3DXVECTOR2 SQuadTreeArray::GetHeightFromNode(DWORD nLeft, DWORD nRight, DWORD nBottom, DWORD nTop)
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
void SQuadTreeArray::CreateBuffer(SNode* pNode)
{
	if (pNode == nullptr) return;

	CreateIndexBuffer(pNode);
}
void SQuadTreeArray::CreateIndexBuffer(SNode* pNode)
{
	DWORD dwStartRow = pNode->m_CornerIndex[0] / m_dwWidth;
	DWORD dwEndRow = (pNode->m_CornerIndex[2] / m_dwWidth);
	DWORD dwStartCol = pNode->m_CornerIndex[0] % m_dwWidth;
	DWORD dwEndCol = (pNode->m_CornerIndex[1] % m_dwWidth);

	int iIndex = 0;

	DWORD Index[4];
	for (DWORD iRow = dwStartRow;	iRow < dwEndRow;
		iRow++)
	{
		for (DWORD iCol = dwStartCol; iCol < dwEndCol;
			iCol++)
		{
			DWORD dwIndex = iRow * m_dwWidth + iCol;
			int iNextRow = iRow + 1;
			int iNextCol = iCol + 1;

			Index[0] = iRow * m_dwWidth + iCol;
			Index[1] = iRow * m_dwWidth + iNextCol;
			Index[2] = iNextRow * m_dwWidth + iCol;
			Index[3] = iNextRow * m_dwWidth + iNextCol;

			m_IndexBufferData.push_back(Index[0]);
			m_IndexBufferData.push_back(Index[1]);
			m_IndexBufferData.push_back(Index[2]);

			m_IndexBufferData.push_back(Index[2]);
			m_IndexBufferData.push_back(Index[1]);
			m_IndexBufferData.push_back(Index[3]);
		}
	}
}

void SQuadTreeArray::UpVectexHeight(SNode& pNode, D3DXVECTOR3& vIntersection, float fDistance, float fHeight)
{

	if (fHeight < 0.0001) return;

	const float& fSellDistance = m_pMap->m_fSellDistance;

	int iHalfCol = m_pMap->m_iNumSellCols / 2;
	int iHalfRow = m_pMap->m_iNumSellRows / 2;


	float fLeft = (vIntersection.x - fDistance) / fSellDistance;
	float fTop = -(vIntersection.z - fDistance) / fSellDistance;
	float fRight = (vIntersection.x + fDistance) / fSellDistance;
	float fBottom = -(vIntersection.z + fDistance) / fSellDistance;



	fLeft = max(fLeft + iHalfCol, 0);
	fBottom = max(fBottom + iHalfRow, 0);
	fRight = min(fRight + iHalfCol, m_pMap->m_iNumSellCols);
	fTop = min(fTop + iHalfRow, m_pMap->m_iNumSellRows);




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

void SQuadTreeArray::UpdateNode(SNode* pNode)
{
	assert(pNode);
	ComputeBoundingBox(pNode);
	for (auto& pChilde : pNode->m_ChildList)
	{
		UpdateNode(pChilde);
	}
}
SQuadTreeArray::SQuadTreeArray()
{
	m_dwTileIndexBufferSize = 0;
	m_pRootNode = nullptr;
}


SQuadTreeArray::~SQuadTreeArray()
{
	Release();
}