#pragma once
#include "Object_Handling.h"


class Line_Axis_s : public Object_Handling
{
public:
	bool   Set_VertexData(D3DXVECTOR3 start_in, D3DXVECTOR3 end_in, D3DXVECTOR4 color_in);
	bool   update_ConstantBuffer();

public:
	bool   set_IndexData();

public:
	bool Create(ID3D11Device* pd3dDevice, const TCHAR* pVsFile, const TCHAR* pPsFile, const TCHAR* pTexFile, const char*  pFuntionName);

public:

	D3D_PRIMITIVE_TOPOLOGY m_Primitive;
	 bool	PreRender(ID3D11DeviceContext* pContext, int viewport_number) { PipeLineSetup.PreRender(pContext, viewport_number, m_Primitive);	return true; }

public:
	Line_Axis_s() { m_Primitive = D3D_PRIMITIVE_TOPOLOGY_LINELIST; }
	virtual ~Line_Axis_s() {}
};

bool   Line_Axis_s::update_ConstantBuffer()
{
	g_pContext->UpdateSubresource(PipeLineSetup.m_pConstantBuffer, 0, NULL, &m_Constant_Data, 0, 0);
	return true;
}

bool   Line_Axis_s::set_IndexData()
{

	m_IndexList.resize(2);
	m_IndexList[0] = 0; 
	m_IndexList[1] = 1;   
	m_icount_Indexes = m_IndexList.size();

	return true;

}

bool  Line_Axis_s::Set_VertexData(D3DXVECTOR3 start_in, D3DXVECTOR3 end_in, D3DXVECTOR4 color_in = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f))
{
	m_VertexList.resize(2);
	
	m_VertexList[0].p = start_in;  m_VertexList[0].n = D3DXVECTOR3(0.0f, 0.0f,0.0f); m_VertexList[0].c = color_in; m_VertexList[0].t = D3DXVECTOR2(0.0f, 0.0f);
	m_VertexList[1].p = end_in;  m_VertexList[1].n = D3DXVECTOR3(0.0f, 0.0f, 0.0f); m_VertexList[1].c = color_in; m_VertexList[1].t = D3DXVECTOR2(1.0f, 1.0f);

	m_icount_Vertexs = m_VertexList.size();
	return true;
}



bool    Line_Axis_s::Create(ID3D11Device* pd3dDevice, const TCHAR* pVsFile, const TCHAR* pPsFile, const TCHAR* pTexFile, const char*  pFuntionName)
{
	PipeLineSetup.LoadTexture(pd3dDevice, pTexFile);
	PipeLineSetup.SetBlendState(pd3dDevice);
	PipeLineSetup.CreateVertextBuffer(pd3dDevice, &m_VertexList.at(0), m_icount_Vertexs);
	PipeLineSetup.CreateIndexBuffer(pd3dDevice, &m_IndexList.at(0), m_icount_Indexes);
	PipeLineSetup.CreateConstantBuffer(pd3dDevice, &m_Constant_Data);
	PipeLineSetup.CreateVertexShader(pd3dDevice, pVsFile);
	PipeLineSetup.CreateLayout(pd3dDevice);
	PipeLineSetup.CreatePixelShader(pd3dDevice, pPsFile, pFuntionName);



	PipeLineSetup.ScreenViewPort_setting();
	PipeLineSetup.Create_RasterizerState();
	PipeLineSetup.CreateTexture2D_DepthStencilView();
	PipeLineSetup.CreateDepthStencilState();

	return true;
}