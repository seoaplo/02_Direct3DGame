#pragma once
#include "STextureManager.h"

const enum Textrue_Type
{
	TexType_Diffuse,
	TexType_Specular,
	TexType_normal,
	TexType_light,
	TexType_Bump,
	TexType_environmentmap,
	TexType_Size
};

const enum OBJECTCLASSTYPE {
	CLASS_GEOM = 0,
	CLASS_BONE,
	CLASS_DUMMY,
	CLASS_BIPED,
};

struct STextureList
{
	union
	{
		STexture* List[6];
		struct Type
		{
			STexture* pDiffuseMap;
			STexture* SpecularMap;
			STexture* NormalMap;
			STexture* LightMap;
			STexture* BumpMap;
			STexture* EnvironmentMap;
		};
	};
};

struct STextureNameList
{
	T_STR pDiffuseMap;
	T_STR SpecularMap;
	T_STR NormalMap;
	T_STR LightMap;
	T_STR BumpMap;
	T_STR EnvironmentMap;

	STextureNameList()
	{
		pDiffuseMap = L"none";
		SpecularMap = L"none";
		NormalMap = L"none";
		LightMap = L"none";
		BumpMap = L"none";
		EnvironmentMap = L"none";
	}
};

struct SMaterial
{
friend class SMaterialManager;
private:
	int			 m_iIndex;
	T_STR		 m_MaterialName;
	STextureList m_TextureList;
private:
	SMaterial();
	~SMaterial();
};

class SMaterialManager : public Singleton<SMaterialManager>
{
	friend class Singleton<SMaterialManager>;
private:
	std::vector<SMaterial> m_MaterialList;
public:
	SMaterial*	GetMaterial(T_STR MaterialName);
	SMaterial*	GetMaterial(int	iIndex);
	int			GetSize() { return m_MaterialList.size(); }

private:
	SMaterialManager() {}
public:
	~SMaterialManager() {}
};

#define I_MaterialManager SMaterialManager::GetInstance()