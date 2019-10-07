#pragma once
#include "SSSMaterialManager.h"

class SSSObjectManager
{
public:
	std::vector<SObject>				m_ObjectList;
	std::vector<STriangle>				m_TriList[SUBMATERIAL_SIZE];
public:
	void	AddObject(INode* pNode, SScene& Scene, Interval& interval, int iMaterialID);
	void	GetMesh(INode* pNode, SMesh& sMesh, Interval& interval);
	void	GetBox(SMesh& sMesh);
	
	void	SetUniqueBuffer(SMesh& tMesh);
	int		IsEqulVerteList(PNCT& vertex, std::vector<PNCT>& vList);
	Point3	GetVertexNormal(Mesh* mesh, int iFace, RVertex* rVertex);
	TriObject* GetTriObjectFromNode(INode* pNode, TimeValue time, bool& deleteit);

	void GetAnimation(INode* pNode, SObject& sObject, SScene& Scene, Interval& interval);
	bool CheckForAnimation(INode* pNode, bool&  bPos, bool& bRot, bool& bScl);
	void DumpPosSample(INode* pNode, SObject& pMesh);
	void DumpRotSample(INode* pNode, SObject& pMesh);
	void DumpSclSample(INode* pNode, SObject& pMesh);

	bool ExportObject(FILE* pStream);

private:
	SSSObjectManager();
	~SSSObjectManager();
};

