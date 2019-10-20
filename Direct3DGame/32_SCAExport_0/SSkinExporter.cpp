#include "SSkinExporter.h"

void SSkinExporter::Convert()
{
	SAExporter::CreateMaterialList();
	m_SkinManager.SetSize(m_iSelectNodeMax);
	for (auto& pNode : m_SelectNodeList)
	{
		int iMaterialNum = SAExporter::FindMaterial(pNode.first);
		m_SkinManager.AddObject(pNode.first, g_Scene, g_Interval, pNode.second, iMaterialNum, g_NodeList);
	}

}
bool SSkinExporter::Export()
{
	m_pStream = nullptr;
	_wfopen_s(&m_pStream, m_filename.c_str(), _T("wb"));
	if (m_pStream == nullptr) return false;

	ExportHeader(m_pStream);
	SAExporter::ExportMaterial(m_pStream);
	m_SkinManager.ExportObject(m_pStream);

	_ftprintf(m_pStream, _T("\n%s %d"), L"#INV_GETNODETM", g_iNodeMaxNum);
	for (auto& pObj : g_NodeList)
	{
		INode* pNode = pObj.first;
		Matrix3 wtm = pNode->GetNodeTM(g_Interval.Start());
		Matrix3 invWtm = Inverse(wtm);

		D3D_MATRIX tm;
		SAGlobal::DumpMatrix3(tm, &invWtm);

		_ftprintf(m_pStream,
			_T("\n\t%10.4f %10.4f %10.4f %10.4f\n\t%10.4f %10.4f %10.4f %10.4f\n\t%10.4f %10.4f %10.4f %10.4f\n\t%10.4f %10.4f %10.4f %10.4f"),
			tm._11,
			tm._12,
			tm._13,
			tm._14,

			tm._21,
			tm._22,
			tm._23,
			tm._24,

			tm._31,
			tm._32,
			tm._33,
			tm._34,

			tm._41,
			tm._42,
			tm._43,
			tm._44);

	}

	fclose(m_pStream);
	MessageBox(GetActiveWindow(), m_filename.c_str(),
		_T("Succeed!"), MB_OK);
	return true;
}
void SSkinExporter::ExportHeader(FILE* pStream)
{
	if (m_pStream == nullptr) return;
	g_Scene.iNumMaterials = g_MaterialManager.m_MaterialList.size();
	g_Scene.iNumObjects = m_SelectNodeList.size();
	_ftprintf(m_pStream, _T("%s"), _T("SkinExporter100"));
	_ftprintf(m_pStream, _T("\n%s %d %d %d %d %d %d"),
		L"#HEADERINFO",
		g_Scene.iFirstFrame,
		g_Scene.iLastFrame,
		g_Scene.iFrameSpeed,
		g_Scene.iTickPerFrame,
		g_Scene.iNumObjects,
		g_Scene.iNumMaterials);
}
void SSkinExporter::GetSelectNode(INode* pNode)
{
	m_SelectNodeList.insert(std::make_pair(pNode, m_iSelectNodeMax));
	m_iSelectNodeMax++;
}
void SSkinExporter::ClearSelectNodeList()
{
	m_iSelectNodeMax = 0;
	m_SelectNodeList.clear();
}
bool SSkinExporter::Release()
{
	ClearSelectNodeList();
	SAExporter::Release();
	return true;
}

SSkinExporter::SSkinExporter()
{
}


SSkinExporter::~SSkinExporter()
{
}
