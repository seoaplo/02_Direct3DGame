#include "SSkinExporter.h"

void SSkinExporter::Convert()
{
	m_SkinManager.Release();
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
	ExportInverseMatrix(m_pStream);

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
void SSkinExporter::ExportInverseMatrix(FILE* pStream)
{
	if (m_pStream == nullptr) return;
	_ftprintf(m_pStream, _T("\n%s %d"), L"#INVERSEINFO", g_NodeList.size());
	for (int iCount = 0; iCount < g_NodeList.size(); iCount++)
	{
		INode* pNode = nullptr;
		for (auto& TargetNode : g_NodeList)
		{
			if (TargetNode.second == iCount)
			{
				pNode = TargetNode.first;
			}
		}

		if (pNode != nullptr)
		{
			Matrix3 Invtm= Inverse(pNode->GetNodeTM(g_Interval.Start()));
			D3D_MATRIX matInvWorld;
			SAGlobal::DumpMatrix3(matInvWorld, &Invtm);
			// World Matrix
			_ftprintf(pStream, _T("\n\t%10.4f %10.4f %10.4f %10.4f\n\t%10.4f %10.4f %10.4f %10.4f\n\t%10.4f %10.4f %10.4f %10.4f\n\t%10.4f %10.4f %10.4f %10.4f"),
				matInvWorld._11,
				matInvWorld._12,
				matInvWorld._13,
				matInvWorld._14,

				matInvWorld._21,
				matInvWorld._22,
				matInvWorld._23,
				matInvWorld._24,

				matInvWorld._31,
				matInvWorld._32,
				matInvWorld._33,
				matInvWorld._34,

				matInvWorld._41,
				matInvWorld._42,
				matInvWorld._43,
				matInvWorld._44);
		}
	}
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
