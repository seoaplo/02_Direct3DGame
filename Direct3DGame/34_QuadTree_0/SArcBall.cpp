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
// ���� ������ �� �������� �������� ���� �ݱ��� �������� ȭ���� ���̿� ���̷� ��ġ����
// ȭ���� ���̿� ���̿� ���� �� ���� ���� ���� ���� �������Ŀ� ���� ���а��� ���Ѵ�.
D3DXVECTOR3 SArcBall::ScreenToVector(float fScreenPtX, float fScreenPtY)
{
	// Scale to screen X*X + Y*Y + Z*Z = R * R(���� ������)
	// �� ������ : ȭ�� ���� = �� ���а� : ȭ�� ���а�
	// �� ���а� = (�� ������ * ȭ�� ���а�) / ȭ�� ����
	float x = -(fScreenPtX - m_Offset.x - m_nWidth / 2) / (m_fRadius * m_nWidth / 2);
	float y = (fScreenPtY - m_Offset.y - m_nHeight / 2) / (m_fRadius * m_nHeight / 2);

	float z = 0.0f;
	// �� z���а� = �� x���а� ���� * �� y���а� ����
	float mag = x * x + y * y;

	// ȭ����̸� �� ������ 1�� �ξ��� ������ 1���� ũ�� ȭ�� ���̱� ������ ����� ������ ��ü�Ѵ�.
	// X*X + Y*Y + Z*Z = 1(���� ������)
	if (mag > 1.0f)
	{
		float scale = 1.0f / sqrtf(mag);
		x *= scale;
		y *= scale;
	}
	else
	{
		// Z���а� ���� = 1 - (X���а� ���� + Y���а� ����)
		z = sqrtf(1.0f - mag);
	}

	return D3DXVECTOR3(x, y, z);
}

D3DXQUATERNION SArcBall::QuatFromBallPoints(const D3DXVECTOR3 &vFrom, const D3DXVECTOR3 &vTo)
{
	// ������ ��� ������� ����� (ȸ���� : ȸ����/2)�̱� ������
	// �Ʒ��� ���� ������� ���� �ι��� ������ ȸ���ȴ�.
	// �̴� ȭ�� ���� �� ���� �Ѱ踦 1 ~ -1�� �ξ��� ������ �ڻ��� 1(90��)���� 2���� 180�� ȸ���ȴ�.
	D3DXVECTOR3 vPart;
	float fDot = D3DXVec3Dot(&vFrom, &vTo); // ȸ�� ��
	D3DXVec3Cross(&vPart, &vFrom, &vTo); // ȸ�� ��

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
		// ���� ȸ���� ������� ���� ������ ������� ���Ͽ� ������Ų��.
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

	// �ӽ÷� Ŭ���̾�Ʈ ��ǥ �������� ȭ���� ���̿� ���̸� �����Ѵ�.
	// �׷��� ������ ����ɶ� ����̽��� ���̿� ���̷� �����Ǿ� ����ȴ�.

	RECT rc;
	GetClientRect(GetForegroundWindow(), &rc);
	SetWindow(rc.right, rc.bottom);
}


SArcBall::~SArcBall()
{
}
