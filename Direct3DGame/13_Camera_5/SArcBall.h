#pragma once
#include "SUtils.h"
#include "SDxBasic.h"

class SArcBall
{
private:
	D3DXMATRIXA16	m_mRotation;			// Matrix for arc ball's orientation
	D3DXMATRIXA16	m_mTranslation;			// Matrix for arc ball's position
	D3DXMATRIXA16	m_mTranslationDelta;	// Matrix for arc ball's position

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
	
	PROPERTY_G(GetTranslationMatrix) D3DXMATRIX& _mTranslation;
	const D3DXMATRIX&	GetTranslationMatrix()const			{ return m_mTranslation; }

	PROPERTY_G(GetTranslationDeltaMatrix) D3DXMATRIX& _mTranslationDelta;
	const D3DXMATRIX&	GetTranslationDeltaMatrix()const	{ return m_mTranslationDelta; }

	PROPERTY_G(isBeingDragged) bool _bDrag;
	bool				isBeingDragged() const				{ return m_bDrag; }

	PROPERTY_S(SetTranslationRadius) float& _fRadiusTranslation;
	void	SetTranslationRadius(float fRadiusTranslation) { m_fRadiusTranslation = fRadiusTranslation; }

	PROPERTY(GetQuatNow, SetQuatNow) D3DXQUATERNION _qNow;
	D3DXQUATERNION		GetQuatNow() const					{ return m_qNow; }
	void				SetQuatNow(D3DXQUATERNION q)		{ m_qNow = q; }
public:
	SArcBall();
	~SArcBall();
};

