#pragma once
#include "SCAMaterialManager.h"

class SOAObjectManager
{
	friend class SCAWriter;
	friend class SCASkinExp;
private:
	std::vector<SOAObject>				m_ObjectList;
	std::vector<SOATriangle>			m_TriList[SUBMATERIAL_SIZE];
public:
	virtual void	AddObject(INode* pNode, SCAScene& Scene, Interval& interval, int iMaterialID);
	virtual void	GetMesh(INode* pNode, SOAMesh& sMesh, Interval& interval);
	
	virtual void	SetUniqueBuffer(SOAMesh& tMesh);
	virtual int		IsEqulVerteList(PNCT& vertex, std::vector<PNCT>& vList);
	virtual Point3	GetVertexNormal(Mesh* mesh, int iFace, RVertex* rVertex);
	virtual TriObject* GetTriObjectFromNode(INode* pNode, TimeValue time, bool& deleteit);

	virtual void GetAnimation(INode* pNode, SCAAnimationTrack& AnimTrack, SCAScene& Scene, Interval& interval);

	SOAObject* FindObject(int iIndex)
	{
		for (auto& Object : m_ObjectList)
		{
			if (Object.iIndex == iIndex)
			{
				return &Object;
			}
		}
		return nullptr;
	}
	SOAObject* FindObject(TSTR name)
	{
		for (auto& Object : m_ObjectList)
		{
			if (Object.name == name)
			{
				return &Object;
			}
		}
		return nullptr;
	}
	SOAObject* FindObject(INode* pNode)
	{
		TSTR str = pNode->GetName();
		for (auto& Object : m_ObjectList)
		{
			if (Object.pINode == pNode)
			{
				return &Object;
			}
		}
		return nullptr;
	}

	virtual bool ExportObject(FILE* pStream);
	virtual bool ExportMesh(FILE* pStream, SOAMesh& sMesh);
	virtual bool ExportAnimation(FILE* pStream, SCAAnimationTrack& AnimTrack);

	virtual void Clear()
	{
		m_ObjectList.clear();
		for (auto Triangle : m_TriList)
		{
			Triangle.clear();
		}
	}
protected:
	SOAObjectManager();
	virtual ~SOAObjectManager();
};

