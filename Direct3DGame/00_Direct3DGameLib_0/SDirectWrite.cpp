#include "SDirectWrite.h"

#pragma region Init&Create
bool SDirectWrite::Set(HWND hWnd, int iWidth, int iHeight, IDXGISurface1* pSurface)
{
	HRESULT hr;
	if (!Init())
	{
		return false;
	}
	m_hWnd = hWnd;

	hr = CreateDeviceIndependentResources();
	hr = CreateDeviceResources(pSurface);
	SetText(D2D1::Point2F(static_cast<float> (iWidth), static_cast<float> (iHeight)),
				(wchar_t*)(L"TBasisSample!"), D2D1::ColorF(1, 1, 1, 1));
	
	m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

	return true;
}

bool SDirectWrite::Init()
{
	m_wszFontFamily = L"Galriola";
	return true;
}

HRESULT	SDirectWrite::CreateDeviceIndependentResources()
{
	HRESULT hr;

	// 2D 팩토리 생성 (멀티 스레드, 싱글 스레드 둘 중 하나 설정해야 함)
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);

	// 디스플레이의 Dots per Inchi를 구한 후 텍스트 크기를 설정
	m_pD2DFactory->GetDesktopDpi(&m_fdpiX, &m_fdpiY);
	m_fDPIScaleX = m_fdpiX / 96.0f;
	m_fDPIScaleY = m_fdpiY / 96.0f;

	// Create a shared DirectWrite factory.
	if (SUCCEEDED(hr))
	{
		hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)&m_pDWriteFactory);
	}
	
	if (SUCCEEDED(hr))
	{
		// 폰트 생성( 폰트 패밀리 이름, 글꼴 컬렉션, 글꼴 두께, 글꼴 스타일, 글꼴 확장, 글꼴 크기, 지역 이름, 생성된 객체 반환 포인터)
		hr = m_pDWriteFactory->CreateTextFormat(
			m_wszFontFamily.c_str(),
			NULL,
			m_FontWeight,
			m_FontStyle,
			DWRITE_FONT_STRETCH_NORMAL,
			20,
			L"en-us",
			&m_pTextFormat
		);
	}
	return hr;
}

HRESULT SDirectWrite::CreateDeviceResources(IDXGISurface1* pSurface)
{
	HRESULT hr = S_OK;
	D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
		D2D1_RENDER_TARGET_TYPE_DEFAULT,
		D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
		static_cast<float>(m_fdpiX),
		static_cast<float>(m_fdpiY));

	hr = m_pD2DFactory->CreateDxgiSurfaceRenderTarget(
		pSurface,
		(const D2D1_RENDER_TARGET_PROPERTIES*)&props,
		&m_pD2DRenderTargetView);

	if (FAILED(hr))
	{
		return hr;
	}

	hr = m_pD2DRenderTargetView->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Yellow), &m_pBlackBrush);
	return S_OK;
}

#pragma endregion

#pragma region Render

bool SDirectWrite::Pre()
{
	if (m_pD2DRenderTargetView)
	{
		m_pD2DRenderTargetView->BeginDraw();
		/*	D2D_MATRIX_3X2_F mat;
			mat._11 = cosf(fTime); mat._12 = -sinf(fTime);
			mat._21 = sinf(fTime); mat._22 = cosf(fTime);
			mat._31 = 400; mat._32=300;*/
		m_pD2DRenderTargetView->SetTransform(D2D1::IdentityMatrix());
		// m_pRT->Clear(D2D1::ColorF(D2D1::ColorF::White));
	}
	return true;
}
bool SDirectWrite::Post()
{
	if (m_pD2DRenderTargetView && FAILED(m_pD2DRenderTargetView->EndDraw()))
	{
		return false;
	}
	return true;
}

// Format 방식
bool SDirectWrite::DrawText(RECT rc, const TCHAR* pText, D2D1::ColorF Color)
{
	if(!Pre()) return false;

	if (m_pD2DRenderTargetView&&m_pBlackBrush)
	{
		D2D1_RECT_F layoutRect = D2D1::RectF(
			static_cast<FLOAT>(rc.left) / m_fDPIScaleX,
			static_cast<FLOAT>(rc.top) / m_fDPIScaleY,
			static_cast<FLOAT>(rc.right) / m_fDPIScaleX,
			static_cast<FLOAT>(rc.bottom) / m_fDPIScaleY);
		m_pBlackBrush->SetColor(Color);
		m_pD2DRenderTargetView->DrawText(pText, (UINT)wcslen(pText), m_pTextFormat, layoutRect, m_pBlackBrush);
	}

	if (!Post()) return false;

	return true;
}
// Layout 방식
bool SDirectWrite::DrawText(D2D1_POINT_2F pos, D2D1::ColorF Color)
{
	if (!Pre()) return false;

	D2D1_POINT_2F origin = D2D1::Point2F(
		static_cast<FLOAT>(pos.x / m_fDPIScaleX),
		static_cast<FLOAT>(pos.y / m_fDPIScaleY));
	m_pBlackBrush->SetColor(Color);
	m_pD2DRenderTargetView->DrawTextLayout(origin, m_pTextLayout, m_pBlackBrush);

	if (!Post()) return false;

	return true;
}

#pragma endregion

#pragma region Release
void SDirectWrite::DiscardDeviceIndependentResources()
{
	SAFE_RELEASE(m_pD2DFactory);
	SAFE_RELEASE(m_pDWriteFactory);
	SAFE_RELEASE(m_pTextFormat);
	SAFE_RELEASE(m_pTextLayout);
}

void SDirectWrite::DiscardDeviceResources()
{
	SAFE_RELEASE(m_pD2DRenderTargetView);
	SAFE_RELEASE(m_pBlackBrush);
}

bool SDirectWrite::Release()
{
	m_wszText.clear();
	m_wszFontFamily.clear();
	DiscardDeviceIndependentResources();
	DiscardDeviceResources();
	return true;
}
#pragma endregion

#pragma region Setter
HRESULT SDirectWrite::SetFont(wchar_t* fontFamily)
{
	HRESULT hr;

	DWRITE_TEXT_RANGE textRange = { 0, m_cTextLength };

	hr = m_pTextLayout->SetFontFamilyName(fontFamily, textRange);

	if (SUCCEEDED(hr))
	{
		m_wszFontFamily.clear();
		m_wszFontFamily = fontFamily;
	}
	return hr;
}

HRESULT SDirectWrite::SetFontSize(float size)
{
	HRESULT hr;
	DWRITE_TEXT_RANGE textRange = {0, m_cTextLength};

	hr = m_pTextLayout->SetFontSize(size, textRange);
	if (SUCCEEDED(hr))
	{
		m_FontSize = size;
	}
	return hr;
}

HRESULT SDirectWrite::SetText(D2D1_POINT_2F size, wchar_t* text, D2D1::ColorF Color)
{
	HRESULT hr = S_OK;

	m_cTextLength = (UINT32)wcslen(text);
	m_wszText.clear();
	m_wszText = text;

	SAFE_RELEASE(m_pTextLayout);

	if (SUCCEEDED(hr))
	{
		// 문자열, 문자열 길이, 문자열 포맷, layout width, layout height, interface pointer
		hr = m_pDWriteFactory->CreateTextLayout(
			m_wszText.c_str(),
			m_cTextLength,
			m_pTextFormat,
			size.x,
			size.y,
			&m_pTextLayout
		);
	}

	// Decklare a typograghy pointer
	IDWriteTypography* pTypography = nullptr;
	// Create a typograghy iterface object;
	if (SUCCEEDED(hr))
	{
		hr = m_pDWriteFactory->CreateTypography(&pTypography);
	}

	// Set the Stylistic set.
	DWRITE_FONT_FEATURE fontFeature = {DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_7, 1};
	if (SUCCEEDED(hr))
	{
		hr = pTypography->AddFontFeature(fontFeature);
	}
	DWRITE_TEXT_RANGE textRange = {0, m_cTextLength};
	
	if (SUCCEEDED(hr))
	{
		hr = m_pTextLayout->SetTypography(pTypography, textRange);
	}

	if (SUCCEEDED(hr))
	{
		hr = m_pTextLayout->SetUnderline(m_fontUnderline, textRange);
	}

	SAFE_RELEASE(pTypography);
	return hr;
}

HRESULT SDirectWrite::SetWeight(DWRITE_FONT_WEIGHT FontWeight)
{
	HRESULT hr;
	DWRITE_TEXT_RANGE textRange = { 0, m_cTextLength };
	
	m_FontWeight = FontWeight;

	hr = m_pTextLayout->SetFontWeight(m_FontWeight, textRange);
	return hr;
}

HRESULT	SDirectWrite::SetItalic(DWRITE_FONT_STYLE FontStyle)
{
	HRESULT hr;
	DWRITE_TEXT_RANGE textRange = { 0, m_cTextLength };

	m_FontStyle = FontStyle;

	hr = m_pTextLayout->SetFontStyle(m_FontStyle, textRange);
	return hr;
}

HRESULT SDirectWrite::SetUnderline(bool underline)
{
	HRESULT hr;
	DWRITE_TEXT_RANGE textRange = { 0, m_cTextLength };

	m_fontUnderline = underline;

	// Set the underline.
	hr = m_pTextLayout->SetUnderline(m_fontUnderline, textRange);
	return hr;
}

bool SDirectWrite::SetPragraghAlighment(DWRITE_PARAGRAPH_ALIGNMENT nPragraghAlighment)
{
	if (m_pTextFormat)
	{
		m_pTextFormat->SetParagraphAlignment(nPragraghAlighment);
		return true;
	}
	return false;
}

bool SDirectWrite::SetTextAlighment(DWRITE_TEXT_ALIGNMENT nTextAlighment)
{
	if (m_pTextFormat)
	{
		m_pTextFormat->SetTextAlignment(nTextAlighment);
		return true;
	}
	return false;
}

void SDirectWrite::OnResize(UINT width, UINT height, IDXGISurface1*	pSurface)
{
	DiscardDeviceResources();
	CreateDeviceResources(pSurface);
	SetText(D2D1::Point2F((FLOAT)width, (FLOAT)height), (wchar_t*)L"SBasisSample!", D2D1::ColorF(1, 1, 1, 1));
}

#pragma endregion

SDirectWrite::SDirectWrite()
{

	m_pBlackBrush = nullptr;
	m_pDWriteFactory = nullptr;
	m_pTextFormat = nullptr;
	m_pD2DFactory = nullptr;
	m_pTextLayout = nullptr;
	m_pD2DRenderTargetView = nullptr;
	m_hWnd = NULL;
	m_cTextLength = 0;
	m_FontWeight = DWRITE_FONT_WEIGHT_NORMAL;
	m_FontStyle = DWRITE_FONT_STYLE_NORMAL;
	m_fontUnderline = FALSE;
	m_FontSize = 72.0f;
}

SDirectWrite::~SDirectWrite()
{
}
