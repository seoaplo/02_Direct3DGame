#pragma once
#include "SQuadTreeMap.h"
class SQuadTreeArray : public SQuadTreeMap
{
private:
	DWORD m_dwTileIndexBufferSize;

	std::vector<SNode> m_NodeList;
	Microsoft::WRL::ComPtr<ID3D11Buffer>	m_pIndexBuffer;
	std::vector<DWORD>						m_IndexBufferData;
public:
	virtual bool	Init() override;
	virtual bool	Frame()override;
	virtual bool	Render(ID3D11DeviceContext*	pContext)override;
	virtual bool	Release()override;
	virtual bool	DeleteNode(SNode* pNode)override;
public:
	bool			Build(float fWidth, float fHeight)override;
	virtual bool	Build(SMap* pMap, int   iMaxDepth = 5, float fMinSize = 10.0f) override;
	bool			BuildTree(SNode* pNode)override;
	virtual bool	SubDivide(SNode* pNode)override;
public:
	virtual SNode*	CreateNode(SNode* pNode, float fLeft, float fRight, float fBottom, float fTop)override;
	virtual SNode*	CreateNode(DWORD iNodeNum, DWORD dwDepth, float fLeft, float fRight, float fBottom, float fTop);
	SNode*			FindNode(SNode* pNode, SBaseObj* pObj)override;
	void			DrawFindNode(SNode* pNode)override;
	void			VisibleNode(SNode* pNode)override;
	void			VisibleObject(SNode* pNode)override;
public:
	int				AddObject(SBaseObj* pObj)override;
	int				CheckRect(SNode* pNode, SBaseObj* pObj)override;
public:
	void			Update(ID3D11Device* pDevice, SCamera* pCamera)override;
public:
	void UpVectexHeight(SNode& pNode, D3DXVECTOR3& vIntersection, float fDistance = 10.0f, float fHeight = 100.0f)override;
	D3DXVECTOR2 GetHeightFromNode(DWORD nLeft, DWORD nRight, DWORD nBottom, DWORD nTop)override;
	void CreateBuffer(SNode* pNode)override;
	void CreateIndexBuffer(SNode* pNode)override;

	void UpdateNode(SNode* pNode);
public:
	SQuadTreeArray();
	virtual ~SQuadTreeArray();
};

