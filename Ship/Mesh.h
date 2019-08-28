#pragma once
#include "Object_Handling.h"
#include "ASE.h"




class Mesh : public Object_Handling
{

public:

	AseMesh ASE;
	vector<Mesh> m_ObjectList;
	vector<Mesh> m_Object_SubObjectList;
	vector<SimpleVertex>  m_tmpVertexList; // 일단 여기에 다 넣을거에요. 임시니까, 인덱스 없이, 겹치는 것들도 다 그대로!
	int   m_iNumFaces;


public: // 애니메이션 Frame()을 위해

	float			m_fTickSpeed;


	Scene		m_Scene; // 씬 정보 저장을 위한
	float		m_fElapseTime;
	int			First_Frame_for_Calculration;// 시작 프레임
	int			Last_Frame_for_Calculration; // 마지막 프레임
	int			Frames_per_second_for_Calculration;// 1초당 프레임 개수(30)
	int			Tick_Per_Frame_for_Calculration;// 1프레임의 틱 값(160)

public:

	D3DXMATRIX    m_matWorld; // 월드 행렬
	D3DXMATRIX    m_matWorldRotate; // 월드 회전 행렬
	D3DXMATRIX    m_matWorldScale; // 월드 신축 행렬
	D3DXMATRIX    m_matWorldTrans; // 월드 이동 행렬
	D3DXMATRIX	  m_matCalculation; // 자식의 최종 애니메이션 계산 행렬


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



bool  Mesh::Load_ASEfile_Trans_VertexLists_to_all_meshes_SceneInfo_Insert() // 이곳에서 서브오브젝트들로 다 쪼개갰지?
{

	ASE.Load(_T("Turret_Deploy.ASE")); //여기서 이미 메터리얼 리스트, 서브메터리얼 리스트 제작 완료.

	for (int iObj = 0; iObj < ASE.GeometryMesh_per_Object_List.size(); iObj++) // 지오메트리 마다 즉 사물 마다 Circulation 합니다. 지금 총 2개 입니다. 그러나, 서브가 있지요?
	{
		Mesh     yang_mesh;// 여기서 정의해서, 이거에다가, ASE대로 다 집어넣고, 그 다음 뿐빠이 할건가 보다.

		int iMtrl = ASE.GeometryMesh_per_Object_List[iObj].Material_Reference;
		int iSubMtrl = ASE.Material_List[iMtrl].Sub_Material_List.size();


		if (iSubMtrl > 0)
		{
			// subMaterial 이 있으면,

			yang_mesh.m_Object_SubObjectList.resize(iSubMtrl); //  그러니까, 기존의 ASE 구조체대로 지금 해주고 있다.
			yang_mesh.m_iNumFaces = ASE.GeometryMesh_per_Object_List[iObj].Position_Face_List.size(); // 기준 페이스수
																									  //
																									  //CreateTriangleList(iObj, yang_mesh);

			for (int iFace = 0; iFace < yang_mesh.m_iNumFaces; iFace++)
			{
				for (int iVer = 0; iVer < 3; iVer++)
				{
					SimpleVertex yang_vector; // 새로 정의해서, 이걸 채워서. 이걸 또 어디에 저장하겠지?

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

					//if (mesh.m_SubList.size() > 0) 이니.
					iID = ASE.GeometryMesh_per_Object_List[iObj].Position_Face_List[iFace].Sub_Material_number;
					yang_mesh.m_Object_SubObjectList[iID].m_VertexList.push_back(yang_vector);

				}
			}
		}

		else // subMaterial 이 없으면,
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

					yang_mesh.m_tmpVertexList[iID] = David_vector;//이게 어디에 쓰이는지 궁금하다.
				}
			}


			// 여기까지 지오메트리 다 읽었다.
			// 이제부터 진행한다.
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

		this->m_ObjectList.push_back(yang_mesh); // 함수 스킬.

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
bool  Mesh::CalculrationMatrix_out()  // 모든 오브젝트들의 Calculation 행렬들 Frame마다, 계산해 나오게 한다.
{
	//
	//m_fElapseTime += g_fSecPerFrame * m_Scene.Frames_per_second * m_Scene.Tick_Per_Frame;// 몇 틱을 더 갔느냐가 더해진다. 틱 단위 시간 (g_fSecPerFrame는 동기화다)
	//if (m_fElapseTime >= m_Scene.Last_Frame* m_Scene.Tick_Per_Frame) // 애니메이션 시간 다 지나면,
	//{
	//	m_fElapseTime = m_Scene.First_Frame * m_Scene.Tick_Per_Frame; // 원래대로 복귀해준다.
	//}


	//vector<Mesh> m_ObjectList;
	//vector<Mesh> m_Object_SubObjectList; //안에 있는 오브젝트 하나하나 마다.시간대별로 Calculation 행렬, 계산해 나오게 한다.



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

	//	//부모가 있는지 없는지
	//	if (pMesh->m_bUsed == false) continue;
	//	if (pMesh->m_pParent)
	//	Interpolate(pMesh, &pMesh->m_pParent->m_matCalculation, m_fElapseTime);//부모 있으면, 부모 애니메이션 행렬을
	//	else
	//	Interpolate(pMesh, &mat, m_fElapseTime);//부모 없으면, 부모 대신 단위행렬을
	//	Interpolate(pMesh, &mat, m_fElapseTime); //부모 없으면
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
	// 쿼터니온을 여기서 정의했다.
	D3DXMatrixIdentity(&David_matCalculation);

	D3DXMATRIX matAnim; // 핵심 행렬을 여기에서 정의했다. 분명히 이것이 핵심적인 역활을 한다.
	D3DXMATRIX matPos, matRotate, matScale; // 여기서 행렬들을 정의했다. 쓸건가 보다.

	matRotate = David_matWorldRotate;
	matPos = David_matWorldTrans;
	matScale = David_matWorldScale;

	D3DXQuaternionRotationMatrix(&qR, &matRotate);
	//Builds a quaternion from a rotation matrix.
	D3DXQuaternionRotationMatrix(&qS, &matScale); // 이거 독특하네. 	왜 Scale 까지 그렇게 했지? 임의의축 Scale은 회전이 들어가서 그런가?


												  // David_FrameTick = m_Scene.iFirstFrame * m_Scene.iTickPerFrame + CurFame;
	float fStart_Tick = David_Scene.First_Frame * David_Scene.Tick_Per_Frame; // 이 씬이 몇 틱부터 시작하느냐.
	float fEnd_Tick = 0.0f; // 이것이 나오겠지?

	TAnimTrack* pStartTrack; // 핵심적인 역활을 할 거야.
	TAnimTrack* pEndTrack;


	// Roation 트랙 안에 담긴 정보들로 결국 행렬 "matRotate" 을 업데이트.

	pStartTrack = NULL;
	pEndTrack = NULL;

	if (David_pRotTrack.size()) // = David_pRotTrack이 있으면
	{
		// pStartTrack를 찾을수 있으면

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
		}              //현재보간中쿼터니온//기존쿼터//나중쿼터//(현재시간 - 처음 Key 시간) / (나중 Key 시간 - 현재 Key 시간)
		D3DXMatrixRotationQuaternion(&matRotate, &qR); 
	}



	// Trans 트랙 안에 담긴 정보들로 결국 행렬 'matPos' 을 업데이트.

	pStartTrack = NULL;
	pEndTrack = NULL; // 이 빈것들을 집어넣고 있어!  대박

	D3DXVECTOR3 Trans(matPos._41, matPos._42, matPos._43);// 벡터임.

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
			// 트랙안에, 시작 부분으로 쓸만한 前 것을 찾았으면, 그걸로 "matPos"를 바꿔준다.
			Trans = pStartTrack->Vector_in_Animation;
			fStart_Tick = pStartTrack->iTick;
		}
		if (pEndTrack)// 
		{// pEndTrack를 찾을수 있으면

			fEnd_Tick = pEndTrack->iTick;
			D3DXVec3Lerp(&Trans, &Trans, &pEndTrack->Vector_in_Animation, (David_FrameTick - fStart_Tick) / (fEnd_Tick - fStart_Tick));
		}                //현재보간中벡터//기존벡터//나중벡터                        //  (현재시간 - 처음 Key 시간) / (나중 Key 시간 - 현재 Key 시간)

		D3DXMatrixTranslation(&matPos, Trans.x, Trans.y, Trans.z); // 이렇게 바뀌어야지.
	}


	// Scale 트랙 안에 담긴 정보들로 결국 행렬 'matScale' 을 업데이트.

	pStartTrack = NULL;
	pEndTrack = NULL;

	D3DXMATRIX matScaleRot, matInvScaleRot;
	D3DXVECTOR3 vScale(matScale._11, matScale._22, matScale._33); // 기존의 정보를 입력. 정확히 스케일 행렬은 11 = x축성분, 22= y축성분, 33=z축 성분 이다.

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


		if (pStartTrack)//NODE_TM 값보다 최신인걸로. 핵심이네.
		{ // pStartTrack를 찾을수 있으면  기존의 정보를 업데이트한다.
			vScale = pStartTrack->Vector_in_Animation;
			
			// 쿼터니온.
			qS = pStartTrack->qRotate; //D3DXQUATERNION  qRotate;
			fStart_Tick = pStartTrack->iTick;
		}

		if (pEndTrack)// pEndTrack를 찾을수 있으면
		{
			fEnd_Tick = pEndTrack->iTick;
			D3DXVec3Lerp(&vScale, &vScale, &pEndTrack->Vector_in_Animation, (David_FrameTick - fStart_Tick) / (fEnd_Tick - fStart_Tick));
			//현재보간中벡터//기존벡터//나중벡터                        //  (현재시간 - 처음 Key 시간) / (나중 Key 시간 - 현재 Key 시간)

			D3DXQuaternionSlerp(&qS, &qS, &pEndTrack->qRotate, (David_FrameTick - fStart_Tick) / (fEnd_Tick - fStart_Tick));
			//현재보간中쿼터//기존쿼터//나중쿼터        //  (현재시간 - 처음 Key 시간) / (나중 Key 시간 - 현재 Key 시간)

			//Interpolates between two quaternions, using spherical linear interpolation.

			/*	D3DXQUATERNION* D3DXQuaternionSlerp(
			_Inout_       D3DXQUATERNION *pOut,
			_In_    const D3DXQUATERNION *pQ1,
			_In_    const D3DXQUATERNION *pQ2,
			_In_          FLOAT          t

			Parameter that indicates how far to interpolate between the quaternions.
			*/

		}
		D3DXMatrixScaling(&matScale, vScale.x, vScale.y, vScale.z); // 현재 보간 中 벡터로 matScale을 업데이트.
		D3DXMatrixRotationQuaternion(&matScaleRot, &qS); // 현재 보간 中 쿼터니온을 행렬 matScaleRot으로 반환 
		D3DXMatrixInverse(&matInvScaleRot, NULL, &matScaleRot); // 이걸 역행렬화함.
		matScale = matInvScaleRot * matScale * matScaleRot; // 이게 임의의 축 Sclae의 기본임. 역행렬 한번하고, 평범상태에서 스케일 하고, 한번더 임의의 축 스케일한다.
	}

	// Visible 트랙 안에 담긴 정보들로 결국 "David_fVisibility"를 업데이트.

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
		{// pStartTrack를 찾을수 있으면
			fCurrent_Alpha = pStartTrack->Vector_in_Animation.x;
			fStart_Tick = pStartTrack->iTick;
		}

		if (pEndTrack)
			// pEndTrack를 찾을수 있으면
		{
			fNext_Alpha = pEndTrack->Vector_in_Animation.x;
			fEnd_Tick = pEndTrack->iTick;

			fOffSet = (David_FrameTick - fStart_Tick) / (fEnd_Tick - fStart_Tick);
			//선형 보간의 s 값.
			fNext_Alpha = (fNext_Alpha - fCurrent_Alpha)*fOffSet;
			// 자체 선형 벡터임.
		}
		David_fVisibility = (fCurrent_Alpha + fNext_Alpha); // 저절로 前의 것에 지금 현재 보간 값을 더함.
	}
	else
	{
		David_fVisibility = 1.0f;
	}
	//

	// 최종 결과


	D3DXMatrixMultiply(&matAnim, &matScale, &matRotate);

	matAnim._41 = matPos._41;
	matAnim._42 = matPos._42;
	matAnim._43 = matPos._43;      //S,R,T

								   // 최종 에미메이션 행렬을 완성한다.	
	D3DXMatrixMultiply(&David_matCalculation, &matAnim, matParents); // 순서중요하다. 자식 * 부모 : 달 자전 지구 공전 해놓고, 태양 공전해야한다.

														 // 인버스 매트릭스 확인 코드.
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
//{// ASE에서 MESH로 전환하는 역활을 한다.
//
//	for (DWORD dwObject = 0; dwObject < m_Parser.m_pAseMesh.size(); dwObject++)
//	{
//		auto pMesh = make_shared<TMesh>(); // 여기에 정의한다.
//		auto pData = make_shared<tAseData>(); // 여기에 정의했다. 이걸 뭔가 써먹겠지?
//
//		pMesh->m_iIndex = dwObject;
//		pMesh->m_matWorld = m_Parser.m_pAseMesh[dwObject]->m_matWorld;		// 월드행렬도 넘긴다	
//		pMesh->m_iNumFace = m_Parser.m_pAseMesh[dwObject]->m_iNumFace;		// 전체페이스 갯수	도 넘긴다				
//		pMesh->m_strNodeName = m_Parser.m_pAseMesh[dwObject]->m_strNodeName; // 노드 네임도 넘긴다.
//		pMesh->m_strParentName = m_Parser.m_pAseMesh[dwObject]->m_strParentName; // 페어런트 네임 창이 따로 있나보다.
//
//		pMesh->m_matCalculation = m_Parser.m_pAseMesh[dwObject]->m_matWorld; // interpolate() 계산 최종 행렬 넘긴다	
//		pMesh->m_matWorldTrans = m_Parser.m_pAseMesh[dwObject]->m_matWorldTrans; //interpolate() 결과 이동 행렬 넘긴다	
//		pMesh->m_matWorldRotate = m_Parser.m_pAseMesh[dwObject]->m_matWorldRotate;//interpolate() 결과 회전 행렬 넘긴다	
//		pMesh->m_matWorldScale = m_Parser.m_pAseMesh[dwObject]->m_matWorldScale;//interpolate() 결과 스케일 행렬 넘긴다	
//
//		재끼고
//
//
//		D3DXMatrixInverse(&pMesh->m_matInverse, NULL, &pMesh->m_matWorld);
//
//		int iRootMtrRef = m_Parser.m_pAseMesh[dwObject]->m_iMtrlRef; //메터리얼 정보도 넘긴다.
//
//		TMtrl* pMtrl = NULL;
//		if (iRootMtrRef >= 0 && iRootMtrRef < m_Material.size())
//		{
//			pMtrl = &m_Material[iRootMtrRef]; //메터리얼 정보도 넘긴다.
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
//				// 2번 인자값=-1  Face Count(_countof )를 계산하지 않는다.
//				pData->SetUniqueBuffer(pData->m_TriList, -1, 0);
//				pMesh->m_iDiffuseTex = -1;
//				if (pMtrl)	pMesh->m_iDiffuseTex = GetMapID(pMtrl, ID_TBASIS_DI);
//				m_iMaxVertex += pData->m_VertexArray.size(); // 버텍스 사이즈 넘긴다.
//				m_iMaxIndex += pData->m_IndexArray.size();// 인덱스 사이즈 넘긴다.
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
//					pSubData->m_iNumFace = pSubData->m_IndexArray.size() / 3; // 다 넘기고 ㅣㅆ다.
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
//		재끼고
//
//
//		watch.Output(L"Sub Mesh");
//		// 에니메이션 트랙 복사		
//		TAnimTrack* pPrevTrack = NULL; // 포지션 트랙 MESH로 넘김.
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
//		재끼고
//
//		pPrevTrack = NULL;
//		for (int iTrack = 0; iTrack < m_Parser.m_pAseMesh[dwObject]->m_RotTrack.size(); iTrack++)
//		{
//			auto pTrack = make_shared<TAnimTrack>();
//
//			pTrack->iTick = m_Parser.m_pAseMesh[dwObject]->m_RotTrack[iTrack].iTick;
//
//			// 임의의 축과 각을 쿼터니언으로 변환			
//			D3DXQuaternionRotationAxis(&pTrack->qRotate, &D3DXVECTOR3(m_Parser.m_pAseMesh[dwObject]->m_RotTrack[iTrack].qRotate.x,
//				m_Parser.m_pAseMesh[dwObject]->m_RotTrack[iTrack].qRotate.y,
//				m_Parser.m_pAseMesh[dwObject]->m_RotTrack[iTrack].qRotate.z),
//				m_Parser.m_pAseMesh[dwObject]->m_RotTrack[iTrack].qRotate.w);
//			// 이전트랙의 쿼터니온과 누적시킴.			
//			if (pPrevTrack != NULL)
//			{
//				D3DXQuaternionMultiply(&pTrack->qRotate, &pPrevTrack->qRotate, &pTrack->qRotate);
//			}
//
//			pPrevTrack = SetDoublyLinkedList(pTrack.get(), pPrevTrack);
//
//			pMesh->m_pRotTrack.push_back(pTrack); // 회전 트랙 MESH로 넘김.
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
//			// 임의의 축과 각을 쿼터니언으로 변환			
//			D3DXQuaternionRotationAxis(&pTrack->qRotate,
//				&D3DXVECTOR3(m_Parser.m_pAseMesh[dwObject]->m_SclTrack[iTrack].qRotate.x,
//					m_Parser.m_pAseMesh[dwObject]->m_SclTrack[iTrack].qRotate.y,
//					m_Parser.m_pAseMesh[dwObject]->m_SclTrack[iTrack].qRotate.z),
//				m_Parser.m_pAseMesh[dwObject]->m_SclTrack[iTrack].qRotate.w);
//
//			pPrevTrack = SetDoublyLinkedList(pTrack.get(), pPrevTrack);
//
//			pMesh->m_pSclTrack.push_back(pTrack);  // 스케일 트랙 MESH로 넘김.
//		}
//
//		// 에니메이션 트랙 복사		
//		pPrevTrack = NULL;
//		for (int iTrack = 0; iTrack < m_Parser.m_pAseMesh[dwObject]->m_VisTrack.size(); iTrack++)
//		{
//			auto pTrack = make_shared<TAnimTrack>();
//			pTrack->iTick = m_Parser.m_pAseMesh[dwObject]->m_VisTrack[iTrack].iTick;
//			pTrack->vVector = m_Parser.m_pAseMesh[dwObject]->m_VisTrack[iTrack].vVector;
//
//			pPrevTrack = SetDoublyLinkedList(pTrack.get(), pPrevTrack);
//			pMesh->m_pVisTrack.push_back(pTrack);    // 스케일 트랙 MESH로 넘김.
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
//	// 사용되지 않은 오브젝트 삭제
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