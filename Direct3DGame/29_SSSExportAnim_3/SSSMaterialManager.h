#pragma once
#include "SSSGlobal.h"
class SSSMaterialManager
{
	friend class SSSWriter;
private:
	std::vector<SMaterial>	m_MaterialList;
public:
	int	AddMaterial(INode* pNode);
	void	GetTexture(Mtl* pMtl, SubMaterial& SubMtrl);
	int		FindMaterial(INode* pNode);
	bool	ExportMaterial(FILE* pStream);
private:
	SSSMaterialManager();
	~SSSMaterialManager();
};

