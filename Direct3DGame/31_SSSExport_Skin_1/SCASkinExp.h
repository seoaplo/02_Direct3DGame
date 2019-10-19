#pragma once
#include "SCAGlobal.h"
#include "SCAMaterialManager.h"
#include "SCAObjectManager.h"

class SCASkinExp
{
public:
	SCAMaterialManager		My_MaterialManager;
	SCAObjectManager		My_ObjectManager;
	std::vector<INode*>		m_SelectNodeList;

	Interface*				My_Pointer_Interface_max;				// Max Interface
	std::wstring			My_wstring_filename;			// file path
	FILE*					My_Pointer_FILE_file;			// file Pointer
	INode*					My_Pointer_INode_rootnode;		// Interface Node
	SCAScene				My_SCAScene_Scene;
	Interval				My_Interval_interval;

public:
	void	Set(const TCHAR* Pointer_TCHAR_filename, Interface* Pointer_Interface_max);
	bool	Export();
	void	Process(INode* Pointer_INode)
	{
		int iMaterial = My_MaterialManager.AddMaterial(Pointer_INode);
		My_ObjectManager.AddObject(Pointer_INode, My_SCAScene_Scene, My_Interval_interval, iMaterial);
	}
	SCAObject* FindObject(INode* pNode)
	{
		return My_ObjectManager.FindObject(pNode);
	}
	SCAObject* FindObject(int iIndex)
	{
		return My_ObjectManager.FindObject(iIndex);
	}
	SCAObject* FindObject(TSTR name)
	{
		return My_ObjectManager.FindObject(name);
	}


	void	Clear()
	{
		My_MaterialManager.m_MaterialList.clear();
		My_ObjectManager.m_ObjectList.clear();
	}
	TCHAR* SaveFileDiallog(TCHAR* Pointer_TCHAR_name, TCHAR* Pointer_TCHAR_Title);
public:
	static SCASkinExp& Get()
	{
		static SCASkinExp Single_export;
		return Single_export;
	}
private:
	SCASkinExp()
	{
		My_Pointer_FILE_file = nullptr;
		My_Pointer_INode_rootnode = nullptr;
		My_Pointer_Interface_max = nullptr;
	};
public:
	~SCASkinExp() {}
};

#define I_SkinExporter SCASkinExp::Get()

