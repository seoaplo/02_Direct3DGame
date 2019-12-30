#pragma once
#include "SQuadTreeMap.h"
#include "SSelect.h"

enum SHeightSetState
{
	QUADTREEMAP_HEIGHT_UP,
	QUADTREEMAP_HEIGHT_DOWN,
	QUADTREEMAP_HEIGHT_FLAT,
	QUADTREEMAP_HEIGHT_SIZE,
};

class SQuadTreeArray : public SQuadTreeMap
{
private:
	DWORD m_dwTileIndexBufferSize;

	std::vector<SNode> m_NodeList;
	Microsoft::WRL::ComPtr<ID3D11Buffer>	m_pIndexBuffer;
	std::vector<DWORD>						m_IndexBufferData;
	std::vector<SNode*>						m_PickingNodeList;
	std::vector<SNode*>						m_HeightNodeList;
public:
	D3DXVECTOR3								m_vPickVector;
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
	void			DrawFindNode();
	void			VisibleNode(SNode* pNode)override;
	void			VisibleObject(SNode* pNode)override;
public:
	int				AddObject(SBaseObj* pObj)override;
	int				CheckRect(SNode* pNode, SBaseObj* pObj)override;
public:
	void			Update(ID3D11Device* pDevice, SCamera* pCamera)override;
public:
	void SetVertexHeight(SNode& pNode, D3DXVECTOR3& vIntersection, DWORD dwMode, float fDistance = 10.0f, float fHeight = 100.0f);
	void CheckBrushNode(SNode* pNode, D3DXVECTOR3& vPickVector, float fDistance);

	D3DXVECTOR2 GetHeightFromNode(DWORD nLeft, DWORD nRight, DWORD nBottom, DWORD nTop)override;
	void CreateBuffer(SNode* pNode)override;
	void CreateIndexBuffer(SNode* pNode)override;

	void UpdateNode();

	SNode*	CheckPicking(SSelect& TargetSelect);
	bool	FindePickingNode(SSelect& TargetSelect);
	bool	FindePickingFace(SSelect& TargetSelect, SNode* pNode);
public:
	virtual bool	SaveFile(FILE* pStream);
	virtual bool	LoadFile(SParser& Paser);
public:
	SQuadTreeArray();
	virtual ~SQuadTreeArray();
};

