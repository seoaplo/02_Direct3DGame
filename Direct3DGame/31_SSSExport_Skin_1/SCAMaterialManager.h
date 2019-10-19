#pragma once
#include "SCAGlobal.h"
class SCAMaterialManager
{
	friend class SCAWriter;
	friend class SCASkinExp;
private:
	std::vector<SCAMaterial>	m_MaterialList;
public:
	int		AddMaterial(INode* pNode);
	void	GetTexture(Mtl* pMtl, SCASubMaterial& SubMtrl);
	int		FindMaterial(INode* pNode);
	bool	ExportMaterial(FILE* pStream);
private:
	SCAMaterialManager();
	~SCAMaterialManager();
};

