#pragma once
#include "SCAMaterialManager.h"
#include "SCAObjectManager.h"

class SCAWriter
{
public:
	SCAMaterialManager		My_MaterialManager;
	SCAObjectManager		My_ObjectManager;

	Interface*				My_Pointer_Interface_max;				// Max Interface
	std::wstring			My_wstring_filename;			// file path
	FILE*					My_Pointer_FILE_stream;			// file Pointer
	INode*					My_Pointer_rootnode;		// Interface Node
	SCAScene				My_SCAScene_scene;			
	Interval				My_Interval_interval;

public:
	void	Set(Interface* pMax);
	void	SetName(const TCHAR* name);
	void	Set(const TCHAR* name, Interface* pMax);
	bool	Export();
	void	PreProcess(INode* pNode);
	void	Clear()
	{
		My_MaterialManager.m_MaterialList.clear();
		My_ObjectManager.m_ObjectList.clear();
	}

	TCHAR* SaveFileDiallog(TCHAR* Pointer_TCHAR_name, TCHAR* Pointer_TCHAR_Title);
public:
	static SCAWriter& Get()
	{
		static SCAWriter Single_export;
		return Single_export;
	}
private:
	SCAWriter() 
	{
		My_Pointer_Interface_max = nullptr;
		My_Pointer_FILE_stream = nullptr;
		My_Pointer_rootnode = nullptr;
	};
public:
	~SCAWriter() {};
};

#define I_Writer SCAWriter::Get()
