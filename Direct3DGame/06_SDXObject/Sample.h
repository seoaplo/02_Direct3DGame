#pragma once

#include "S3DGameLib.h"
#include "SDXObject.h"

class Sample : public SCore
{
public:
	SDXObject TestObject;
	vector<xTexture> m_TexList;

	ID3D11RasterizerState*	m_pRSWireFrame;
	ID3D11RasterizerState*	m_pRSSolidFrame;
	bool					m_bWireFrameRender;
public:
	//====================================================================
	// ������ �Լ�
	//====================================================================
	bool	Init() override;
	bool	Frame() override;
	bool	Render() override;
	bool	Release() override;
public:
	Sample();
	~Sample();
};

