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
	static TCHAR TextureNameCheck[MAX_PATH];
	static TCHAR READ_LINE[MAX_PATH];

	static TCHAR VertexNameCheck[MAX_PATH];
	static TCHAR IndexNameCheck[MAX_PATH];


	int iMaxObj = 0;

	ZeroMemory(ExporterCheck, _countof(ExporterCheck));
	ZeroMemory(CheckFile, _countof(CheckFile));
	ZeroMemory(ObjectCheck, _countof(ObjectCheck));
	ZeroMemory(VertexNameCheck, _countof(VertexNameCheck));
	ZeroMemory(IndexNameCheck, _countof(IndexNameCheck));
	ZeroMemory(READ_LINE, _countof(READ_LINE));

	// zero width no break space가 출력되는 문제를 임시로 처리하기 위해 앞에 n\ 문자을 사용
	_ftscanf_s(pStream, _T("\n%s%d"), CheckFile, _countof(CheckFile), &iMaxObj);
	

	T_STR FileCheck;
	FileCheck = CheckFile;
	T_STR::iterator CheckZeroWidth = FileCheck.begin();
	int iCheckZeroWidthindex = FileCheck.find(65279, 0);

	CheckZeroWidth += iCheckZeroWidthindex;
	FileCheck.erase(CheckZeroWidth);
	if (wcscmp(FileCheck.c_str(), L"SSSExporter100") != 0)
	{
		FileCheck = m_FilePath;
		FileCheck += _T("은 SSS Export File이 아닙니다");
		MessageBox(0, FileCheck.data(), _T("Fatal error"), MB_OK);
		return false;
	}

	m_VertexList.resize(iMaxObj);

	int iMaxFace = 0;
	int iMaxTexture = 0;
	for (int iCount = 0; iCount < iMaxObj; iCount++)
	{
		_ftscanf_s(pStream, _T("%s %d %d"), ObjectNameCheck, _countof(ObjectNameCheck), &iMaxFace, &iMaxTexture);
		int iMaxVertex = 0;
		int iMaxIndex = 0;
		if (wcscmp(m_MeshDataName.data(), ObjectNameCheck)!= 0)
		{
			_ftscanf_s(pStream, _T("%s %d"), VertexNameCheck, _countof(VertexNameCheck), &iMaxVertex);
			for (int iLine = 0; iLine < iMaxVertex; iLine++)
			{
				_fgetts(ObjectNameCheck, _countof(ObjectCheck), pStream);
			}
			_ftscanf_s(pStream, _T("%s %d"), IndexNameCheck, _countof(IndexNameCheck), &iMaxIndex);
			for (int iLine = 0; iLine < iMaxIndex; iLine++)
			{
				_fgetts(ObjectNameCheck, _countof(ObjectCheck), pStream);
			}
		}
		else
		{
			_ftscanf_s(pStream, _T("%s %d"), VertexNameCheck, _countof(VertexNameCheck), &iMaxVertex);
			bRet = CreateVertexFromFile(pStream, iMaxVertex);
			_ftscanf_s(pStream, _T("%s %d"), IndexNameCheck, _countof(IndexNameCheck), &iMaxIndex);
			bRet = CreateIndexFromFile(pStream, iMaxIndex);
			fclose(pStream);
			return bRet;
		}
	}

	DEBUGMSG(_T("Mesh object can't find"));
	fclose(pStream);

	return bRet;
}
bool SMesh::CreateVertexFromFile(FILE* pStream, int iMaxVertexData)
{
	if (pStream == nullptr)
	{
		DEBUGMSG("file pointer error");
		return false;
	}

	m_VertexList.resize(iMaxVertexData);
	for (int iVertex = 0; iVertex < iMaxVertexData; iVertex++)
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

bool SMesh::CreateIndexFromFile(FILE* pStream, int iMaxIndexData)
{
	if (pStream == nullptr)
	{
		DEBUGMSG("file pointer error");
		return false;
	}

	m_IndexList.resize(iMaxIndexData);
	for (int iCount = 0; iCount < iMaxIndexData; iCount++)
	{
		int iIndex;
		_ftscanf_s(pStream, _T("%d"), &iIndex);
		m_IndexList[iCount] = iIndex;
	}

	m_dxobj.m_iNumIndex = m_IndexList.size();
	m_dxobj.m_iIndexSize = sizeof(DWORD);
	void** pData = nullptr;
	if (m_IndexList.size() > 0) pData = (void**)&m_IndexList.at(0);

	m_dxobj.g_pIndexBuffer.Attach(DXGame::CreateIndexBuffer(m_pDevice,
		pData,
		m_dxobj.m_iNumIndex,
		m_dxobj.m_iIndexSize));

	return true;
}

SMesh::SMesh()
{
	m_dxobj.m_iPrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
}


SMesh::~SMesh()
{
}
