#pragma once
#include "SNode.h"
#include "SCamera.h"

class SQuadTree
{
public:
	enum QUADTREECORNER { TopLeft = 0, TopRight = 1, BottomLeft = 2, BottomRight = 3, };
	
	ID3D11Device*			m_pDevice;
	SCamera*				m_pCamera;
	
	PNCT2_VERTEX*	m_pVertexList;
	SNode*			m_pRootNode;

	float			m_fWidth;
	float			m_fHeight;

	float			m_fMinDivideSize;

	int				m_iNumDepth;
	int				m_iRenderDepth;
	int				m_iMaxDepthLimit;

	
	std::queue<SNode*>			m_QuadTreeQueue;
	std::vector<SBaseObj*>		m_DrawObjList;
public:
	bool			Init();
	bool			Frame();
	bool			Render();
	bool			Release();
public:
	bool			Build(float fWidth, float fHeight);
	bool			BuildTree(SNode* pNode);
	bool			SubDivide(SNode* pNode);
public:
	SNode*			CreateNode(SNode* pParentNode, float fLeft, float fRight, float fBottom, float fTop);
	SNode*			FindNode(SNode* pNode, SBaseObj* pObj);
	void			DrawFindNode(SNode* pNode);
	void			VisibleNode(SNode* pNode);
	void			VisibleObject(SNode* pNode);
public:
	int				AddObject(SBaseObj* pObj);
	int				CheckRect(SNode* pNode, SBaseObj* pObj);

public:
	void			Update(ID3D11Device* pDevice, SCamera* pCamera);
	void			SetRenderDepth(int iRenderDepth){ m_iRenderDepth = iRenderDepth; }

	void			SetMaxDepthLimit(int iMaxDepth) { m_iMaxDepthLimit = iMaxDepth; }
	void			SetMinDivideSize(int iMinDivideSize) { m_fMinDivideSize = (float)iMinDivideSize; }
public:
	SQuadTree();
	~SQuadTree();
};