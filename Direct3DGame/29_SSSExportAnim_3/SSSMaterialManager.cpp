#include "SSSMaterialManager.h"


void	SSSMaterialManager::AddMaterial(INode* pNode)
{
	
	if (FindMaterial(pNode) > -1)
	{
		return;
	}

	Mtl* pMtl = pNode->GetMtl();

	m_MaterialList.push_back(SMaterial());
	SMaterial& Material = m_MaterialList[m_MaterialList.size() - 1];
	Material.iMaterialID = m_MaterialList.size() - 1;

	Material.szName = SSSGlobal::FixupName(pMtl->GetName());	// material 이름에 오류가 날 문자를 없애거나 바꿔야 한다.
	// sub-material
	int iNumSub = pMtl->NumSubMtls();
	if (iNumSub > 0)
	{
		for (int iSub = 0; iSub < iNumSub; iSub++)
		{
			Mtl* pSubMtl = pMtl->GetSubMtl(iSub);
			Material.SubMaterialList[iNumSub].szName = SSSGlobal::FixupName(pSubMtl->GetName());
			GetTexture(pSubMtl, Material.SubMaterialList[iNumSub]);
		}
	}
	else
	{
		Material.SubMaterialList[iNumSub].szName = Material.szName;
		GetTexture(pMtl, Material.SubMaterialList[iNumSub]);
	}

}
void SSSMaterialManager::GetTexture(Mtl* pMtl, SubMaterial& SubMtrl)
{
	int iNumMap = pMtl->NumSubTexmaps();
	for (int iSubMap = 0; iSubMap < iNumMap; iSubMap++)
	{
		Texmap* tex = pMtl->GetSubTexmap(iSubMap);
		if (tex)
		{
			if (tex->ClassID() == Class_ID(BMTEX_CLASS_ID, 0X00))
			{
				STextureMap Texmap;
				SubMtrl.TextureMapList[iSubMap].iMapType = iSubMap;

				TSTR fullName;
				TSTR mapName = ((BitmapTex*)(tex))->GetMapName();
				SplitPathFile(mapName, &fullName, &SubMtrl.TextureMapList[iSubMap].szName);
			}
		}
	}
}
int		SSSMaterialManager::FindMaterial(INode* pNode)
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
bool	SSSMaterialManager::ExportMaterial(FILE* pStream)
{
	if (pStream == nullptr)
	{
		return false;
	}
	return true;
}
SSSMaterialManager::SSSMaterialManager()
{
}


SSSMaterialManager::~SSSMaterialManager()
{
}
