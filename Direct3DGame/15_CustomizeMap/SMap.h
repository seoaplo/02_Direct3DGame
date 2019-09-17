#pragma once
#include "SDXShape.h"

#define RANDOM(n)			(rand() % (n))
#define RANDOMSCOPE(s,e)	(rand() %(e-s)) + s;
#define RANDOMIZE()			srand((unsigned)time(NULL))

struct SMapDesc
{
	int			iNumCols;
	int			iNumRows;
	float		fSellDistance;
	float		fScaleHeight;
	T_STR		strTextureFile;
	T_STR		strShaderFile;
};

class SMap : public SModel
{
public:
	SMapDesc				m_MapDesc;
	vector<PNCT_VERTEX>		m_VertexList;
	DWORD					m_dwShaderIndex;
	int						m_iNumVertices;		// ���ؽ� ����
	int						m_iNumFace;			// ���̽� ����
	int						m_iDiffuseTex;		
	int						m_iNumCols;			// ���� ���ؽ� ����
	int						m_iNumRows;			// ���� ���ؽ� ����
	int						m_iNumSellCols;		// ���� �� ����
	int						m_iNumSellRows;		// ���� ���ؽ� ����
	float					m_fSellDistance;	// �� ���� ����
	D3DXMATRIX				m_matWorld;
	D3DXMATRIX				m_matView;
	D3DXMATRIX				m_matProj;
	D3DXMATRIX				m_matNormal;
	D3DXVECTOR3				m_vLightDir;
	
	int*					m_pNormalLookupTable;
	D3DXVECTOR3*			m_pFaceNormals;
	bool					m_bStaticLight;
public:
	bool			Frame();
	bool			Render(ID3D11DeviceContext*	pContext);
	bool			Release();
	HRESULT			CreateVertexBuffer();
	HRESULT			CreateIndexBuffer();
	bool			CreateVertexData();
	bool			CreateIndexData();
	bool			UpdateBuffer();
	HRESULT			SetInputLayout();
public:
	//===================================================================================
	// �ش� ������ Ÿ���� �����ϴ� �Լ�
	//===================================================================================
	DXGI_FORMAT		MAKE_TYPELESS(DXGI_FORMAT format);
	DXGI_FORMAT		MAKE_SRGB(DXGI_FORMAT format);
public:
	virtual bool	ReLoadVBuffer();
	virtual bool	ReLoadIBuffer();
	virtual	void	UpdateIndexBuffer(ID3D11DeviceContext*	pContext, DWORD* pdwIndexArray, int iFaceCount);
	virtual void	CalcVertexColor(D3DXVECTOR3 vLightDir);
	virtual	bool	CreateMap(SMapDesc& MapDesc);
	virtual	bool	Init(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	virtual bool	Load(SMapDesc& MapDesc);
	//===========================================================================================
	// ���� �븻 ������ ���� �ʱ�ȭ �Լ� �� ���̽� �븻 ���
	//===========================================================================================
	virtual void		InitFaceNormals();
	virtual void		CalcFaceNormals();
	virtual D3DXVECTOR3	ComputeFaceNormal(DWORD dwIndex0, DWORD dwIndex1, DWORD dwIndex2);
	//===========================================================================================
	// ���� �븻 ��� ���̺� ���� �� ���
	//===========================================================================================
	virtual	void		GenNormalLookupTable();
	virtual	void		CalcPerVertexNormalsFastLookup();
	virtual	float		GetHeightmap(int row, int col);
	virtual float		GetHeightOfVertex(UINT index);
	virtual D3DXVECTOR3	GetNormalOfVertex(UINT index);
	virtual D3DXVECTOR4	GetColorOfVertex(UINT index);
	virtual D3DXVECTOR2 GetTextureOfVertex(float fOffsetX, float fOffsetY);
	virtual float		GetHeight(float fPosX, float fPosZ);
	virtual float		Lerp(float fStart, float fEnd, float fTangent);
public:
	SMap();
	~SMap();
};

