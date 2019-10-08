#pragma once
#include "SSSMaterialManager.h"
#include "SSSObjectManager.h"

class SSSWriter
{
public:
	SSSMaterialManager		m_MaterialManager;
	SSSObjectManager		m_ObjectManager;

	Interface*				m_pMax;				// Max Interface
	std::wstring			m_filename;			// file path
	FILE*					m_pStream;			// file Pointer
	INode*					m_pRootNode;		// Interface Node
	SScene					m_Scene;			
	Interval				m_Interval;

public:
	void	Set(const TCHAR* name, Interface* pMax);
	bool	Export();
	void	PreProcess(INode* pNode);

public:
	SSSWriter();
	~SSSWriter();
};

