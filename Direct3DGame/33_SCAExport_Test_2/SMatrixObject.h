#pragma once
#include "SMatrixMesh.h"

class SMatrixObject
{
public:
	int m_iIndex;

	T_STR	m_ObjectName;
	SScene	m_Scene;
	S_BOX	m_BoundBox;
	std::vector<SMatrixMesh> m_MeshList;

	D3DXMATRIX  m_matWorld;
	D3DXMATRIX  m_matView;
	D3DXMATRIX  m_matProj;

	D3DXMATRIX m_matAnim;
	D3DXMATRIX m_matAnimInv;
	D3DXMATRIX  m_matCalculation;

	std::vector<SAnimTrack>   m_PosAnimList;
	std::vector<SAnimTrack>   m_RotAnimList;
	std::vector<SAnimTrack>   m_ScaleAnimList;

	// 0 frame 분해된 에니메이션 트랙의 행렬
	D3DXMATRIX m_matAnimSelf;
	D3DXMATRIX m_matPos;
	D3DXMATRIX m_matRot;
	D3DXMATRIX m_matScl;
	D3DXVECTOR3 m_vPosTrack;
	D3DXVECTOR3 m_vScaleTrack;
	D3DXQUATERNION m_qRotTrack;
public:
	SMatrixMesh* CreateMesh();
	bool Init();
	bool Frame(float fAnimElapseTime);
	bool Render(ID3D11DeviceContext* pContext);
	bool Release();

	bool Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const TCHAR* pShaderFile = nullptr);
	bool SetMatrix(D3DXMATRIX* pWorld, D3DXMATRIX* pView = nullptr, D3DXMATRIX* pProj = nullptr);

	void Interpolate(float fElapseTime);
	bool GetAnimationTrack(float fElapseTime, std::vector<SAnimTrack>& trackList, SAnimTrack& StartTrack, SAnimTrack& EndTrack);
public:
	SMatrixObject();
	~SMatrixObject();
};


class SMatrixObjectList
{
public:
	
	T_STR		m_ObjectName;
	int			m_iIndex;

	SScene	m_Scene;
	float m_fAnimationSpeed;
	float m_fAnimElapseTime;
	bool	bDebug;

	D3DXMATRIX  m_matWorld;
	D3DXMATRIX  m_matView;
	D3DXMATRIX  m_matProj;

	D3DXMATRIX	m_MatrixList[AnimationMatrixList];
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_pMatListConstantBuffer;
	std::vector<SMatrixObject> m_MatrixObjectList;
public:
	bool Init();
	bool Frame();
	bool Render(ID3D11DeviceContext* pContext);
	bool Release();

	bool Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	bool SetMatrix(D3DXMATRIX* pWorld, D3DXMATRIX* pView = nullptr, D3DXMATRIX* pProj = nullptr);
public:
	SMatrixObjectList();
	~SMatrixObjectList();
};


class SMatrixObjectListManager : public Singleton<SMatrixObjectListManager>
{
	friend class Singleton<SMatrixObjectListManager>;
private:
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pContext;
	std::vector<SMatrixObjectList> m_MatrixObjectLists;
public:
	void Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	{
		m_pDevice = pDevice;
		m_pContext = pContext;
	}
	SMatrixObjectList*	Create();
	SMatrixObjectList*	GetMatrixObjectList(T_STR GetMatrixObjectListName);
	SMatrixObjectList*	GetMatrixObjectList(int	iIndex);
	int			GetSize();
private:
	SMatrixObjectListManager() {}
public:
	~SMatrixObjectListManager() {}
};

#define I_MatrixObjectListManager SMatrixObjectListManager::GetInstance()