#pragma once
#include "SDXShape.h"
#include "SDxSO.h"

class SBoxObj : public SBox
{
public:
	int						m_iNumSteamOut;
	DXGame::SDxSO			m_RenderSO;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader>	m_pGS;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader>	m_pGSStreamOutput;
	Microsoft::WRL::ComPtr<ID3D11VertexShader>		m_pVSPassThroughMain;
	Microsoft::WRL::ComPtr<ID3DBlob>	m_pVSBlob;
	Microsoft::WRL::ComPtr<ID3DBlob>	m_pGSBlob;
public:
	bool	Init();
	HRESULT LoadShaderFile(ID3D11Device* pDevice, const TCHAR* pShaderFile);
	bool	SetSO(ID3D11Device* pDevice, ID3D11DeviceContext*  pContext);
	bool	Render(ID3D11DeviceContext*  pContext);
	bool	PostRender(ID3D11DeviceContext*  pContext);
	bool	RenderMesh(ID3D11DeviceContext*  pContext, bool bFirstRender = true);
public:
	SBoxObj();
	~SBoxObj();
};

