#pragma once
#include "SCAMaterialManager.h"

class SCAObjectManager
{
	friend class SCAWriter;
	friend class SCASkinExp;
private:
	std::vector<SCAObject>				m_ObjectList;
	std::vector<SCATriangle>				m_TriList[SUBMATERIAL_SIZE];
private:
	void	AddObject(INode* pNode, SCAScene& Scene, Interval& interval, int iMaterialID);
	void	GetMesh(INode* pNode, SCAMesh& sMesh, Interval& interval);
	void	GetBox(SCAMesh& sMesh);
	
	void	SetUniqueBuffer(SCAMesh& tMesh);
	int		IsEqulVerteList(PNCT& vertex, std::vector<PNCT>& vList);
	Point3	GetVertexNormal(Mesh* mesh, int iFace, RVertex* rVertex);
	TriObject* GetTriObjectFromNode(INode* pNode, TimeValue time, bool& deleteit);

	void GetAnimation(INode* pNode, SCAAnimationTrack& AnimTrack, SCAScene& Scene, Interval& interval);

	bool ExportObject(FILE* pStream);
	bool ExportMesh(FILE* pStream, SCAMesh& sMesh);
	bool ExportAnimation(FILE* pStream, SCAAnimationTrack& AnimTrack);

private:
	SCAObjectManager();
	~SCAObjectManager();
};

