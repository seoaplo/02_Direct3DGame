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
	D3DXVECTOR3								m_vPickVector;
public:
	const D3DXVECTOR3& GetPickVector()const { return m_vPickVector; };
	void SetPickVector(const D3DXVECTOR3& SrcVector3) { m_vPickVector = SrcVector3; }
public:
	virtual bool	Init() override;
	virtual bool	Frame()override;
	virtual bool	Render(ID3D11DeviceContext*	pContext)override;
	virtual bool	Release()override;
	virtual bool	DeleteNode(const SNode* const  pNode)override;
public:
	 bool			Build(float fWidth, float fHeight)override;
	 virtual bool	Build(SMap* const pMap, int   iMaxDepth = 5, float fMinSize = 10.0f)override;
	 bool			BuildTree(SNode* const pNode)override;
	virtual  bool	SubDivide(SNode* const pNode)override;
public:
	virtual SNode* const 	CreateNode(const SNode* const pNode, float fLeft, float fRight, float fBottom, float fTop)override;
	virtual SNode* const 	CreateNode(DWORD iNodeNum, DWORD dwDepth, float fLeft, float fRight, float fBottom, float fTop);
	void			DrawFindNode();
	void			VisibleNode(SNode* const pNode)override;
	void			VisibleObject(const SNode* const  pNode)override;
public:
	int				AddObject(SBaseObj*const  pObj)override;
	int				CheckRect(SNode* const pNode, SBaseObj*const  pObj)override;
public:
	void			Update(ID3D11Device* pDevice, SCamera* pCamera)override;
public:
	void SetVertexHeight(SNode& pNode, const D3DXVECTOR3& vIntersection, DWORD dwMode, float fDistance = 10.0f, float fHeight = 100.0f);
	void CheckBrushNode(SNode* pNode, const D3DXVECTOR3& vPickVector, float fDistance);

	D3DXVECTOR2 GetHeightFromNode(DWORD nLeft, DWORD nRight, DWORD nBottom, DWORD nTop)override;
	void CreateBuffer(SNode* pNode)override;
	void CreateIndexBuffer(SNode* pNode)override;

	void UpdateNode();

	SNode* const CheckPicking(SSelect& TargetSelect);
	const bool	FindePickingNode(SSelect& TargetSelect);
	const bool	FindePickingFace(SSelect& TargetSelect, SNode* pNode);
public:
	const virtual bool	SaveFile(FILE* pStream);
	const virtual bool	LoadFile(SParser& Paser);
public:
	SQuadTreeArray();
	virtual ~SQuadTreeArray();
};

