#pragma once
#include "SSSMaterialManager.h"

class SSSObjectManager
{
	friend class SSSWriter;
private:
	std::vector<SObject>				m_ObjectList;
	std::vector<STriangleList>			m_TriLists;
private:
	void	AddObject(INode* pNode, SScene& Scene, Interval& interval, int iMaterialID);
	void	GetMesh(INode* pNode, SMesh& sMesh, Interval& interval);
	
	void	SetUniqueBuffer(SMesh& tMesh);
	int		IsEqulVerteList(PNCT& vertex, std::vector<PNCT>& vList, int iVertexMax);
	Point3	GetVertexNormal(Mesh* mesh, int iFace, RVertex* rVertex);
	TriObject* GetTriObjectFromNode(INode* pNode, TimeValue time, bool& deleteit);

	void GetAnimation(INode* pNode, SAnimationTrack& AnimTrack, SScene& Scene, Interval& interval);

	bool ExportObject(FILE* pStream);
	bool ExportMesh(FILE* pStream, SMesh& sMesh);
	bool ExportAnimation(FILE* pStream, SAnimationTrack& AnimTrack);

private:
	SSSObjectManager();
	~SSSObjectManager();
};

