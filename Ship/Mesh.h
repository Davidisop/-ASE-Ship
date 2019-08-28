#pragma once
#include "Object_Handling.h"
#include "ASE.h"




class Mesh : public Object_Handling
{

public:

	AseMesh ASE;
	vector<Mesh> m_ObjectList;
	vector<Mesh> m_Object_SubObjectList;
	vector<SimpleVertex>  m_tmpVertexList; // �ϴ� ���⿡ �� �����ſ���. �ӽôϱ�, �ε��� ����, ��ġ�� �͵鵵 �� �״��!
	int   m_iNumFaces;


public: // �ִϸ��̼� Frame()�� ����

	float			m_fTickSpeed;


	Scene		m_Scene; // �� ���� ������ ����
	float		m_fElapseTime;
	int			First_Frame_for_Calculration;// ���� ������
	int			Last_Frame_for_Calculration; // ������ ������
	int			Frames_per_second_for_Calculration;// 1�ʴ� ������ ����(30)
	int			Tick_Per_Frame_for_Calculration;// 1�������� ƽ ��(160)

public:

	D3DXMATRIX    m_matWorld; // ���� ���
	D3DXMATRIX    m_matWorldRotate; // ���� ȸ�� ���
	D3DXMATRIX    m_matWorldScale; // ���� ���� ���
	D3DXMATRIX    m_matWorldTrans; // ���� �̵� ���
	D3DXMATRIX	  m_matCalculation; // �ڽ��� ���� �ִϸ��̼� ��� ���


public:

	bool	 Load_ASEfile_Trans_VertexLists_to_all_meshes_SceneInfo_Insert();
	int		 IsEqulVertexList(vector<SimpleVertex>& list, SimpleVertex& v);
	bool	 CalculrationMatrix_out();
	float	 SetAnimationSpeed(float fSpeed);
	void	 Interpolate(Mesh* pMesh, D3DXMATRIX* matParents,
		float David_FrameTick,
		D3DXMATRIX& David_matCalculation,
		D3DXMATRIX& David_matWorldRotate, D3DXMATRIX& David_matWorldTrans, D3DXMATRIX& David_matWorldScale,
		vector<TAnimTrack>	David_pRotTrack, vector<TAnimTrack>	David_pPosTrack, vector<TAnimTrack>	David_pSclTrack, vector<TAnimTrack>	David_pVisTrack,
		Scene David_Scene,
		float& David_fVisibility);

public:
	bool   Create(ID3D11Device* pd3dDevice, const TCHAR* pVsFile, const TCHAR* pPsFile, const TCHAR* pTexFile);
	bool   update_ConstantBuffer();

	
public:

	//bool Draw(ID3D11DeviceContext*  pContext, TModel* pParent);
	//bool Convert(ID3D11Device* pDevice);
	//bool InheriteCollect();

public:
	Mesh()
	{
		m_iNumFaces = 0;
		m_fTickSpeed = 0;
		m_fElapseTime = 0;
		First_Frame_for_Calculration = 0;
		Last_Frame_for_Calculration = 0;
		Frames_per_second_for_Calculration = 0;
		Tick_Per_Frame_for_Calculration = 0;
		D3DXMatrixIdentity(&m_matWorld);
		D3DXMatrixIdentity(&m_matWorldTrans);
		D3DXMatrixIdentity(&m_matWorldRotate);
		D3DXMatrixIdentity(&m_matWorldScale);
		D3DXMatrixIdentity(&m_matCalculation);
	}
	virtual ~Mesh() {}
};



bool  Mesh::Load_ASEfile_Trans_VertexLists_to_all_meshes_SceneInfo_Insert() // �̰����� ���������Ʈ��� �� �ɰ�����?
{

	ASE.Load(_T("Turret_Deploy.ASE")); //���⼭ �̹� ���͸��� ����Ʈ, ������͸��� ����Ʈ ���� �Ϸ�.

	for (int iObj = 0; iObj < ASE.GeometryMesh_per_Object_List.size(); iObj++) // ������Ʈ�� ���� �� �繰 ���� Circulation �մϴ�. ���� �� 2�� �Դϴ�. �׷���, ���갡 ������?
	{
		Mesh     yang_mesh;// ���⼭ �����ؼ�, �̰ſ��ٰ�, ASE��� �� ����ְ�, �� ���� �Ӻ��� �Ұǰ� ����.

		int iMtrl = ASE.GeometryMesh_per_Object_List[iObj].Material_Reference;
		int iSubMtrl = ASE.Material_List[iMtrl].Sub_Material_List.size();


		if (iSubMtrl > 0)
		{
			// subMaterial �� ������,

			yang_mesh.m_Object_SubObjectList.resize(iSubMtrl); //  �׷��ϱ�, ������ ASE ����ü��� ���� ���ְ� �ִ�.
			yang_mesh.m_iNumFaces = ASE.GeometryMesh_per_Object_List[iObj].Position_Face_List.size(); // ���� ���̽���
																									  //
																									  //CreateTriangleList(iObj, yang_mesh);

			for (int iFace = 0; iFace < yang_mesh.m_iNumFaces; iFace++)
			{
				for (int iVer = 0; iVer < 3; iVer++)
				{
					SimpleVertex yang_vector; // ���� �����ؼ�, �̰� ä����. �̰� �� ��� �����ϰ���?

					int iID = iFace * 3 + iVer;
					// pos
					if (ASE.GeometryMesh_per_Object_List[iObj].vertexList.size() > 0)
					{
						int iIndex = ASE.GeometryMesh_per_Object_List[iObj].Position_Face_List[iFace].ASE_Index[iVer];
						yang_vector.p = ASE.GeometryMesh_per_Object_List[iObj].vertexList[iIndex];
					}
					// normal
					if (ASE.GeometryMesh_per_Object_List[iObj].normal_List.size() > 0)
					{
						yang_vector.n =
							ASE.GeometryMesh_per_Object_List[iObj].normal_List[iID];
					}
					// color
					yang_vector.c = D3DXVECTOR4(1, 1, 1, 1);
					if (ASE.GeometryMesh_per_Object_List[iObj].Color_Face_List.size() > 0)
					{
						int iIndex = ASE.GeometryMesh_per_Object_List[iObj].Color_Face_List[iFace].ASE_Index[iVer];
						yang_vector.c.x = ASE.GeometryMesh_per_Object_List[iObj].Color_List[iIndex].x;
						yang_vector.c.y = ASE.GeometryMesh_per_Object_List[iObj].Color_List[iIndex].y;
						yang_vector.c.z = ASE.GeometryMesh_per_Object_List[iObj].Color_List[iIndex].z;
						yang_vector.c.w = 1.0f;
					}
					// tex
					if (ASE.GeometryMesh_per_Object_List[iObj].Texture_Face_List.size() > 0)
					{
						int iIndex = ASE.GeometryMesh_per_Object_List[iObj].Texture_Face_List[iFace].ASE_Index[iVer];
						yang_vector.t.x = ASE.GeometryMesh_per_Object_List[iObj].Texture_List[iIndex].x;
						yang_vector.t.y = ASE.GeometryMesh_per_Object_List[iObj].Texture_List[iIndex].y;
					}

					//if (mesh.m_SubList.size() > 0) �̴�.
					iID = ASE.GeometryMesh_per_Object_List[iObj].Position_Face_List[iFace].Sub_Material_number;
					yang_mesh.m_Object_SubObjectList[iID].m_VertexList.push_back(yang_vector);

				}
			}
		}

		else // subMaterial �� ������,
		{

			yang_mesh.m_iNumFaces = ASE.GeometryMesh_per_Object_List[iObj].Position_Face_List.size();
			yang_mesh.m_tmpVertexList.resize(yang_mesh.m_iNumFaces * 3);


			//CreateTriangleList(iObj, mesh, mesh.m_tmpVertexList);


			for (int iFace = 0; iFace < yang_mesh.m_iNumFaces; iFace++)
			{
				for (int iVer = 0; iVer < 3; iVer++)
				{
					SimpleVertex David_vector;
					int iID = iFace * 3 + iVer;
					// pos
					if (ASE.GeometryMesh_per_Object_List[iObj].vertexList.size() > 0)
					{
						int iIndex = ASE.GeometryMesh_per_Object_List[iObj].Position_Face_List[iFace].ASE_Index[iVer];

						David_vector.p = ASE.GeometryMesh_per_Object_List[iObj].vertexList[iIndex];
					}
					// normal
					if (ASE.GeometryMesh_per_Object_List[iObj].normal_List.size() > 0)
					{
						David_vector.n = ASE.GeometryMesh_per_Object_List[iObj].normal_List[iID];
					}
					// color
					David_vector.c = D3DXVECTOR4(1, 1, 1, 1);
					if (ASE.GeometryMesh_per_Object_List[iObj].Color_Face_List.size() > 0)
					{
						int iIndex = ASE.GeometryMesh_per_Object_List[iObj].Color_Face_List[iFace].ASE_Index[iVer];
						David_vector.c.x = ASE.GeometryMesh_per_Object_List[iObj].Color_List[iIndex].x;
						David_vector.c.y = ASE.GeometryMesh_per_Object_List[iObj].Color_List[iIndex].y;
						David_vector.c.z = ASE.GeometryMesh_per_Object_List[iObj].Color_List[iIndex].z;
						David_vector.c.w = 1.0f;
					}
					// tex
					if (ASE.GeometryMesh_per_Object_List[iObj].Texture_Face_List.size() > 0)
					{
						int iIndex = ASE.GeometryMesh_per_Object_List[iObj].Texture_Face_List[iFace].ASE_Index[iVer];
						David_vector.t.x = ASE.GeometryMesh_per_Object_List[iObj].Texture_List[iIndex].x;
						David_vector.t.y = ASE.GeometryMesh_per_Object_List[iObj].Texture_List[iIndex].y;
					}

					yang_mesh.m_tmpVertexList[iID] = David_vector;//�̰� ��� ���̴��� �ñ��ϴ�.
				}
			}


			// ������� ������Ʈ�� �� �о���.
			// �������� �����Ѵ�.
			// create ib + vb
			// xMesh.m_tmpVertexList


			for (int ivb = 0; ivb < yang_mesh.m_tmpVertexList.size(); ivb++)
			{
				SimpleVertex isop_vector = yang_mesh.m_tmpVertexList[ivb];
				int David_iPos = IsEqulVertexList(yang_mesh.m_VertexList, isop_vector);

				if (David_iPos < 0)
				{
					yang_mesh.m_VertexList.push_back(isop_vector);
					David_iPos = yang_mesh.m_VertexList.size() - 1;
				}
				yang_mesh.m_IndexList.push_back(David_iPos);
			}


			yang_mesh.m_icount_Indexes = yang_mesh.m_IndexList.size();
			yang_mesh.m_icount_Vertexs = yang_mesh.m_VertexList.size();

			/*	int iRef = ASE.GeometryMesh_per_Object_List[iObj].Material_Reference;
			T_STR texName = ASE.Material_List[iRef].TextureMap_List[0].name;
			yang_mesh.Create(g_pd3dDevice, L"../../data/shader/map.hlsl", texName);
			*/
		}
		//m_xObj.m_ObjectList.push_back(mesh);

		this->m_ObjectList.push_back(yang_mesh); // �Լ� ��ų.

	}

	m_Scene.First_Frame = ASE.m_Scene.First_Frame;
	m_Scene.Last_Frame = ASE.m_Scene.Last_Frame;
	m_Scene.Frames_per_second = ASE.m_Scene.Frames_per_second;
	m_Scene.Tick_Per_Frame = ASE.m_Scene.Tick_Per_Frame;

	return true;
}
int   Mesh::IsEqulVertexList(vector<SimpleVertex>& list, SimpleVertex& v)
{
	for (int ilist = 0; ilist < list.size(); ilist++)
	{
		if (v == list[ilist])
		{
			return ilist;
		}
	}
	return -1;
}
bool  Mesh::CalculrationMatrix_out()  // ��� ������Ʈ���� Calculation ��ĵ� Frame����, ����� ������ �Ѵ�.
{
	//
	//m_fElapseTime += g_fSecPerFrame * m_Scene.Frames_per_second * m_Scene.Tick_Per_Frame;// �� ƽ�� �� �����İ� ��������. ƽ ���� �ð� (g_fSecPerFrame�� ����ȭ��)
	//if (m_fElapseTime >= m_Scene.Last_Frame* m_Scene.Tick_Per_Frame) // �ִϸ��̼� �ð� �� ������,
	//{
	//	m_fElapseTime = m_Scene.First_Frame * m_Scene.Tick_Per_Frame; // ������� �������ش�.
	//}


	//vector<Mesh> m_ObjectList;
	//vector<Mesh> m_Object_SubObjectList; //�ȿ� �ִ� ������Ʈ �ϳ��ϳ� ����.�ð��뺰�� Calculation ���, ����� ������ �Ѵ�.



	//D3DXMATRIX mat;
	//D3DXMatrixIdentity(&mat);


	//Interpolate(&David_Mesh_in, &mat, m_fElapseTime, David_Mesh_in.);

	//void  Mesh::Interpolate(Mesh* pMesh, D3DXMATRIX* matParents,
	//	float David_FrameTick,
	//	D3DXMATRIX David_matCalculation,
	//	D3DXMATRIX David_matWorldRotate, D3DXMATRIX David_matWorldTrans, D3DXMATRIX David_matWorldScale,
	//	vector<TAnimTrack>	David_pRotTrack, vector<TAnimTrack>	David_pPosTrack, vector<TAnimTrack>	David_pSclTrack, vector<TAnimTrack>	David_pVisTrack,
	//	Scene David_Scene,
	//	float David_fVisibility);

	//for (DWORD dwObject = 0; dwObject < m_pData.size(); dwObject++)
	//{
	//	Mesh* pMesh = m_pMesh[dwObject];

	//	//�θ� �ִ��� ������
	//	if (pMesh->m_bUsed == false) continue;
	//	if (pMesh->m_pParent)
	//	Interpolate(pMesh, &pMesh->m_pParent->m_matCalculation, m_fElapseTime);//�θ� ������, �θ� �ִϸ��̼� �����
	//	else
	//	Interpolate(pMesh, &mat, m_fElapseTime);//�θ� ������, �θ� ��� ���������
	//	Interpolate(pMesh, &mat, m_fElapseTime); //�θ� ������
	//}
	return true;
}
float Mesh::SetAnimationSpeed(float fSpeed)
{
	m_fTickSpeed = fSpeed;

	return m_fTickSpeed;
}

void  Mesh::Interpolate(Mesh* pMesh, D3DXMATRIX* matParents,
			float David_FrameTick,
			D3DXMATRIX& David_matCalculation,
			D3DXMATRIX& David_matWorldRotate, D3DXMATRIX& David_matWorldTrans, D3DXMATRIX& David_matWorldScale,
			vector<TAnimTrack>	David_pRotTrack, vector<TAnimTrack>	David_pPosTrack, vector<TAnimTrack>	David_pSclTrack, vector<TAnimTrack>	David_pVisTrack,
			Scene David_Scene,
			float& David_fVisibility)
{
	// TM		= AnimMat * ParentTM;
	// AaniMat	= TM * Inverse(ParentTM)
	D3DXQUATERNION qR;
	D3DXQUATERNION qS;
	// ���ʹϿ��� ���⼭ �����ߴ�.
	D3DXMatrixIdentity(&David_matCalculation);

	D3DXMATRIX matAnim; // �ٽ� ����� ���⿡�� �����ߴ�. �и��� �̰��� �ٽ����� ��Ȱ�� �Ѵ�.
	D3DXMATRIX matPos, matRotate, matScale; // ���⼭ ��ĵ��� �����ߴ�. ���ǰ� ����.

	matRotate = David_matWorldRotate;
	matPos = David_matWorldTrans;
	matScale = David_matWorldScale;

	D3DXQuaternionRotationMatrix(&qR, &matRotate);
	//Builds a quaternion from a rotation matrix.
	D3DXQuaternionRotationMatrix(&qS, &matScale); // �̰� ��Ư�ϳ�. 	�� Scale ���� �׷��� ����? �������� Scale�� ȸ���� ���� �׷���?


												  // David_FrameTick = m_Scene.iFirstFrame * m_Scene.iTickPerFrame + CurFame;
	float fStart_Tick = David_Scene.First_Frame * David_Scene.Tick_Per_Frame; // �� ���� �� ƽ���� �����ϴ���.
	float fEnd_Tick = 0.0f; // �̰��� ��������?

	TAnimTrack* pStartTrack; // �ٽ����� ��Ȱ�� �� �ž�.
	TAnimTrack* pEndTrack;


	// Roation Ʈ�� �ȿ� ��� ������� �ᱹ ��� "matRotate" �� ������Ʈ.

	pStartTrack = NULL;
	pEndTrack = NULL;

	if (David_pRotTrack.size()) // = David_pRotTrack�� ������
	{
		// pStartTrack�� ã���� ������

		for (DWORD dwTrack = 0; dwTrack < David_pRotTrack.size(); dwTrack++)
		{
			TAnimTrack *pTrack = &David_pRotTrack[dwTrack];
			_ASSERT(pTrack);

			if (pTrack->iTick > David_FrameTick)
			{			
				pEndTrack = pTrack;		
				break;
			}
			pStartTrack = pTrack;
		}
	
		if (pStartTrack)
		{	qR = pStartTrack->qRotate;
			fStart_Tick = pStartTrack->iTick;}

		if (pEndTrack)
		{	fEnd_Tick = pEndTrack->iTick;
			D3DXQuaternionSlerp(&qR, &qR, &pEndTrack->qRotate, (David_FrameTick - fStart_Tick) / (fEnd_Tick - fStart_Tick));
		}              //���纸�������ʹϿ�//��������//��������//(����ð� - ó�� Key �ð�) / (���� Key �ð� - ���� Key �ð�)
		D3DXMatrixRotationQuaternion(&matRotate, &qR); 
	}



	// Trans Ʈ�� �ȿ� ��� ������� �ᱹ ��� 'matPos' �� ������Ʈ.

	pStartTrack = NULL;
	pEndTrack = NULL; // �� ��͵��� ����ְ� �־�!  ���

	D3DXVECTOR3 Trans(matPos._41, matPos._42, matPos._43);// ������.

	if (David_pPosTrack.size())
	{
	
		
		for (DWORD dwTrack = 0; dwTrack < David_pPosTrack.size(); dwTrack++)
		{
			TAnimTrack *pTrack = &David_pPosTrack[dwTrack];
			_ASSERT(pTrack);

			if (pTrack->iTick > David_FrameTick)
			{
				pEndTrack = pTrack;
				break;
			}
			pStartTrack = pTrack;
		}


		if (pStartTrack)
		{
			// Ʈ���ȿ�, ���� �κ����� ������ �� ���� ã������, �װɷ� "matPos"�� �ٲ��ش�.
			Trans = pStartTrack->Vector_in_Animation;
			fStart_Tick = pStartTrack->iTick;
		}
		if (pEndTrack)// 
		{// pEndTrack�� ã���� ������

			fEnd_Tick = pEndTrack->iTick;
			D3DXVec3Lerp(&Trans, &Trans, &pEndTrack->Vector_in_Animation, (David_FrameTick - fStart_Tick) / (fEnd_Tick - fStart_Tick));
		}                //���纸���麤��//��������//���ߺ���                        //  (����ð� - ó�� Key �ð�) / (���� Key �ð� - ���� Key �ð�)

		D3DXMatrixTranslation(&matPos, Trans.x, Trans.y, Trans.z); // �̷��� �ٲ�����.
	}


	// Scale Ʈ�� �ȿ� ��� ������� �ᱹ ��� 'matScale' �� ������Ʈ.

	pStartTrack = NULL;
	pEndTrack = NULL;

	D3DXMATRIX matScaleRot, matInvScaleRot;
	D3DXVECTOR3 vScale(matScale._11, matScale._22, matScale._33); // ������ ������ �Է�. ��Ȯ�� ������ ����� 11 = x�༺��, 22= y�༺��, 33=z�� ���� �̴�.

	if (David_pSclTrack.size())
	{

		for (DWORD dwTrack = 0; dwTrack < David_pSclTrack.size(); dwTrack++)
		{
			TAnimTrack *pTrack = &David_pSclTrack[dwTrack];
			_ASSERT(pTrack);

			if (pTrack->iTick > David_FrameTick)
			{
				pEndTrack = pTrack;
				break;
			}
			pStartTrack = pTrack;
		}


		if (pStartTrack)//NODE_TM ������ �ֽ��ΰɷ�. �ٽ��̳�.
		{ // pStartTrack�� ã���� ������  ������ ������ ������Ʈ�Ѵ�.
			vScale = pStartTrack->Vector_in_Animation;
			
			// ���ʹϿ�.
			qS = pStartTrack->qRotate; //D3DXQUATERNION  qRotate;
			fStart_Tick = pStartTrack->iTick;
		}

		if (pEndTrack)// pEndTrack�� ã���� ������
		{
			fEnd_Tick = pEndTrack->iTick;
			D3DXVec3Lerp(&vScale, &vScale, &pEndTrack->Vector_in_Animation, (David_FrameTick - fStart_Tick) / (fEnd_Tick - fStart_Tick));
			//���纸���麤��//��������//���ߺ���                        //  (����ð� - ó�� Key �ð�) / (���� Key �ð� - ���� Key �ð�)

			D3DXQuaternionSlerp(&qS, &qS, &pEndTrack->qRotate, (David_FrameTick - fStart_Tick) / (fEnd_Tick - fStart_Tick));
			//���纸��������//��������//��������        //  (����ð� - ó�� Key �ð�) / (���� Key �ð� - ���� Key �ð�)

			//Interpolates between two quaternions, using spherical linear interpolation.

			/*	D3DXQUATERNION* D3DXQuaternionSlerp(
			_Inout_       D3DXQUATERNION *pOut,
			_In_    const D3DXQUATERNION *pQ1,
			_In_    const D3DXQUATERNION *pQ2,
			_In_          FLOAT          t

			Parameter that indicates how far to interpolate between the quaternions.
			*/

		}
		D3DXMatrixScaling(&matScale, vScale.x, vScale.y, vScale.z); // ���� ���� �� ���ͷ� matScale�� ������Ʈ.
		D3DXMatrixRotationQuaternion(&matScaleRot, &qS); // ���� ���� �� ���ʹϿ��� ��� matScaleRot���� ��ȯ 
		D3DXMatrixInverse(&matInvScaleRot, NULL, &matScaleRot); // �̰� �����ȭ��.
		matScale = matInvScaleRot * matScale * matScaleRot; // �̰� ������ �� Sclae�� �⺻��. ����� �ѹ��ϰ�, ������¿��� ������ �ϰ�, �ѹ��� ������ �� �������Ѵ�.
	}

	// Visible Ʈ�� �ȿ� ��� ������� �ᱹ "David_fVisibility"�� ������Ʈ.

	pStartTrack = NULL;
	pEndTrack = NULL;

	float fCurrent_Alpha, fNext_Alpha, fOffSet;
	fCurrent_Alpha = 0.0f;
	fNext_Alpha = 0.0f;

	if (David_pVisTrack.size())
	{

		for (DWORD dwTrack = 0; dwTrack < David_pVisTrack.size(); dwTrack++)
		{
			TAnimTrack *pTrack = &David_pVisTrack[dwTrack];
			_ASSERT(pTrack);

			if (pTrack->iTick > David_FrameTick)
			{
				pEndTrack = pTrack;
				break;
			}
			pStartTrack = pTrack;
		}



		if (pStartTrack)
		{// pStartTrack�� ã���� ������
			fCurrent_Alpha = pStartTrack->Vector_in_Animation.x;
			fStart_Tick = pStartTrack->iTick;
		}

		if (pEndTrack)
			// pEndTrack�� ã���� ������
		{
			fNext_Alpha = pEndTrack->Vector_in_Animation.x;
			fEnd_Tick = pEndTrack->iTick;

			fOffSet = (David_FrameTick - fStart_Tick) / (fEnd_Tick - fStart_Tick);
			//���� ������ s ��.
			fNext_Alpha = (fNext_Alpha - fCurrent_Alpha)*fOffSet;
			// ��ü ���� ������.
		}
		David_fVisibility = (fCurrent_Alpha + fNext_Alpha); // ������ ���� �Ϳ� ���� ���� ���� ���� ����.
	}
	else
	{
		David_fVisibility = 1.0f;
	}
	//

	// ���� ���


	D3DXMatrixMultiply(&matAnim, &matScale, &matRotate);

	matAnim._41 = matPos._41;
	matAnim._42 = matPos._42;
	matAnim._43 = matPos._43;      //S,R,T

								   // ���� ���̸��̼� ����� �ϼ��Ѵ�.	
	D3DXMatrixMultiply(&David_matCalculation, &matAnim, matParents); // �����߿��ϴ�. �ڽ� * �θ� : �� ���� ���� ���� �س���, �¾� �����ؾ��Ѵ�.

														 // �ι��� ��Ʈ���� Ȯ�� �ڵ�.
	D3DXVECTOR3 v0, v1, v2, v3;
	v0 = David_matCalculation.m[0];
	v1 = David_matCalculation.m[1];
	v2 = David_matCalculation.m[2];
	D3DXVec3Cross(&v3, &v1, &v2);
	if (D3DXVec3Dot(&v3, &v0) < 0.0f)
	{
		D3DXMATRIX matW;
		D3DXMatrixScaling(&matW, -1.0f, -1.0f, -1.0f);
		D3DXMatrixMultiply(&David_matCalculation,
			&David_matCalculation, &matW);
	}
	David_matWorldRotate= matRotate;
	David_matWorldTrans= matPos;
	David_matWorldScale= matScale;
}

bool Mesh::Create(ID3D11Device* pd3dDevice, const TCHAR* pVsFile, const TCHAR* pPsFile, const TCHAR* pTexFile)
{
	PipeLineSetup.LoadTexture(pd3dDevice, pTexFile);
	PipeLineSetup.SetBlendState(pd3dDevice);
	PipeLineSetup.CreateVertextBuffer(pd3dDevice, &m_VertexList.at(0), m_icount_Vertexs);

	if (m_icount_Indexes != 0)	PipeLineSetup.CreateIndexBuffer(pd3dDevice, &m_IndexList.at(0), m_icount_Indexes);

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
bool Mesh::update_ConstantBuffer()
{
	g_pContext->UpdateSubresource(PipeLineSetup.m_pConstantBuffer, 0, NULL, &m_Constant_Data, 0, 0);
	return true;
}
//
//bool Mesh::Convert(ID3D11Device* pDevice)
//{// ASE���� MESH�� ��ȯ�ϴ� ��Ȱ�� �Ѵ�.
//
//	for (DWORD dwObject = 0; dwObject < m_Parser.m_pAseMesh.size(); dwObject++)
//	{
//		auto pMesh = make_shared<TMesh>(); // ���⿡ �����Ѵ�.
//		auto pData = make_shared<tAseData>(); // ���⿡ �����ߴ�. �̰� ���� ��԰���?
//
//		pMesh->m_iIndex = dwObject;
//		pMesh->m_matWorld = m_Parser.m_pAseMesh[dwObject]->m_matWorld;		// ������ĵ� �ѱ��	
//		pMesh->m_iNumFace = m_Parser.m_pAseMesh[dwObject]->m_iNumFace;		// ��ü���̽� ����	�� �ѱ��				
//		pMesh->m_strNodeName = m_Parser.m_pAseMesh[dwObject]->m_strNodeName; // ��� ���ӵ� �ѱ��.
//		pMesh->m_strParentName = m_Parser.m_pAseMesh[dwObject]->m_strParentName; // ��Ʈ ���� â�� ���� �ֳ�����.
//
//		pMesh->m_matCalculation = m_Parser.m_pAseMesh[dwObject]->m_matWorld; // interpolate() ��� ���� ��� �ѱ��	
//		pMesh->m_matWorldTrans = m_Parser.m_pAseMesh[dwObject]->m_matWorldTrans; //interpolate() ��� �̵� ��� �ѱ��	
//		pMesh->m_matWorldRotate = m_Parser.m_pAseMesh[dwObject]->m_matWorldRotate;//interpolate() ��� ȸ�� ��� �ѱ��	
//		pMesh->m_matWorldScale = m_Parser.m_pAseMesh[dwObject]->m_matWorldScale;//interpolate() ��� ������ ��� �ѱ��	
//
//		�糢��
//
//
//		D3DXMatrixInverse(&pMesh->m_matInverse, NULL, &pMesh->m_matWorld);
//
//		int iRootMtrRef = m_Parser.m_pAseMesh[dwObject]->m_iMtrlRef; //���͸��� ������ �ѱ��.
//
//		TMtrl* pMtrl = NULL;
//		if (iRootMtrRef >= 0 && iRootMtrRef < m_Material.size())
//		{
//			pMtrl = &m_Material[iRootMtrRef]; //���͸��� ������ �ѱ��.
//			_ASSERT(pMtrl);
//		}
//		CStopwatch watch;
//		if (pMesh->m_iNumFace > 0)
//		{
//			SetTriangleBuffer(pMesh.get(), pData.get(), m_Parser.m_pAseMesh[dwObject].get(), 0xff);
//			if (pMtrl->m_SubMaterial.size() > 0)
//			{
//				std::sort(pData->m_TriList.begin(), pData->m_TriList.end(), DescendingTriSort<PNCT_VERTEX>());
//			}
//
//			if (pMtrl == NULL || pMtrl->m_SubMaterial.size() <= 0)
//			{
//				// 2�� ���ڰ�=-1  Face Count(_countof )�� ������� �ʴ´�.
//				pData->SetUniqueBuffer(pData->m_TriList, -1, 0);
//				pMesh->m_iDiffuseTex = -1;
//				if (pMtrl)	pMesh->m_iDiffuseTex = GetMapID(pMtrl, ID_TBASIS_DI);
//				m_iMaxVertex += pData->m_VertexArray.size(); // ���ؽ� ������ �ѱ��.
//				m_iMaxIndex += pData->m_IndexArray.size();// �ε��� ������ �ѱ��.
//			}
//			else
//			{
//				int iAddCount = 0;
//				for (int iSub = 0; iSub < pMtrl->m_SubMaterial.size(); iSub++)
//				{
//					auto pSubMesh = make_shared<TMesh>();
//					auto pSubData = make_shared<tAseData>();
//
//					iAddCount += pSubData->SetUniqueBuffer(pData->m_TriList, iSub, iAddCount);
//
//					TMtrl* pSubMtrl = &pMtrl->m_SubMaterial[iSub];
//					_ASSERT(pSubMtrl);
//					pSubMesh->m_iDiffuseTex = -1;
//					if (pSubMtrl)
//						pSubMesh->m_iDiffuseTex = GetMapID(pSubMtrl, ID_TBASIS_DI);
//
//					m_iMaxVertex += pSubData->m_VertexArray.size();
//					m_iMaxIndex += pSubData->m_IndexArray.size();
//
//					pSubData->m_iNumFace = pSubData->m_IndexArray.size() / 3; // �� �ѱ�� �Ӥ���.
//					pSubMesh->m_iNumFace = pSubData->m_IndexArray.size() / 3;
//
//					if (pSubData->m_VertexArray.size() > 0)
//					{
//						pMesh->m_pSubMesh.push_back(pSubMesh);
//						pData->m_pSubMesh.push_back(pSubData);
//					}
//				}
//			}
//		}
//
//		�糢��
//
//
//		watch.Output(L"Sub Mesh");
//		// ���ϸ��̼� Ʈ�� ����		
//		TAnimTrack* pPrevTrack = NULL; // ������ Ʈ�� MESH�� �ѱ�.
//		for (int iTrack = 0; iTrack < m_Parser.m_pAseMesh[dwObject]->m_PosTrack.size(); iTrack++)
//		{
//			auto pTrack = make_shared<TAnimTrack>();
//			pTrack->iTick = m_Parser.m_pAseMesh[dwObject]->m_PosTrack[iTrack].iTick;
//			pTrack->vVector = m_Parser.m_pAseMesh[dwObject]->m_PosTrack[iTrack].vVector;
//
//			pPrevTrack = SetDoublyLinkedList(pTrack.get(), pPrevTrack);
//			pMesh->m_pPosTrack.push_back(pTrack);
//		}
//
//		�糢��
//
//		pPrevTrack = NULL;
//		for (int iTrack = 0; iTrack < m_Parser.m_pAseMesh[dwObject]->m_RotTrack.size(); iTrack++)
//		{
//			auto pTrack = make_shared<TAnimTrack>();
//
//			pTrack->iTick = m_Parser.m_pAseMesh[dwObject]->m_RotTrack[iTrack].iTick;
//
//			// ������ ��� ���� ���ʹϾ����� ��ȯ			
//			D3DXQuaternionRotationAxis(&pTrack->qRotate, &D3DXVECTOR3(m_Parser.m_pAseMesh[dwObject]->m_RotTrack[iTrack].qRotate.x,
//				m_Parser.m_pAseMesh[dwObject]->m_RotTrack[iTrack].qRotate.y,
//				m_Parser.m_pAseMesh[dwObject]->m_RotTrack[iTrack].qRotate.z),
//				m_Parser.m_pAseMesh[dwObject]->m_RotTrack[iTrack].qRotate.w);
//			// ����Ʈ���� ���ʹϿ°� ������Ŵ.			
//			if (pPrevTrack != NULL)
//			{
//				D3DXQuaternionMultiply(&pTrack->qRotate, &pPrevTrack->qRotate, &pTrack->qRotate);
//			}
//
//			pPrevTrack = SetDoublyLinkedList(pTrack.get(), pPrevTrack);
//
//			pMesh->m_pRotTrack.push_back(pTrack); // ȸ�� Ʈ�� MESH�� �ѱ�.
//		}
//
//		pPrevTrack = NULL;
//		for (int iTrack = 0; iTrack < m_Parser.m_pAseMesh[dwObject]->m_SclTrack.size(); iTrack++)
//		{
//			auto pTrack = make_shared<TAnimTrack>();
//
//			pTrack->iTick = m_Parser.m_pAseMesh[dwObject]->m_SclTrack[iTrack].iTick;
//			pTrack->vVector = m_Parser.m_pAseMesh[dwObject]->m_SclTrack[iTrack].vVector;
//
//			// ������ ��� ���� ���ʹϾ����� ��ȯ			
//			D3DXQuaternionRotationAxis(&pTrack->qRotate,
//				&D3DXVECTOR3(m_Parser.m_pAseMesh[dwObject]->m_SclTrack[iTrack].qRotate.x,
//					m_Parser.m_pAseMesh[dwObject]->m_SclTrack[iTrack].qRotate.y,
//					m_Parser.m_pAseMesh[dwObject]->m_SclTrack[iTrack].qRotate.z),
//				m_Parser.m_pAseMesh[dwObject]->m_SclTrack[iTrack].qRotate.w);
//
//			pPrevTrack = SetDoublyLinkedList(pTrack.get(), pPrevTrack);
//
//			pMesh->m_pSclTrack.push_back(pTrack);  // ������ Ʈ�� MESH�� �ѱ�.
//		}
//
//		// ���ϸ��̼� Ʈ�� ����		
//		pPrevTrack = NULL;
//		for (int iTrack = 0; iTrack < m_Parser.m_pAseMesh[dwObject]->m_VisTrack.size(); iTrack++)
//		{
//			auto pTrack = make_shared<TAnimTrack>();
//			pTrack->iTick = m_Parser.m_pAseMesh[dwObject]->m_VisTrack[iTrack].iTick;
//			pTrack->vVector = m_Parser.m_pAseMesh[dwObject]->m_VisTrack[iTrack].vVector;
//
//			pPrevTrack = SetDoublyLinkedList(pTrack.get(), pPrevTrack);
//			pMesh->m_pVisTrack.push_back(pTrack);    // ������ Ʈ�� MESH�� �ѱ�.
//		}
//		m_pMesh.push_back(pMesh);
//		m_pData.push_back(pData);
//	}
//	m_Scene.iNumMesh = m_pMesh.size();
//	return InheriteCollect();
//	return true;
//}
// 
//bool Mesh::InheriteCollect()
//{
//	D3DXMATRIX m_matInverse;
//	D3DXQUATERNION qR;
//	D3DXVECTOR3 vTrans, vScale;
//
//	for (DWORD dwObject = 0; dwObject < m_pMesh.size(); dwObject++)
//	{
//		if (!m_pMesh[dwObject]->m_strParentName.empty())
//		{
//			TMesh* pParentNode = SearchToCollects(m_pMesh[dwObject]->m_strParentName);
//			if (pParentNode)
//			{
//				m_pMesh[dwObject]->m_pParent = pParentNode;
//				m_matInverse = m_pMesh[dwObject]->m_matWorld * pParentNode->m_matInverse;
//				D3DXMatrixDecompose(&vScale, &qR, &vTrans, &m_matInverse);
//				D3DXMatrixScaling(&m_pMesh[dwObject]->m_matWorldScale, vScale.x, vScale.y, vScale.z);
//				D3DXMatrixTranslation(&m_pMesh[dwObject]->m_matWorldTrans, vTrans.x, vTrans.y, vTrans.z);
//				D3DXMatrixRotationQuaternion(&m_pMesh[dwObject]->m_matWorldRotate, &qR);
//				pParentNode->m_pChildMesh.push_back(m_pMesh[dwObject].get());
//			}
//		}
//	}
//	// ������ ���� ������Ʈ ����
//	for (DWORD dwObject = 0; dwObject < m_pMesh.size(); dwObject++)
//	{
//		if (m_pMesh[dwObject]->m_pChildMesh.size() <= 0 &&
//			(m_pMesh[dwObject]->m_ClassType == CLASS_BONE || m_pMesh[dwObject]->m_ClassType == CLASS_DUMMY))
//		{
//			m_pMesh[dwObject]->m_bUsed = false;
//		}
//	}
//	return true;
//}
//
//bool Mesh::Draw(ID3D11DeviceContext*  pContext, TModel* pParent)
//{
//	CStopwatch stopwatch;
//	for (DWORD dwObject = 0; dwObject < m_pMesh.size(); dwObject++)
//	{
//		auto pMesh = m_pMesh[dwObject].get();
//		pMesh->m_matCalculation = pMesh->m_matCalculation * m_matControlWorld;
//		pParent->SetMatrix(&pMesh->m_matCalculation, &pParent->m_matView, &pParent->m_matProj);
//		UpdateConstantBuffer(pContext, pParent);
//
//		if (pMesh->m_pSubMesh.size() > 0)
//		{
//			for (DWORD dwSub = 0; dwSub < pMesh->m_pSubMesh.size(); dwSub++)
//			{
//				auto pSubMesh = pMesh->m_pSubMesh[dwSub].get();
//				if (pSubMesh->m_iNumFace < 1) continue;
//				pContext->PSSetShaderResources(0, 1, pSubMesh->m_dxobj.g_pTextureSRV.GetAddressOf());
//				pContext->DrawIndexed(
//					pSubMesh->m_dxobj.m_iNumIndex,
//					pSubMesh->m_dxobj.m_iBeginIB,
//					pSubMesh->m_dxobj.m_iBeginVB);
//			}
//		}
//		else
//		{
//			if (pMesh->m_iNumFace < 1) continue;
//			pContext->PSSetShaderResources(0, 1, pMesh->m_dxobj.g_pTextureSRV.GetAddressOf());
//			pContext->DrawIndexed(pMesh->m_dxobj.m_iNumIndex,
//				pMesh->m_dxobj.m_iBeginIB,
//				pMesh->m_dxobj.m_iBeginVB);
//		}
//	}
//	//stopwatch.Output(L"\nTAseObj::Draw");
//	return true;
//}