#pragma once
#include "SQuadTree.h"
#include "SMap.h"
#include "SSelect.h"
class SQuadTreeMap : public SQuadTree
{
public:
	DWORD	m_dwWidth;
	DWORD   m_dwHeight;
	DWORD	m_iSellNum;
	SMap*	m_pMap;
public:
	virtual bool Build(SMap* pMap,	int   iMaxDepth = 5, float fMinSize = 10.0f);
	SNode*  CreateNode(SNode* pParentNode, float fLeft, float fRight, float fBottom, float fTop) override;
	bool   SubDivide(SNode* pNode)override;
	void   ComputeBoundingBox(SNode* pNode);
	std::vector<PNCT_VERTEX*> FindVectexList(SNode& pNode, D3DXVECTOR3& vIntersection, float fDistance);
	D3DXVECTOR2 GetHeightFromNode(DWORD nLeft, DWORD nRight, DWORD nBottom, DWORD nTop);
	virtual void CreateBuffer(SNode* pNode);
	virtual void CreateIndexBuffer(SNode* pNode);
public:
	bool  Render(ID3D11DeviceContext*	pContext)override;
public:
	SQuadTreeMap();
	virtual ~SQuadTreeMap();
};

