#pragma once
#include "SAExporter.h"
#include "SOAManager.h"

class SOAExporter : public SAExporter
{
private:
	SOAManager m_SOAManager;
public:
	void	Convert()override;
	bool	Export()override;
	bool	Release()override;
	void	ExportHeader(FILE* pStream)override;

public:
	static SOAExporter& Get()
	{
		static SOAExporter Single_export;
		return Single_export;
	}
private:
	SOAExporter();
public:
	~SOAExporter();
};

#define I_SOAExporter SOAExporter::Get()