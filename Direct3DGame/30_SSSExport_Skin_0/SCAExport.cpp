// SCAExport.cpp : DLL 응용 프로그램을 위해 내보낸 함수를 정의합니다.
//

#include "header.h"
#include "resource.h"
#include "SCAWriter.h"
#include "SCASkinExp.h"

#define SCAExport_CLASS_ID Class_ID(0x7d591383, 0x18c6002a)

extern HINSTANCE Global_HandleInstance;

INT_PTR CALLBACK DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


class SCAExport : public UtilityObj
{
public:
	HWND		My_WindowHandle_panel;
	Interface*	My_Pointer_interface;
public:
	virtual void BeginEditParams(Interface *Pointer_interface, IUtil *Pointer_Int_util)
	{
		My_Pointer_interface = Pointer_interface;
		My_WindowHandle_panel = Pointer_interface->AddRollupPage(
			Global_HandleInstance,
			MAKEINTRESOURCE(IDD_DIALOG1),
			DlgProc,
			_T("SCA_Exporter"), 0);
	}
	virtual void EndEditParams(Interface *Pointer_interface, IUtil *Pointer_Int_util)
	{
		Pointer_interface->DeleteRollupPage(My_WindowHandle_panel);
	}
	virtual void DeleteThis()
	{
	}

	//virtual void SetStartupParam(MSTR param) {}
	virtual void SelectionSetChanged(Interface *Pointer_interface, IUtil *Pointer_Int_util)
	{
		I_SkinExporter.Clear();
		int iNumSelect = Pointer_interface->GetSelNodeCount();
		if (iNumSelect <= 0)
		{
			return;
		}
		for (int iSelObj = 0; iSelObj < iNumSelect;
			iSelObj++)
		{
			INode* Pointer_INode = Pointer_interface->GetSelNode(iSelObj);
			I_SkinExporter.Process(Pointer_INode);
		}
	}
public:
	static SCAExport* GetInstance()
	{
		static SCAExport Single_export;
		return &Single_export;
	}
private:
	SCAExport() { My_WindowHandle_panel = NULL; };
public:
	~SCAExport() {};
};
class SSSExportClassDesc : public ClassDesc2
{
public:

	virtual int IsPublic() { return TRUE; }
	virtual void* Create(BOOL) { return SCAExport::GetInstance(); }
	virtual const TCHAR *	ClassName() { return _T("SCAExport100"); }
	virtual SClass_ID SuperClassID() { return UTILITY_CLASS_ID; }
	virtual Class_ID ClassID() { return SCAExport_CLASS_ID; }
	virtual const TCHAR* Category() { return _T("SCAExporter"); }

	virtual const TCHAR* InternalName() { return _T("SCAExportClassDesc"); }	// returns fixed parsable name (scripter-visible name)
	virtual HINSTANCE HInstance() { return Global_HandleInstance; }					// returns owning module handle
};


ClassDesc2* GetExportDesc()
{
	static SSSExportClassDesc SSSExportDesc;
	return &SSSExportDesc;
}


INT_PTR CALLBACK DlgProc(HWND hWnd,
	UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_INITDIALOG:
		break;
	case WM_DESTROY:
		break;
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case IDC_SKINEXP:
		{
			TSTR szExpFile = SCASkinExp::Get().SaveFileDiallog(L"skm", L"SkinWriter");
			if (szExpFile != NULL)
			{
				I_SkinExporter.Set(szExpFile,
					SCAExport::GetInstance()->My_Pointer_interface);
				I_SkinExporter.Export();
			}

		}break;
		case IDC_MATRIXEXP:
		{
			MessageBox(GetActiveWindow(), _T("Exp!"),
				_T("IDC_MATRIXEXP!"), MB_OK);

		}break;
		case IDC_SCAEXP:
		{
			TSTR szExpFile = SCAWriter::Get().SaveFileDiallog(L"sca", L"SCAWriter");
			if (szExpFile != NULL)
			{
				I_SkinExporter.Set(szExpFile,
					SCAExport::GetInstance()->My_Pointer_interface);
				I_SkinExporter.Export();
			}

		}break;
		}
	}
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_MOUSEMOVE:
		GetCOREInterface()->RollupMouseMessage(
			hWnd, msg, wParam, lParam);
		break;
	default:
		return 0;
	}
	return 1;
}