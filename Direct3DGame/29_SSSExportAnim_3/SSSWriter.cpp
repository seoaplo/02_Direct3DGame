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
	m_pRootNode = m_pMax->GetRootNode();		// 최상단 노드를 저장해둔다.
	m_Interval = m_pMax->GetAnimRange();		// Animation 시간 정보를 저장한다.

	m_Scene.iFirstFrame = m_Interval.Start() / GetTicksPerFrame();	// 시작 프레임
	m_Scene.iLastFrame = m_Interval.End() / GetTicksPerFrame();		// 종료 프레임
	m_Scene.iFrameSpeed = GetFrameRate();							// 프레임 당 속도
	m_Scene.iTickPerFrame = GetTicksPerFrame();						// 틱 당 속도

	PreProcess(m_pRootNode);					// 재귀 호출
}
void  SSSWriter::PreProcess(INode* pNode)
{
	// 끝에 도달하면 재귀 함수 정지
	if (pNode == NULL) return;			

	// 노드 저장
	int iMaterial = m_MaterialManager.AddMaterial(pNode);								
	m_ObjectManager.AddObject(pNode, m_Scene, m_Interval, iMaterial);
	// 자식 노드의 수를 얻은 다음 반복한다.
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
