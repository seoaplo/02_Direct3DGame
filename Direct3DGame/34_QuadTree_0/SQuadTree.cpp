#include "SQuadTree.h"

bool SQuadTree::Build(float fWidth, float fHeight)
{
	m_fWidth = fWidth;
	m_fHeight = fHeight;

	m_pRootNode = CreateNode(nullptr, -fWidth / 2, fWidth / 2, -fHeight / 2, fHeight / 2);

	if (BuildTree(m_pRootNode))	return true;
	else						return false;
}

bool SQuadTree::BuildTree(SNode* pNode)
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
bool SQuadTree::SubDivide(SNode* pNode)
{
	assert(pNode);
	if ((DWORD)m_iMaxDepthLimit <= pNode->m_dwDepth)
	{
		pNode->m_IsLeaf = TRUE;
		return false;
	}

	DWORD dwCurrentDepth = pNode->m_dwDepth + 1;
	if (m_iNumDepth < dwCurrentDepth) m_iNumDepth = dwCurrentDepth;

	float fWidthSplit = (pNode->m_CornerList[TopRight].x - pNode->m_CornerList[TopLeft].x) / 2;
	float fHeightSplit = (pNode->m_CornerList[TopLeft].z - pNode->m_CornerList[BottomRight].z) / 2;

	if (fWidthSplit < m_fMinDivideSize || fHeightSplit < m_fMinDivideSize)
	{
		pNode->m_IsLeaf = true;
		return false;
	}
	SNode* pCreateNode = nullptr;
	
	// 왼쪽 상단 노드 (0번 노드)
	pCreateNode = CreateNode(pNode,
							pNode->m_CornerList[TopLeft].x,
							pNode->m_CornerList[TopLeft].x + fWidthSplit,
							pNode->m_CornerList[TopLeft].z - fHeightSplit,
							pNode->m_CornerList[TopLeft].z);
	pNode->m_ChildList.push_back(pCreateNode);

	// 오른쪽 상단 노드 (1번 노드)
	pCreateNode = CreateNode(pNode,
							pNode->m_CornerList[TopLeft].x + fWidthSplit,
							pNode->m_CornerList[TopRight].x,
							pNode->m_CornerList[TopLeft].z - fHeightSplit,
							pNode->m_CornerList[TopLeft].z);
	pNode->m_ChildList.push_back(pCreateNode);

	// 왼쪽 하단 노드 (2번 노드)
	pCreateNode = CreateNode(pNode,
							pNode->m_CornerList[TopLeft].x,
							pNode->m_CornerList[TopLeft].x +fWidthSplit,
							pNode->m_CornerList[BottomLeft].z,
							pNode->m_CornerList[TopLeft].z - fHeightSplit);
	pNode->m_ChildList.push_back(pCreateNode);

	// 오른쪽 하단 노드 (2번 노드)
	pCreateNode = CreateNode(pNode,
		pNode->m_CornerList[TopLeft].x + fWidthSplit,
		pNode->m_CornerList[TopRight].x,
		pNode->m_CornerList[BottomRight].z,
		pNode->m_CornerList[TopLeft].z - fHeightSplit);
	pNode->m_ChildList.push_back(pCreateNode);

	return true;
}

SNode* SQuadTree::CreateNode(
				SNode* pParentNode, 
				float fTopLeft, float fTopRight,
				float fBottomLeft, float fBottomRight)
{
	SNode* pNode = nullptr;
	SAFE_NEW(pNode, SNode);
	assert(pNode);

	pNode->m_ChildList.reserve(4);
	pNode->m_CornerList.reserve(4);

	S_BOX& TargetBox = pNode->m_sBox;

	TargetBox.vMin = D3DXVECTOR3(fTopLeft, 0.0f, fBottomLeft);
	TargetBox.vMax = D3DXVECTOR3(fTopRight, 0.0f, fBottomRight);

	TargetBox.vCenter = (TargetBox.vMax + TargetBox.vMin) / 2.0f;

	TargetBox.fExtent[0] = TargetBox.vMax.x - TargetBox.vCenter.x;
	TargetBox.fExtent[1] = TargetBox.vMax.y - TargetBox.vCenter.y;
	TargetBox.fExtent[2] = TargetBox.vMax.z - TargetBox.vCenter.z;

	pNode->m_CornerList.push_back(D3DXVECTOR3(TargetBox.vMin.x, 0.0f, TargetBox.vMax.z));
	pNode->m_CornerList.push_back(TargetBox.vMax);
	pNode->m_CornerList.push_back(TargetBox.vMin);
	pNode->m_CornerList.push_back(D3DXVECTOR3(TargetBox.vMax.x, 0.0f, TargetBox.vMin.z));

	if (pParentNode)
	{
		pNode->m_dwDepth = pParentNode->m_dwDepth + 1;
	}
	return pNode;
}
SQuadTree::SQuadTree()
{
}


SQuadTree::~SQuadTree()
{
}
