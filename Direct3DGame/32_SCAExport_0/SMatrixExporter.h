#pragma once
#include "SAExporter.h"
#include "SMatrixManager.h"

class SMatrixExporter : public SAExporter
{
public:
	SMatrixManager m_MatrixManager;
public:
	void	Convert();
	bool	Export();
	bool	Release();
	void	ExportHeader(FILE* pStream);
public:
	static SMatrixExporter& Get()
	{
		static SMatrixExporter Single_export;
		return Single_export;
	}
private:
	SMatrixExporter();
public:
	~SMatrixExporter();
};

#define I_SMatrixExporter SMatrixExporter::Get()

