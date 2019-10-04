#include "SSSFileLoader.h"

bool SSSFileLoader::Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;
	m_iMaterialSize = 0;
	m_iMeshSize = 0;

	return true; 
}
int SSSFileLoader::Load(T_STR FilePath)
{
	SScene	SceneData;
	bool bCheck = false;
	static TCHAR SSSExporter[] = L"SSSExporter100";
	static TCHAR Header[] = L"#HEADERINFO";
	static TCHAR Material[] = L"#MATERIAL_INFO";
	static TCHAR Object[] = L"#OBJECT_INFO";
	static TCHAR Vertex[] = L"VertexList";
	static TCHAR Index[] = L"IndexList";
	static TCHAR Animation[] = L"#AnimationData";
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

	for (int iMaterial = 0; iMaterial < SceneData.iNumMaterials; iMaterial++)
	{
		m_MaterialList.push_back(SMaterial());
		m_MaterialList[m_iMaterialSize].iIndex = m_iMaterialSize;
		m_iMaterialSize++;
	}
	for (int iMesh = 0; iMesh < SceneData.iNumObjects; iMesh++)
	{
		m_MeshList.push_back(SMesh());
		m_MeshList[m_iMeshSize].iIndex = m_iMaterialSize;
		m_iMaterialSize++;
	}

	bCheck = m_Paser.GetDataFromFileNext(Material);
	if (bCheck == false) return -1;
	// Material
	for (int iMaterial = m_MaterialList.size() - SceneData.iNumMaterials; iMaterial < m_MaterialList.size(); iMaterial++)
	{
		m_Paser.GetNextLine();
		_stscanf_s(m_Paser.m_pBuffer, _T("%s%d"), String, MAX_PATH, &m_MaterialList[iMaterial].iSubMaterialNum, &SceneData.iNumObjects);
		m_MaterialList[iMaterial].MaterialName = String;
		ZeroMemory(String, _countof(String));
		
		if (m_MaterialList[iMaterial].iSubMaterialNum <= 0)
		{
			m_MaterialList[iMaterial].iSubMaterialNum = 1;
		}

		// SubMaterial
		m_MaterialList[iMaterial].SubMaterial.resize(m_MaterialList[iMaterial].iSubMaterialNum);
		for (int iSubMaterial = 0; iSubMaterial < m_MaterialList[iMaterial].iSubMaterialNum; iSubMaterial++)
		{
			m_Paser.GetNextLine();
			_stscanf_s(m_Paser.m_pBuffer, _T("%s%d"),
						String, MAX_PATH,
						&m_MaterialList[iMaterial].SubMaterial[iSubMaterial].iTextureMapNum);
			m_MaterialList[iMaterial].SubMaterial[iSubMaterial].MaterialName = String;
			m_MaterialList[iMaterial].SubMaterial[iSubMaterial].iIndex = iSubMaterial;
			ZeroMemory(String, _countof(String));

			int iMaxTextureMap = m_MaterialList[iMaterial].SubMaterial[iSubMaterial].iTextureMapNum;
			
			if (iMaxTextureMap <= 0) continue;
			m_MaterialList[iMaterial].SubMaterial[iSubMaterial].TextrueMapList.resize(iMaxTextureMap);

			// Texture Map
			for (int iTextureMap = 0; iTextureMap < iMaxTextureMap; iTextureMap++)
			{
				m_Paser.GetNextLine();
				_stscanf_s(m_Paser.m_pBuffer, _T("%d%s"),
					&m_MaterialList[iMaterial].SubMaterial[iSubMaterial].TextrueMapList[iTextureMap].iMapID,
					String, MAX_PATH);
				m_MaterialList[iMaterial].SubMaterial[iSubMaterial].TextrueMapList[iTextureMap].TextureName = String;
				T_STR FilePath = L"../../data/Obj/";
				FilePath += String;

				int iTextureNumber;
				iTextureNumber = I_TextureManager.Load(m_pDevice, FilePath);
				m_MaterialList[iMaterial].SubMaterial[iSubMaterial].TextrueMapList[iTextureMap].STexture 
					= I_TextureManager.GetPtr(iTextureNumber);
				m_MaterialList[iMaterial].SubMaterial[iSubMaterial].TextrueMapList[iTextureMap].iIndexMap = iTextureNumber;
				ZeroMemory(String, _countof(String));
			}
		}
	}
	
	bCheck = m_Paser.GetDataFromFileNext(Object);
	if (!bCheck) return -1;
	for (int iMesh = m_MeshList.size() - SceneData.iNumObjects; iMesh < m_MeshList.size(); iMesh++)
	{
		m_Paser.GetNextLine();
		TCHAR ParentName[MAX_PATH];
		ZeroMemory(ParentName, _countof(ParentName));

		_stscanf_s(m_Paser.m_pBuffer, _T("\n%s %s %d %d %d"),
			String, MAX_PATH,
			ParentName, MAX_PATH,
			&m_MeshList[iMesh].iMtrlID,
			&m_MeshList[iMesh].iSubMeshNum,
			&m_MeshList[iMesh].iTriangleNum);
		m_MeshList[iMesh].name = String;
		m_MeshList[iMesh].ParentName = ParentName;
		m_MeshList[iMesh].iMtrlID = m_MaterialList.size() - SceneData.iNumMaterials + m_MeshList[iMesh].iMtrlID;
		m_MeshList[iMesh].m_Scene = SceneData;
		m_MeshList[iMesh].iIndex = iMesh;
		m_Paser.GetNextLine();
		_stscanf_s(m_Paser.m_pBuffer, _T("%f %f %f %f %f"),
			&m_MeshList[iMesh].m_matWorld._11,
			&m_MeshList[iMesh].m_matWorld._12,
			&m_MeshList[iMesh].m_matWorld._13,
			&m_MeshList[iMesh].m_matWorld._14);

		m_Paser.GetNextLine();
		_stscanf_s(m_Paser.m_pBuffer, _T("%f %f %f %f %f"),
			&m_MeshList[iMesh].m_matWorld._21,
			&m_MeshList[iMesh].m_matWorld._22,
			&m_MeshList[iMesh].m_matWorld._23,
			&m_MeshList[iMesh].m_matWorld._24);

		m_Paser.GetNextLine();
		_stscanf_s(m_Paser.m_pBuffer, _T("%f %f %f %f %f"),
			&m_MeshList[iMesh].m_matWorld._31,
			&m_MeshList[iMesh].m_matWorld._32,
			&m_MeshList[iMesh].m_matWorld._33,
			&m_MeshList[iMesh].m_matWorld._34);

		m_Paser.GetNextLine();
		_stscanf_s(m_Paser.m_pBuffer, _T("%f %f %f %f %f"),
			&m_MeshList[iMesh].m_matWorld._41,
			&m_MeshList[iMesh].m_matWorld._42,
			&m_MeshList[iMesh].m_matWorld._43,
			&m_MeshList[iMesh].m_matWorld._44);


		m_MeshList[iMesh].m_VertexList.resize(m_MeshList[iMesh].iSubMeshNum);
		m_MeshList[iMesh].m_IndexList.resize(m_MeshList[iMesh].iSubMeshNum);
		m_MeshList[iMesh].m_dxobjList.resize(m_MeshList[iMesh].iSubMeshNum);

		for (int iSubMesh = 0; iSubMesh < m_MeshList[iMesh].iSubMeshNum; iSubMesh++)
		{
			int iVertexSize;

			bCheck = m_Paser.GetDataFromFileNext(Vertex, &iVertexSize, INT_DATA);
			if (!bCheck) return -1;

			m_MeshList[iMesh].m_VertexList[iSubMesh].resize(iVertexSize);
			for (int iVertex = 0; iVertex < iVertexSize; iVertex++)
			{
				m_Paser.GetNextLine();
				_stscanf_s(m_Paser.m_pBuffer, _T("%f %f %f %f %f %f %f %f %f %f %f %f"),
					&m_MeshList[iMesh].m_VertexList[iSubMesh][iVertex].p.x,
					&m_MeshList[iMesh].m_VertexList[iSubMesh][iVertex].p.y,
					&m_MeshList[iMesh].m_VertexList[iSubMesh][iVertex].p.z,
					&m_MeshList[iMesh].m_VertexList[iSubMesh][iVertex].n.x,
					&m_MeshList[iMesh].m_VertexList[iSubMesh][iVertex].n.y,
					&m_MeshList[iMesh].m_VertexList[iSubMesh][iVertex].n.z,
					&m_MeshList[iMesh].m_VertexList[iSubMesh][iVertex].c.x,
					&m_MeshList[iMesh].m_VertexList[iSubMesh][iVertex].c.y,
					&m_MeshList[iMesh].m_VertexList[iSubMesh][iVertex].c.z,
					&m_MeshList[iMesh].m_VertexList[iSubMesh][iVertex].c.w,
					&m_MeshList[iMesh].m_VertexList[iSubMesh][iVertex].t.x,
					&m_MeshList[iMesh].m_VertexList[iSubMesh][iVertex].t.y);
			}
			int iIndexSize;
			m_Paser.GetDataFromFileNext(Index, &iIndexSize, INT_DATA);
			m_MeshList[iMesh].m_IndexList[iSubMesh].resize(iIndexSize);
;
			for (int iIndex = 0; iIndex < iIndexSize; iIndex+=3)
			{
				m_Paser.GetNextLine();
				_stscanf_s(m_Paser.m_pBuffer, _T("%d %d %d"),
					&m_MeshList[iMesh].m_IndexList[iSubMesh][iIndex + 0],
					&m_MeshList[iMesh].m_IndexList[iSubMesh][iIndex + 1],
					&m_MeshList[iMesh].m_IndexList[iSubMesh][iIndex + 2]);
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
		m_MeshList[iMesh].m_PosAnimList.resize(iMaxTransposeNum);
		m_MeshList[iMesh].m_RotAnimList.resize(iMaxRotationNum);
		m_MeshList[iMesh].m_ScaleAnimList.resize(iMaxScaleNum);
		int iCountTrack;
		for (int iTranspose = 0; iTranspose < iMaxTransposeNum; iTranspose++)
		{
			m_Paser.GetNextLine();
			_stscanf_s(m_Paser.m_pBuffer, _T("\n%d %d %f %f %f"),
				&iCountTrack,
				&m_MeshList[iMesh].m_PosAnimList[iTranspose].i,
				&m_MeshList[iMesh].m_PosAnimList[iTranspose].p.x,
				&m_MeshList[iMesh].m_PosAnimList[iTranspose].p.y,
				&m_MeshList[iMesh].m_PosAnimList[iTranspose].p.z);
		}
		for (int iRotation = 0; iRotation < iMaxRotationNum; iRotation++)
		{
			m_Paser.GetNextLine();
			_stscanf_s(m_Paser.m_pBuffer, _T("\n%d %d %f %f %f %f"),
				&iCountTrack,
				&m_MeshList[iMesh].m_RotAnimList[iRotation].i,
				&m_MeshList[iMesh].m_RotAnimList[iRotation].q.x,
				&m_MeshList[iMesh].m_RotAnimList[iRotation].q.y,
				&m_MeshList[iMesh].m_RotAnimList[iRotation].q.z,
				&m_MeshList[iMesh].m_RotAnimList[iRotation].q.w);
		}
		for (int iScale = 0; iScale < iMaxScaleNum; iScale++)
		{
			m_Paser.GetNextLine();
			_stscanf_s(m_Paser.m_pBuffer, _T("\n%d %d %f %f %f %f %f %f %f"),
				&iCountTrack,
				&m_MeshList[iMesh].m_ScaleAnimList[iScale].i,
				&m_MeshList[iMesh].m_ScaleAnimList[iScale].p.x,
				&m_MeshList[iMesh].m_ScaleAnimList[iScale].p.y,
				&m_MeshList[iMesh].m_ScaleAnimList[iScale].p.z,

				&m_MeshList[iMesh].m_ScaleAnimList[iScale].q.x,
				&m_MeshList[iMesh].m_ScaleAnimList[iScale].q.y,
				&m_MeshList[iMesh].m_ScaleAnimList[iScale].q.z,
				&m_MeshList[iMesh].m_ScaleAnimList[iScale].q.w);
		}
	}
	m_Paser.CloseStream();

	for (int iObject = m_MeshList.size() - SceneData.iNumObjects; iObject < m_MeshList.size(); iObject++)
	{
		m_ObjectList.push_back(SObject());
		m_ObjectList[iObject].m_pMesh = &m_MeshList[iObject];

		D3DXMATRIX matParent, matInvParent;
		if (m_MeshList[iObject].ParentName == L"none")
		{
			D3DXMatrixIdentity(&matParent);
			m_MeshList[iObject].m_pParent = nullptr;
		}
		else
		{
			for (int iCount = m_MeshList.size() - SceneData.iNumObjects; iCount < m_MeshList.size(); iCount++)
			{
				if (m_MeshList[iObject].ParentName == m_MeshList[iCount].name)
				{
					matParent = m_MeshList[iCount].m_matWorld;
					break;
				}
			}
		}
		D3DXMatrixInverse(&matInvParent, NULL, &matParent);

		m_MeshList[iObject].m_matAnimSelf =
			m_MeshList[iObject].m_matWorld * matInvParent;
		D3DXMatrixInverse(
			&m_MeshList[iObject].m_matInvWorld, NULL,
			&m_MeshList[iObject].m_matWorld);

		D3DXMatrixDecompose(
			&m_MeshList[iObject].m_vScaleTrack,
			&m_MeshList[iObject].m_qRotTrack,
			&m_MeshList[iObject].m_vPosTrack,
			&m_MeshList[iObject].m_matAnimSelf);

		D3DXMatrixTranslation(&m_MeshList[iObject].m_matPos,
			m_MeshList[iObject].m_vPosTrack.x,
			m_MeshList[iObject].m_vPosTrack.y,
			m_MeshList[iObject].m_vPosTrack.z);
		D3DXMatrixScaling(&m_MeshList[iObject].m_matScl,
			m_MeshList[iObject].m_vScaleTrack.x,
			m_MeshList[iObject].m_vScaleTrack.y,
			m_MeshList[iObject].m_vScaleTrack.z);
		D3DXMatrixRotationQuaternion(
			&m_MeshList[iObject].m_matRot,
			&m_MeshList[iObject].m_qRotTrack);

		if (m_ObjectList[iObject].m_pMesh->iMtrlID < 0)
		{
			m_ObjectList[iObject].m_pMaterial = nullptr;
		}
		else
		{
			m_ObjectList[iObject].m_pMaterial = &m_MaterialList[m_ObjectList[iObject].m_pMesh->iMtrlID];
		}

		m_ObjectList[iObject].Create(m_pDevice, m_pContext, _T("Test.hlsl"));
	}
	
	return 0;
}
SObject* SSSFileLoader::GetPtr(int iIndex)
{
	return &m_ObjectList[iIndex];
}
SSSFileLoader::SSSFileLoader()
{

}


SSSFileLoader::~SSSFileLoader()
{
}
