#include "SQuadTreeArray.h"


bool SQuadTreeArray::Init()
{
	m_vPickVector = D3DXVECTOR3(9999.9f, 9999.9f, 9999.9f);
	m_PickingNodeList.clear();

	m_DrawNodeList.clear();
	m_DrawObjList.clear();
	return true;
}
bool SQuadTreeArray::Frame()
{
	for (int i = 0; i < m_iObjListSize; i++)
	{
		m_DrawObjList[i] = nullptr;
	}
	m_iObjListSize = 0;

	for (int i = 0; i < m_iNodeListSize; i++)
	{
		m_DrawNodeList[i] = nullptr;
	}
	m_iNodeListSize = 0;

	if (!m_pCamera) return false;
	DrawFindNode();
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
	pContext->IASetIndexBuffer(m_pIndexBuffer.Get(),
		DXGI_FORMAT_R32_UINT, 0);
	for(int i = 0; i < m_iNodeListSize; i++)
	{
		SNode* pNode = m_DrawNodeList[i];


		int iNodeIndexSize = pow(4, m_iMaxDepthLimit - pNode->m_dwDepth) * m_dwTileIndexBufferSize;
		int iStartIndex = pNode->m_dwDepth - 1;
		int iDepthPow = 0;

		for (int iCount = 1; iCount <= iStartIndex; iCount++)
		{
			iDepthPow += pow(4, iCount);
		}
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
	m_vPickVector = D3DXVECTOR3(9999.9f, 9999.9f, 9999.9f);
	m_PickingNodeList.clear();
	m_HeightNodeList.clear();
	return true;
}
bool SQuadTreeArray::Release()
{
	SQuadTreeMap::Release();
	m_vPickVector = D3DXVECTOR3(9999.9f, 9999.9f, 9999.9f);
	m_PickingNodeList.clear();
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
	Init();

	m_dwWidth = pMap->m_iNumSellCols;
	m_dwHeight = pMap->m_iNumSellRows;
	m_iSellNum = pMap->m_iSellNum;
	m_pMap = pMap;
	m_dwTileIndexBufferSize = m_iSellNum * m_iSellNum * 6;

	for (auto NodePtr : m_DrawNodeList) NodePtr = nullptr;
	m_iNodeListSize = 0;


	m_iMaxDepthLimit = log2(pMap->m_iNumCols);
	int NodeMaxNum = 0;
	for (int iCount = 0; iCount <= m_iMaxDepthLimit; iCount++)
	{
		NodeMaxNum += pow(4, iCount);
	}
	m_NodeList.resize(NodeMaxNum);
	m_DrawNodeList.resize(NodeMaxNum);


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
		dwEdgeCenter[3],
		dwCenter,
		pNode->m_CornerIndex[2],
		dwEdgeCenter[2]);
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
	return true;
}

bool SQuadTreeArray::BuildTree(SNode* pNode)
{

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

	SetRenderDepth(m_iMaxDepthLimit);
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

void SQuadTreeArray::DrawFindNode()
{
	std::queue<SNode*> DrawNodeQueue0;
	std::queue<SNode*> DrawNodeQueue1;
	DrawNodeQueue0.push(&m_NodeList[0]);
	DWORD dwDepth = 0;
	SNode* pTargetNode = nullptr;
	S_POSITION s_Pos;

	while (dwDepth <= m_iRenderDepth)
	{
		std::queue<SNode*>& TargetNodeQueue = dwDepth % 2 == 0 ? DrawNodeQueue0 : DrawNodeQueue1;
		std::queue<SNode*>& TargetNodeChildQueue = dwDepth % 2 == 0 ? DrawNodeQueue1 : DrawNodeQueue0;

		while (TargetNodeQueue.empty() == false)
		{
			pTargetNode = TargetNodeQueue.front();
			TargetNodeQueue.pop();

			s_Pos = m_pCamera->CheckPoitionOBBInPlane(&pTargetNode->m_sBox);
			// 리프노드 일 경우는 완전히 제외되지 않았다면(걸쳐 있거나 완전 포함)추가한다.
			if (pTargetNode->m_IsLeaf &&  s_Pos != P_BACK)
			{
				VisibleNode(pTargetNode);
			}
			// 완전히 포함되어 있으면 자식을 탐색하지 않고 노드를 추가한다.
			else if (s_Pos == P_FRONT)
			{
				VisibleNode(pTargetNode);
			}
			// 걸쳐져 있는 노드에 포함된 오브젝트 체크
			else if (s_Pos == P_SPANNING)
			{
				VisibleObject(pTargetNode);
				for (int iCount = 0; iCount < pTargetNode->m_ChildList.size(); iCount++)
				{
					TargetNodeChildQueue.push(pTargetNode->m_ChildList[iCount]);
				}
			}
		}
		if (TargetNodeChildQueue.empty())
		{
			break;
		}
		dwDepth++;
	}
}

void SQuadTreeArray::VisibleNode(SNode* pNode)
{
	assert(m_pCamera);
	if (pNode->m_dwDepth > m_iRenderDepth) return;

	m_DrawNodeList[m_iNodeListSize] = pNode;
	m_iNodeListSize++;

	VisibleObject(pNode);
}
void SQuadTreeArray::VisibleObject(SNode* pNode)
{
	for (auto& Object : pNode->m_ObjectList)
	{
		S_BOX& CheckBox = Object->m_Box;
		if (m_pCamera->CheckOBBInPlane(&CheckBox))
		{
			if (m_DrawObjList.size() <= m_iObjListSize)
			{
				m_DrawObjList.push_back(Object);

				for (int i = m_iObjListSize; i < m_DrawObjList.size(); i++)
				{
					m_DrawObjList[i] = nullptr;
				}
			}
			else
			{
				m_DrawObjList[m_iObjListSize] = Object;
				m_iObjListSize++;
			}
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
SNode*	SQuadTreeArray::CheckPicking(SSelect& TargetSelect)
{
	FindePickingNode(TargetSelect);

	SNode* pNode = nullptr;
	float fLength = 9999.9f;
	D3DXVECTOR3 vIntersection;

	for (auto& pPickNode : m_PickingNodeList)
	{
		if (FindePickingFace(TargetSelect, pPickNode))
		{


			if (fLength > TargetSelect.m_fPickDistance)
			{
				fLength = TargetSelect.m_fPickDistance;
				pNode = pPickNode;
				m_vPickVector = TargetSelect.m_vIntersection;
			}
		}
	}

	return pNode;
}

bool SQuadTreeArray::FindePickingNode(SSelect& TargetSelect)
{

	std::queue<SNode*> DrawNodeQueue0;
	std::queue<SNode*> DrawNodeQueue1;
	DrawNodeQueue0.push(&m_NodeList[0]);
	DWORD dwDepth = 0;
	SNode* pTargetNode = nullptr;
	S_POSITION s_Pos;

	while (dwDepth <= m_iMaxDepthLimit)
	{
		std::queue<SNode*>& TargetNodeQueue = dwDepth % 2 == 0 ? DrawNodeQueue0 : DrawNodeQueue1;
		std::queue<SNode*>& TargetNodeChildQueue = dwDepth % 2 == 0 ? DrawNodeQueue1 : DrawNodeQueue0;

		while (TargetNodeQueue.empty() == false)
		{
			pTargetNode = TargetNodeQueue.front();
			TargetNodeQueue.pop();

			if (TargetSelect.ChkOBBToRay(&pTargetNode->m_sBox, &TargetSelect.m_Ray))
			{
				if (pTargetNode->m_IsLeaf)
				{
					m_PickingNodeList.push_back(pTargetNode);
				}
				else
				{
					TargetNodeChildQueue.push(pTargetNode->m_ChildList[0]);
					TargetNodeChildQueue.push(pTargetNode->m_ChildList[1]);
					TargetNodeChildQueue.push(pTargetNode->m_ChildList[2]);
					TargetNodeChildQueue.push(pTargetNode->m_ChildList[3]);
				}
			}
		}
		if (TargetNodeChildQueue.empty())
		{
			break;
		}
		dwDepth++;
	}
	return true;
}

bool SQuadTreeArray::FindePickingFace(SSelect& TargetSelect, SNode* pNode)
{
	if (pNode == nullptr) return false;

	/*int iStartIndex = pNode->m_dwDepth - 1;
	int iDepthPow = 0;

	for (int iCount = 1; iCount <= iStartIndex; iCount++)
	{
		iDepthPow += pow(4, iCount);
	}
	iStartIndex = pNode->m_dwIndex - iDepthPow - 1;
	iStartIndex *= m_dwTileIndexBufferSize;

	int iMaxFace = m_dwTileIndexBufferSize / 3;

	int v0 = iStartIndex;
	int v1 = iStartIndex + 1;
	int v2 = iStartIndex + 2;
	for (int iFace = 0; iFace < iMaxFace; iFace++)
	{

		int iIndex0 = m_IndexBufferData[v0];
		int iIndex1 = m_IndexBufferData[v0 + 1];
		int iIndex2 = m_IndexBufferData[v0 + 2];

		D3DXVECTOR3& Vertex0 = m_pMap->m_VertexList[iIndex0].p;
		D3DXVECTOR3& Vertex1 = m_pMap->m_VertexList[iIndex1].p;
		D3DXVECTOR3& Vertex2 = m_pMap->m_VertexList[iIndex2].p;


		D3DXVECTOR3& vStart = TargetSelect.m_Ray.vOrigin;
		D3DXVECTOR3& vEnd = TargetSelect.m_Ray.vDirection * 9999.9f;

		D3DXVECTOR3 vNormal = m_pMap->ComputeFaceNormal(iIndex0, iIndex1, iIndex2);

		if (TargetSelect.GetIntersection(
			vStart, vEnd, vNormal,
			Vertex0, Vertex1, Vertex2)
			)
		{
			if (TargetSelect.PointInPolygon(
				TargetSelect.m_vIntersection, vNormal,
				Vertex0, Vertex1, Vertex2))
			{
				return true;
			}
		}
		v0 += 3;
		v1 = v0 + 1;
		v2 = v0 + 2;
	}*/

	DWORD dwWidth = m_dwWidth;

	int dwStartRow = (pNode->m_CornerIndex[0]) / dwWidth;
	int dwEndRow = (pNode->m_CornerIndex[2]) / dwWidth;
	int dwStartCol = (pNode->m_CornerIndex[0]) % dwWidth;
	int dwEndCol = (pNode->m_CornerIndex[1]) % dwWidth;

	dwStartRow = max(dwStartRow, 0);
	dwStartCol = max(dwStartCol, 0);
	dwEndRow = min(dwEndRow, m_pMap->m_iNumSellRows - 1);
	dwEndCol = min(dwEndCol, m_pMap->m_iNumSellCols - 1);

	int iIndex = 0;
	for (DWORD iRow = dwStartRow;	iRow < dwEndRow;
		iRow++)
	{
		for (DWORD iCol = dwStartCol; iCol < dwEndCol;
			iCol++)
		{
			DWORD dwIndex = iRow * dwWidth + iCol;
			int iNextRow = iRow + 1;
			int iNextCol = iCol + 1;

			int i0 = iRow * dwWidth + iCol;
			int i1 = iRow * dwWidth + iNextCol;
			int i2 = iNextRow * dwWidth + iCol;

			int i3 = i2;
			int i4 = i1;
			int i5 = iNextRow * dwWidth + iNextCol;
			iIndex += 6;

			D3DXVECTOR3 Vertex0 = m_pMap->m_VertexList[i0].p;
			D3DXVECTOR3 Vertex1 = m_pMap->m_VertexList[i1].p;
			D3DXVECTOR3 Vertex2 = m_pMap->m_VertexList[i2].p;


			D3DXVECTOR3 vStart = TargetSelect.m_Ray.vOrigin;
			D3DXVECTOR3 vEnd = TargetSelect.m_Ray.vDirection * 99999.0f;

			D3DXVECTOR3 vNormal = m_pMap->ComputeFaceNormal(i0, i1, i2);

			if (TargetSelect.GetIntersection(
				vStart, vEnd, vNormal,
				Vertex0, Vertex1, Vertex2)
				)
			{
				if (TargetSelect.PointInPolygon(
					TargetSelect.m_vIntersection, vNormal,
					Vertex0, Vertex1, Vertex2))
				{
					return true;
				}
			}

			Vertex0 = m_pMap->m_VertexList[i3].p;
			Vertex1 = m_pMap->m_VertexList[i4].p;
			Vertex2 = m_pMap->m_VertexList[i5].p;

			vNormal = m_pMap->ComputeFaceNormal(i3, i4, i5);

			if (TargetSelect.GetIntersection(
				vStart, vEnd, vNormal,
				Vertex0, Vertex1, Vertex2)
				)
			{
				if (TargetSelect.PointInPolygon(
					TargetSelect.m_vIntersection, vNormal,
					Vertex0, Vertex1, Vertex2))
				{
					return true;
				}
			}
		}
	}

	return false;
}
void SQuadTreeArray::SetVertexHeight(SNode& pNode, D3DXVECTOR3& vIntersection, DWORD dwMode, float fDistance, float fHeight)
{

	if (fHeight < 0.0001) return;
	if (dwMode > QUADTREEMAP_HEIGHT_SIZE) return;

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

			if (dwMode == QUADTREEMAP_HEIGHT_UP)
			{
				TargetVertex->p.y += max(fTargetHeight, 0.0f) * I_Timer.GetSPF();
			}
			else if (dwMode == QUADTREEMAP_HEIGHT_DOWN)
			{
				TargetVertex->p.y -= max(fTargetHeight, 0.0f) * I_Timer.GetSPF();
			}
			else
			{
				if (TargetVertex->p.y < 0.0f)
				{
					TargetVertex->p.y += max(fTargetHeight, 0.0f) * I_Timer.GetSPF();
					TargetVertex->p.y = min(TargetVertex->p.y, 0.0f);
				}
				else if(TargetVertex->p.y > 0.0f)
				{
					TargetVertex->p.y -= max(fTargetHeight, 0.0f) * I_Timer.GetSPF();
					TargetVertex->p.y = max(TargetVertex->p.y, 0.0f);
				}
			}

		}
	}
	CheckBrushNode(&pNode, vIntersection, fDistance);
	m_pMap->ReLoadVBuffer();
	UpdateNode();
}

void SQuadTreeArray::CheckBrushNode(SNode* pNode, D3DXVECTOR3& vPickVector, float fDistance)
{
	bool bFindParent = true;
	D3DXVECTOR2 vPickVector2D =
		D3DXVECTOR2(vPickVector.x,
					vPickVector.z);

	while (bFindParent != false && pNode->m_dwDepth > 0)
	{
		bFindParent = false;
		for (int iCount = 0; iCount < 4; iCount++)
		{
			D3DXVECTOR2 TargetVertex =
				D3DXVECTOR2(pNode->m_sBox.vCenter.x,
							pNode->m_sBox.vCenter.z);

			TargetVertex -= vPickVector2D;


			float fLength = sqrt((pNode->m_sBox.fExtent[0] * pNode->m_sBox.fExtent[0]) +
				(pNode->m_sBox.fExtent[2] * pNode->m_sBox.fExtent[2]));
			/*float fLength = pNode->m_sBox.fExtent[0] > pNode->m_sBox.fExtent[2] ?
							pNode->m_sBox.fExtent[0] : pNode->m_sBox.fExtent[2];*/

			if (D3DXVec2Length(&TargetVertex) < fDistance + fLength)
			{
				bFindParent = true;
				int iParentIndex = ((pNode->m_dwIndex - 1) / 4);
				iParentIndex = max(iParentIndex, 0);

				pNode = &m_NodeList[iParentIndex];
				break;
			}
		}
	}
	std::queue<SNode*> FindBrushNodeQueue0;
	std::queue<SNode*> FindBrushNodeQueue1;

	if (pNode->m_dwDepth == m_iMaxDepthLimit)
	{
		m_HeightNodeList.push_back(pNode);
		return;
	}

	DWORD dwDepth = pNode->m_dwDepth + 1;
	if (dwDepth % 2 == 0)
	{
		FindBrushNodeQueue0.push(pNode->m_ChildList[0]);
		FindBrushNodeQueue0.push(pNode->m_ChildList[1]);
		FindBrushNodeQueue0.push(pNode->m_ChildList[2]);
		FindBrushNodeQueue0.push(pNode->m_ChildList[3]);
	}
	else
	{
		FindBrushNodeQueue1.push(pNode->m_ChildList[0]);
		FindBrushNodeQueue1.push(pNode->m_ChildList[1]);
		FindBrushNodeQueue1.push(pNode->m_ChildList[2]);
		FindBrushNodeQueue1.push(pNode->m_ChildList[3]);
	}
	SNode* pTargetNode = nullptr;
	S_POSITION s_Pos;

	while (dwDepth <= m_iMaxDepthLimit)
	{
		std::queue<SNode*>& TargetNodeQueue = dwDepth % 2 == 0 ? FindBrushNodeQueue0 : FindBrushNodeQueue1;
		std::queue<SNode*>& TargetNodeChildQueue = dwDepth % 2 == 0 ? FindBrushNodeQueue1 : FindBrushNodeQueue0;

		while (TargetNodeQueue.empty() == false)
		{
			pTargetNode = TargetNodeQueue.front();
			TargetNodeQueue.pop();

			D3DXVECTOR2 TargetVertex =
				D3DXVECTOR2(pTargetNode->m_sBox.vCenter.x,
							pTargetNode->m_sBox.vCenter.z);

			/*float fLength = pTargetNode->m_sBox.fExtent[0] > pTargetNode->m_sBox.fExtent[2] ?
							pTargetNode->m_sBox.fExtent[0] : pTargetNode->m_sBox.fExtent[2];*/

			float fLength = sqrt((pTargetNode->m_sBox.fExtent[0] * pTargetNode->m_sBox.fExtent[0]) +
				(pTargetNode->m_sBox.fExtent[2] * pTargetNode->m_sBox.fExtent[2]));

			TargetVertex -= vPickVector2D;

			if (D3DXVec2Length(&TargetVertex) < fDistance + fLength)
			{
				m_HeightNodeList.push_back(pTargetNode);
				if (pTargetNode->m_IsLeaf == FALSE)
				{
					TargetNodeChildQueue.push(pTargetNode->m_ChildList[0]);
					TargetNodeChildQueue.push(pTargetNode->m_ChildList[1]);
					TargetNodeChildQueue.push(pTargetNode->m_ChildList[2]);
					TargetNodeChildQueue.push(pTargetNode->m_ChildList[3]);
				}
			}
		}
		if (TargetNodeChildQueue.empty())
		{
			break;
		}
		dwDepth++;
	}
	UpdateNode();
}

void SQuadTreeArray::UpdateNode()
{

	int iStartIndex = m_HeightNodeList.size() - 1;

	for (; iStartIndex >= 0; iStartIndex--)
	{
		if (m_HeightNodeList[iStartIndex]->m_IsLeaf)
		{
			ComputeBoundingBox(m_HeightNodeList[iStartIndex]);
		}
		else
		{
			ComputeBoundingBoxFromChild(m_HeightNodeList[iStartIndex]);
		}
	}
}

bool SQuadTreeArray::SaveFile(FILE* pStream)
{
	if (pStream == nullptr) return false;

	_ftprintf(pStream, _T("\n%s %d %d %d %d %d %f"),
		L"#TREE_HEADERINFO",
		m_dwWidth,
		m_dwHeight,
		m_iSellNum,
		m_dwTileIndexBufferSize,
		m_iMaxDepthLimit,
		m_fMinDivideSize);


	_ftprintf(pStream, _T("\n%s %d"),
		L"#NODE",
		m_NodeList.size());

	for (auto& Node : m_NodeList)
	{
		_ftprintf(pStream, _T("\n%d %d"),
			Node.m_dwDepth,
			Node.m_IsLeaf);

		_ftprintf(pStream, _T("%10.4f"),
			Node.m_sBox.vCenter);

		_ftprintf(pStream, _T("%10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f"),
			Node.m_sBox.vPos[0],
			Node.m_sBox.vPos[1],
			Node.m_sBox.vPos[2],
			Node.m_sBox.vPos[3],
			Node.m_sBox.vPos[4],
			Node.m_sBox.vPos[5],
			Node.m_sBox.vPos[6], 
			Node.m_sBox.vPos[7]);

		_ftprintf(pStream, _T("%10.4f %10.4f %10.4f %10.4f %10.4f %10.4f"),
			Node.m_sBox.vMax.x,
			Node.m_sBox.vMax.y,
			Node.m_sBox.vMax.z,
			Node.m_sBox.vMin.x,
			Node.m_sBox.vMin.y,
			Node.m_sBox.vMin.z);

		_ftprintf(pStream, _T("%10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f"),
			Node.m_sBox.vAxis[0].x,
			Node.m_sBox.vAxis[0].y,
			Node.m_sBox.vAxis[0].z,
			Node.m_sBox.vAxis[1].x,
			Node.m_sBox.vAxis[1].y,
			Node.m_sBox.vAxis[1].z,
			Node.m_sBox.vAxis[2].x,
			Node.m_sBox.vAxis[2].y,
			Node.m_sBox.vAxis[2].z);

		_ftprintf(pStream, _T("%10.4f %10.4f %10.4f"),
			Node.m_sBox.fExtent[0],
			Node.m_sBox.fExtent[1],
			Node.m_sBox.fExtent[2]);
	}

	_ftprintf(pStream, _T("\n%s %d"),
		L"#INDEX",
		m_IndexBufferData.size());

	for (int iIndex = 0; iIndex < m_IndexBufferData.size(); iIndex += 3)
	{
		_ftprintf(pStream, _T("\n%d %d %d"),
			m_IndexBufferData[iIndex + 0],
			m_IndexBufferData[iIndex + 1],
			m_IndexBufferData[iIndex + 2]);
	}
	return true;
}


bool SQuadTreeArray::LoadFile(SParser& Paser)
{
	bool bCheck;
	if (m_pDevice == nullptr) return false;

	static TCHAR String[MAX_PATH];
	ZeroMemory(String, _countof(String));


	bCheck = Paser.GetDataFromFileNext(L"#TREE_HEADERINFO");
	if (bCheck == false) return false;

	_stscanf_s(Paser.m_pBuffer, _T("\n%s %d %d %d %d %d %f"), String, MAX_PATH,
			&m_dwWidth,
			&m_dwHeight,
			&m_iSellNum,
			&m_dwTileIndexBufferSize,
			&m_iMaxDepthLimit,
			&m_fMinDivideSize);
	ZeroMemory(String, _countof(String));

	int iNodeListSize = 0;
	bCheck = Paser.GetDataFromFileNext(L"#NODE", &iNodeListSize, INT_DATA);

	if (bCheck == false) return false;
	m_NodeList.resize(iNodeListSize);
	ZeroMemory(String, _countof(String));

	for (auto& Node : m_NodeList)
	{
		Paser.GetNextLine();
		_stscanf_s(Paser.m_pBuffer, _T("%d %d %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f"),
			&Node.m_dwDepth,
			&Node.m_IsLeaf,
			&Node.m_sBox.vCenter,
			&Node.m_sBox.vPos[0],
			&Node.m_sBox.vPos[1],
			&Node.m_sBox.vPos[2],
			&Node.m_sBox.vPos[3],
			&Node.m_sBox.vPos[4],
			&Node.m_sBox.vPos[5],
			&Node.m_sBox.vPos[6],
			&Node.m_sBox.vPos[7],
			&Node.m_sBox.vMax.x,
			&Node.m_sBox.vMax.y,
			&Node.m_sBox.vMax.z,
			&Node.m_sBox.vMin.x,
			&Node.m_sBox.vMin.y,
			&Node.m_sBox.vMin.z,
			&Node.m_sBox.vAxis[0].x,
			&Node.m_sBox.vAxis[0].y,
			&Node.m_sBox.vAxis[0].z,
			&Node.m_sBox.vAxis[1].x,
			&Node.m_sBox.vAxis[1].y,
			&Node.m_sBox.vAxis[1].z,
			&Node.m_sBox.vAxis[2].x,
			&Node.m_sBox.vAxis[2].y,
			&Node.m_sBox.vAxis[2].z,
			&Node.m_sBox.fExtent[0],
			&Node.m_sBox.fExtent[1],
			&Node.m_sBox.fExtent[2]);
	}
	ZeroMemory(String, _countof(String));

	int iMaxIndex = 0;
	bCheck = Paser.GetDataFromFileNext(L"#INDEX", &iMaxIndex, INT_DATA);
	if (bCheck == false) return false;
	ZeroMemory(String, _countof(String));

	m_IndexBufferData.resize(iMaxIndex);
	for (int iIndex = 0; iIndex < iMaxIndex; iIndex += 3)
	{
		Paser.GetNextLine();
		_stscanf_s(Paser.m_pBuffer, _T("%d %d %d"),
			&m_IndexBufferData[iIndex + 0],
			&m_IndexBufferData[iIndex + 1],
			&m_IndexBufferData[iIndex + 2]);
	}

	m_pIndexBuffer.Attach(
		DXGame::CreateIndexBuffer(m_pDevice,
			&m_IndexBufferData.at(0),
			m_IndexBufferData.size(), sizeof(DWORD)));

	SetRenderDepth(m_iMaxDepthLimit);

	return true;
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