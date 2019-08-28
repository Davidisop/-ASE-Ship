#pragma once
#include "Object_Handling.h"


class Plane : public Object_Handling
{
public:
	bool   Set_VertexData();
	bool   set_IndexData();
	bool   update_ConstantBuffer();

public:
	bool Create(ID3D11Device* pd3dDevice, const TCHAR* pVsFile, const TCHAR* pPsFile, const TCHAR* pTexFile);


public:
	Plane() {}
	virtual ~Plane() {}
};

bool   Plane::update_ConstantBuffer()
{

	g_pContext->UpdateSubresource(PipeLineSetup.m_pConstantBuffer, 0, NULL, &m_Constant_Data, 0, 0);

	return true;
}



bool   Plane::set_IndexData()
{

	m_IndexList.resize(6);
	int iner_index = 0;

	m_IndexList[iner_index++] = 0;
	m_IndexList[iner_index++] = 1;
	m_IndexList[iner_index++] = 2;
	m_IndexList[iner_index++] = 2;
	m_IndexList[iner_index++] = 1;
	m_IndexList[iner_index++] = 3;

	m_icount_Indexes = m_IndexList.size();

	return true;

}

bool  Plane:: Set_VertexData()
{
	

	m_VertexList.resize(4);

	int iner_index = 0;
	m_VertexList[iner_index++] = { D3DXVECTOR3(-1.0f, 1.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.5f), D3DXVECTOR2(0.0f, 0.0f) };
	m_VertexList[iner_index++] = { D3DXVECTOR3(1.0f, 1.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), D3DXVECTOR4(1.0f, 0.0f, 1.0f, 0.5f), D3DXVECTOR2(1.0f, 0.0f) };
	m_VertexList[iner_index++] = { D3DXVECTOR3(-1.0f, -1.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), D3DXVECTOR4(0.0f, 1.0f, 0.0f, 0.5f), D3DXVECTOR2(0.0f, 1.0f) };
	m_VertexList[iner_index++] = { D3DXVECTOR3(1.0f, -1.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), D3DXVECTOR4(1.0f, 1.0f, 1.0f, 0.5f), D3DXVECTOR2(1.0f, 1.0f) };

	m_icount_Vertexs = m_VertexList.size();

	return true;
}






bool    Plane::Create(ID3D11Device* pd3dDevice, const TCHAR* pVsFile, const TCHAR* pPsFile, const TCHAR* pTexFile)
{
	PipeLineSetup.LoadTexture(pd3dDevice, pTexFile);
	PipeLineSetup.SetBlendState(pd3dDevice);
	PipeLineSetup.CreateVertextBuffer(pd3dDevice, &m_VertexList.at(0), m_icount_Vertexs);
	PipeLineSetup.CreateIndexBuffer(pd3dDevice, &m_IndexList.at(0), m_icount_Indexes);
	PipeLineSetup.CreateConstantBuffer(pd3dDevice, &m_Constant_Data);
	PipeLineSetup.CreateVertexShader(pd3dDevice, pVsFile);
	PipeLineSetup.CreateLayout(pd3dDevice);
	PipeLineSetup.CreatePixelShader(pd3dDevice, pPsFile);



	PipeLineSetup.ScreenViewPort_setting();
	PipeLineSetup.Create_RasterizerState();
	PipeLineSetup.CreateTexture2D_DepthStencilView();
	PipeLineSetup.CreateDepthStencilState();

	return true;
}