#include "SQuadTreeMT.h"


bool SQuadTreeMT::Build(SMap* pMap, int iNodeColl, int iNodeRow, int iCellDistance, int iCellSize)
{
	m_dwWidth = iNodeColl;
	m_dwHeight = iNodeRow;
	m_pMap = pMap;

	int iRemainder = (m_dwWidth * m_dwHeight) % 4 > 0 ? 1 : 0;
	m_iMaxDepthLimit = ((m_dwWidth * m_dwHeight) / 4) + iRemainder;
	m_fMinDivideSize = pMap->m_MapDesc.fSellDistance;

	m_pRootNode = CreateNode(nullptr,
		0, m_dwWidth - 1,
		m_dwWidth * (m_dwHeight - 1),
		m_dwWidth * m_dwHeight - 1);
	return BuildTree(m_pRootNode);
}
void SQuadTreeMT::CreateBuffer(SNode* pNode)
{
	if (pNode == nullptr) return;
	CreateTextureVertexBuffer(pNode);
	CreateIndexBuffer(pNode);
}
void SQuadTreeMT::CreateTextureVertexBuffer(SNode* pNode)
{
	
}
void SQuadTreeMT::CreateIndexBuffer(SNode* pNode)
{
	DWORD dwStartRow = pNode->m_CornerIndex[0] / m_dwWidth;
	DWORD dwEndRow = pNode->m_CornerIndex[2] / m_dwWidth;
	DWORD dwStartCol = pNode->m_CornerIndex[0] % m_dwWidth;
	DWORD dwEndCol = pNode->m_CornerIndex[1] % m_dwWidth;

	pNode->m_IndexData.resize(
		(dwEndRow - dwStartRow)*(dwEndCol - dwStartCol) * 2 * 3);

	int iIndex = 0;
	for (DWORD iRow = dwStartRow;	iRow < dwEndRow;
		iRow++)
	{
		for (DWORD iCol = dwStartCol;	iCol < dwEndCol;
			iCol++)
		{
			DWORD dwIndex = iRow * m_dwWidth + iCol;
			int iNextRow = iRow + 1;
			int iNextCol = iCol + 1;
			pNode->m_IndexData[iIndex + 0] = iRow * m_dwWidth + iCol;
			pNode->m_IndexData[iIndex + 1] = iRow * m_dwWidth + iNextCol;
			pNode->m_IndexData[iIndex + 2] = iNextRow * m_dwWidth + iCol;

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

SQuadTreeMT::SQuadTreeMT()
{
}


SQuadTreeMT::~SQuadTreeMT()
{
}
