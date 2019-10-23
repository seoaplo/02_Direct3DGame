#include "SArcBall.h"

void SArcBall::Reset()
{
	D3DXQuaternionIdentity(&m_qDown);
	D3DXQuaternionIdentity(&m_qNow);
	D3DXMatrixIdentity(&m_mRotation);
	D3DXMatrixIdentity(&m_mTranslation);
	D3DXMatrixIdentity(&m_mTranslationDelta);
	m_bDrag = false;
	m_fRadiusTranslation = 1.0f;
	m_fRadius = 1.0f;
}

void SArcBall::SetWindow(int nWidth, int nHeight, float fRadius)
{
	m_nWidth = nWidth;
	m_nHeight = nHeight;
	m_fRadius = fRadius;
	m_vCenter = D3DXVECTOR2(m_nWidth / 2.0f, m_nHeight / 2.0f);
}

void SArcBall::SetOffset(int nX, int nY)
{
	m_Offset.x = nX;
	m_Offset.y = nY;
}
//=====================================================================
// 모델의 중점을 구 방정식의 중점으로 보고 반구를 기준으로 화면의 넓이와 높이로 일치시켜
// 화면의 넓이와 높이에 대한 구 성분 값에 따라 최종 구방정식에 따라 성분값을 구한다.
D3DXVECTOR3 SArcBall::ScreenToVector(float fScreenPtX, float fScreenPtY)
{
	// Scale to screen X*X + Y*Y + Z*Z = R * R(구면 방정식)
	// 구 반지름 : 화면 넓이 = 구 성분값 : 화면 성분값
	// 구 성분값 = (구 반지름 * 화면 성분값) / 화면 넓이
	float x = -(fScreenPtX - m_Offset.x - m_nWidth / 2) / (m_fRadius * m_nWidth / 2);
	float y = (fScreenPtY - m_Offset.y - m_nHeight / 2) / (m_fRadius * m_nHeight / 2);

	float z = 0.0f;
	// 구 z성분값 = 구 x성분값 제곱 * 구 y성분값 제곱
	float mag = x * x + y * y;

	// 화면넓이를 구 반지름 1로 두었기 때문에 1보다 크면 화면 밖이기 때문에 가까운 값으로 대체한다.
	// X*X + Y*Y + Z*Z = 1(구면 방정식)
	if (mag > 1.0f)
	{
		float scale = 1.0f / sqrtf(mag);
		x *= scale;
		y *= scale;
	}
	else
	{
		// Z성분값 제곱 = 1 - (X성분값 제곱 + Y성분값 제곱)
		z = sqrtf(1.0f - mag);
	}

	return D3DXVECTOR3(x, y, z);
}

D3DXQUATERNION SArcBall::QuatFromBallPoints(const D3DXVECTOR3 &vFrom, const D3DXVECTOR3 &vTo)
{
	// 임의의 축과 사원수의 관계는 (회전각 : 회전각/2)이기 때문에
	// 아래와 같이 사원수를 쓰면 두배의 값으로 회전된다.
	// 이는 화면 가로 및 세로 한계를 1 ~ -1로 두었기 때문에 코사인 1(90도)에서 2배인 180로 회전된다.
	D3DXVECTOR3 vPart;
	float fDot = D3DXVec3Dot(&vFrom, &vTo); // 회전 각
	D3DXVec3Cross(&vPart, &vFrom, &vTo); // 회전 축

	return D3DXQUATERNION(vPart.x, vPart.y, vPart.z, fDot);
}

void SArcBall::OnBegin(int nX, int nY)
{
	// Only enter the drag state if the click falls
	// inside the clic rectangle
	if (nX >= m_Offset.x &&
		nX < m_Offset.x + m_nWidth &&
		nY >= m_Offset.y &&
		nY < m_Offset.y + m_nHeight)
	{
		m_bDrag = true;
		m_qDown = m_qNow;
		m_vDownPt = ScreenToVector((float)nX, (float)nY);
	}
}

void SArcBall::OnMove(int nX, int nY)
{
	if (m_bDrag)
	{
		m_vCurrentPt = ScreenToVector((float)nX, (float)nY);
		// 현재 회전된 사원수와 현재 설정된 사원수를 곱하여 누적시킨다.
		m_qNow = m_qDown * QuatFromBallPoints(m_vDownPt, m_vCurrentPt);
	}
	m_ptLastMouse.x = nX;
	m_ptLastMouse.y = nY;
}

void SArcBall::OnEnd()
{
	m_bDrag = false;
}


SArcBall::SArcBall()
{
	Reset();
	m_vDownPt = D3DXVECTOR3(0, 0, 0);
	m_vCurrentPt = D3DXVECTOR3(0, 0, 0);
	m_Offset.x = m_Offset.y = 0;

	// 임시로 클라이언트 좌표 기준으로 화면의 넓이와 높이를 세팅한다.
	// 그러나 실제로 적용될때 디바이스의 넓이와 높이로 수정되어 적용된다.

	RECT rc;
	GetClientRect(GetForegroundWindow(), &rc);
	SetWindow(rc.right, rc.bottom);
}


SArcBall::~SArcBall()
{
}
