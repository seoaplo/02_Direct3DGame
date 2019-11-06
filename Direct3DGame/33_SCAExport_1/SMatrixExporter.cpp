#include "SMatrixExporter.h"


void SMatrixExporter::Convert()
{
	m_MatrixManager.Release();
	SAExporter::CreateMaterialList();
	m_MatrixManager.SetSize(g_iNodeMaxNum);
	for (auto& pNode : SAExporter::g_NodeList)
	{
		int iMaterialNum = SAExporter::FindMaterial(pNode.first);
		m_MatrixManager.AddObject(pNode.first, g_Scene, g_Interval, pNode.second, iMaterialNum);
	}
}

bool SMatrixExporter::Export()
{
	m_pStream = nullptr;
	_wfopen_s(&m_pStream, m_filename.c_str(), _T("wb"));
	if (m_pStream == nullptr) return false;

	ExportHeader(m_pStream);
	m_MatrixManager.ExportObject(m_pStream);

	fclose(m_pStream);

	MessageBox(GetActiveWindow(), m_filename.c_str(),
		_T("Succeed!"), MB_OK);
	return true;
}
bool SMatrixExporter::Release()
{
	SAExporter::Release();
	m_MatrixManager.Release();

	return true;
}
void SMatrixExporter::ExportHeader(FILE* pStream)
{
	if (m_pStream == nullptr) return;
	g_Scene.iNumMaterials = g_MaterialManager.m_MaterialList.size();
	g_Scene.iNumObjects = m_MatrixManager.m_ObjectList.size();
	_ftprintf(m_pStream, _T("%s"), _T("MatrixExporter100"));
	_ftprintf(m_pStream, _T("\n%s %d %d %d %d %d"),
		L"#HEADERINFO",
		g_Scene.iFirstFrame,
		g_Scene.iLastFrame,
		g_Scene.iFrameSpeed,
		g_Scene.iTickPerFrame,
		g_Scene.iNumObjects);
}

SMatrixExporter::SMatrixExporter()
{
}


SMatrixExporter::~SMatrixExporter()
{
}
