#include "SSSFileLoader.h"

static TCHAR SSSExporter[] = L"SSSExporter100";
static TCHAR Header[] = L"#HEADERINFO";
static TCHAR Material[] = L"#MATERIAL_INFO";
static TCHAR Object[] = L"#OBJECT_INFO";
static TCHAR SubMesh[] = L"SubMesh";
static TCHAR Animation[] = L"#AnimationData";

bool SSSFileLoader::Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;

	return true; 
}
bool SSSFileLoader::Load(T_STR FilePath)
{
	if (m_pDevice == nullptr) return -1;
	if (m_pContext == nullptr) return -1;
	SScene	SceneData;
	bool bCheck = false;

	static TCHAR String[MAX_PATH];
	ZeroMemory(String, _countof(String));
	ZeroMemory(&SceneData, sizeof(SceneData));

	m_Paser.OpenStream(FilePath.c_str());
	
	bCheck = m_Paser.GetDataFromFileNext(SSSExporter);
	if (bCheck == false) return -1;

	bCheck = m_Paser.GetDataFromFile(Header);
	if (bCheck == false) return -1;

	_stscanf_s(m_Paser.m_pBuffer, _T("%s%d%d%d%d%d%d"), String, MAX_PATH,
		&SceneData.iFirstFrame,
		&SceneData.iLastFrame,
		&SceneData.iFrameSpeed,
		&SceneData.iTickPerFrame,
		&SceneData.iNumObjects,
		&SceneData.iNumMaterials);
	ZeroMemory(String, _countof(String));


	bCheck = m_Paser.GetDataFromFileNext(Material);
	if (bCheck == false) return -1;
	// Material

	int iSubMaterialSize = 0;
	for (int iMaterial = 0; iMaterial < SceneData.iNumMaterials; iMaterial++)
	{
		SMaterial* pMaterial = I_MaterialManager.Create();
		m_Paser.GetNextLine();
		_stscanf_s(m_Paser.m_pBuffer, _T("%s%d"), String, MAX_PATH, &iSubMaterialSize);
		pMaterial->m_MaterialName = String;
		ZeroMemory(String, _countof(String));

		// SubMaterial
		for (int iSubMaterial = 0; iSubMaterial < iSubMaterialSize; iSubMaterial++)
		{
			SSubMaterial* pSubMaterial = pMaterial->Create();
			int iTextureSize = 0;
			m_Paser.GetNextLine();
			_stscanf_s(m_Paser.m_pBuffer, _T("%s%d%d"),
						String, MAX_PATH,
						&pSubMaterial->iIndex,
						&iTextureSize);
			pSubMaterial->m_Name = String;
			ZeroMemory(String, _countof(String));

			if (iTextureSize <= 0) continue;

			STextureList& pTextureList = pSubMaterial->m_TextureList;
			// Texture Map
			for (int iTextureMap = 0; iTextureMap < iTextureSize; iTextureMap++)
			{
				int iTextureType = -1;
				m_Paser.GetNextLine();
				_stscanf_s(m_Paser.m_pBuffer, _T("%d%s"),
					&iTextureType,
					String, MAX_PATH);
				
				T_STR FilePath = L"../../data/Obj/";
				FilePath += String;

				int iTextureNumber;
				iTextureNumber = I_TextureManager.Load(m_pDevice, FilePath);
				pTextureList.ConvertToMax(I_TextureManager.GetPtr(iTextureNumber), iTextureType);
				ZeroMemory(String, _countof(String));
			}
		}
	}
	
	bCheck = m_Paser.GetDataFromFileNext(Object);
	if (!bCheck) return -1;
	for (int iMesh = 0; iMesh < SceneData.iNumObjects; iMesh++)
	{
		m_Paser.GetNextLine();
		TCHAR ParentName[MAX_PATH];
		ZeroMemory(ParentName, _countof(ParentName));
		int iSubMeshSize = 0;

		SDrawObject* pDrawObject = I_DrawObjectManager.Create();
		_stscanf_s(m_Paser.m_pBuffer, _T("\n%s %s %d %d %d"),
			String, MAX_PATH,
			ParentName, MAX_PATH,
			&pDrawObject->m_iClassType,
			&pDrawObject->m_iMaterialID,
			&iSubMeshSize);
		pDrawObject->m_ObjectName = String;
		pDrawObject->m_ParentName = ParentName;
		pDrawObject->m_iMaterialID = I_MaterialManager.GetSize() - SceneData.iNumMaterials + pDrawObject->m_iMaterialID;
		pDrawObject->m_Scene = SceneData;
		m_Paser.GetNextLine();
		_stscanf_s(m_Paser.m_pBuffer, _T("%f %f %f %f %f"),
			&pDrawObject->m_matAnim._11,
			&pDrawObject->m_matAnim._12,
			&pDrawObject->m_matAnim._13,
			&pDrawObject->m_matAnim._14);

		m_Paser.GetNextLine();
		_stscanf_s(m_Paser.m_pBuffer, _T("%f %f %f %f %f"),
			&pDrawObject->m_matAnim._21,
			&pDrawObject->m_matAnim._22,
			&pDrawObject->m_matAnim._23,
			&pDrawObject->m_matAnim._24);

		m_Paser.GetNextLine();
		_stscanf_s(m_Paser.m_pBuffer, _T("%f %f %f %f %f"),
			&pDrawObject->m_matAnim._31,
			&pDrawObject->m_matAnim._32,
			&pDrawObject->m_matAnim._33,
			&pDrawObject->m_matAnim._34);

		m_Paser.GetNextLine();
		_stscanf_s(m_Paser.m_pBuffer, _T("%f %f %f %f %f"),
			&pDrawObject->m_matAnim._41,
			&pDrawObject->m_matAnim._42,
			&pDrawObject->m_matAnim._43,
			&pDrawObject->m_matAnim._44);

		for (int iSubMesh = 0; iSubMesh < iSubMeshSize; iSubMesh++)
		{
			int iVertexSize;
			int iIndexSize;
			int iSumMaterialNum;
			SMesh& pMesh = *pDrawObject->CreateMesh();

			bCheck = m_Paser.GetDataFromFileNext(SubMesh);
			if (!bCheck) return -1;

			_stscanf_s(m_Paser.m_pBuffer, _T("%s %d %d %d"),
				String, MAX_PATH,
				&iSumMaterialNum,
				&iVertexSize,
				&iIndexSize);
			ZeroMemory(String, _countof(String));

			pMesh._VertexList.resize(iVertexSize);
			pMesh._IndexList.resize(iIndexSize);
			pMesh.iSubMtrlIndex = iSumMaterialNum;
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
			for (int iIndex = 0; iIndex < iIndexSize; iIndex+=3)
			{
				m_Paser.GetNextLine();
				_stscanf_s(m_Paser.m_pBuffer, _T("%d %d %d"),
					&pMesh._IndexList[iIndex + 0],
					&pMesh._IndexList[iIndex + 1],
					&pMesh._IndexList[iIndex + 2]);
			}

		}

		int iMaxScaleNum = 0;
		int iMaxRotationNum = 0;
		int iMaxTransposeNum = 0;
		bCheck = m_Paser.GetDataFromFile(Animation);
		if (bCheck == false) return -1;


		_stscanf_s(m_Paser.m_pBuffer, _T("%s%d%d%d"), String, MAX_PATH,
			&iMaxTransposeNum,
			&iMaxRotationNum,
			&iMaxScaleNum);
		ZeroMemory(String, _countof(String));
		pDrawObject->m_PosAnimList.resize(iMaxTransposeNum);
		pDrawObject->m_RotAnimList.resize(iMaxRotationNum);
		pDrawObject->m_ScaleAnimList.resize(iMaxScaleNum);
		int iCountTrack;
		for (int iTranspose = 0; iTranspose < iMaxTransposeNum; iTranspose++)
		{
			m_Paser.GetNextLine();
			_stscanf_s(m_Paser.m_pBuffer, _T("\n%d %d %f %f %f"),
				&iCountTrack,
				&pDrawObject->m_PosAnimList[iTranspose].iTick,
				&pDrawObject->m_PosAnimList[iTranspose].Position.x,
				&pDrawObject->m_PosAnimList[iTranspose].Position.y,
				&pDrawObject->m_PosAnimList[iTranspose].Position.z);
		}
		for (int iRotation = 0; iRotation < iMaxRotationNum; iRotation++)
		{
			m_Paser.GetNextLine();
			_stscanf_s(m_Paser.m_pBuffer, _T("\n%d %d %f %f %f %f"),
				&iCountTrack,
				&pDrawObject->m_RotAnimList[iRotation].iTick,
				&pDrawObject->m_RotAnimList[iRotation].Quaternion.x,
				&pDrawObject->m_RotAnimList[iRotation].Quaternion.y,
				&pDrawObject->m_RotAnimList[iRotation].Quaternion.z,
				&pDrawObject->m_RotAnimList[iRotation].Quaternion.w);
		}
		for (int iScale = 0; iScale < iMaxScaleNum; iScale++)
		{
			m_Paser.GetNextLine();
			_stscanf_s(m_Paser.m_pBuffer, _T("\n%d %d %f %f %f %f %f %f %f"),
				&iCountTrack,
				&pDrawObject->m_ScaleAnimList[iScale].iTick,
				&pDrawObject->m_ScaleAnimList[iScale].Position.x,
				&pDrawObject->m_ScaleAnimList[iScale].Position.y,
				&pDrawObject->m_ScaleAnimList[iScale].Position.z,

				&pDrawObject->m_ScaleAnimList[iScale].Quaternion.x,
				&pDrawObject->m_ScaleAnimList[iScale].Quaternion.y,
				&pDrawObject->m_ScaleAnimList[iScale].Quaternion.z,
				&pDrawObject->m_ScaleAnimList[iScale].Quaternion.w);
		}
		pDrawObject->Init();
		pDrawObject->Create(m_pDevice, m_pContext, L"Test.hlsl");
	}
	m_Paser.CloseStream();
	
	return true;
}
SSSFileLoader::SSSFileLoader()
{

}


SSSFileLoader::~SSSFileLoader()
{
}
