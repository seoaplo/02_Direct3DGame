#pragma once
#include "SAGlobal.h"

class SSkinManager
{

public:
	std::vector<SCAObject>				m_ObjectList;
	std::vector<SCATriangleList>		m_TriLists;
	std::vector<SCABipedVertex>			m_bipedList;
public:
	void	AddObject(INode* pNode, SAScene& Scene, Interval& interval, int iObjectNum, int iMaterialID, NodeList& MatrixNodeList);
	void	GetMesh(INode* pNode, SCAMesh& sMesh, Interval& interval, NodeList& MatrixNodeList);
	void	SetUniqueBuffer(SCAMesh& tMesh);
	int		IsEqulVerteList(PNCTIW_VERTEX& vertex, std::vector<PNCTIW_VERTEX>& vList, int iVertexMax);
	Point3	GetVertexNormal(Mesh* mesh, int iFace, RVertex* rVertex);
	TriObject* GetTriObjectFromNode(INode* pNode, TimeValue time, bool& deleteit);
	
	void SetBippedInfo(INode* pNode, SCAMesh&, NodeList&);
	Modifier*  FindModifier(INode* pNode, Class_ID classID);
	void PhysiqueData(INode* pNode, Modifier*, SCAMesh&, NodeList&);
	void SkinData(INode* pNode, Modifier*, SCAMesh&, NodeList&);

	bool ExportObject(FILE* pStream);
	bool ExportMesh(FILE* pStream, SCAMesh& sMesh);

	void	SetSize(int iSize) { m_ObjectList.resize(iSize); }
	void	Release();
public:
	SSkinManager();
	~SSkinManager();
};

