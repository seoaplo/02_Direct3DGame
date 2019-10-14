#include "header.h"
#include "SCAWriter.h"
#include <iostream>
#include <fstream>


//===========================================================================
// Max File Read
//===========================================================================
void SCAWriter::Set(Interface* pMax)
{
	My_Pointer_Interface_max = pMax;
	My_Pointer_rootnode = My_Pointer_Interface_max->GetRootNode();		// �ֻ�� ��带 �����صд�.
	My_Interval_interval = My_Pointer_Interface_max->GetAnimRange();		// Animation �ð� ������ �����Ѵ�.

	My_SCAScene_scene.iFirstFrame = My_Interval_interval.Start() / GetTicksPerFrame();	// ���� ������
	My_SCAScene_scene.iLastFrame = My_Interval_interval.End() / GetTicksPerFrame();		// ���� ������
	My_SCAScene_scene.iFrameSpeed = GetFrameRate();							// ������ �� �ӵ�
	My_SCAScene_scene.iTickPerFrame = GetTicksPerFrame();						// ƽ �� �ӵ�

	PreProcess(My_Pointer_rootnode);					// ��� ȣ��
}
void SCAWriter::SetName(const TCHAR* name)
{
	My_wstring_filename = name;
}

void  SCAWriter::Set(const TCHAR* name, Interface* pMax)
{
	SetName(name);
	Set(pMax);
}
void  SCAWriter::PreProcess(INode* pNode)
{
	// ���� �����ϸ� ��� �Լ� ����
	if (pNode == NULL) return;			

	// ��� ����
	int iMaterial = My_MaterialManager.AddMaterial(pNode);								
	My_ObjectManager.AddObject(pNode, My_SCAScene_scene, My_Interval_interval, iMaterial);
	// �ڽ� ����� ���� ���� ���� �ݺ��Ѵ�.
	int iNumChildren = pNode->NumberOfChildren();			 
	for (int iChild = 0; iChild < iNumChildren; iChild++)
	{
		INode* pChild = pNode->GetChildNode(iChild);
		PreProcess(pChild);
	}
}
bool SCAWriter::Export()
{

	My_Pointer_FILE_stream = nullptr;
	// Header

	My_SCAScene_scene.iNumMaterials = My_MaterialManager.m_MaterialList.size();
	My_SCAScene_scene.iNumObjects = My_ObjectManager.m_ObjectList.size();
	_wfopen_s(&My_Pointer_FILE_stream, My_wstring_filename.c_str(), _T("wb"));
	_ftprintf(My_Pointer_FILE_stream, _T("%s"), _T("SSSExporter100"));
	_ftprintf(My_Pointer_FILE_stream, _T("\n%s %d %d %d %d %d %d"),
		L"#HEADERINFO",
		My_SCAScene_scene.iFirstFrame,
		My_SCAScene_scene.iLastFrame,
		My_SCAScene_scene.iFrameSpeed,
		My_SCAScene_scene.iTickPerFrame,
		My_SCAScene_scene.iNumObjects,
		My_SCAScene_scene.iNumMaterials);

	My_MaterialManager.ExportMaterial(My_Pointer_FILE_stream);
	My_ObjectManager.ExportObject(My_Pointer_FILE_stream);

	fclose(My_Pointer_FILE_stream);

	MessageBox(GetActiveWindow(), My_wstring_filename.c_str(),
		_T("Succeed!"), MB_OK);

	My_MaterialManager.m_MaterialList.clear();
	My_ObjectManager.m_ObjectList.clear();
	return true;
}

TCHAR* SCAWriter::SaveFileDiallog(TCHAR* Pointer_TCHAR_name, TCHAR* Pointer_TCHAR_Title)
{
	TCHAR ARRAY_TCHAR_file[MAX_PATH] = { 0, };
	TCHAR ARRAY_TCHAR_filetitle[MAX_PATH] = { 0, };
	static TCHAR* Static_Pointer_TCHAR_filter;
	OPENFILENAME OPENFILENAME_filename;

	//=======================================================================
	// Initiallize
	//=======================================================================
	ZeroMemory(&OPENFILENAME_filename, sizeof(OPENFILENAME));
	_tcscpy_s(ARRAY_TCHAR_file, _T("*."));
	_tcscat_s(ARRAY_TCHAR_file, Pointer_TCHAR_name);
	_tcscat_s(ARRAY_TCHAR_file, _T("\0"));

	//=======================================================================
	// Set OPENFILENAME
	//=======================================================================
	OPENFILENAME_filename.lStructSize = sizeof(OPENFILENAME);
	OPENFILENAME_filename.hwndOwner = GetActiveWindow();
	OPENFILENAME_filename.lpstrFilter = Static_Pointer_TCHAR_filter;
	OPENFILENAME_filename.lpstrCustomFilter = NULL;
	OPENFILENAME_filename.nMaxCustFilter = 0L;
	OPENFILENAME_filename.nFilterIndex = 1;
	OPENFILENAME_filename.lpstrFile = ARRAY_TCHAR_file;
	OPENFILENAME_filename.nMaxFile = sizeof(ARRAY_TCHAR_file);
	OPENFILENAME_filename.lpstrFileTitle = ARRAY_TCHAR_filetitle;
	OPENFILENAME_filename.nMaxFileTitle = sizeof(ARRAY_TCHAR_filetitle);
	OPENFILENAME_filename.lpstrInitialDir = NULL;
	OPENFILENAME_filename.lpstrTitle = Pointer_TCHAR_Title;
	OPENFILENAME_filename.Flags = 0L;
	OPENFILENAME_filename.nFileOffset = 0;
	OPENFILENAME_filename.nFileExtension = 0;
	OPENFILENAME_filename.lpstrDefExt = Pointer_TCHAR_name;


	//=======================================================================
	// Save File
	//=======================================================================
	if (!GetSaveFileName(&OPENFILENAME_filename))
	{
		return NULL;
	}
	return ARRAY_TCHAR_file;
}