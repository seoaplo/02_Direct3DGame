#pragma once
#include "SOAObjectManager.h"
#include "SCAWriter.h"

class SCAObjectManager : public SOAObjectManager
{
	friend class SCASkinExp;
private:
	std::vector<SCAObject>				m_CharacterList;
	std::vector<SCATriangle>			m_SCATriList[SUBMATERIAL_SIZE];
	std::vector<SCABipedVertex>			m_bipedList;
public:
	void	AddObject(INode* pNode, SCAScene& Scene, Interval& interval, int iMaterialID);
	void	GetMesh(INode* pNode, SCAMesh& sMesh, Interval& interval);

	void	SetUniqueBuffer(SCAMesh& sMesh);
	int		IsEqulVerteList(PNCTIW_VERTEX& vertex, std::vector<PNCTIW_VERTEX>& vList);
	//void GetAnimation(INode* pNode, SCAAnimationTrack& AnimTrack, SCAScene& Scene, Interval& interval);

	void SetBippedInfo(INode* pNode, SCAMesh&);
	Modifier*  FindModifier(
		INode* pNode, Class_ID classID);
	void PhysiqueData(INode* pNode, Modifier*, SCAMesh&);
	void SkinData(INode* pNode, Modifier*, SCAMesh&);

	bool ExportObject(FILE* pStream)override;
	bool ExportMesh(FILE* pStream, SCAMesh& sMesh);
	bool ExportAnimation(FILE* pStream, SCAAnimationTrack& AnimTrack)override;


	SCAObject* FindObject(int iIndex)
	{
		for (auto& Object : m_CharacterList)
		{
			if (Object.iIndex == iIndex)
			{
				return &Object;
			}
		}
		return nullptr;
	}
	SCAObject* FindObject(TSTR name)
	{
		for (auto& Object : m_CharacterList)
		{
			if (Object.name == name)
			{
				return &Object;
			}
		}
		return nullptr;
	}
	SCAObject* FindObject(INode* pNode)
	{
		for (auto& Object : m_CharacterList)
		{
			if (Object.pINode == pNode)
			{
				return &Object;
			}
		}
		return nullptr;
	}


	void Clear() override
	{
		m_CharacterList.clear();
		for (auto Triangle : m_SCATriList)
		{
			Triangle.clear();
		}
	}
public:
	SCAObjectManager();
	~SCAObjectManager();
};

