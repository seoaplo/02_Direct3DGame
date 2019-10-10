#include "SMaterial.h"
SSubMaterial* SMaterial::Create()
{
	m_SubMaterialList.push_back(SSubMaterial());
	return &m_SubMaterialList[m_SubMaterialList.size() - 1];
}
SMaterial*	SMaterialManager::Create()
{
	m_MaterialList.push_back(SMaterial());
	m_MaterialList[m_MaterialList.size() - 1].m_iIndex = m_MaterialList.size() - 1;
	return &m_MaterialList[m_MaterialList.size() - 1];
}
SMaterial*	SMaterialManager::GetMaterial(T_STR MaterialName)
{
	for (auto& TargetMaterial : m_MaterialList)
	{
		if (TargetMaterial.m_MaterialName == MaterialName)
		{
			return &TargetMaterial;
		}
	}
	return nullptr;
}
SMaterial*	SMaterialManager::GetMaterial(int	iIndex)
{
	if (iIndex >= m_MaterialList.size()) return nullptr;
	else return &m_MaterialList[iIndex];
}
int	 SMaterialManager::GetSize() 
{
	return m_MaterialList.size(); 
}

SMaterial*	SMaterialManager::SetAddMaterial(SMaterial& TargetMaterial)
{
	m_MaterialList.push_back(TargetMaterial);
	return &m_MaterialList[m_MaterialList.size() - 1];
}