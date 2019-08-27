#pragma once
#include "SUtils.h"
#include "SDxBasic.h"
#include "STemplate.h"

class SDirectWrite : public Singleton<SDirectWrite>
{
private:
	friend class Singleton<SDirectWrite>;
public:
	//===============================================================================================
	// 화면 DPI
	//===============================================================================================
	float				m_fDPIScaleX;
	float				m_fDPIScaleY;
	FLOAT				m_fdpiX;
	FLOAT				m_fdpiY;
	//===============================================================================================
	// Direct Interface Pointer
	//===============================================================================================
	ID2D1RenderTarget*			m_pD2DRenderTargetView;
	ID2D1Factory*				m_pD2DFactory;
	ID2D1SolidColorBrush*		m_pBlackBrush;
	IDWriteFactory*				m_pDWriteFactory;
	IDWriteTextFormat*			m_pTextFormat;
	IDWriteTextLayout*			m_pTextLayout;

	//===============================================================================================
	// 레이아웃 문자열 설정 값
	//===============================================================================================
	DWRITE_FONT_WEIGHT			m_FontWeight;			// 폰트 굵기
	DWRITE_FONT_STYLE			m_FontStyle;			// 폰트 스타일
	BOOL						m_fontUnderline;		// 밑줄
	wstring						m_wszFontFamily;		// Font용 문자열
	float						m_FontSize;				// 폰트 사이즈
	wstring						m_wszText;				// Layout용 문자열
	UINT32						m_cTextLength;			// 문자열 길이
	//===============================================================================================
	// Other Value
	//===============================================================================================
	HWND				m_hWnd;
public:
	//===============================================================================================
	// Initialize and Create
	//===============================================================================================
	bool			Set(HWND hWnd, int iWidth, int iHeight, IDXGISurface1* m_pSurface);
	bool			Init();
	HRESULT			CreateDeviceIndependentResources();					// 디바이스에 종속되지 않은 리소스 생성
	HRESULT			CreateDeviceResources(IDXGISurface1* m_pSurface);	// 디바이스에 종속되는 리소스 생성
	//===============================================================================================
	// Render
	//===============================================================================================
	bool			Render() {return true;}
	bool			Pre();
	bool			DrawText(RECT rc, const TCHAR* pText, D2D1::ColorF Color = D2D1::ColorF(1, 0, 0, 1)); // Format 방식
	bool			DrawText(D2D1_POINT_2F origin, D2D1::ColorF Color = D2D1::ColorF(1, 0, 0, 1));	// Layout 방식
	bool			Post();

	//===============================================================================================
	// Release
	//===============================================================================================
	bool			Release(); // Enter Release Function

	void			DiscardDeviceIndependentResources();		// 디바이스에 종속되지 않은 리소스들 해제
	void			DiscardDeviceResources();				// 디바이스에 종속되는 리소스들 해제

	//===============================================================================================
	// Setter
	//===============================================================================================
	HRESULT			SetText(D2D1_POINT_2F pos, wchar_t *text, D2D1::ColorF Color);
	HRESULT			SetFont(wchar_t *FontFamily);
	HRESULT			SetFontSize(float size);
	HRESULT			SetWeight(DWRITE_FONT_WEIGHT FontWeight);
	HRESULT			SetItalic(DWRITE_FONT_STYLE FontStyle);
	HRESULT			SetUnderline(bool underline);

	// 출력시 필요한 값 셋팅
	bool			SetPragraghAlighment(DWRITE_PARAGRAPH_ALIGNMENT nPragraghAlighment);
	bool			SetTextAlighment(DWRITE_TEXT_ALIGNMENT nTextAlighment);

	//===============================================================================================
	// Other
	//===============================================================================================
	void			OnResize(UINT width, UINT height, IDXGISurface1* pSurface);
	bool			Frame() { return true;}
private:
	SDirectWrite();
public:
	virtual ~SDirectWrite();
};
#define I_DirectWrite SDirectWrite::GetInstance()
