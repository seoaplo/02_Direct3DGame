// SAExport.cpp : DLL 응용 프로그램을 위해 내보낸 함수를 정의합니다.
//

#include "header.h"
#include "resource.h"
#include "SAExporter.h"
#include "SOAExporter.h"
#include "SSkinExporter.h"
#include "SMatrixExporter.h"

#define SAExport_CLASS_ID Class_ID(0x43984858, 0x361c3cf6)

extern HINSTANCE Global_HandleInstance;

INT_PTR CALLBACK DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


class SAExport : public UtilityObj
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
			_T("SA_Exporter"), 0);
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
		int iNumSelect = Pointer_interface->GetSelNodeCount();

		if (iNumSelect <= 0) return;
		else I_SSkinExporter.ClearSelectNodeList();

		for (int iSelObj = 0; iSelObj < iNumSelect; iSelObj++)
		{
			INode* Pointer_INode = Pointer_interface->GetSelNode(iSelObj);
			I_SSkinExporter.GetSelectNode(Pointer_INode);
		}
	}
public:
	static SAExport* GetInstance()
	{
		static SAExport Single_export;
		return &Single_export;
	}
private:
	SAExport() { My_WindowHandle_panel = NULL; };
public:
	~SAExport() {};
};
class SSSExportClassDesc : public ClassDesc2
{
public:

	virtual int IsPublic() { return TRUE; }
	virtual void* Create(BOOL) { return SAExport::GetInstance(); }
	virtual const TCHAR *	ClassName() { return _T("SAExport100"); }
	virtual SClass_ID SuperClassID() { return UTILITY_CLASS_ID; }
	virtual Class_ID ClassID() { return SAExport_CLASS_ID; }
	virtual const TCHAR* Category() { return _T("SAExporter"); }

	virtual const TCHAR* InternalName() { return _T("SAExportClassDesc"); }	// returns fixed parsable name (scripter-visible name)
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
			TSTR szExpFile = I_SSkinExporter.SaveFileDiallog(L"skm", L"SkinWriter");
			if (szExpFile != NULL)
			{
				I_SSkinExporter.Set(szExpFile, SAExport::GetInstance()->My_Pointer_interface);
				I_SSkinExporter.Convert();
				I_SSkinExporter.Export();
			}
		}break;
		case IDC_MATRIXEXP:
		{
			TSTR szExpFile = I_SMatrixExporter.SaveFileDiallog(L"smc", L"MatrixWriter");
			if (szExpFile != NULL)
			{
				I_SMatrixExporter.Set(szExpFile, SAExport::GetInstance()->My_Pointer_interface);
				I_SMatrixExporter.Convert();
				I_SMatrixExporter.Export();
			}

		}break;
		case IDC_SCAEXP:
		{
			TSTR szExpFile = I_SSkinExporter.SaveFileDiallog(L"skm", L"SkinWriter");
			if (szExpFile != NULL)
			{
				I_SSkinExporter.Set(szExpFile, SAExport::GetInstance()->My_Pointer_interface);
				I_SSkinExporter.Convert();
				I_SSkinExporter.Export();
			}
			szExpFile = I_SMatrixExporter.SaveFileDiallog(L"smc", L"MatrixWriter");
			if (szExpFile != NULL)
			{
				I_SMatrixExporter.Set(szExpFile, SAExport::GetInstance()->My_Pointer_interface);
				I_SMatrixExporter.Convert();
				I_SMatrixExporter.Export();
			}

		}break;
		case IDC_SOAEXP:
		{
			TSTR szExpFile = I_SOAExporter.SaveFileDiallog(L"SOA", L"SAExporter");
			if (szExpFile != NULL)
			{
				I_SOAExporter.Set(szExpFile, SAExport::GetInstance()->My_Pointer_interface);
				I_SOAExporter.Convert();
				I_SOAExporter.Export();
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