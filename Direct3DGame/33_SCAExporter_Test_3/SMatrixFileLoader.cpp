#include "SMatrixFileLoader.h"


static TCHAR MatrixExporter[] = L"MatrixExporter100";
static TCHAR Header[] = L"#HEADERINFO";
static TCHAR Material[] = L"#MATERIAL_INFO";
static TCHAR Object[] = L"#OBJECT_INFO";
static TCHAR Mesh[] = L"Mesh";
static TCHAR SubMesh[] = L"SubMesh";
static TCHAR Animation[] = L"#AnimationData";

bool SMatrixFileLoader::Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;

	return true;
}
int SMatrixFileLoader::Load(T_STR FilePath)
{
	int Key = -1;
	if (m_pDevice == nullptr) return Key;
	if (m_pContext == nullptr) return Key;
	SScene	SceneData;
	bool bCheck = false;

	static TCHAR String[MAX_PATH];
	ZeroMemory(String, _countof(String));
	ZeroMemory(&SceneData, sizeof(SceneData));

	m_Paser.OpenStream(FilePath.c_str());

	bCheck = m_Paser.GetDataFromFileNext(MatrixExporter);
	if (bCheck == false) return Key;

	bCheck = m_Paser.GetDataFromFile(Header);
	if (bCheck == false) return Key;

	_stscanf_s(m_Paser.m_pBuffer, _T("%s%d%d%d%d%d"), String, MAX_PATH,
		&SceneData.iFirstFrame,
		&SceneData.iLastFrame,
		&SceneData.iFrameSpeed,
		&SceneData.iTickPerFrame,
		&SceneData.iNumObjects);
	ZeroMemory(String, _countof(String));

	SMatrixObjectList* pObjectList = I_MatrixObjectListManager.Create();
	pObjectList->m_MatrixObjectList.resize(SceneData.iNumObjects);
	pObjectList->m_Scene = SceneData;
	Key = I_MatrixObjectListManager.GetSize() - 1;

	bCheck = m_Paser.GetDataFromFileNext(Object);
	if (!bCheck) return Key;
	for (int iMesh = 0; iMesh < SceneData.iNumObjects; iMesh++)
	{
		m_Paser.GetNextLine();
		TCHAR ParentName[MAX_PATH];
		ZeroMemory(ParentName, _countof(ParentName));

		SMatrixObject& MatrixObject = pObjectList->m_MatrixObjectList[iMesh];
		_stscanf_s(m_Paser.m_pBuffer, _T("%s %f %f %f %f %f %f"),
			String, MAX_PATH,
			&MatrixObject.m_BoundBox.vMax.x,
			&MatrixObject.m_BoundBox.vMax.y,
			&MatrixObject.m_BoundBox.vMax.z,
			&MatrixObject.m_BoundBox.vMin.x,
			&MatrixObject.m_BoundBox.vMin.y,
			&MatrixObject.m_BoundBox.vMin.z);
		MatrixObject.m_ObjectName = String;
		MatrixObject.m_Scene = SceneData;
		m_Paser.GetNextLine();

		// Animation Matrix Start Frame
		_stscanf_s(m_Paser.m_pBuffer, _T("%f %f %f %f"),
			&MatrixObject.m_matAnim._11,
			&MatrixObject.m_matAnim._12,
			&MatrixObject.m_matAnim._13,
			&MatrixObject.m_matAnim._14);

		m_Paser.GetNextLine();
		_stscanf_s(m_Paser.m_pBuffer, _T("%f %f %f %f"),
			&MatrixObject.m_matAnim._21,
			&MatrixObject.m_matAnim._22,
			&MatrixObject.m_matAnim._23,
			&MatrixObject.m_matAnim._24);

		m_Paser.GetNextLine();
		_stscanf_s(m_Paser.m_pBuffer, _T("%f %f %f %f"),
			&MatrixObject.m_matAnim._31,
			&MatrixObject.m_matAnim._32,
			&MatrixObject.m_matAnim._33,
			&MatrixObject.m_matAnim._34);

		m_Paser.GetNextLine();
		_stscanf_s(m_Paser.m_pBuffer, _T("%f %f %f %f"),
			&MatrixObject.m_matAnim._41,
			&MatrixObject.m_matAnim._42,
			&MatrixObject.m_matAnim._43,
			&MatrixObject.m_matAnim._44);

		// Animation Inverse Matrix Start Frame

		m_Paser.GetNextLine();
		_stscanf_s(m_Paser.m_pBuffer, _T("%f %f %f %f"),
			&MatrixObject.m_matAnimInv._11,
			&MatrixObject.m_matAnimInv._12,
			&MatrixObject.m_matAnimInv._13,
			&MatrixObject.m_matAnimInv._14);

		m_Paser.GetNextLine();
		_stscanf_s(m_Paser.m_pBuffer, _T("%f %f %f %f"),
			&MatrixObject.m_matAnimInv._21,
			&MatrixObject.m_matAnimInv._22,
			&MatrixObject.m_matAnimInv._23,
			&MatrixObject.m_matAnimInv._24);

		m_Paser.GetNextLine();
		_stscanf_s(m_Paser.m_pBuffer, _T("%f %f %f %f"),
			&MatrixObject.m_matAnimInv._31,
			&MatrixObject.m_matAnimInv._32,
			&MatrixObject.m_matAnimInv._33,
			&MatrixObject.m_matAnimInv._34);

		m_Paser.GetNextLine();
		_stscanf_s(m_Paser.m_pBuffer, _T("%f %f %f %f"),
			&MatrixObject.m_matAnimInv._41,
			&MatrixObject.m_matAnimInv._42,
			&MatrixObject.m_matAnimInv._43,
			&MatrixObject.m_matAnimInv._44);
		SMatrixMesh& pMesh = *MatrixObject.CreateMesh();

		bCheck = m_Paser.GetDataFromFileNext(SubMesh);
		if (!bCheck) continue;

		int iVertexSize;
		int iIndexSize;

		_stscanf_s(m_Paser.m_pBuffer, _T("%s %d %d"),
			String, MAX_PATH,
			&iVertexSize,
			&iIndexSize);
		ZeroMemory(String, _countof(String));

		pMesh._VertexList.resize(iVertexSize);
		pMesh._IndexList.resize(iIndexSize);
		for (int iVertex = 0; iVertex < iVertexSize; iVertex++)
		{
			m_Paser.GetNextLine();
			_stscanf_s(m_Paser.m_pBuffer, _T("%f %f %f %f %f %f %f %f %f %f %f %f"),
				&pMesh._VertexList[iVertex].p.x,
				&pMesh._VertexList[iVertex].p.y,
				&pMesh._VertexList[iVertex].p.z,
				&pMesh._VertexList[iVertex].n.x,
				&pMesh._VertexList[iVertex].n.y,
				&pMesh._VertexList[iVertex].n.z,
				&pMesh._VertexList[iVertex].c.x,
				&pMesh._VertexList[iVertex].c.y,
				&pMesh._VertexList[iVertex].c.z,
				&pMesh._VertexList[iVertex].c.w,
				&pMesh._VertexList[iVertex].t.x,
				&pMesh._VertexList[iVertex].t.y);
		}
		;
		for (int iIndex = 0; iIndex < iIndexSize; iIndex += 3)
		{
			m_Paser.GetNextLine();
			_stscanf_s(m_Paser.m_pBuffer, _T("%d %d %d"),
				&pMesh._IndexList[iIndex + 0],
				&pMesh._IndexList[iIndex + 1],
				&pMesh._IndexList[iIndex + 2]);
		}

		int iMaxScaleNum = 0;
		int iMaxRotationNum = 0;
		int iMaxTransposeNum = 0;
		bCheck = m_Paser.GetDataFromFile(Animation);
		if (bCheck == false) return Key;

		_stscanf_s(m_Paser.m_pBuffer, _T("%s%d%d%d"), String, MAX_PATH,
			&iMaxTransposeNum,
			&iMaxRotationNum,
			&iMaxScaleNum);
		ZeroMemory(String, _countof(String));
		MatrixObject.m_PosAnimList.resize(iMaxTransposeNum);
		MatrixObject.m_RotAnimList.resize(iMaxRotationNum);
		MatrixObject.m_ScaleAnimList.resize(iMaxScaleNum);
		int iCountTrack;
		for (int iTranspose = 0; iTranspose < iMaxTransposeNum; iTranspose++)
		{
			m_Paser.GetNextLine();
			_stscanf_s(m_Paser.m_pBuffer, _T("\n%d %d %f %f %f"),
				&iCountTrack,
				&MatrixObject.m_PosAnimList[iTranspose].iTick,
				&MatrixObject.m_PosAnimList[iTranspose].Position.x,
				&MatrixObject.m_PosAnimList[iTranspose].Position.y,
				&MatrixObject.m_PosAnimList[iTranspose].Position.z);
		}
		for (int iRotation = 0; iRotation < iMaxRotationNum; iRotation++)
		{
			m_Paser.GetNextLine();
			_stscanf_s(m_Paser.m_pBuffer, _T("\n%d %d %f %f %f %f"),
				&iCountTrack,
				&MatrixObject.m_RotAnimList[iRotation].iTick,
				&MatrixObject.m_RotAnimList[iRotation].Quaternion.x,
				&MatrixObject.m_RotAnimList[iRotation].Quaternion.y,
				&MatrixObject.m_RotAnimList[iRotation].Quaternion.z,
				&MatrixObject.m_RotAnimList[iRotation].Quaternion.w);
		}
		for (int iScale = 0; iScale < iMaxScaleNum; iScale++)
		{
			m_Paser.GetNextLine();
			_stscanf_s(m_Paser.m_pBuffer, _T("\n%d %d %f %f %f %f %f %f %f"),
				&iCountTrack,
				&MatrixObject.m_ScaleAnimList[iScale].iTick,
				&MatrixObject.m_ScaleAnimList[iScale].Position.x,
				&MatrixObject.m_ScaleAnimList[iScale].Position.y,
				&MatrixObject.m_ScaleAnimList[iScale].Position.z,

				&MatrixObject.m_ScaleAnimList[iScale].Quaternion.x,
				&MatrixObject.m_ScaleAnimList[iScale].Quaternion.y,
				&MatrixObject.m_ScaleAnimList[iScale].Quaternion.z,
				&MatrixObject.m_ScaleAnimList[iScale].Quaternion.w);
		}
		MatrixObject.Init();
		MatrixObject.Create(m_pDevice, m_pContext, L"Debug.hlsl");
	}
	m_Paser.CloseStream();

	return Key;
}

SMatrixFileLoader::SMatrixFileLoader()
{
}


SMatrixFileLoader::~SMatrixFileLoader()
{
}
