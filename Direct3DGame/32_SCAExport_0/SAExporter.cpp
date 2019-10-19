#include "SAExporter.h"

int						SAExporter::g_iNodeMaxNum;
std::map<DWORD, INode*>	SAExporter::g_NodeList;
INode*					SAExporter::g_pRootNode;
SAScene					SAExporter::g_Scene;			
Interval					SAExporter::g_Interval;			
SAMaterialManager		SAExporter::g_MaterialManager;	


TCHAR* SAExporter::SaveFileDiallog(TCHAR* szName, TCHAR* szTitle)
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
	_tcscat_s(ARRAY_TCHAR_file, szName);
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
	OPENFILENAME_filename.lpstrTitle = szTitle;
	OPENFILENAME_filename.Flags = 0L;
	OPENFILENAME_filename.nFileOffset = 0;
	OPENFILENAME_filename.nFileExtension = 0;
	OPENFILENAME_filename.lpstrDefExt = szName;


	//=======================================================================
	// Save File
	//=======================================================================
	if (!GetSaveFileName(&OPENFILENAME_filename))
	{
		return NULL;
	}
	return ARRAY_TCHAR_file;
}
void SAExporter::Set(const TCHAR* name, Interface* pMax)
{
	m_pMax = pMax;
	m_filename = name;
	SAExporter::g_pRootNode = m_pMax->GetRootNode();		// �ֻ�� ��带 �����صд�.
	SAExporter::g_Interval = m_pMax->GetAnimRange();		// Animation �ð� ������ �����Ѵ�.

	SAExporter::g_Scene.iFirstFrame = SAExporter::g_Interval.Start() / GetTicksPerFrame();	// ���� ������
	SAExporter::g_Scene.iLastFrame = SAExporter::g_Interval.End() / GetTicksPerFrame();		// ���� ������
	SAExporter::g_Scene.iFrameSpeed = GetFrameRate();							// ������ �� �ӵ�
	SAExporter::g_Scene.iTickPerFrame = GetTicksPerFrame();						// ƽ �� �ӵ�

	PreProcess(SAExporter::g_pRootNode);
}

void  SAExporter::PreProcess(INode* pNode)
{
	// ���� �����ϸ� ��� �Լ� ����
	if (pNode == NULL) return;

	ObjectState os = pNode->EvalWorldState(g_Interval.Start());
	Object* CheckID = nullptr;
	int ChildNum = pNode->NumChildren();

	CheckID = pNode->GetObjectRef();

	if (os.obj)
	{
		// ���ʿ��� ���(�þ��� ���� � ���̴� ���)
		if (os.obj->ClassID() == Class_ID(TARGET_CLASS_ID, 0))
		{
			return;
		}

		switch (os.obj->SuperClassID())
		{
			// �ش� ����̸� ����
		case GEOMOBJECT_CLASS_ID:
		case HELPER_CLASS_ID:
			// ��� ����
			SAExporter::g_NodeList.insert(std::make_pair(g_iNodeMaxNum, pNode));
			SAExporter::g_iNodeMaxNum++;
			break;
		default:
			return;
		}
	}

	// �ڽ� ����� ���� ���� ���� �ݺ��Ѵ�.
	int iNumChildren = pNode->NumberOfChildren();
	for (int iChild = 0; iChild < iNumChildren; iChild++)
	{
		INode* pChild = pNode->GetChildNode(iChild);
		PreProcess(pChild);
	}
}
void SAExporter::ExportMaterial(FILE* pStream)
{
	SAExporter::g_MaterialManager.ExportMaterial(pStream);
}
int	SAExporter::FindMaterial(INode* pNode)
{
	return SAExporter::g_MaterialManager.FindMaterial(pNode);
}
bool SAExporter::CreateMaterialList()
{
	for (int iNodeNum = 0; iNodeNum < SAExporter::g_iNodeMaxNum; iNodeNum++)
	{
		INode* pNode = SAExporter::g_NodeList.find(iNodeNum)->second;
		SAExporter::g_MaterialManager.AddMaterial(pNode);
	}

	return true;
}
bool SAExporter::Release()
{
	SAExporter::g_iNodeMaxNum = 0;
	SAExporter::g_MaterialManager.Release();
	SAExporter::g_NodeList.clear();

	return true;
}
SAExporter::SAExporter()
{
}


SAExporter::~SAExporter()
{
}
