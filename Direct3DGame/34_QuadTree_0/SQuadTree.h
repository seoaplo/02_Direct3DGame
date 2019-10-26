#pragma once
#include "SNode.h"
class SQuadTree
{
public:
	enum QUADTREECORNER { TopLeft = 0, TopRight = 1, BottomLeft = 2, BottomRight = 3, };
	PNCT2_VERTEX*	m_pVertexList;
	SNode*			m_pRootNode;

	float			m_fWidth;
	float			m_fHeight;

	float			m_fMinDivideSize;

	int				m_iNumDepth;
	int				m_iRenderDepth;
	int				m_iMaxDepthLimit;
public:
	bool			Build(float fWidth, float fHeight);
	bool			BuildTree(SNode* pNode);
	bool			SubDivide(SNode* pNode);
public:
	SNode*			CreateNode(SNode* pParentNode, float fTopLeft, float fTopRight, float fBottomLeft, float fBottomRight);
public:
	void			SetRenderDepth(int iRenderDepth)
	{
		m_iRenderDepth = iRenderDepth;
	}

	void			SetMaxDepthLimit(int iMaxDepth) { m_iMaxDepthLimit = iMaxDepth; }
	void			SetMinDivideSize(int iMinDivideSize) { m_fMinDivideSize = (float)iMinDivideSize; }
public:
	SQuadTree();
	~SQuadTree();
};

