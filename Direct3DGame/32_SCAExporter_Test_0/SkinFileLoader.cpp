#include "SkinFileLoader.h"

static TCHAR SkinExporter[] = L"SkinExporter100";
static TCHAR Header[] = L"#HEADERINFO";
static TCHAR Material[] = L"#MATERIAL_INFO";
static TCHAR Object[] = L"#OBJECT_INFO";
static TCHAR SubMesh[] = L"SubMesh";

bool SkinFileLoader::Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;

	return true;
}
bool SkinFileLoader::Load(T_STR FilePath)
{
	if (m_pDevice == nullptr) return -1;
	if (m_pContext == nullptr) return -1;
	SScene	SceneData;
	bool bCheck = false;

	static TCHAR String[MAX_PATH];
	ZeroMemory(String, _countof(String));
	ZeroMemory(&SceneData, sizeof(SceneData));

	m_Paser.OpenStream(FilePath.c_str());

	bCheck = m_Paser.GetDataFromFileNext(SkinExporter);
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

				T_STR FilePath = L"../../data/Character/";
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

		SSkinObject* pSkinObject = I_SkinObjectManager.Create();
		_stscanf_s(m_Paser.m_pBuffer, _T("\n%s %s %d %d"),
			String, MAX_PATH,
			ParentName, MAX_PATH,
			&pSkinObject->m_iMaterialID,
			&iSubMeshSize);
		pSkinObject->m_ObjectName = String;
		pSkinObject->m_iMaterialID = I_MaterialManager.GetSize() - SceneData.iNumMaterials + pSkinObject->m_iMaterialID;
		pSkinObject->m_Scene = SceneData;
		m_Paser.GetNextLine();
		_stscanf_s(m_Paser.m_pBuffer, _T("%f %f %f %f %f"),
			&pSkinObject->m_matObject._11,
			&pSkinObject->m_matObject._12,
			&pSkinObject->m_matObject._13,
			&pSkinObject->m_matObject._14);

		m_Paser.GetNextLine();
		_stscanf_s(m_Paser.m_pBuffer, _T("%f %f %f %f %f"),
			&pSkinObject->m_matObject._21,
			&pSkinObject->m_matObject._22,
			&pSkinObject->m_matObject._23,
			&pSkinObject->m_matObject._24);

		m_Paser.GetNextLine();
		_stscanf_s(m_Paser.m_pBuffer, _T("%f %f %f %f %f"),
			&pSkinObject->m_matObject._31,
			&pSkinObject->m_matObject._32,
			&pSkinObject->m_matObject._33,
			&pSkinObject->m_matObject._34);

		m_Paser.GetNextLine();
		_stscanf_s(m_Paser.m_pBuffer, _T("%f %f %f %f %f"),
			&pSkinObject->m_matObject._41,
			&pSkinObject->m_matObject._42,
			&pSkinObject->m_matObject._43,
			&pSkinObject->m_matObject._44);

		for (int iSubMesh = 0; iSubMesh < iSubMeshSize; iSubMesh++)
		{
			int iVertexSize;
			int iIndexSize;
			int iSumMaterialNum;
			SSkinMesh& pSkinMesh = *pSkinObject->CreateMesh();

			bCheck = m_Paser.GetDataFromFileNext(SubMesh);
			if (!bCheck) continue;

			_stscanf_s(m_Paser.m_pBuffer, _T("%s %d %d %d"),
				String, MAX_PATH,
				&iSumMaterialNum,
				&iVertexSize,
				&iIndexSize);
			ZeroMemory(String, _countof(String));

			pSkinMesh._VertexList.resize(iVertexSize);
			pSkinMesh.m_IW_VertexList.resize(iVertexSize);
			pSkinMesh._IndexList.resize(iIndexSize);
			pSkinMesh.iSubMtrlIndex = iSumMaterialNum;
			for (int iVertex = 0; iVertex < iVertexSize; iVertex++)
			{
				m_Paser.GetNextLine();
				_stscanf_s(m_Paser.m_pBuffer, _T("%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f"),
					&pSkinMesh._VertexList[iVertex].p.x,
					&pSkinMesh._VertexList[iVertex].p.y,
					&pSkinMesh._VertexList[iVertex].p.z,
					&pSkinMesh._VertexList[iVertex].n.x,
					&pSkinMesh._VertexList[iVertex].n.y,
					&pSkinMesh._VertexList[iVertex].n.z,
					&pSkinMesh._VertexList[iVertex].c.x,
					&pSkinMesh._VertexList[iVertex].c.y,
					&pSkinMesh._VertexList[iVertex].c.z,
					&pSkinMesh._VertexList[iVertex].c.w,
					&pSkinMesh._VertexList[iVertex].t.x,
					&pSkinMesh._VertexList[iVertex].t.y,

					&pSkinMesh.m_IW_VertexList[iVertex].I1.x,
					&pSkinMesh.m_IW_VertexList[iVertex].I1.y,
					&pSkinMesh.m_IW_VertexList[iVertex].I1.z,
					&pSkinMesh.m_IW_VertexList[iVertex].I1.w,
					&pSkinMesh.m_IW_VertexList[iVertex].I2.x,
					&pSkinMesh.m_IW_VertexList[iVertex].I2.y,
					&pSkinMesh.m_IW_VertexList[iVertex].I2.z,
					&pSkinMesh.m_IW_VertexList[iVertex].I2.w,
					&pSkinMesh.m_IW_VertexList[iVertex].W1.x,
					&pSkinMesh.m_IW_VertexList[iVertex].W1.y,
					&pSkinMesh.m_IW_VertexList[iVertex].W1.z,
					&pSkinMesh.m_IW_VertexList[iVertex].W1.w,
					&pSkinMesh.m_IW_VertexList[iVertex].W2.x,
					&pSkinMesh.m_IW_VertexList[iVertex].W2.y,
					&pSkinMesh.m_IW_VertexList[iVertex].W2.z,
					&pSkinMesh.m_IW_VertexList[iVertex].W2.w);
			}
			;
			for (int iIndex = 0; iIndex < iIndexSize; iIndex += 3)
			{
				m_Paser.GetNextLine();
				_stscanf_s(m_Paser.m_pBuffer, _T("%d %d %d"),
					&pSkinMesh._IndexList[iIndex + 0],
					&pSkinMesh._IndexList[iIndex + 1],
					&pSkinMesh._IndexList[iIndex + 2]);
			}

		}
		pSkinObject->Init();
		pSkinObject->Create(m_pDevice, m_pContext, L"Test.hlsl");
	}
	m_Paser.CloseStream();

	return true;
}

SkinFileLoader::SkinFileLoader()
{
}


SkinFileLoader::~SkinFileLoader()
{
}
