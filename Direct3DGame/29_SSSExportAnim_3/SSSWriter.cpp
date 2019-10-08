#include "header.h"
#include "SSSWriter.h"
#include <iostream>
#include <fstream>


//===========================================================================
// Max File Read
//===========================================================================
void  SSSWriter::Set(const TCHAR* name, Interface* pMax)
{
	m_pMax = pMax;
	m_filename = name;
	m_pRootNode = m_pMax->GetRootNode();		// �ֻ�� ��带 �����صд�.
	m_Interval = m_pMax->GetAnimRange();		// Animation �ð� ������ �����Ѵ�.

	m_Scene.iFirstFrame = m_Interval.Start() / GetTicksPerFrame();	// ���� ������
	m_Scene.iLastFrame = m_Interval.End() / GetTicksPerFrame();		// ���� ������
	m_Scene.iFrameSpeed = GetFrameRate();							// ������ �� �ӵ�
	m_Scene.iTickPerFrame = GetTicksPerFrame();						// ƽ �� �ӵ�

	PreProcess(m_pRootNode);					// ��� ȣ��
}
void  SSSWriter::PreProcess(INode* pNode)
{
	// ���� �����ϸ� ��� �Լ� ����
	if (pNode == NULL) return;			

	// ��� ����
	int iMaterial = m_MaterialManager.AddMaterial(pNode);								
	m_ObjectManager.AddObject(pNode, m_Scene, m_Interval, iMaterial);
	// �ڽ� ����� ���� ���� ���� �ݺ��Ѵ�.
	int iNumChildren = pNode->NumberOfChildren();			 
	for (int iChild = 0; iChild < iNumChildren; iChild++)
	{
		INode* pChild = pNode->GetChildNode(iChild);
		PreProcess(pChild);
	}
}
bool SSSWriter::Export()
{

	m_pStream = nullptr;
	// Header

	m_Scene.iNumMaterials = m_MaterialManager.m_MaterialList.size();
	m_Scene.iNumObjects = m_ObjectManager.m_ObjectList.size();
	_wfopen_s(&m_pStream, m_filename.c_str(), _T("wb"));
	_ftprintf(m_pStream, _T("%s"), _T("SSSExporter100"));
	_ftprintf(m_pStream, _T("\n%s %d %d %d %d %d %d"),
		L"#HEADERINFO",
		m_Scene.iFirstFrame,
		m_Scene.iLastFrame,
		m_Scene.iFrameSpeed,
		m_Scene.iTickPerFrame,
		m_Scene.iNumObjects,
		m_Scene.iNumMaterials);

	m_MaterialManager.ExportMaterial(m_pStream);
	m_ObjectManager.ExportObject(m_pStream);

	fclose(m_pStream);

	MessageBox(GetActiveWindow(), m_filename.c_str(),
		_T("Succeed!"), MB_OK);
	return true;
}
SSSWriter::SSSWriter()
{
}


SSSWriter::~SSSWriter()
{
}
