#pragma once
#include "SSSGlobal.h"
class SSSMaterialManager
{
private:
	std::vector<SMaterial>	m_MaterialList;
public:
	void	AddMaterial(INode* pNode);
	void	GetTexture(Mtl* pMtl, SubMaterial& SubMtrl);
	int		FindMaterial(INode* pNode);
	bool	ExportMaterial(FILE* pStream);
private:
	SSSMaterialManager();
	~SSSMaterialManager();
};

