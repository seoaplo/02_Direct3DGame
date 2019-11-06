#pragma once
#include "SAGlobal.h"
#include "SAMaterialManager.h"
class SOAManager
{
public:
	std::vector<SOAObject>				m_ObjectList;
	std::vector<SOATriangleList>		m_TriLists;
public:
	void	AddObject(INode* pNode, SAScene& Scene, Interval& interval, int iObjectNum, int iMaterialID);
	void	GetMesh(INode* pNode, SOAMesh& sMesh, Interval& interval);
	void	SetUniqueBuffer(SOAMesh& sMesh);
	int		IsEqulVerteList(PNCT& vertex, std::vector<PNCT>& vList, int iVertexMax);
	Point3	GetVertexNormal(Mesh* mesh, int iFace, RVertex* rVertex);
	TriObject* GetTriObjectFromNode(INode* pNode, TimeValue time, bool& deleteit);

	void GetAnimation(INode* pNode, SAAnimationTrack& AnimTrack, SAScene& Scene, Interval& interval);

	bool ExportObject(FILE* pStream);
	bool ExportMesh(FILE* pStream, SOAMesh& sMesh);
	bool ExportAnimation(FILE* pStream, SAAnimationTrack& AnimTrack);

	void	SetSize(int iSize) { m_ObjectList.resize(iSize); }
	void	Release();
public:
	SOAManager();
	virtual ~SOAManager();
};

