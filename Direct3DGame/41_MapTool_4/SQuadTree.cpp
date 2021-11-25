#include "SQuadTree.h"


 bool SQuadTree::Init()
{
	m_iObjListSize = 0;
	m_iNodeListSize = 0;
	return true;
}
 bool SQuadTree::Frame()
{
	for (auto ObjPtr : m_DrawObjList) ObjPtr = nullptr;
	m_iObjListSize = 0;

	for (auto NodePtr : m_DrawNodeList) NodePtr = nullptr;
	m_iNodeListSize = 0;

	if (!m_pCamera) return false;
	DrawFindNode(m_pRootNode);
	return true;
}
 bool SQuadTree::Render(ID3D11DeviceContext* const pContext)
{
	return true;
}
 bool SQuadTree::Release()
{
	if(m_DrawObjList.size() > 0) m_DrawObjList.clear();
	if (m_DrawNodeList.size() > 0) m_DrawNodeList.clear();
	DeleteNode(m_pRootNode);
	m_pRootNode = nullptr;
	return true;
}
 bool SQuadTree::DeleteNode(const SNode* const pNode)
{
	if (pNode == nullptr) return true;
	for (int iChild = 0; iChild < pNode->m_ChildList.size(); iChild++)
	{
		DeleteNode(pNode->m_ChildList[iChild]);
	}
	delete pNode;
	return true;
}
void SQuadTree::Update(ID3D11Device* const pDevice, SCamera* const pCamera)
{
	m_pDevice = pDevice;
	m_pCamera = pCamera;
}

 bool SQuadTree::Build(float fWidth, float fHeight)
{
	m_fWidth = fWidth;
	m_fHeight = fHeight;

	m_pRootNode = CreateNode(nullptr, -fWidth / 2, fWidth / 2, -fHeight / 2, fHeight / 2);

	if (BuildTree(m_pRootNode))	return true;
	else						return false;
}

 bool SQuadTree::BuildTree( SNode* const pNode)
{
	if (SubDivide(pNode))
	{
		for (int iNode = 0; iNode < 4; iNode++)
		{
			BuildTree(pNode->m_ChildList[iNode]);
		}
	}
	return true;
}
 bool SQuadTree::SubDivide(SNode* const pNode)
{
	assert(pNode);
	if (m_dwMaxDepthLimit <= pNode->m_dwDepth)
	{
		pNode->m_IsLeaf = TRUE;
		return false;
	}

	float fWidthSplit = (pNode->m_CornerList[TopRight].x - pNode->m_CornerList[TopLeft].x) / 2;
	float fHeightSplit = (pNode->m_CornerList[TopLeft].z - pNode->m_CornerList[BottomRight].z) / 2;

	if (fWidthSplit < m_fMinDivideSize || fHeightSplit < m_fMinDivideSize)
	{
		pNode->m_IsLeaf = true;
		return false;
	}
	SNode* pCreateNode = nullptr;
	
	float fLeft = pNode->m_CornerList[BottomLeft].x;
	float fBottom = pNode->m_CornerList[BottomLeft].z;
	float fRight = pNode->m_CornerList[TopRight].x;
	float fTop = pNode->m_CornerList[TopRight].z;

	// 왼쪽 상단 노드 (0번 노드)
	pCreateNode = CreateNode(pNode,
							fLeft, 
							fLeft + fWidthSplit,
							fTop - fHeightSplit,
							fTop);
	pNode->m_ChildList.push_back(pCreateNode);

	// 오른쪽 상단 노드 (1번 노드)
	pCreateNode = CreateNode(pNode,
							fLeft + fWidthSplit,
							fRight,
							fTop - fHeightSplit,
							fTop);
	pNode->m_ChildList.push_back(pCreateNode);

	// 왼쪽 하단 노드 (2번 노드)
	pCreateNode = CreateNode(pNode,
							fLeft,
							fLeft + fWidthSplit,
							fBottom,
							fBottom + fHeightSplit);
	pNode->m_ChildList.push_back(pCreateNode);

	// 오른쪽 하단 노드 (2번 노드)
	pCreateNode = CreateNode(pNode,
							fLeft + fWidthSplit,
							fRight,
							fBottom,
							fBottom + fHeightSplit);
	pNode->m_ChildList.push_back(pCreateNode);

	return true;
}

SNode * const SQuadTree::CreateNode(const
				SNode* const pParentNode,
				float fLeft, float fRight,
				float fBottom, float fTop)
{
	SNode* pNode = nullptr;
	SAFE_NEW(pNode, SNode);
	assert(pNode);

	pNode->m_ChildList.reserve(4);
	pNode->m_CornerList.reserve(4);

	S_BOX& TargetBox = pNode->m_sBox;

	TargetBox.vMin = D3DXVECTOR3(fLeft, 0.0f, fBottom);
	TargetBox.vMax = D3DXVECTOR3(fRight, 0.0f, fTop);

	TargetBox.vCenter = (TargetBox.vMax + TargetBox.vMin) / 2.0f;

	TargetBox.fExtent[0] = TargetBox.vMax.x - TargetBox.vCenter.x;
	TargetBox.fExtent[1] = TargetBox.vMax.y - TargetBox.vCenter.y;
	TargetBox.fExtent[2] = TargetBox.vMax.z - TargetBox.vCenter.z;

	TargetBox.vAxis[0] = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	TargetBox.vAxis[1] = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	TargetBox.vAxis[2] = D3DXVECTOR3(0.0f, 0.0f, 1.0f);

	pNode->m_CornerList.push_back(D3DXVECTOR3(TargetBox.vMin.x, 0.0f, TargetBox.vMax.z));
	pNode->m_CornerList.push_back(TargetBox.vMax);
	pNode->m_CornerList.push_back(TargetBox.vMin);
	pNode->m_CornerList.push_back(D3DXVECTOR3(TargetBox.vMax.x, 0.0f, TargetBox.vMin.z));

	if (pParentNode)
	{
		pNode->m_dwDepth = pParentNode->m_dwDepth + 1;
		if ((DWORD)m_dwMaxDepthLimit < pNode->m_dwDepth)
		{
			m_dwMaxDepthLimit = pNode->m_dwDepth;
		}
	}
	return pNode;
}
SNode* const SQuadTree::FindNode(const SNode* const pNode, SBaseObj* const pObj)
{
	assert(pNode);
	SNode* FindNode = const_cast<SNode*>(pNode);
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
		// 완전히 오브젝트가 포함된 부모노드를 꺼온다
		FindNode = m_QuadTreeQueue.front();
		m_QuadTreeQueue.pop();
	} 
	while (FindNode);
	return FindNode;
}

void SQuadTree::DrawFindNode(SNode* const pNode)
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

void SQuadTree::VisibleNode(SNode* const pNode)
{
	assert(m_pCamera);
	if (pNode->m_dwDepth > static_cast<DWORD>(m_iRenderDepth)) return;
	//m_DrawNodeList.push_back(pNode);
	m_DrawNodeList[m_iNodeListSize] = pNode;
	m_iNodeListSize++;

	VisibleObject(pNode);

	if (m_pCamera->CheckOBBInPlane(&pNode->m_sBox))
	{
		
		/*for (auto& ChildNode : pNode->m_ChildList)
		{
			VisibleNode(ChildNode);
		}*/
	}
}
void SQuadTree::VisibleObject(const SNode* const pNode)
{
	for (auto& Object : pNode->m_ObjectList)
	{
		S_BOX& CheckBox = Object->m_Box;
		if (m_pCamera->CheckOBBInPlane(&CheckBox))
		{
			m_DrawObjList[m_iObjListSize] = Object;
			m_iObjListSize++;
		}
	}
}
 int SQuadTree::AddObject(SBaseObj* const pObj)
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
 int	SQuadTree::CheckRect(SNode* const pNode, SBaseObj* const pObj)
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
SQuadTree::SQuadTree()
{
	m_pRootNode = nullptr;
}


SQuadTree::~SQuadTree()
{
	Release();
}
