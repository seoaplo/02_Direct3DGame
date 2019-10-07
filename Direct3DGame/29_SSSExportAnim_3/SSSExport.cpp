// SSSExport.cpp : DLL 응용 프로그램을 위해 내보낸 함수를 정의합니다.
//

#include "header.h"
#include "SSSWriter.h"

#define SSSExport_CLASS_ID Class_ID(0x297b1fa7, 0x76e6ce6)

class SSSExport : public SceneExport
{
private:
	SSSWriter	m_Wirter;
public:
	int				ExtCount();
	const MCHAR *	Ext(int n);					// Extension #n (i.e. "3DS")
	const MCHAR *	LongDesc();					// Long ASCII description (i.e. "Autodesk 3D Studio File")
	const MCHAR *	ShortDesc();				// Short ASCII description (i.e. "3D Studio")
	const MCHAR *	AuthorName();				// ASCII Author name
	const MCHAR *	CopyrightMessage();			// ASCII Copyright message
	const MCHAR *	OtherMessage1();			// Other message #1
	const MCHAR *	OtherMessage2();			// Other message #2
	unsigned int	Version();					// Version number * 100 (i.e. v3.01 = 301)
	void			ShowAbout(HWND hWnd);		// Show DLL's "About..." box
	int				DoExport(const MCHAR *name, ExpInterface *ei, Interface* pInterface, BOOL suppressPrompts = FALSE, DWORD options = 0);	// Export file
public:
	SSSExport() {};
	~SSSExport() {};
};
class SSSExportClassDesc : public ClassDesc2
{
public:
	virtual int IsPublic() { return TRUE; }
	virtual void* Create(BOOL) { return new SSSExport(); }
	virtual const TCHAR *	ClassName() { return _T("SSSExport100"); }
	virtual SClass_ID SuperClassID() { return SCENE_EXPORT_CLASS_ID; }
	virtual Class_ID ClassID() { return SSSExport_CLASS_ID; }
	virtual const TCHAR* Category() { return _T("SSSExporter"); }

	virtual const TCHAR* InternalName() { return _T("SSSExportClassDesc"); }	// returns fixed parsable name (scripter-visible name)
	virtual HINSTANCE HInstance() { return hInstance; }					// returns owning module handle
};


ClassDesc2* GetExportDesc()
{
	static SSSExportClassDesc SSSExportDesc;
	return &SSSExportDesc;
}

int	SSSExport::ExtCount()
{
	return true;
}
const MCHAR* SSSExport::Ext(int n) // 확장자
{
	return	_T("SSS");
}
const MCHAR* SSSExport::LongDesc() // 이름
{
	return	_T("SSS Exporter 2.0");
}
const MCHAR* SSSExport::ShortDesc() // 이름 짧게
{
	return	_T("SSS Exporter");
}
const MCHAR* SSSExport::AuthorName() // 만든이
{
	return	_T("SSS");
}
const MCHAR* SSSExport::CopyrightMessage()
{
	return	_T("");
}
const MCHAR* SSSExport::OtherMessage1()
{
	return	_T("");
}
const MCHAR* SSSExport::OtherMessage2()
{
	return	_T("");
}
unsigned int SSSExport::Version() // 버젼이 몇인가
{
	return 100;
}
void SSSExport::ShowAbout(HWND hWnd)
{

}
int	SSSExport::DoExport(const MCHAR* name, ExpInterface *ei, Interface* pInterface, BOOL suppressPrompts, DWORD options)
{
	BOOL Ret = FALSE;
	m_Wirter.Set(name, pInterface);
	Ret = m_Wirter.Export();
	return Ret;
}


