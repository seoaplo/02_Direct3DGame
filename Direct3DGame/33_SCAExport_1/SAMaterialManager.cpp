#include "SAMaterialManager.h"


int	SAMaterialManager::AddMaterial(INode* pNode)
{
	int iMaterial = FindMaterial(pNode);

	if (iMaterial > -1) return iMaterial;

	Mtl* pMtl = pNode->GetMtl();
	if (pMtl == nullptr) return -1;

	m_MaterialList.push_back(SAMaterial());
	SAMaterial& Material = m_MaterialList[m_MaterialList.size() - 1];
	Material.pMaterial = pMtl;
	Material.iMaterialID = m_MaterialList.size() - 1;
	Material.szName = SAGlobal::FixupName(pMtl->GetName());	// material 이름에 오류가 날 문자를 없애거나 바꿔야 한다.
	// sub-material
	Material.SubMaterialNum = pMtl->NumSubMtls();
	if (Material.SubMaterialNum > 0)
	{
		Material.SubMaterialList.resize(Material.SubMaterialNum);
		for (int iSub = 0; iSub < Material.SubMaterialNum; iSub++)
		{
			Mtl* pSubMtl = pMtl->GetSubMtl(iSub);
			if (pSubMtl == nullptr) continue;

			Material.SubMaterialList[iSub].szName = SAGlobal::FixupName(pSubMtl->GetName());
			GetTexture(pSubMtl, Material.SubMaterialList[iSub]);
		}
	}
	else
	{
		Material.SubMaterialNum = 1;
		Material.SubMaterialList.resize(Material.SubMaterialNum);
		Material.SubMaterialList[0].szName = Material.szName;
		GetTexture(pMtl, Material.SubMaterialList[0]);
	}
	return Material.iMaterialID;

}
void SAMaterialManager::GetTexture(Mtl* pMtl, SASubMaterial& SubMtrl)
{
	int iNumMap = pMtl->NumSubTexmaps();
	for (int iSubMap = 0; iSubMap < iNumMap; iSubMap++)
	{
		Texmap* tex = pMtl->GetSubTexmap(iSubMap);
		if (tex)
		{
			if (tex->ClassID() == Class_ID(BMTEX_CLASS_ID, 0X00))
			{
				SATextureMap Texmap;
				SubMtrl.TextureMapList[iSubMap].bUse = true;

				TSTR fullName;
				TSTR mapName = ((BitmapTex*)(tex))->GetMapName();
				SplitPathFile(mapName, &fullName, &SubMtrl.TextureMapList[iSubMap].szName);

				SubMtrl.iTextureNumSize++;
			}
		}
	}
}
int		SAMaterialManager::FindMaterial(INode* pNode)
{
	Mtl* pMtl = pNode->GetMtl();
	if (pMtl)
	{
		for (int iMtl = 0; iMtl < m_MaterialList.size(); iMtl++)
		{
			if (m_MaterialList[iMtl].pMaterial == pMtl)
			{
				return iMtl;
			}
		}
	}
	return -1;
}
bool	SAMaterialManager::ExportMaterial(FILE* pStream)
{
	if (pStream == nullptr)
	{
		return false;
	}

	_ftprintf(pStream, _T("\n%s"), L"#MATERIAL_INFO");
	for (int iMtl = 0; iMtl < m_MaterialList.size(); iMtl++)
	{
		_ftprintf(pStream, _T("\n%s %d"),
			m_MaterialList[iMtl].szName,
			m_MaterialList[iMtl].SubMaterialNum);

		for (int iSubMtrl = 0; iSubMtrl < m_MaterialList[iMtl].SubMaterialNum; iSubMtrl++)
		{
			_ftprintf(pStream, _T("\n%s %d %d"),
				m_MaterialList[iMtl].SubMaterialList[iSubMtrl].szName,
				iSubMtrl,
				m_MaterialList[iMtl].SubMaterialList[iSubMtrl].iTextureNumSize
			);

			for (int iTex = 0; iTex < TexType_Number; iTex++)
			{
				if (m_MaterialList[iMtl].SubMaterialList[iSubMtrl].TextureMapList[iTex].bUse == false) continue;
				_ftprintf(pStream, _T("\n%d %s"),
					iTex,
					m_MaterialList[iMtl].SubMaterialList[iSubMtrl].TextureMapList[iTex].szName);
			}
		}
	}
	return true;
}
void SAMaterialManager::Release()
{
	m_MaterialList.clear();
}
SAMaterialManager::SAMaterialManager()
{
}


SAMaterialManager::~SAMaterialManager()
{
}
