#pragma once
#include "SUtils.h"
#include "SDxBasic.h"

class SArcBall
{
private:
	D3DXMATRIX		m_mRotation;			// Matrix for arc ball's orientation
	D3DXMATRIX		m_mTranslation;			// Matrix for arc ball's position
	D3DXMATRIX		m_mTranslationDelta;	// Matrix for arc ball's position

	POINT			m_Offset;				// window offset, or upper-left corner of window
	UINT			m_nWidth;				// arc ball's window width
	UINT			m_nHeight;				// arc ball's window height
	D3DXVECTOR2		m_vCenter;				// center of arc ball
	float			m_fRadius;				// arc ball's radius in screen coords
	float			m_fRadiusTranslation;	// arc ball's radius for translating the target

	D3DXQUATERNION	m_qDown;				// Quaternion before button down
	D3DXQUATERNION	m_qNow;					// Composite quaternion for current drag
	bool			m_bDrag;				// Whether user is dragging arc ball

	POINT			m_ptLastMouse;			// position of last mouse point
	D3DXVECTOR3		m_vDownPt;				// stating point of rotation arc
	D3DXVECTOR3		m_vCurrentPt;			// current point of rotation arc
public:

	D3DXVECTOR3						ScreenToVector(float fScreenPtX, float fScreenPtY);
	static	D3DXQUATERNION	WINAPI	QuatFromBallPoints(const D3DXVECTOR3 &vForm, const D3DXVECTOR3 &vTo);
public:
	// Functions to change behavior
	void	Reset();
	void	SetWindow(int nWidth, int nHeight, float fRadius = 0.9f);
	void	SetOffset(int nX, int nY);

	// Call these from client and use GetRotationMatrix() to read new rotation matrix
	void	OnBegin(int nX, int nY);	// start the rotation(pass current mouse position)
	void	OnMove(int nX, int nY);	// continue the rotation
	void	OnEnd();					// end the rotation

	// Or call this to automatically handle left, middle right buttons
	//LRESULT		HandleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	// Functions to get/set state
	const D3DXMATRIX*	GetRotationMatrix()					{ return D3DXMatrixRotationQuaternion(&m_mRotation, &m_qNow); }
	const D3DXMATRIX&	GesTRanslationMatrix()const			{ return m_mTranslation; }
	const D3DXMATRIX&	GesTRanslationDeltaMatrix()const	{ return m_mTranslationDelta; }
	bool				isBeingDragged() const				{ return m_bDrag; }
	void	SesTRanslationRadius(float fRadiusTranslation) { m_fRadiusTranslation = fRadiusTranslation; }
	D3DXQUATERNION		GetQuatNow() const					{ return m_qNow; }
	void				SetQuatNow(D3DXQUATERNION q)		{ m_qNow = q; }

	_declspec(property(get = GesTRanslationMatrix))			D3DXMATRIX& _mTranslation;
	_declspec(property(get = GesTRanslationDeltaMatrix))	D3DXMATRIX& _mTranslationDelta;
	_declspec(property(get = isBeingDragged))				bool _bDrag;
	_declspec(property(put = SesTRanslationRadius)) float& _fRadiusTranslation;
	_declspec(property(get = GetQuatNow, put = SetQuatNow)) D3DXQUATERNION _qNow;

public:
	SArcBall();
	~SArcBall();
};

