#include "Plane.h"
#include "Frustum_Box.h"
#include "Box.h"
#include "Line.h"
#include "Plane_for_SkyBox.h"
#include "Map.h"
#include "Mesh.h"

int end_decision;

enum TSceneState
{
	GAME_SCENE_LOBBY = 0,
	GAME_SCENE_PLAY = 1,
	GAME_SCENE_END = 2,
};

class TScene
{
public:
	int		m_iSceneID;
	bool	m_bNextSceneStart;

public:
	virtual bool	Init() { return true; }
	virtual bool	Frame() { return true; }
	virtual bool	Render() { return true; }
	virtual bool	Release() { return true; }

public:
	TScene()
	{
		m_bNextSceneStart = false;
	}
	virtual ~TScene() {}
};



class TSceneLobby : public TScene
{
public:

	Canon_Camera Main_Camera;
	Canon_Camera Right_Camera;
	Canon_Camera Top_Camera;
	Canon_Camera Beneath_Camera;

	void camera_operation();

public:
	//Frustum_Box Camera_Frustum_Box;

public:
	
	Plane_for_SkyBox up;
	Plane_for_SkyBox down;
	Plane_for_SkyBox left;
	Plane_for_SkyBox right;
	Plane_for_SkyBox front;
	Plane_for_SkyBox back;

public:
	
	Mesh    Mesh;

public:
	Map		yang;

public: // 노멀 벡타를 화면에 출력하기 위해

	Line_Axis_s Normal_lines;

public:

	
	Line_Axis_s F_Axis_s;

public:
	bool mesh_getLocal();
	bool QUATERNIONIZE_Rotation_TRACK();
	bool Rotation_TRACK_Accumulation();
	bool mesh_CalculrationMatrix_out();
	bool m_matCalculation_Decompose();
	bool mesh_CalculrationMatrix_accumulation(GeometryMesh& David_GeometryMesh);


public:
	virtual bool Init();
	bool	Frame();
	bool	Render();
	bool	Release();

public:
	TSceneLobby();
	virtual ~TSceneLobby();
};


bool TSceneLobby::mesh_getLocal()
{
	D3DXMATRIX Inverse_Matrix_For_Local;
	D3DXMatrixInverse(&Inverse_Matrix_For_Local, NULL, &Mesh.ASE.GeometryMesh_per_Object_List[0].m_matWorld);

	for (int iVertex = 0; iVertex < Mesh.m_ObjectList[0].m_VertexList.size(); iVertex++)
	{D3DXVec3TransformCoord(&Mesh.m_ObjectList[0].m_VertexList[iVertex].p, &Mesh.m_ObjectList[0].m_VertexList[iVertex].p, &Inverse_Matrix_For_Local);}

	
	D3DXMatrixInverse(&Inverse_Matrix_For_Local, NULL, &Mesh.ASE.GeometryMesh_per_Object_List[1].m_matWorld);
	for (int iVertex = 0; iVertex < Mesh.m_ObjectList[1].m_VertexList.size(); iVertex++)
	{
		D3DXVec3TransformCoord(&Mesh.m_ObjectList[1].m_VertexList[iVertex].p, &Mesh.m_ObjectList[1].m_VertexList[iVertex].p, &Inverse_Matrix_For_Local);
	}
	return true;
}


bool TSceneLobby::QUATERNIONIZE_Rotation_TRACK()
{
	for (int i = 0; i < Mesh.ASE.GeometryMesh_per_Object_List.size(); i++)
	{
		for (int j = 0;j < Mesh.ASE.GeometryMesh_per_Object_List[i].m_pRotTrack.size(); j++)
		{
			TAnimTrack pTrack;
			D3DXVECTOR3 David_vector;
			float David_Angle;
			pTrack.iTick = Mesh.ASE.GeometryMesh_per_Object_List[i].m_pRotTrack[j].iTick;
			David_vector.x = Mesh.ASE.GeometryMesh_per_Object_List[i].m_pRotTrack[j].qRotate.x;
			David_vector.y = Mesh.ASE.GeometryMesh_per_Object_List[i].m_pRotTrack[j].qRotate.y;
			David_vector.z = Mesh.ASE.GeometryMesh_per_Object_List[i].m_pRotTrack[j].qRotate.z;
			David_Angle = Mesh.ASE.GeometryMesh_per_Object_List[i].m_pRotTrack[j].qRotate.w;
			D3DXQuaternionRotationAxis
			(&pTrack.qRotate,&David_vector,David_Angle);
			Mesh.ASE.GeometryMesh_per_Object_List[i].m_pRotTrack[j] = pTrack;
		}
	}
	return true;
}


bool TSceneLobby::Rotation_TRACK_Accumulation()
{
	for (int i = 0; i < Mesh.ASE.GeometryMesh_per_Object_List.size(); i++)
	{
		for (int j = 0; j < Mesh.ASE.GeometryMesh_per_Object_List[i].m_pRotTrack.size() - 1; j++)
		{
			D3DXQuaternionMultiply(&Mesh.ASE.GeometryMesh_per_Object_List[i].m_pRotTrack[j + 1].qRotate,
				&Mesh.ASE.GeometryMesh_per_Object_List[i].m_pRotTrack[j].qRotate,
				&Mesh.ASE.GeometryMesh_per_Object_List[i].m_pRotTrack[j + 1].qRotate);
		}
	}
	return true;
}


bool TSceneLobby::m_matCalculation_Decompose()
{
	D3DXVECTOR3 vScale;
	D3DXQUATERNION qR;
	D3DXVECTOR3 vTrans;

	D3DXMatrixDecompose(&vScale, &qR, &vTrans, &Mesh.ASE.GeometryMesh_per_Object_List[0].m_matCalculation);
	D3DXMatrixScaling(&Mesh.ASE.GeometryMesh_per_Object_List[0].m_matWorldScale, vScale.x, vScale.y, vScale.z);
	D3DXMatrixTranslation(&Mesh.ASE.GeometryMesh_per_Object_List[0].m_matWorldTrans, vTrans.x, vTrans.y, vTrans.z);
	D3DXMatrixRotationQuaternion(&Mesh.ASE.GeometryMesh_per_Object_List[0].m_matWorldRotate, &qR);

	return true;
}


bool TSceneLobby::mesh_CalculrationMatrix_out()  // 모든 오브젝트들의 Calculation 행렬들 Frame마다, 계산해 나오게 한다.
{
	Mesh.m_fElapseTime += g_fSecPerFrame * Mesh.m_Scene.Frames_per_second * Mesh.m_Scene.Tick_Per_Frame;// 몇 틱을 더 갔느냐가 더해진다. 틱 단위 시간 (g_fSecPerFrame는 동기화다)
	if (Mesh.m_fElapseTime >= Mesh.m_Scene.Last_Frame* Mesh.m_Scene.Tick_Per_Frame)
		// 애니메이션 시간 16000 다 지나면,
	{
		Mesh.m_fElapseTime = Mesh.m_Scene.First_Frame * Mesh.m_Scene.Tick_Per_Frame; // 원래대로 복귀해준다.
	}
	//vector<Mesh> m_ObjectList;
	//vector<Mesh> m_Object_SubObjectList; //안에 있는 오브젝트 하나하나 마다.시간대별로 Calculation 행렬, 계산해 나오게 한다.


	D3DXMATRIX Parent_mat;
	D3DXMatrixIdentity(&Parent_mat);
	D3DXMATRIX Parent_inverse_mat;

	static int a = 0;

	for (int i = 0; i < Mesh.ASE.GeometryMesh_per_Object_List.size(); i++)
	{
		if(a==0)
		{
			if (Mesh.ASE.GeometryMesh_per_Object_List[i].Parent_GeoMesh_ptr != NULL) // 부모가 있으면, 이 경우는 쉽게 가자.
			{			
				D3DXMatrixInverse(&Parent_inverse_mat, NULL, &Mesh.ASE.GeometryMesh_per_Object_List[i].Parent_GeoMesh_ptr->m_matWorld);
				Mesh.ASE.GeometryMesh_per_Object_List[i].m_matWorld = Mesh.ASE.GeometryMesh_per_Object_List[i].m_matWorld *Parent_inverse_mat;
			
				D3DXVECTOR3 vScale;
				D3DXQUATERNION qR;
				D3DXVECTOR3 vTrans;

				D3DXMatrixDecompose(&vScale, &qR, &vTrans, &Mesh.ASE.GeometryMesh_per_Object_List[i].m_matWorld);
				D3DXMatrixScaling(&Mesh.ASE.GeometryMesh_per_Object_List[i].m_matWorldScale, vScale.x, vScale.y, vScale.z);
				D3DXMatrixTranslation(&Mesh.ASE.GeometryMesh_per_Object_List[i].m_matWorldTrans, vTrans.x, vTrans.y, vTrans.z);
				D3DXMatrixRotationQuaternion(&Mesh.ASE.GeometryMesh_per_Object_List[i].m_matWorldRotate, &qR);
			}
		}

		Mesh.Interpolate(&Mesh, &Parent_mat,
		Mesh.m_fElapseTime,
		Mesh.ASE.GeometryMesh_per_Object_List[i].m_matCalculation,
		Mesh.ASE.GeometryMesh_per_Object_List[i].m_matWorldRotate,
		Mesh.ASE.GeometryMesh_per_Object_List[i].m_matWorldTrans,
		Mesh.ASE.GeometryMesh_per_Object_List[i].m_matWorldScale,
		Mesh.ASE.GeometryMesh_per_Object_List[i].m_pRotTrack,
		Mesh.ASE.GeometryMesh_per_Object_List[i].m_PosTrack,
		Mesh.ASE.GeometryMesh_per_Object_List[i].m_pSclTrack,
		Mesh.ASE.GeometryMesh_per_Object_List[i].m_pVisTrack,
		Mesh.ASE.m_Scene,					  
		Mesh.ASE.GeometryMesh_per_Object_List[i].m_fVisibility);
	}
	a++;
	return true;
}


bool TSceneLobby::mesh_CalculrationMatrix_accumulation(GeometryMesh& David_GeometryMesh)
{
			if (David_GeometryMesh.Parent_GeoMesh_ptr != NULL) // 부모가 있으면, 이 경우는 쉽게 가자.
			{
				mesh_CalculrationMatrix_accumulation(*David_GeometryMesh.Parent_GeoMesh_ptr);//다 들어가겠죠. 그래서 최종 자신이 루트인게 나오겠죠.
				D3DXMatrixMultiply(&David_GeometryMesh.m_matCalculation_acculated,
					&David_GeometryMesh.m_matCalculation,
					&David_GeometryMesh.Parent_GeoMesh_ptr->m_matCalculation_acculated);
				return true;
			}	
			else if(David_GeometryMesh.Parent_GeoMesh_ptr == NULL) // 이제부터 쌓기 시작해야 한다.
			{
				David_GeometryMesh.m_matCalculation_acculated = David_GeometryMesh.m_matCalculation;
				return true; // 돌아나와야죠 ㅋㅋ
			}
	return true;
}



void TSceneLobby::camera_operation()
{

	D3DXVECTOR4   vYawPitchRoll(0, 0, 0, 0);


	// 요
	if (I_Input.Key('U'))
	{
		vYawPitchRoll.x += 0.1f*g_fSecPerFrame;
	}


	if (I_Input.Key('I'))
	{
		vYawPitchRoll.x -= 0.1f*g_fSecPerFrame;
	}

	// 피치

	if (I_Input.Key('J'))
	{
		vYawPitchRoll.y += 0.1f*g_fSecPerFrame;
	}

	if (I_Input.Key('K'))
	{
		vYawPitchRoll.y -= 0.1f*g_fSecPerFrame;
	}

	// 롤

	if (I_Input.Key('N'))
	{
		vYawPitchRoll.z += 0.1f*g_fSecPerFrame;
	}

	if (I_Input.Key('M'))
	{
		vYawPitchRoll.z -= 0.1f*g_fSecPerFrame;
	}


	// 스피드 부스터
	if (I_Input.Key('B'))
	{
		Main_Camera.SetSpeed(g_fSecPerFrame*3.0f);
		/*Right_Camera.SetSpeed(g_fSecPerFrame*3.0f);
		Top_Camera.SetSpeed(g_fSecPerFrame*3.0f);
		Beneath_Camera.SetSpeed(g_fSecPerFrame*3.0f);*/
	}

	// 위 아래 걸음


	if (I_Input.Key('Q'))
	{
		Main_Camera.Move_on_Up_line(-g_fSecPerFrame * 5.0f);
		/*Right_Camera.Move_on_Up_line(-g_fSecPerFrame * 5.0f);
		Top_Camera.Move_on_Up_line(-g_fSecPerFrame * 5.0f);
		Beneath_Camera.Move_on_Up_line(-g_fSecPerFrame * 5.0f);*/


	}

	if (I_Input.Key('W'))
	{
		Main_Camera.Move_on_Up_line(g_fSecPerFrame*5.0f);
		/*Right_Camera.Move_on_Up_line(g_fSecPerFrame * 5.0f);
		Top_Camera.Move_on_Up_line(g_fSecPerFrame * 5.0f);
		Beneath_Camera.Move_on_Up_line(g_fSecPerFrame * 5.0f);*/
	}



	// 앞,뒤 걸음

	if (I_Input.Key('A'))
	{
		Main_Camera.Move_on_Look_line(-g_fSecPerFrame * 5.0f);
	/*	Right_Camera.Move_on_Look_line(-g_fSecPerFrame * 5.0f);
		Top_Camera.Move_on_Look_line(-g_fSecPerFrame * 5.0f);
		Beneath_Camera.Move_on_Look_line(-g_fSecPerFrame * 5.0f);*/
	}

	if (I_Input.Key('S'))
	{
		Main_Camera.Move_on_Look_line(g_fSecPerFrame * 5.0f);
	/*	Right_Camera.Move_on_Look_line(g_fSecPerFrame * 5.0f);
		Top_Camera.Move_on_Look_line(g_fSecPerFrame * 5.0f);
		Beneath_Camera.Move_on_Look_line(g_fSecPerFrame * 5.0f);*/
	}

	// 양 옆 게 걸음

	if (I_Input.Key('Z'))
	{
		Main_Camera.Move_on_Right_line(-g_fSecPerFrame * 5.0f);
		/*Right_Camera.Move_on_Right_line(-g_fSecPerFrame * 5.0f);
		Top_Camera.Move_on_Right_line(-g_fSecPerFrame * 5.0f);
		Beneath_Camera.Move_on_Right_line(-g_fSecPerFrame * 5.0f);*/
	}

	if (I_Input.Key('X'))
	{
		Main_Camera.Move_on_Right_line(g_fSecPerFrame*5.0f);
	/*	Right_Camera.Move_on_Right_line(g_fSecPerFrame * 5.0f);
		Top_Camera.Move_on_Right_line(g_fSecPerFrame * 5.0f);
		Beneath_Camera.Move_on_Right_line(g_fSecPerFrame * 5.0f);*/
	}

	Main_Camera.ViewMatrix_Update_From_Keyboard_Mouse(vYawPitchRoll);
	/*Right_Camera.ViewMatrix_Update_From_Keyboard_Mouse(vYawPitchRoll);
	Top_Camera.ViewMatrix_Update_From_Keyboard_Mouse(vYawPitchRoll);
	Beneath_Camera.ViewMatrix_Update_From_Keyboard_Mouse(vYawPitchRoll);*/
}

bool	TSceneLobby::Init()
{


	Main_Camera.Set_View_Matrix(D3DXVECTOR3(0.0f, 1000.0f, 1.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	Main_Camera.Set_Projection_Matrix(0.25, 1, 1, 100);

	/*Right_Camera.Set_View_Matrix(D3DXVECTOR3(250.0f, 50.0f, 50.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	Right_Camera.Set_Projection_Matrix(0.5, 1, 1, 100);*/

	Mesh.Load_ASEfile_Trans_VertexLists_to_all_meshes_SceneInfo_Insert();
	QUATERNIONIZE_Rotation_TRACK();
	Rotation_TRACK_Accumulation();
	mesh_getLocal();
	



	/*Camera_Frustum_Box.Set_VertexData(); Camera_Frustum_Box.set_IndexData();
	Camera_Frustum_Box.Create(g_pd3dDevice, L"HLSL.vsh", L"HLSL.psh", L"../../data/bitmap1.bmp", "PSLine");*/


/*
	FSphere.Set_VertexData(); FSphere.set_IndexData();
	FSphere.transposition(D3DXVECTOR3(-3.0f,0.0f,0.0f));
	FSphere.Create(g_pd3dDevice, L"HLSL.vsh", "VS_NoMatrix", L"HLSL.psh", L"GeometryShader.hlsl", L"../../data/main_start_sel.bmp");
	*/
	

	
	/*yang.Extract_Height_Map_Datas_R_0255_G_0255_B_0255_A0255_From_looking_GrayScale_TextureFile_Resource_Using_CPU(g_pd3dDevice, g_pContext, L"../../data/map/HEIGHT_MOUNTAIN.bmp");
	yang.Cell_distance_input_x_z_HighScale(60, 60,5);
	yang.Set_VertexData();
	yang.set_IndexData();
	Sun_Light_Rambert_Function();
	yang.Create(g_pd3dDevice, L"HLSL.vsh", L"HLSL.psh", L"../../data/map/mountain.jpg");
*/

	//


	for (int iObj = 0; iObj < Mesh.ASE.GeometryMesh_per_Object_List.size(); iObj++)
	{

		Mesh.m_ObjectList[iObj].Create(g_pd3dDevice, L"HLSL.vsh", L"HLSL.psh", Mesh.ASE.Material_List[iObj].TextureMap_List[0].name.c_str());
	}




	//T_STR texName = Mesh.ASE.Material_List[0].TextureMap_List[0].name;
	//Mesh.m_ObjectList[0].Create(g_pd3dDevice, L"HLSL.vsh", L"HLSL.psh", texName.c_str());


	//int SubMatrial_numbers = Mesh.ASE.Material_List[1].Sub_Material_List.size();
	//for (int iSub = 0; iSub < SubMatrial_numbers; iSub++)
	//{
	//	Mesh.m_ObjectList[0].m_Object_SubObjectList[iSub].m_icount_Vertexs = Mesh.m_ObjectList[0].m_Object_SubObjectList[iSub].m_VertexList.size();
	//	T_STR texName = Mesh.ASE.Material_List[1].Sub_Material_List[iSub].TextureMap_List[0].name;
	//	Mesh.m_ObjectList[0].m_Object_SubObjectList[iSub].Create(g_pd3dDevice, L"HLSL.vsh", L"HLSL.psh", texName.c_str());
	//}



	//



	up.Set_VertexData(); up.set_IndexData();
	up.Create(g_pd3dDevice, L"HLSL.vsh", L"HLSL.psh", L"../../data/st00_cm_up.bmp",0, "VS_SKY", "PS_SKY");


	down.Set_VertexData(); down.set_IndexData();
	down.Create(g_pd3dDevice, L"HLSL.vsh", L"HLSL.psh", L"../../data/st00_cm_down.bmp",0, "VS_SKY", "PS_SKY");


	left.Set_VertexData(); left.set_IndexData();
	left.Create(g_pd3dDevice, L"HLSL.vsh", L"HLSL.psh", L"../../data/st00_cm_left.bmp",0, "VS_SKY", "PS_SKY");


	right.Set_VertexData(); right.set_IndexData();
	right.Create(g_pd3dDevice, L"HLSL.vsh", L"HLSL.psh", L"../../data/st00_cm_right.bmp",0, "VS_SKY", "PS_SKY");


	front.Set_VertexData(); front.set_IndexData();
	front.Create(g_pd3dDevice, L"HLSL.vsh", L"HLSL.psh", L"../../data/st00_cm_front.bmp",0, "VS_SKY", "PS_SKY");


	back.Set_VertexData(); back.set_IndexData();
	back.Create(g_pd3dDevice, L"HLSL.vsh", L"HLSL.psh", L"../../data/st00_cm_back.bmp",0, "VS_SKY", "PS_SKY");
	

	//

	Normal_lines.Set_VertexData(D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f));	Normal_lines.set_IndexData();
	Normal_lines.Create(g_pd3dDevice, L"HLSL.vsh", L"HLSL.psh", L"../../data/David.jpg", "PSLine");


	//


	F_Axis_s.Set_VertexData(D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f,0.0f,0.0f), D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f));	F_Axis_s.set_IndexData();
	F_Axis_s.Create(g_pd3dDevice, L"HLSL.vsh", L"HLSL.psh", L"../../data/David.jpg","PSLine");





	return true;
};

 
bool	TSceneLobby::Frame()
{

	camera_operation();
	//Camera_Frustum_Box.total_Vertex_Update_for_Frustum(&Main_Camera.m_matView, &Main_Camera.m_matProj);

	Main_Camera.camera_position;
	D3DXMATRIX mTranslate_following_camera;
	D3DXMatrixTranslation(&mTranslate_following_camera, Main_Camera.camera_position.x, Main_Camera.camera_position.y, Main_Camera.camera_position.z);


	//float t = g_fGameTimer * D3DX_PI;
	//D3DXMATRIX mTranslate_ship; D3DXMATRIX mSpin_ship; D3DXMATRIX mOrbit_ship;
	D3DXMATRIX mTranslate_map;
	D3DXMATRIX mTranslate_front; 	D3DXMATRIX mTranslate_down; D3DXMATRIX mTranslate_right; D3DXMATRIX mTranslate_up; D3DXMATRIX mTranslate_left; D3DXMATRIX mTranslate_back;
	D3DXMATRIX mSpin_up; D3DXMATRIX mSpin_down; D3DXMATRIX mSpin_right; D3DXMATRIX mSpin_left; D3DXMATRIX mSpin_front; D3DXMATRIX mSpin_back;
	D3DXMATRIX mScale; D3DXMATRIX mScale_2;

	D3DXMatrixScaling(&mScale, 100.0f, 100.0f, 0.0f);
	D3DXMatrixScaling(&mScale_2, 50.0f, 50.0f, 50.0f);

	//UP
	D3DXMatrixRotationX(&mSpin_up, -D3DX_PI/2);
	D3DXMatrixTranslation(&mTranslate_up, 0.0f, 100.0f, 0.0f); // 트랜스래잇 행렬
	up.m_matWorld = mScale * mSpin_up * mTranslate_up*mTranslate_following_camera;

	//DOWN
	D3DXMatrixRotationX(&mSpin_down, D3DX_PI / 2);
	D3DXMatrixTranslation(&mTranslate_down, 0.0f, -100.0f, 0.0f); // 트랜스래잇 행렬
	down.m_matWorld = mScale * mSpin_down * mTranslate_down*mTranslate_following_camera;

	//RIGHT															  
	D3DXMatrixRotationY(&mSpin_right, D3DX_PI / 2);
	D3DXMatrixTranslation(&mTranslate_right, 100.0f, 0.0f, 0.0f); // 트랜스래잇 행렬
	right.m_matWorld = mScale* mSpin_right*mTranslate_right*mTranslate_following_camera;


	//LEFT													 
	D3DXMatrixRotationY(&mSpin_left, -D3DX_PI / 2);
	D3DXMatrixTranslation(&mTranslate_left, -100.0f, -0.0f, 0.0f); // 트랜스래잇 행렬
	left.m_matWorld = mScale * mSpin_left*mTranslate_left*mTranslate_following_camera;
	
	//FRONT
	//D3DXMatrixRotationY(&mSpin_front, -D3DX_PI);
	D3DXMatrixTranslation(&mTranslate_front, 0.0f, 0.0f, +100.0f); // 트랜스래잇 행렬
	front.m_matWorld = mScale * mTranslate_front*mTranslate_following_camera;

	//BACK
	D3DXMatrixRotationY(&mSpin_back, -D3DX_PI);
	D3DXMatrixTranslation(&mTranslate_back, 0.0f, 0.0f, -100.0f); // 트랜스래잇 행렬
	back.m_matWorld = mScale  * mSpin_back*mTranslate_back*mTranslate_following_camera;

	
	//D3DXMatrixTranslation(&mTranslate_map, 0.0f, -1500.0f, 0.0f); // 트랜스래잇 행렬
	//yang.m_matWorld = mTranslate_map;

	///*D3DXMATRIX mTranslate_ship; D3DXMATRIX mSpin_ship; D3DXMATRIX mOrbit_ship;*/
	//D3DXMatrixTranslation(&mTranslate_ship, 300.0f, 800.0f, 0.0f); // 트랜스래잇 행렬
	//D3DXMatrixRotationY(&mOrbit_ship, t/10);
	//D3DXMatrixRotationZ(&mSpin_ship, t / 10);
	

	//Mesh.m_ObjectList[1].m_matWorld = mTranslate_ship * mOrbit_ship;

	//int SubMatrial_numbers = Mesh.ASE.Material_List[1].Sub_Material_List.size();
	//for (int iSub = 0; iSub < SubMatrial_numbers; iSub++)
	//{
	//	Mesh.m_ObjectList[0].m_Object_SubObjectList[iSub].m_matWorld= mSpin_ship* mTranslate_ship* mOrbit_ship;
	//}

	mesh_CalculrationMatrix_out();
	


	////

	

	//D3DXMatrixTranslation(&mTranslate, 0.0f, 3.0f, 0.0f); // 트랜스래잇 행렬
	//D3DXMatrixRotationZ(&earth_mOrbit, t*0.2);


	//D3DXMatrixTranslation(&mTranslate_2, 0.0f, 0.0f, 3.0f); // 트랜스래잇 행렬
	//D3DXMatrixRotationX(&moon_Orbit, t*0.5);



	//FSphere.m_matWorld = mScale * mTranslate * earth_mOrbit;
	


	return true;
};


bool	TSceneLobby::Render()
{

	// 1번 뷰포트
	/*
	FSphere.HandOver_world_view_projection_Matrixs_to_ConstantData_Final_extract(&FSphere.m_matWorld, &Main_Camera.m_matView, &Main_Camera.m_matProj);		FSphere.update_ConstantBuffer();
	FBox.HandOver_world_view_projection_Matrixs_to_ConstantData_Final_extract(&FBox.m_matWorld, &Main_Camera.m_matView, &Main_Camera.m_matProj);			FBox.update_ConstantBuffer();*/
	/*
	for (int iObj = 0; iObj < Mesh.ASE.GeometryMesh_per_Object_List.size(); iObj++)
	{
		Mesh.m_ObjectList[iObj].HandOver_world_view_projection_Matrixs_to_ConstantData_Final_extract(&Mesh.m_ObjectList[iObj].m_matWorld, &Main_Camera.m_matView, &Main_Camera.m_matProj);
		Mesh.m_ObjectList[iObj].update_ConstantBuffer();
	}
*/



	mesh_CalculrationMatrix_accumulation(Mesh.ASE.GeometryMesh_per_Object_List[0]);
	mesh_CalculrationMatrix_accumulation(Mesh.ASE.GeometryMesh_per_Object_List[1]);
	mesh_CalculrationMatrix_accumulation(Mesh.ASE.GeometryMesh_per_Object_List[2]);


	Mesh.m_ObjectList[0].HandOver_world_view_projection_Matrixs_to_ConstantData_Final_extract
	(&Mesh.ASE.GeometryMesh_per_Object_List[0].m_matCalculation_acculated,&Main_Camera.m_matView, &Main_Camera.m_matProj);
	Mesh.m_ObjectList[0].update_ConstantBuffer();

	Mesh.m_ObjectList[1].HandOver_world_view_projection_Matrixs_to_ConstantData_Final_extract
	(&Mesh.ASE.GeometryMesh_per_Object_List[1].m_matCalculation_acculated,	&Main_Camera.m_matView, &Main_Camera.m_matProj);
	Mesh.m_ObjectList[1].update_ConstantBuffer();

	Mesh.m_ObjectList[2].HandOver_world_view_projection_Matrixs_to_ConstantData_Final_extract
	(&Mesh.ASE.GeometryMesh_per_Object_List[2].m_matCalculation_acculated, &Main_Camera.m_matView, &Main_Camera.m_matProj);
	Mesh.m_ObjectList[2].update_ConstantBuffer();



	//int SubMatrial_numbers = Mesh.ASE.Material_List[1].Sub_Material_List.size();
	//for (int iSub = 0; iSub < SubMatrial_numbers; iSub++)
	//{
	//	Mesh.m_ObjectList[0].m_Object_SubObjectList[iSub].HandOver_world_view_projection_Matrixs_to_ConstantData_Final_extract(&Mesh.m_ObjectList[0].m_Object_SubObjectList[iSub].m_matWorld, &Main_Camera.m_matView, &Main_Camera.m_matProj);
	//	Mesh.m_ObjectList[0].m_Object_SubObjectList[iSub].update_ConstantBuffer();
	//}
	
	/*
	yang.HandOver_world_view_projection_Matrixs_to_ConstantData_Final_extract(&yang.m_matWorld, &Main_Camera.m_matView, &Main_Camera.m_matProj);	yang.update_ConstantBuffer();
	*/
	up.HandOver_world_view_projection_Matrixs_to_ConstantData_Final_extract(&up.m_matWorld, &Main_Camera.m_matView, &Main_Camera.m_matProj);	up.update_ConstantBuffer();
	down.HandOver_world_view_projection_Matrixs_to_ConstantData_Final_extract(&down.m_matWorld, &Main_Camera.m_matView, &Main_Camera.m_matProj);	down.update_ConstantBuffer();
	left.HandOver_world_view_projection_Matrixs_to_ConstantData_Final_extract(&left.m_matWorld, &Main_Camera.m_matView, &Main_Camera.m_matProj);	left.update_ConstantBuffer();
	right.HandOver_world_view_projection_Matrixs_to_ConstantData_Final_extract(&right.m_matWorld, &Main_Camera.m_matView, &Main_Camera.m_matProj);	right.update_ConstantBuffer();
	front.HandOver_world_view_projection_Matrixs_to_ConstantData_Final_extract(&front.m_matWorld, &Main_Camera.m_matView, &Main_Camera.m_matProj);	front.update_ConstantBuffer();
	back.HandOver_world_view_projection_Matrixs_to_ConstantData_Final_extract(&back.m_matWorld, &Main_Camera.m_matView, &Main_Camera.m_matProj);	back.update_ConstantBuffer();

	F_Axis_s.HandOver_world_view_projection_Matrixs_to_ConstantData_Final_extract(&F_Axis_s.m_matWorld, &Main_Camera.m_matView, &Main_Camera.m_matProj);	F_Axis_s.update_ConstantBuffer();
	Normal_lines.HandOver_world_view_projection_Matrixs_to_ConstantData_Final_extract(&Normal_lines.m_matWorld, &Main_Camera.m_matView, &Main_Camera.m_matProj); Normal_lines.update_ConstantBuffer();

	

	//


	up.Render(g_pContext, 0);
	down.Render(g_pContext, 0);
	left.Render(g_pContext, 0);
	right.Render(g_pContext, 0);
	front.Render(g_pContext, 0);
	back.Render(g_pContext, 0);

	//back.PipeLineSetup.CreateDepthStencilState(TRUE); g_pContext->OMSetDepthStencilState(back.PipeLineSetup.m_pDepthStencilState, 0);


	//




	//Camera_Frustum_Box.Render(g_pContext, 1);
	
	/*
	yang.Render(g_pContext, 0);
*/



	
	Mesh.m_ObjectList[1].Render(g_pContext, 0);
	Mesh.m_ObjectList[2].Render(g_pContext, 0);
	Mesh.m_ObjectList[0].Render(g_pContext, 0);

	/*for (int iSub = 0; iSub < SubMatrial_numbers; iSub++)
	{
		Mesh.m_ObjectList[0].m_Object_SubObjectList[iSub].Render(g_pContext, 0);
	}*/


	//X축 랜더
	F_Axis_s.Set_VertexData(D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(10000.0f, 0.0f, 0.0f), D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f));
	g_pContext->UpdateSubresource(F_Axis_s.PipeLineSetup.m_pVertextBuffer, 0, NULL, &F_Axis_s.m_VertexList[0], 0, 0);
	F_Axis_s.PreRender(g_pContext, 0);	
	F_Axis_s.PostRender(g_pContext);
	//Y축 랜더
	
	F_Axis_s.Set_VertexData(D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 10000.0f, 0.0f), D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f));
	g_pContext->UpdateSubresource(F_Axis_s.PipeLineSetup.m_pVertextBuffer, 0, NULL, &F_Axis_s.m_VertexList[0], 0, 0);
	F_Axis_s.PreRender(g_pContext, 0);	F_Axis_s.PostRender(g_pContext);
	//Z축 랜더

	F_Axis_s.Set_VertexData(D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 10000.0f), D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f));
	g_pContext->UpdateSubresource(F_Axis_s.PipeLineSetup.m_pVertextBuffer, 0, NULL, &F_Axis_s.m_VertexList[0], 0, 0);
	F_Axis_s.PreRender(g_pContext, 0);	F_Axis_s.PostRender(g_pContext);



	// 노멀 벡터 라인들 출력


	//for (int iVertex = 0; iVertex < yang.m_VertexList.size(); iVertex += 30)
	//{
	//	D3DXVECTOR3 vNormal = yang.m_VertexList[iVertex].n;
	//	Normal_lines.Set_VertexData(yang.m_VertexList[iVertex].p, yang.m_VertexList[iVertex].p + vNormal * 100);
	//	//
	//	g_pContext->UpdateSubresource(Normal_lines.PipeLineSetup.m_pVertextBuffer, 0, NULL, &Normal_lines.m_VertexList[0], 0, 0);
	//	//
	//	Normal_lines.PreRender(g_pContext, 0); Normal_lines.PostRender(g_pContext);
	//}





	///////////////////////////////////////////////////////////////////////////////////////

	//// 2번 뷰포트

	///*FSphere.HandOver_world_view_projection_Matrixs_to_ConstantData_Final_extract(&FSphere.m_matWorld, &Right_Camera.m_matView, &Right_Camera.m_matProj);		FSphere.update_ConstantBuffer();
	//FBox.HandOver_world_view_projection_Matrixs_to_ConstantData_Final_extract(&FBox.m_matWorld, &Right_Camera.m_matView, &Right_Camera.m_matProj);			FBox.update_ConstantBuffer();*/
	//
	//Camera_Frustum_Box.HandOver_world_view_projection_Matrixs_to_ConstantData_Final_extract(&Camera_Frustum_Box.m_matWorld, &Right_Camera.m_matView, &Right_Camera.m_matProj);		Camera_Frustum_Box.update_ConstantBuffer();
	//F_Axis_s.HandOver_world_view_projection_Matrixs_to_ConstantData_Final_extract(&F_Axis_s.m_matWorld, &Right_Camera.m_matView, &Right_Camera.m_matProj);	F_Axis_s.update_ConstantBuffer();
	//F_Axis_y.HandOver_world_view_projection_Matrixs_to_ConstantData_Final_extract(&F_Axis_y.m_matWorld, &Right_Camera.m_matView, &Right_Camera.m_matProj);	F_Axis_y.update_ConstantBuffer();
	//F_Axis_z.HandOver_world_view_projection_Matrixs_to_ConstantData_Final_extract(&F_Axis_z.m_matWorld, &Right_Camera.m_matView, &Right_Camera.m_matProj);	F_Axis_z.update_ConstantBuffer();




	//up.HandOver_world_view_projection_Matrixs_to_ConstantData_Final_extract(&up.m_matWorld, &Right_Camera.m_matView, &Right_Camera.m_matProj);	up.update_ConstantBuffer();
	//down.HandOver_world_view_projection_Matrixs_to_ConstantData_Final_extract(&down.m_matWorld, &Right_Camera.m_matView, &Right_Camera.m_matProj);	down.update_ConstantBuffer();
	//left.HandOver_world_view_projection_Matrixs_to_ConstantData_Final_extract(&left.m_matWorld, &Right_Camera.m_matView, &Right_Camera.m_matProj);	left.update_ConstantBuffer();
	//right.HandOver_world_view_projection_Matrixs_to_ConstantData_Final_extract(&right.m_matWorld, &Right_Camera.m_matView, &Right_Camera.m_matProj);	right.update_ConstantBuffer();
	//front.HandOver_world_view_projection_Matrixs_to_ConstantData_Final_extract(&front.m_matWorld, &Right_Camera.m_matView, &Right_Camera.m_matProj);	front.update_ConstantBuffer();
	//back.HandOver_world_view_projection_Matrixs_to_ConstantData_Final_extract(&back.m_matWorld, &Right_Camera.m_matView, &Right_Camera.m_matProj);	back.update_ConstantBuffer();


	//up.Render(g_pContext, 2);
	//down.Render(g_pContext, 2);
	//left.Render(g_pContext, 2);
	//right.Render(g_pContext, 2);
	//front.Render(g_pContext, 2);
	//back.Render(g_pContext, 2);

	//
	//F_Axis_s.PreRender(g_pContext, 2);	F_Axis_s.PostRender(g_pContext);
	//F_Axis_y.PreRender(g_pContext, 2);	F_Axis_y.PostRender(g_pContext);
	//F_Axis_z.PreRender(g_pContext, 2);	F_Axis_z.PostRender(g_pContext);
	//Camera_Frustum_Box.Render(g_pContext, 2);
	//
	//// 3번 뷰포트


	///*FSphere.HandOver_world_view_projection_Matrixs_to_ConstantData_Final_extract(&FSphere.m_matWorld, &Top_Camera.m_matView, &Top_Camera.m_matProj);		FSphere.update_ConstantBuffer();
	//FBox.HandOver_world_view_projection_Matrixs_to_ConstantData_Final_extract(&FBox.m_matWorld, &Top_Camera.m_matView, &Top_Camera.m_matProj);			FBox.update_ConstantBuffer();*/

	//Camera_Frustum_Box.HandOver_world_view_projection_Matrixs_to_ConstantData_Final_extract(&Camera_Frustum_Box.m_matWorld, &Top_Camera.m_matView, &Top_Camera.m_matProj);		Camera_Frustum_Box.update_ConstantBuffer();
	//F_Axis_s.HandOver_world_view_projection_Matrixs_to_ConstantData_Final_extract(&F_Axis_s.m_matWorld, &Top_Camera.m_matView, &Top_Camera.m_matProj);	F_Axis_s.update_ConstantBuffer();
	//F_Axis_y.HandOver_world_view_projection_Matrixs_to_ConstantData_Final_extract(&F_Axis_y.m_matWorld, &Top_Camera.m_matView, &Top_Camera.m_matProj);	F_Axis_y.update_ConstantBuffer();
	//F_Axis_z.HandOver_world_view_projection_Matrixs_to_ConstantData_Final_extract(&F_Axis_z.m_matWorld, &Top_Camera.m_matView, &Top_Camera.m_matProj);	F_Axis_z.update_ConstantBuffer();


	///*FSphere.Render(g_pContext, 3);
	//FBox.Render(g_pContext, 3);*/
	
	//F_Axis_s.PreRender(g_pContext, 3);	F_Axis_s.PostRender(g_pContext);
	//F_Axis_y.PreRender(g_pContext, 3);	F_Axis_y.PostRender(g_pContext);
	//F_Axis_z.PreRender(g_pContext, 3);	F_Axis_z.PostRender(g_pContext);



	//// 4번 뷰포트


	///*FSphere.HandOver_world_view_projection_Matrixs_to_ConstantData_Final_extract(&FSphere.m_matWorld, &Beneath_Camera.m_matView, &Beneath_Camera.m_matProj);		FSphere.update_ConstantBuffer();
	//FBox.HandOver_world_view_projection_Matrixs_to_ConstantData_Final_extract(&FBox.m_matWorld, &Beneath_Camera.m_matView, &Beneath_Camera.m_matProj);			FBox.update_ConstantBuffer();*/
	//
	//Camera_Frustum_Box.HandOver_world_view_projection_Matrixs_to_ConstantData_Final_extract(&Camera_Frustum_Box.m_matWorld, &Beneath_Camera.m_matView, &Beneath_Camera.m_matProj);		Camera_Frustum_Box.update_ConstantBuffer();
	//F_Axis_s.HandOver_world_view_projection_Matrixs_to_ConstantData_Final_extract(&F_Axis_s.m_matWorld, &Beneath_Camera.m_matView, &Beneath_Camera.m_matProj);	F_Axis_s.update_ConstantBuffer();
	//F_Axis_y.HandOver_world_view_projection_Matrixs_to_ConstantData_Final_extract(&F_Axis_y.m_matWorld, &Beneath_Camera.m_matView, &Beneath_Camera.m_matProj);	F_Axis_y.update_ConstantBuffer();
	//F_Axis_z.HandOver_world_view_projection_Matrixs_to_ConstantData_Final_extract(&F_Axis_z.m_matWorld, &Beneath_Camera.m_matView, &Beneath_Camera.m_matProj);	F_Axis_z.update_ConstantBuffer();


	///*FSphere.Render(g_pContext, 4);
	//FBox.Render(g_pContext, 4);*/
	//Camera_Frustum_Box.Render(g_pContext, 4);
	//F_Axis_s.PreRender(g_pContext, 4);	F_Axis_s.PostRender(g_pContext);
	//F_Axis_y.PreRender(g_pContext, 4);	F_Axis_y.PostRender(g_pContext);
	//F_Axis_z.PreRender(g_pContext, 4);	F_Axis_z.PostRender(g_pContext);



	return true;
};

bool	TSceneLobby::Release()
{
	
	
	return true;
};
TSceneLobby::TSceneLobby()
{
	m_iSceneID = GAME_SCENE_LOBBY;
	m_bNextSceneStart = false;
}
TSceneLobby::~TSceneLobby()
{

}

















class TSceneGame : public TScene
{
public:
	virtual bool    Init();
	bool    Frame();
	bool    Render();
	bool    Release();

public:

	TSceneGame();
	virtual ~TSceneGame();
};



TSceneGame::TSceneGame()
{
	m_iSceneID = GAME_SCENE_PLAY;
	m_bNextSceneStart = false;
}

bool   TSceneGame::Init()
{

	return true;
};

bool    TSceneGame::Frame()
{

	return true;
}

bool   TSceneGame::Render()
{

	return true;
}

bool    TSceneGame::Release()
{
	return true;
}

TSceneGame::~TSceneGame()
{
}



class TSceneEnd : public TScene
{

public:

public:
	virtual bool	Init();
	bool	Frame();
	bool	Render();
	bool	Release();

public:

	TSceneEnd();
	virtual ~TSceneEnd();
};




bool	TSceneEnd::Init()
{



	return true;
};
bool	TSceneEnd::Frame()
{


	return true;
};

bool	TSceneEnd::Render()
{

	return true;
};

bool	TSceneEnd::Release()
{
	return true;
};

TSceneEnd::TSceneEnd()
{
	m_iSceneID = GAME_SCENE_END;
	m_bNextSceneStart = false;
}

TSceneEnd::~TSceneEnd()
{}

