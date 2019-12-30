#pragma once
#include "SDXShape.h"
#include "SParser.h"

#define RANDOM(n)			(rand() % (n))
#define RANDOMSCOPE(s,e)	(rand() %(e-s)) + s;
#define RANDOMIZE()			srand((unsigned)time(NULL))

struct SMapDesc
{
	int			iNumCols;
	int			iNumRows;
	int			iNumSell;
	float		fSellDistance;
	float		fScaleHeight;
	T_STR		strTextureFile;
	T_STR		strShaderFile;
};

struct SCBTile
{
	DWORD TileNum;
	DWORD Temp0;
	DWORD Temp1;
	DWORD Temp2;
};

class SMap : public SModel
{
public:
	SMapDesc				m_MapDesc;
	vector<PNCT_VERTEX>		m_VertexList;		// 버텍스 리스트
	DWORD					m_dwShaderIndex;
	int						m_iSellNum;			// 셀 갯수
	int						m_iNumVertices;		// 버텍스 갯수
	int						m_iNumFace;			// 페이스 갯수
	int						m_iDiffuseTex;		
	int						m_iNumCols;			// 가로 버텍스 갯수
	int						m_iNumRows;			// 세로 버텍스 갯수
	int						m_iNumSellCols;		// 가로 셀 갯수
	int						m_iNumSellRows;		// 세로 버텍스 갯수
	float					m_fSellDistance;	// 셀 간의 길이
	D3DXMATRIX				m_matWorld;
	D3DXMATRIX				m_matView;
	D3DXMATRIX				m_matProj;
	D3DXMATRIX				m_matNormal;
	D3DXVECTOR3				m_vLightDir;
	
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_CBTileNum;
	STexture*				m_pBaseTexture;
	STexture*				m_pSplatTexture0;
	STexture*				m_pSplatTexture1;
	STexture*				m_pSplatTexture2;

	ID3D11ShaderResourceView* m_pAlphaSRV;
	ID3D11ShaderResourceView* m_pBrush;
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
	HRESULT			CreateConstantBuffer()override;
	void			UpdateConstantBuffer(ID3D11DeviceContext* pContext, SModel* pParent)override;
	void			RenderSetTexture(ID3D11DeviceContext*  pContext);
	void			SetTexture(ID3D11Device* pDevice, T_STR strBaseTexture, T_STR strAlphaTex0, T_STR strAlphaTex1, T_STR strAlphaTex2, ID3D11ShaderResourceView* pAlpha, ID3D11ShaderResourceView* pBrush);
	bool			UpdateBuffer();
	HRESULT			SetInputLayout();
public:
	//===================================================================================
	// 해당 포맷의 타입을 변경하는 함수
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
	virtual bool	LoadFile(SParser& Paser, T_STR m_AlphaTextureString);
	virtual bool	SaveFile(FILE* pStream, T_STR m_AlphaTextureString);
	//===========================================================================================
	// 정점 노말 연산을 위한 초기화 함수 및 페이스 노말 계산
	//===========================================================================================
	virtual void		InitFaceNormals();
	virtual void		CalcFaceNormals();
	virtual D3DXVECTOR3	ComputeFaceNormal(DWORD dwIndex0, DWORD dwIndex1, DWORD dwIndex2);
	//===========================================================================================
	// 정점 노말 룩업 테이블 생성 및 계산
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
	virtual ~SMap();
};

