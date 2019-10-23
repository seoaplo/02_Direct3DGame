#pragma once
#include "SAGlobal.h"
class SAMaterialManager
{
public:
	std::vector<SAMaterial>	m_MaterialList;
public:
	int		AddMaterial(INode* pNode);
	void	GetTexture(Mtl* pMtl, SASubMaterial& SubMtrl);
	int		FindMaterial(INode* pNode);
	bool	ExportMaterial(FILE* pStream);
	void	Release();
public:
	SAMaterialManager();
	~SAMaterialManager();
};