#pragma once
#include "SAGlobal.h"
#include "SAMaterialManager.h"

class SAExporter
{
public:
	
//============================================================================
// Union Data
//============================================================================
	static int						g_iNodeMaxNum;
	static std::map<DWORD, INode*>	g_NodeList;
	static INode*					g_pRootNode;		// Interface Node
	static SAScene					g_Scene;			// Scene Data
	static Interval					g_Interval;			// Time
	static SAMaterialManager		g_MaterialManager;	// Material Manager

//=============================================================================
// inheritance Data
//=============================================================================
	Interface*				m_pMax;				// Max Interface
	std::wstring			m_filename;			// file path
	FILE*					m_pStream;			// file Pointer

public:
	TCHAR* SaveFileDiallog(TCHAR* szName, TCHAR* szTitle);
	void	Set(const TCHAR* name, Interface* pMax);
	virtual void	Convert() = 0;
	virtual bool	Export() = 0;
	virtual bool	Release();
protected:
	virtual void	PreProcess(INode* pNode);
	virtual void	ExportHeader(FILE* pStream) = 0;
	void			ExportMaterial(FILE* pStream);

	bool			CreateMaterialList();
	int				FindMaterial(INode* pNode);

public:
	SAExporter();
	virtual ~SAExporter();
};

