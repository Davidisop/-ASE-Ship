#pragma once
#include "Object_BaseCamp.h"


class Object_Handling : public Object_BaseCamp
{
public:
	

	RECT			m_rtCollision;  // (API와 같음) 오브젝트를 둘러싼 :  윈도우 사각형  =  충돌사각형 
	RECT			m_rtDetection;  // (API와 같음) 오브젝트를 더 크게 둘러싼 :  윈도우 사각형  =  감시 사각형 
	tPoint			m_for_update_Rects;
	tPoint			m_pos; // (API와 같음  ) 오브젝트의 중점 : 순수 윈도우 좌표 
	tPoint			m_pos_past;
public:

	bool  m_bDead;
	bool  m_bDetectRect;

public:

	int      m_Collision_number;

public:

	D3DVECTOR  Gen(float x, float y);
	void   MoveX(float fDis);
	void   MoveY(float fDis);
	void   in_Texture_SetData_factors(float l, float t, float r, float b, float texture_width, float texture_height);
	bool   Window_SetData_factors(float left, float top, float width, float height);
	bool   SetData_RECT(RECT rt);
	void   SetVertexData();
	void   GenCenter();
	void   GenCenter_Rects_Adapt_new();
	


public:

	Object_Handling() : m_bDead(false), m_bDetectRect(false), m_Collision_number(0)
	{
		
	}
	virtual ~Object_Handling() {}
};




D3DVECTOR  Object_Handling::Gen(float x, float y)
{
	D3DVECTOR D3DVECTOR_Gen;   // float x;float y;float z; // // 0 ~ 900 - > 0 ~1 -> -1 ~ +1
	D3DVECTOR_Gen.x = x / g_rtClient.right;
	D3DVECTOR_Gen.y = y / g_rtClient.bottom;
	D3DVECTOR_Gen.x = D3DVECTOR_Gen.x * 2.0f - 1.0f;
	D3DVECTOR_Gen.y = (D3DVECTOR_Gen.y * 2 - 1.0f)*-1.0f;
	return D3DVECTOR_Gen;
}


void  Object_Handling::in_Texture_SetData_factors(float l, float t, float r, float b, float texture_width, float texture_height)
{
	m_VertexList[0].t.x = (l) / texture_width;
	m_VertexList[0].t.y = (t) / texture_height;
	m_VertexList[1].t.x = (l + r) / texture_width;
	m_VertexList[1].t.y = (t) / texture_height;
	m_VertexList[2].t.x = (l) / texture_width;
	m_VertexList[2].t.y = (t + b) / texture_height;
	m_VertexList[3].t.x = (l) / texture_width;
	m_VertexList[3].t.y = (t + b) / texture_height;
	m_VertexList[4].t.x = (l + r) / texture_width;
	m_VertexList[4].t.y = (t) / texture_height;
	m_VertexList[5].t.x = (l + r) / texture_width;
	m_VertexList[5].t.y = (t + b) / texture_height;
}



bool    Object_Handling::Window_SetData_factors(float left, float top, float width, float height)
{
	//1)초기데이터 세팅

	RECT rt;
	
	rt.left = left;  rt.top = top;
	rt.right = width; rt.bottom = height;

	SetData_RECT(rt);
	return true;
}

bool    Object_Handling::SetData_RECT(RECT rt)
{
	//1)초기데이터 세팅

	m_rtCollision.left = rt.left;
	m_rtCollision.right = rt.right;
	m_rtCollision.top = rt.top;
	m_rtCollision.bottom = rt.bottom;

	m_rtDetection.left = m_rtCollision.left - 100;
	m_rtDetection.top = m_rtCollision.top - 100;
	m_rtDetection.right = m_rtCollision.right + 100;
	m_rtDetection.bottom = m_rtCollision.bottom + 100;

	SetVertexData();
	return true;
}

void   Object_Handling::SetVertexData()
{
	D3DVECTOR pos;
	pos = Gen(m_rtCollision.left, m_rtCollision.top); // DX 좌표화
	m_VertexList[0].p.x = pos.x; m_VertexList[0].p.y = pos.y;  m_VertexList[0].p.z = 0.0f;

	pos = Gen(m_rtCollision.left + m_rtCollision.right, m_rtCollision.top); // DX 좌표화 : 근데 왜, left + right를 하지? right이 width 이니까.
	m_VertexList[1].p.x = pos.x; m_VertexList[1].p.y = pos.y;  m_VertexList[1].p.z = 0.0f;
	
	pos = Gen(m_rtCollision.left, m_rtCollision.top + m_rtCollision.bottom); // DX 좌표화 : 근데 왜, top + bottom을 하지? bottom이 height이니까.
	m_VertexList[2].p.x = pos.x; m_VertexList[2].p.y = pos.y;  m_VertexList[2].p.z = 0.0f;
	
	m_VertexList[3] = m_VertexList[2];                    // 왜 [2]와 같다 하지? 
	m_VertexList[4] = m_VertexList[1];

	pos = Gen(m_rtCollision.left + m_rtCollision.right, m_rtCollision.top + m_rtCollision.bottom);
	m_VertexList[5].p.x = pos.x; m_VertexList[5].p.y= pos.y;  m_VertexList[5].p.z = 0.0f;

	GenCenter();
}


void Object_Handling::GenCenter_Rects_Adapt_new()
{
	m_pos.x = (m_vCenter.x + 1) * g_rtClient.right/2;
	m_pos.y = ((-1)*m_vCenter.y + 1.0f) * g_rtClient.bottom/2;


	m_rtCollision.left = m_pos.x - m_for_update_Rects.x / 2;
	m_rtCollision.right = m_pos.x + m_for_update_Rects.x / 2;
	m_rtCollision.top = m_pos.y + m_for_update_Rects.y/2;
	m_rtCollision.bottom = m_pos.y - m_for_update_Rects.y / 2;

	m_rtDetection.left = m_rtCollision.left - 165;
	m_rtDetection.top = m_rtCollision.top - 100;
	m_rtDetection.right = m_rtCollision.right + 100;
	m_rtDetection.bottom = m_rtCollision.bottom + 100;

}


void    Object_Handling::GenCenter()
{
	m_vCenter.x = 0.0f;    //x,y,z
	m_vCenter.y = 0.0f;

	for (int iV = 0; iV < 6; iV++)
	{
		D3DVECTOR vertex;
		vertex.x = m_VertexList[iV].p.x;
		vertex.y = m_VertexList[iV].p.y;
		vertex.z = m_VertexList[iV].p.z;
		m_vCenter.x += vertex.x;
		m_vCenter.y += vertex.y;
	}

	m_vCenter.x /= 6.0f;
	m_vCenter.y /= 6.0f;

	GenCenter_Rects_Adapt_new();
}

void   Object_Handling::MoveX(float fDis)
{
	m_pos_past.x = m_pos.x;
	for (int iV = 0; iV < 6; iV++) { m_VertexList[iV].p.x += fDis; }
	GenCenter();
}

void   Object_Handling::MoveY(float fDis)
{
	m_pos_past.y = m_pos.y;
	for (int iV = 0; iV < 6; iV++) { m_VertexList[iV].p.y += fDis; }
	GenCenter();
}

