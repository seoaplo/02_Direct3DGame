#pragma once
#include "STextureManager.h"

const enum MAXSDK_MaterialTextureType
{
	MaxTexType_AmbientColor = 0,
	MaxTexType_DiffuseColor,
	MaxTexType_SpecularColor,
	MaxTexType_SpecularLevel,
	MaxTexType_Glossiness,
	MaxTexType_Self_IIIumination,
	MaxTexType_Opacity,
	MaxTexType_FilterColor,
	MaxTexType_Bump,
	MaxTexType_Reflection1,
	MaxTexType_Reflection2,
	MaxTexType_Displacement,
	MaxTexType_Number,
};

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
	STextureList()
	{
		List[0] = nullptr;
		List[1] = nullptr;
		List[2] = nullptr;
		List[3] = nullptr;
		List[4] = nullptr;
		List[5] = nullptr;
	}
	bool ConvertToMax(STexture* TargetTexture, int iMaxTexType)
	{
		switch (iMaxTexType)
		{
		case MaxTexType_DiffuseColor:
			List[TexType_Diffuse] = TargetTexture;
			return true;
		case MaxTexType_SpecularColor:
			List[TexType_Specular] = TargetTexture;
			return true;
		case MaxTexType_Bump:
			List[TexType_Bump] = TargetTexture;
			return true;
		case MaxTexType_Reflection1:
			List[TexType_environmentmap] = TargetTexture;
			return true;
		default:
			break;
		}
		return false;
	}
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

struct SSubMaterial
{
friend class SMaterial;
friend class SMaterialManager;
public:
	int iIndex;
	T_STR m_Name;
	STextureList m_TextureList;
private:
	// 서브 마테리얼 텍스쳐 셋
	void SetTexture(STexture* pTexture, int iType)
	{
		m_TextureList.List[iType] = pTexture;
	}
public:
	SSubMaterial()
	{
		iIndex = -1;
	}
	~SSubMaterial() {}
};

class SMaterial
{
friend class SMaterialManager;
public:
	int			 m_iIndex;
	T_STR		 m_MaterialName;
	std::vector<SSubMaterial> m_SubMaterialList;
public:
	SSubMaterial* Create();
public:
	SMaterial() 
	{
		m_iIndex = -1;
		m_MaterialName = L"none";
	};
	~SMaterial() {};
};

class SMaterialManager : public Singleton<SMaterialManager>
{
	friend class Singleton<SMaterialManager>;
private:
	std::vector<SMaterial> m_MaterialList;
public:
	SMaterial*	Create();
	SMaterial*	GetMaterial(T_STR MaterialName);
	SMaterial*	GetMaterial(int	iIndex);
	int			GetSize();
private:
	SMaterial*	SetAddMaterial(SMaterial& TargetMaterial);
private:
	SMaterialManager() {}
public:
	~SMaterialManager() {}
};

#define I_MaterialManager SMaterialManager::GetInstance()