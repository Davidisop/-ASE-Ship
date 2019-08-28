#include "PipeLineSetup.h"
#include "Canon_Camera.h"

class Object_BaseCamp
{
public:
	PipeLineSetup  PipeLineSetup;
	D3DVECTOR      m_vCenter;

public:

	D3DXVECTOR3      m_vPosition;
	D3DXVECTOR3      m_vLook;
	D3DXVECTOR3      m_vSide;
	D3DXVECTOR3      m_vUp;

	bool object_position_look_side_up_extract_From_WorldMatrix()
	{
		m_vPosition.x = m_matWorld._41;
		m_vPosition.y = m_matWorld._42;
		m_vPosition.z = m_matWorld._43;
		m_vSide.x = m_matWorld._11;
		m_vSide.y = m_matWorld._12;
		m_vSide.z = m_matWorld._13;
		m_vUp.x = m_matWorld._21;
		m_vUp.y = m_matWorld._22;
		m_vUp.z = m_matWorld._23;
		m_vLook.x = m_matWorld._31;
		m_vLook.y = m_matWorld._32;
		m_vLook.z = m_matWorld._33;
		D3DXVec3Normalize(&m_vLook, &m_vLook);
		D3DXVec3Normalize(&m_vSide, &m_vSide);
		D3DXVec3Normalize(&m_vUp, &m_vUp);
	
		return true;
	}

public:
	vector<SimpleVertex> m_VertexList;
	UINT				 m_icount_Vertexs;

public:

	vector<WORD>		m_IndexList;
	UINT				m_icount_Indexes;

public:
	T_CB_DATA		m_Constant_Data;

public:// 사물 자체 월드

	D3DXMATRIX		m_matWorld;
	void			transposition(D3DXVECTOR3 position_move_in);
	void			transposition_real_time(D3DXVECTOR3 position_move_in);

public: //카메라와 연동
	
	D3DXMATRIX	m_matView;
	D3DXMATRIX	m_matProj;

	void HandOver_world_view_projection_Matrixs_to_ConstantData_Final_extract(D3DXMATRIX* pWorld, D3DXMATRIX* pView, D3DXMATRIX* pProj);


public:
	virtual bool	Init() { return true;}
	virtual bool	Frame(float fGameTimer, float fFps)	{return true;}
	virtual bool	PreRender(ID3D11DeviceContext* pContext, int viewport_number)	
					{ PipeLineSetup.PreRender(pContext, viewport_number);	return true;}
	virtual bool	Render(ID3D11DeviceContext* pContext, int viewport_number) 
					{ PipeLineSetup.Render(pContext, m_icount_Vertexs, m_icount_Indexes, viewport_number); return true;}
	virtual bool	PostRender(ID3D11DeviceContext* pContext) 
					{ PipeLineSetup.PostRender(pContext, m_icount_Vertexs, m_icount_Indexes); return true;}
	virtual bool	Release() { return true;}
public:
	Object_BaseCamp() {
		D3DXMatrixIdentity(&m_matWorld); 
		m_icount_Vertexs = 0;
		m_icount_Indexes = 0;
	}
	virtual ~Object_BaseCamp() {}
};




void  Object_BaseCamp::HandOver_world_view_projection_Matrixs_to_ConstantData_Final_extract(	D3DXMATRIX* pWorld,	D3DXMATRIX* pView,	D3DXMATRIX* pProj)
{
	
	m_matWorld = *pWorld;
	m_matView = *pView;
	m_matProj = *pProj;

	object_position_look_side_up_extract_From_WorldMatrix(); // 사물 Coordinate 최종 추출.

	D3DXMatrixTranspose(&m_Constant_Data.matWorld,		&m_matWorld);
	D3DXMatrixTranspose(&m_Constant_Data.matView,		&m_matView);
	D3DXMatrixTranspose(&m_Constant_Data.matProj,		&m_matProj);
}




void	Object_BaseCamp::transposition_real_time(D3DXVECTOR3 position_move_in)
{

	m_matWorld._41 += position_move_in.x*g_fSecPerFrame;
	m_matWorld._42 += position_move_in.y*g_fSecPerFrame;
	m_matWorld._43 += position_move_in.z*g_fSecPerFrame;

	D3DXMatrixTranspose(&m_Constant_Data.matWorld, &m_matWorld);

}

void Object_BaseCamp::transposition(D3DXVECTOR3 position_move_in)
{
	m_matWorld._41 = position_move_in.x;
	m_matWorld._42 = position_move_in.y;
	m_matWorld._43 = position_move_in.z;
}

