#pragma once
#include "SMesh.h"


struct SPositionAnim
{
	int     iTick;
	Point3  Position;
};

struct SRotationAnim
{
	int     iTick;
	D3DXQUATERNION    Quaternion;
};

struct SScaleAnim
{
	int     iTick;
	Point3  Position;
	D3DXQUATERNION    Quaternion;
};

struct SAnimationTrack
{
	std::vector<SPositionAnim>	PositionTrack;
	std::vector<SRotationAnim>	RotationTrack;
	std::vector<SScaleAnim>		ScaleTrack;
};


class SDrawObject
{
public:
	int iIndex;
	T_STR	m_ObjectName;
	T_STR	m_ParentName;
	SDrawObject*	m_pParentObject;
	std::vector<SMesh> m_MeshList;

	bool Frame();
	bool Render(ID3D11DeviceContext* pContext);

	bool SetParent(SDrawObject* pParentObject);
	bool SetParent(int	iIndex);
	bool SetParent(T_STR ParentName);

	bool SetMaterial(SMaterial* material);
	bool SetMaterial(T_STR MaterialName);
	bool SetMaterial(int iMaterialNumber);

public:
	SDrawObject();
	~SDrawObject();
};

class SDrawObjectManager : public Singleton<SDrawObjectManager>
{
	friend class Singleton<SDrawObjectManager>;
private:
	std::vector<SDrawObject> m_DrawObjectList;
public:
	SDrawObject*	GetDrawObject(T_STR DrawObjectName);
	SDrawObject*	GetDrawObject(int	iIndex);
	int			GetSize() { return m_DrawObjectList.size(); }

private:
	SDrawObjectManager() {}
public:
	~SDrawObjectManager() {}
};

#define I_DrawObjectManager SDrawObjectManager::GetInstance()