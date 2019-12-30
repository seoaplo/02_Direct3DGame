#pragma once
#include "SDXShape.h"

struct S_RAY
{
	float			fExtent;
	D3DXVECTOR3		vOrigin;
	D3DXVECTOR3		vDirection;
	S_RAY()
	{
		//  fExtent > 0 : 세그먼트로 사용 
		fExtent = -1;
	}
};


class SSelect
{
public:
	S_RAY		m_Ray;
	D3DXVECTOR3 m_vIntersection;
	D3DXMATRIX  m_matWorld;
	D3DXMATRIX  m_matView;
	D3DXMATRIX  m_matProj;
	D3DXVECTOR3 m_vDxR;
	D3DXVECTOR3 m_vCenter;
	DWORD		m_dwClientWidth;
	DWORD		m_dwClientHeight;
	HWND		m_hWnd;
public:
	D3DXVECTOR3		m_vSrcVex[3];
	float			m_fPickDistance;
	D3DXMATRIX		m_matWorldPick;
	D3DXVECTOR3 qvec;
	D3DXVECTOR3 tvec;
	D3DXVECTOR3 pvec;
public:
	void Init(HWND hWnd, DWORD dwClientWidth, DWORD dwClientHeight);
	void SetMatrix(D3DXMATRIX* pWorld, D3DXMATRIX* pView, D3DXMATRIX* pProj);
	void Update();
	bool IntersectRayToSphere(S_SPHERE* pSphere, S_RAY* pRay = NULL);

	bool ChkOBBToRay(S_BOX* pBox, S_RAY* pRay = NULL);
	bool IntersectBox(S_BOX* pBox, S_RAY* pRay = NULL);

	bool AABBtoRay(S_BOX* a, S_RAY* pRay = NULL);
	bool OBBtoRay(S_BOX* a, S_RAY* pRay = NULL);
	D3DXVECTOR3 GetIntersection();

	// 교점을 찾는다.
	bool    GetIntersection(
		const D3DXVECTOR3& vStart,
		const D3DXVECTOR3& vEnd,
		const D3DXVECTOR3& vNormal,
		const D3DXVECTOR3& v0,
		const D3DXVECTOR3& v1,
		const D3DXVECTOR3& v2);
	// 폴리곤안에 위치하는지 판단한다.
	bool    PointInPolygon(
		const D3DXVECTOR3& vert,
		const D3DXVECTOR3& faceNormal,
		const D3DXVECTOR3& v0,
		const D3DXVECTOR3& v1,
		const D3DXVECTOR3& v2);
	bool	ChkPick(D3DXVECTOR3& v0, D3DXVECTOR3& v1, D3DXVECTOR3& v2);
	bool	IntersectTriangle(const D3DXVECTOR3& orig, const D3DXVECTOR3& dir,
		D3DXVECTOR3& v0, D3DXVECTOR3& v1, D3DXVECTOR3& v2,
		FLOAT* t, FLOAT* u, FLOAT* v);

public:
	SSelect();
	virtual ~SSelect();
};

