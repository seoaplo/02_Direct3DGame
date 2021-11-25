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
	DWORD			m_dwMaxDepthLimit;

	
	std::queue<SNode*>			m_QuadTreeQueue;


	int m_iObjListSize;
	std::vector<SBaseObj*>		m_DrawObjList;

	int m_iNodeListSize;
	std::vector<SNode*>			m_DrawNodeList;
public:
	virtual bool	Init();
	virtual bool	Frame();
	virtual bool	Render(ID3D11DeviceContext*	pContext);
	virtual bool	Release();
	virtual bool	DeleteNode(const SNode* const pNode);
public:
	virtual bool			Build(float fWidth, float fHeight);
	virtual bool			BuildTree( SNode* const pNode);
	virtual bool	SubDivide( SNode* const pNode);
public:
	virtual SNode* const 	CreateNode(const SNode* const pParentNode, float fLeft, float fRight, float fBottom, float fTop);
	virtual SNode* const			FindNode(const SNode* const pNode,  SBaseObj* const pObj);
	virtual void			DrawFindNode( SNode* const pNode);
	virtual void			VisibleNode( SNode* const pNode);
	virtual void			VisibleObject(const SNode* const pNode);
public:
	virtual int				AddObject( SBaseObj* const pObj);
	virtual int				CheckRect( SNode* const  pNode,  SBaseObj* const pObj);

public:
	virtual void			Update(ID3D11Device* const pDevice, SCamera* const pCamera);
	void			SetRenderDepth(int iRenderDepth){ m_iRenderDepth = iRenderDepth; }

	void			SetMaxDepthLimit(int iMaxDepth) { m_dwMaxDepthLimit = iMaxDepth; }
	void			SetMinDivideSize(int iMinDivideSize) { m_fMinDivideSize = (float)iMinDivideSize; }
public:
	SQuadTree();
	virtual ~SQuadTree();
};