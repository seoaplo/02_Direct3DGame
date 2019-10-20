#pragma once
#include "SAExporter.h"
#include "SSkinManager.h"
class SSkinExporter : public SAExporter
{
public:
	int	m_iSelectNodeMax;
	NodeList m_SelectNodeList;
	SSkinManager m_SkinManager;
public:
	void	Convert();
	bool	Export();
	bool	Release();
	void	ExportHeader(FILE* pStream);

	void	GetSelectNode(INode* pNode);
	void	ClearSelectNodeList();

public:
	static SSkinExporter& Get()
	{
		static SSkinExporter Single_export;
		return Single_export;
	}
private:
	SSkinExporter();
public:
	~SSkinExporter();
};


#define I_SSkinExporter SSkinExporter::Get()