#pragma once
#include "Object_Handling.h"


struct F_Plane
{
	float a, b, c, d;

	void CreatePlane(D3DXVECTOR3 v0, D3DXVECTOR3 v1, D3DXVECTOR3 v2)
	{
		D3DXVECTOR3 vEdge0 = v1 - v0;
		D3DXVECTOR3 vEdge1 = v2 - v0;
		D3DXVECTOR3 vNormal;
		D3DXVec3Cross(&vNormal, &vEdge0, &vEdge1);

		a = vNormal.x;
		b = vNormal.y;
		c = vNormal.z;
		d = -(v0.x*a + v0.y*b + v0.z*c);

	}

	void CreatePlane(D3DXVECTOR3 vNormal, D3DXVECTOR3 v0)
	{
		D3DXVec3Normalize(&vNormal, &vNormal);

		a = vNormal.x;
		b = vNormal.y;
		c = vNormal.z;
		d = -(v0.x*a + v0.y*b + v0.z*c);
	}

	F_Plane() { a = b = c = d = 0.0f; }
};





class Frustum_Box : public Object_Handling
{
public:

	F_Plane			m_Plane[6];

public:


	D3DXMATRIX m_mView_for_making_Frustum;
	D3DXMATRIX m_mProj_for_making_Frustum;

	bool total_Vertex_Update_for_Frustum(D3DXMATRIX* pView, D3DXMATRIX* pProj)
	{
		Set_VertexData();
		Set_View_Proj_Matrixs(pView, pProj);
		Inverse_view_projection_matrix_multiply_to_Vertexs();
		Create_Frustum_Planes();
		g_pContext->UpdateSubresource(PipeLineSetup.m_pVertextBuffer, 0, NULL, &m_VertexList[0], 0, 0);

		return true;
	}

	void   Set_View_Proj_Matrixs(D3DXMATRIX* pView, D3DXMATRIX* pProj)
	{
		m_mView_for_making_Frustum = *pView;
		m_mProj_for_making_Frustum = *pProj;
	}
	bool Inverse_view_projection_matrix_multiply_to_Vertexs()
	{

		D3DXMATRIX Inverse_View_Projection_mats;
		D3DXMatrixMultiply(&Inverse_View_Projection_mats, &m_mView_for_making_Frustum, &m_mProj_for_making_Frustum);
		D3DXMatrixInverse(&Inverse_View_Projection_mats, NULL, &Inverse_View_Projection_mats);

		//
		D3DXVec3TransformCoord(&m_VertexList[0].p, &m_VertexList[0].p, &Inverse_View_Projection_mats);
		D3DXVec3TransformCoord(&m_VertexList[1].p, &m_VertexList[1].p, &Inverse_View_Projection_mats);
		D3DXVec3TransformCoord(&m_VertexList[2].p, &m_VertexList[2].p, &Inverse_View_Projection_mats);
		D3DXVec3TransformCoord(&m_VertexList[3].p, &m_VertexList[3].p, &Inverse_View_Projection_mats);
		D3DXVec3TransformCoord(&m_VertexList[4].p, &m_VertexList[4].p, &Inverse_View_Projection_mats);
		D3DXVec3TransformCoord(&m_VertexList[5].p, &m_VertexList[5].p, &Inverse_View_Projection_mats);
		D3DXVec3TransformCoord(&m_VertexList[6].p, &m_VertexList[6].p, &Inverse_View_Projection_mats);
		D3DXVec3TransformCoord(&m_VertexList[7].p, &m_VertexList[7].p, &Inverse_View_Projection_mats);
		return true;
	}

	bool    Create_Frustum_Planes()
	{
	
		m_Plane[0].CreatePlane(m_VertexList[0].p, m_VertexList[1].p, m_VertexList[2].p);// -z // 노멀들이 다 바깥쪽을 향하고 있다.
		m_Plane[1].CreatePlane(m_VertexList[6].p, m_VertexList[5].p, m_VertexList[4].p);// +z
		m_Plane[2].CreatePlane(m_VertexList[2].p, m_VertexList[6].p, m_VertexList[7].p);// +x
		m_Plane[3].CreatePlane(m_VertexList[5].p, m_VertexList[1].p, m_VertexList[0].p);// -x
		m_Plane[4].CreatePlane(m_VertexList[1].p, m_VertexList[5].p, m_VertexList[6].p);// -z
		m_Plane[5].CreatePlane(m_VertexList[0].p, m_VertexList[3].p, m_VertexList[7].p);// +z
		return true;
		
	}


	bool	ClassifyPoint(D3DXVECTOR3 v) //점 1개를 집어넣어서
	{
		for (int iPlane = 0; iPlane < 6; iPlane++)
		{
			float fDistance = m_Plane[iPlane].a *v.x + m_Plane[iPlane].b *	v.y + m_Plane[iPlane].c *	v.z + m_Plane[iPlane].d;

			if (fDistance > 0.0f)
			{
				return false; // 기본적으로 노멀들이 다 바깥쪽을 향하고 있는 것이다.
			}
		}
		return true;
	}


	bool	ClassifySphere(D3DXVECTOR3 Sphere_Center, float fRadius)//
	{
		for (int iPlane = 0; iPlane < 6; iPlane++)
		{
			float fDistance = m_Plane[iPlane].a *Sphere_Center.x + m_Plane[iPlane].b *	Sphere_Center.y + m_Plane[iPlane].c *	Sphere_Center.z + m_Plane[iPlane].d;

			if (fDistance > fRadius)
			{
				return false;
			}
		}
		return true; // 내부에 있는 걸로 간주. Draw 한다.

	}





public:
	bool   Set_VertexData();
	bool   update_ConstantBuffer();
	bool   set_IndexData();

public:
	bool Create(ID3D11Device* pd3dDevice, const TCHAR* pVsFile, const TCHAR* pPsFile, const TCHAR* pTexFile, const char*  pFuntionName);


public:
	Frustum_Box() {
		D3DXMatrixIdentity(&m_mView_for_making_Frustum);
		D3DXMatrixIdentity(&m_mProj_for_making_Frustum);
	}
	virtual ~Frustum_Box() {}
};

bool   Frustum_Box::update_ConstantBuffer()
{
	g_pContext->UpdateSubresource(PipeLineSetup.m_pConstantBuffer, 0, NULL, &m_Constant_Data, 0, 0);

	return true;
}



bool  Frustum_Box::Set_VertexData()
{
	m_VertexList.resize(8);
	int iner_index = 0;

	//앞면

	m_VertexList[iner_index++] = SimpleVertex(D3DXVECTOR3(-1.0f, -1.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR4(1.0f, 1.0f, 0.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f));
	m_VertexList[iner_index++] = SimpleVertex(D3DXVECTOR3(-1.0f, 1.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR4(1.0f, 0.5f, 0.0f, 1.0f), D3DXVECTOR2(0.0f, 1.0f));
	m_VertexList[iner_index++] = SimpleVertex(D3DXVECTOR3(1.0f, 1.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR4(0.5f, 0.0f, 1.0f, 1.0f), D3DXVECTOR2(1.0f, 0.0f));
	m_VertexList[iner_index++] = SimpleVertex(D3DXVECTOR3(1.0f, -1.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR4(0.5f, 1.0f, 0.5f, 1.0f), D3DXVECTOR2(0.0f, 1.0f));

	// 뒷면
	m_VertexList[iner_index++] = SimpleVertex(D3DXVECTOR3(-1.0f, -1.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR4(0.5f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f));
	m_VertexList[iner_index++] = SimpleVertex(D3DXVECTOR3(-1.0f, 1.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR4(1.0f, 0.0f, 1.0f, 1.0f), D3DXVECTOR2(1.0f, 1.0f));
	m_VertexList[iner_index++] = SimpleVertex(D3DXVECTOR3(1.0f, 1.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f));
	m_VertexList[iner_index++] = SimpleVertex(D3DXVECTOR3(1.0f, -1.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR4(0.5f, 1.0f, 0.5f, 1.0f), D3DXVECTOR2(1.0f, 1.0f));


	m_icount_Vertexs = m_VertexList.size();

	return true;
}



bool   Frustum_Box::set_IndexData()
{

	m_IndexList.resize(36);
	int iner_index = 0;

	//FRONT
	m_IndexList[iner_index++] = 0;	m_IndexList[iner_index++] = 1;	m_IndexList[iner_index++] = 2;	m_IndexList[iner_index++] = 0;	m_IndexList[iner_index++] = 2;	m_IndexList[iner_index++] = 3;
	//BACK
	m_IndexList[iner_index++] = 6;	m_IndexList[iner_index++] = 5;	m_IndexList[iner_index++] = 4;	m_IndexList[iner_index++] = 6;	m_IndexList[iner_index++] = 4;	m_IndexList[iner_index++] = 7;
	//RIGHT
	m_IndexList[iner_index++] = 2;	m_IndexList[iner_index++] = 6;	m_IndexList[iner_index++] = 7;	m_IndexList[iner_index++] = 2;	m_IndexList[iner_index++] = 7;	m_IndexList[iner_index++] = 3;
	//LEFT
	m_IndexList[iner_index++] = 5;	m_IndexList[iner_index++] = 1;	m_IndexList[iner_index++] = 0;	m_IndexList[iner_index++] = 5;	m_IndexList[iner_index++] = 0;	m_IndexList[iner_index++] = 4;
	//UP
	m_IndexList[iner_index++] = 1;	m_IndexList[iner_index++] = 5;	m_IndexList[iner_index++] = 6;	m_IndexList[iner_index++] = 1;	m_IndexList[iner_index++] = 6;	m_IndexList[iner_index++] = 2;
	//DOWN
	m_IndexList[iner_index++] = 0;	m_IndexList[iner_index++] = 3;	m_IndexList[iner_index++] = 7;	m_IndexList[iner_index++] = 0;	m_IndexList[iner_index++] = 7;	m_IndexList[iner_index++] = 4;

	m_icount_Indexes = m_IndexList.size();

	return true;
}

bool    Frustum_Box::Create(ID3D11Device* pd3dDevice, const TCHAR* pVsFile, const TCHAR* pPsFile, const TCHAR* pTexFile, const char*  pFuntionName)
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