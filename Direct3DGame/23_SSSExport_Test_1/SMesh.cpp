#include "SMesh.h"


bool SMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const TCHAR* pMeshDataName, const TCHAR* pLoadShaderFile)
{

	DEBUGMSG(_T("미구현된 함수"));
	return false;

	if (pDevice == nullptr)
	{
		MessageBox(0, _T("pDevice가 nullptr입니다"), _T("Fatal error"), MB_OK);
		return false;
	}

	m_pDevice = pDevice;
	m_pContext = pContext;
	m_MeshDataName = pMeshDataName;

	if (FAILED(LoadShaderFile(pDevice, pLoadShaderFile)))
	{
		MessageBox(0, _T("LoadShaderFile 실패"), _T("Fatal error"), MB_OK);
		return false;
	}
	if (FAILED(SetInputLayout()))
	{
		MessageBox(0, _T("SetInputLayout 실패"), _T("Fatal error"), MB_OK);
		return false;
	}

	if (FAILED(CreateVertexBuffer()))
	{
		MessageBox(0, _T("CreateVertexBuffer 실패"), _T("Fatal error"), MB_OK);
		return false;
	}
	/*if (FAILED(CreateIndexBuffer()))
	{
		MessageBox(0, _T("CreateIndexBuffer 실패"), _T("Fatal error"), MB_OK);
		return false;
	}*/
	if (FAILED(CreateConstantBuffer()))
	{
		MessageBox(0, _T("CreateConstantBuffer 실패"), _T("Fatal error"), MB_OK);
		return false;
	}
	if (!UpdateBuffer())
	{
		MessageBox(0, _T("UpdateBuffer 실패"), _T("Fatal error"), MB_OK);
		return false;
	}
	if (FAILED(CreateResource()))
	{
		MessageBox(0, _T("CreateResource 실패"), _T("Fatal error"), MB_OK);
		return false;
	}

	return Init();
}
bool SMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const TCHAR* pMeshDataName, const TCHAR* pExportFile, const TCHAR* pLoadShaderFile)
{

	if (pDevice == nullptr)
	{
		MessageBox(0, _T("pDevice가 nullptr입니다"), _T("Fatal error"), MB_OK);
		return false;
	}

	m_pDevice = pDevice;
	m_pContext = pContext;
	m_MeshDataName = pMeshDataName;
	m_FilePath = pExportFile;

	if (FAILED(LoadShaderFile(pDevice, pLoadShaderFile)))
	{
		MessageBox(0, _T("LoadShaderFile 실패"), _T("Fatal error"), MB_OK);
		return false;
	}
	if (FAILED(SetInputLayout()))
	{
		MessageBox(0, _T("SetInputLayout 실패"), _T("Fatal error"), MB_OK);
		return false;
	}

	if (FAILED(LoadExportFile()))
	{
		MessageBox(0, _T("LoadExport 실패"), _T("Fatal error"), MB_OK);
		return false;
	}
	if (FAILED(CreateVertexBuffer()))
	{
		MessageBox(0, _T("CreateIndexBuffer 실패"), _T("Fatal error"), MB_OK);
		return false;
	}
	if (FAILED(CreateConstantBuffer()))
	{
		MessageBox(0, _T("CreateConstantBuffer 실패"), _T("Fatal error"), MB_OK);
		return false;
	}

	return Init();
}
bool SMesh::PostRender(ID3D11DeviceContext* pContext)
{
	UpdateConstantBuffer(pContext);
	pContext->Draw(m_dxobj.m_iNumVertex, 0);
	return true;
}
bool SMesh::LoadExportFile()
{
	if (m_FilePath.size() <= 0)
	{
		DEBUGMSG(_T("Export file path Error"));
		return false;
	}

	bool bRet = false;
	FILE* pStream = nullptr;
	_wfopen_s(&pStream, m_FilePath.c_str(), _T("rb"));

	static TCHAR ExporterCheck[MAX_PATH];
	static TCHAR CheckFile[MAX_PATH];
	static TCHAR ObjectCheck[MAX_PATH];
	static TCHAR ObjectNameCheck[MAX_PATH];
	static TCHAR PNCT_LINE[MAX_PATH];

	int iMaxObj = 0;

	ZeroMemory(ExporterCheck, _countof(ExporterCheck));
	ZeroMemory(CheckFile, _countof(CheckFile));
	ZeroMemory(ObjectCheck, _countof(ObjectCheck));
	ZeroMemory(PNCT_LINE, _countof(PNCT_LINE));

	_ftscanf_s(pStream, _T("%s %d"), CheckFile, _countof(CheckFile), &iMaxObj);

	T_STR FileCheck;
	FileCheck = CheckFile;
	if (wcscmp(FileCheck.data(), _T("sssexporter100")) != 0)
	{
		FileCheck = m_FilePath;
		FileCheck += _T("은 SSS Export File이 아닙니다");
		MessageBox(0, FileCheck.data(), _T("Fatal error"), MB_OK);
		return false;
	}

	m_VertexList.resize(iMaxObj);

	int iMaxVertex = 0;
	for (int iCount = 0; iCount < iMaxObj; iCount++)
	{
		_ftscanf_s(pStream, _T("%s %d"), ObjectNameCheck, _countof(ObjectNameCheck), &iMaxVertex);
		iMaxVertex *= 3;

		if (wcscmp(m_MeshDataName.data(), ObjectNameCheck)== 0)
		{
			bRet = CreateShaderFile(pStream, iMaxVertex);
			fclose(pStream);

			return bRet;
		}
		for (int iLine = 0; iLine < iMaxVertex; iLine++)
		{
			_fgetts(ObjectNameCheck, _countof(ObjectCheck), pStream);
		}
	}

	DEBUGMSG(_T("Mesh object can't find"));
	fclose(pStream);

	return bRet;
}
bool SMesh::CreateShaderFile(FILE* pStream, int iVertexNum)
{
	if (pStream == nullptr)
	{
		DEBUGMSG("file pointer error");
		return false;
	}
	m_VertexList.resize(iVertexNum);
	for (int iVertex = 0; iVertex < iVertexNum; iVertex++)
	{
		PNCT_VERTEX pnct;
		_ftscanf_s(pStream, _T("%f %f %f"), &pnct.p.x, &pnct.p.y, &pnct.p.z);

		_ftscanf_s(pStream, _T("%f %f %f"), &pnct.n.x, &pnct.n.y, &pnct.n.z);

		_ftscanf_s(pStream, _T("%f %f %f %f"), &pnct.c.x, &pnct.c.y, &pnct.c.z, &pnct.c.w);

		_ftscanf_s(pStream, _T("%f %f"),
			&pnct.t.x, &pnct.t.y);

		m_VertexList[iVertex] = pnct;
	}

	m_dxobj.m_iNumVertex = m_VertexList.size();
	m_dxobj.m_iVertexSize = sizeof(PNCT_VERTEX);
	void** pData = nullptr;
	if (m_VertexList.size() > 0) pData = (void**)&m_VertexList.at(0);

	m_dxobj.g_pVertexBuffer.Attach(DXGame::CreateVertexBuffer(m_pDevice,
		pData,
		m_dxobj.m_iNumVertex,
		m_dxobj.m_iVertexSize));

	return true;
}
SMesh::SMesh()
{
	m_dxobj.m_iPrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
}


SMesh::~SMesh()
{
}
