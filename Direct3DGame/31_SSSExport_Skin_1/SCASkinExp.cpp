#include "SCASkinExp.h"

void  SCASkinExp::Set(const TCHAR* Pointer_TCHAR_filename, Interface* Pointer_Interface_max)
{
	My_Pointer_Interface_max = Pointer_Interface_max;
	My_wstring_filename = Pointer_TCHAR_filename;
	My_Pointer_INode_rootnode = Pointer_Interface_max->GetRootNode();		// 최상단 노드를 저장해둔다.
	My_Interval_interval = Pointer_Interface_max->GetAnimRange();		// Animation 시간 정보를 저장한다.

	My_SCAScene_Scene.iFirstFrame = My_Interval_interval.Start() / GetTicksPerFrame();	// 시작 프레임
	My_SCAScene_Scene.iLastFrame = My_Interval_interval.End() / GetTicksPerFrame();		// 종료 프레임
	My_SCAScene_Scene.iFrameSpeed = GetFrameRate();							// 프레임 당 속도
	My_SCAScene_Scene.iTickPerFrame = GetTicksPerFrame();						// 틱 당 속도

	for (auto pNode : m_SelectNodeList)
	{
		Process(pNode);
	}
}
bool  SCASkinExp::Export()
{
	My_Pointer_FILE_file = nullptr;
	// Header

	My_SCAScene_Scene.iNumMaterials = My_MaterialManager.m_MaterialList.size();
	My_SCAScene_Scene.iNumObjects =  My_ObjectManager.m_ObjectList.size();
	_wfopen_s(&My_Pointer_FILE_file, My_wstring_filename.c_str(), _T("wb"));
	_ftprintf(My_Pointer_FILE_file, _T("%s"), _T("SCAExporter100"));
	_ftprintf(My_Pointer_FILE_file, _T("\n%s %d %d %d %d %d %d"),
		L"#HEADERINFO",
		My_SCAScene_Scene.iFirstFrame,
		My_SCAScene_Scene.iLastFrame,
		My_SCAScene_Scene.iFrameSpeed,
		My_SCAScene_Scene.iTickPerFrame,
		My_SCAScene_Scene.iNumObjects,
		My_SCAScene_Scene.iNumMaterials);

	My_MaterialManager.ExportMaterial(My_Pointer_FILE_file);
	My_ObjectManager.ExportObject(My_Pointer_FILE_file);

	fclose(My_Pointer_FILE_file);

	MessageBox(GetActiveWindow(), My_wstring_filename.c_str(),
		_T("Succeed!"), MB_OK);
	return true;
}

TCHAR* SCASkinExp::SaveFileDiallog(TCHAR* Pointer_TCHAR_name, TCHAR* Pointer_TCHAR_Title)
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