#pragma once
#include "xStd.h"
#define ID_TBASIS_AM 0   // ambient
#define ID_TBASIS_DI 1   // diffuse
#define ID_TBASIS_SP 2   // specular
#define ID_TBASIS_SH 3   // shininesNs
#define ID_TBASIS_SS 4   // shininess strength
#define ID_TBASIS_SI 5   // self-illumination
#define ID_TBASIS_OP 6   // opacity
#define ID_TBASIS_FI 7   // filter color
#define ID_TBASIS_BU 8   // bump 
#define ID_TBASIS_RL 9   // reflection
#define ID_TBASIS_RR 10  // refraction 
#define ID_TBASIS_DP 11  // displacement
#define ID_TBASIS_GN 12  // Generic
#define ID_TBASIS_EV 13  // EnvMap

struct Scene
{
	int First_Frame;// 시작 프레임
	int Last_Frame; // 마지막 프레임
	int Frames_per_second;// 1초당 프레임 개수(30)
	int Tick_Per_Frame;// 1프레임의 틱 값(160)
};

struct TextureMap // STL에서 MAP을 생각하면, NUMBER - 이름 딱딱
{
	int Type_number;
	T_STR name;//3. TCHAR 코드 : 상황에 맞게 유니코드, 멀티바이트 알아서 변환해서 컴파일러가 돌아간다. 
};

struct Material
{
	vector<TextureMap> TextureMap_List;
	vector<Material>Sub_Material_List;
};

struct Index
{
	int ASE_Index[3];
	int Sub_Material_number;
};

struct TAnimTrack
{
	int     iTick;
	int		Track_type_S1_R2_T3;
	D3DXQUATERNION  qRotate;
	D3DXVECTOR3   Vector_in_Animation;

public:

	TAnimTrack()
	{
		iTick = 0;
		Track_type_S1_R2_T3 = 0;
	}
};



struct GeometryMesh  // 이렇게 많은 게 들어간단 말이야?
{
	int Material_Reference;
	T_STR node_name;
	T_STR node_parent_name;
	GeometryMesh* Parent_GeoMesh_ptr;
	
	vector<D3DXVECTOR3>  vertexList;
	vector<Index> Position_Face_List;
	vector<D3DXVECTOR3> Texture_List;
	vector<Index> Texture_Face_List;
	vector<D3DXVECTOR3> Color_List;
	vector<Index> Color_Face_List;
	vector<D3DXVECTOR3> normal_List;

public: //애니메이션을 위해. 일단은 이동만.

	D3DXMATRIX    m_matWorld; // 월드 행렬
	D3DXMATRIX    m_matWorldRotate; // 월드 회전 행렬
	D3DXMATRIX    m_matWorldScale; // 월드 신축 행렬
	D3DXMATRIX    m_matWorldTrans; // 월드 이동 행렬

	vector<TAnimTrack>  m_pSclTrack; // 신축 트랙
	vector<TAnimTrack>  m_pRotTrack; // 회전 트랙
	vector<TAnimTrack>  m_PosTrack;  // 이동 트랙 
	vector<TAnimTrack>	m_pVisTrack; // 이건 뭔가??

	D3DXMATRIX			m_matCalculation; // 최종 계산 행렬
	vector <D3DXMATRIX>	m_C_Matrix_List; //프레임마다 계산 행렬을 미리 저장할 것인가...? 어쨋든 저장해놓고 쓰면 되지 않나.

	D3DXMATRIX			m_matCalculation_acculated; // 최종 계산 행렬


public: // 이게 뭔지 모르지만

	float			    m_fVisibility;

public:


	GeometryMesh()
	{
		int Material_Reference = 0;
		m_fVisibility = 0;
		Parent_GeoMesh_ptr = NULL;

		D3DXMatrixIdentity(&m_matWorld);
		D3DXMatrixIdentity(&m_matWorldTrans);
		D3DXMatrixIdentity(&m_matWorldRotate);
		D3DXMatrixIdentity(&m_matWorldScale);
		D3DXMatrixIdentity(&m_matCalculation);
		D3DXMatrixIdentity(&m_matCalculation_acculated);
	}


};

class AseMesh
{
public:
	
	vector <Material> Material_List;
	vector <GeometryMesh> GeometryMesh_per_Object_List;
	

	TCHAR Main_Buffer[256];
	TCHAR one_string_Buffer[256];
	TCHAR one_string_Buffer_2[256];
	int Integer_Buffer;
	FILE* Stream;


public: // 씬 저장을 위해

	Scene		  m_Scene;


public: //  Load_Geometry_Object() 함수 안에 멤버 변수로 있어요. 이걸, 그냥 전역으로 뺄게요 ㅋㅋㅋ 

	int iNumFaces = 0; // 이걸 적극 사용할 것임
	int iNumVertex = 0;

public:
	bool Load(T_STR name) // 전체 진행
	{
		Stream = NULL;
		Stream = _tfopen(name.c_str(), _T("rt"));
		if (Stream == NULL) return false;

		if (Find_one_specific_string(_T("*SCENE")) == false) return false;
		Load_Scene();

		if (Find_one_specific_string(_T("*MATERIAL_LIST")) == false) return false;
		Load_Material();

		if (Find_one_specific_string(_T("*GEOMOBJECT")) == false) return false;
		Load_Geometry_Object();






		fclose(Stream);
		Help_meet_Parent_Child();
		return true;
	}

	bool Help_meet_Parent_Child()
	{
		if (this->GeometryMesh_per_Object_List.size())
		{
			for (int i = 0; i < this->GeometryMesh_per_Object_List.size(); i++)
			{
				for (int j = 0; j < this->GeometryMesh_per_Object_List.size(); j++)
				{
					if (this->GeometryMesh_per_Object_List[i].node_name
						== this->GeometryMesh_per_Object_List[j].node_parent_name)
					{
						this->GeometryMesh_per_Object_List[j].Parent_GeoMesh_ptr = &this->GeometryMesh_per_Object_List[i];
					
						break;
					}
				}
			}
		}
		return true;
	}


	bool Find_one_specific_string(T_STR string_aimed)
	{
		while (!feof(Stream))
		{
			_fgetts(Main_Buffer, 256, Stream);
			_stscanf(Main_Buffer, _T("%s"), one_string_Buffer);

			if (!_tcsicmp(one_string_Buffer, string_aimed.c_str()))  //같으면 0반환.
			{
				return true;
			}
		}
		return false; //다르면,
	}

	bool Find_new_Object_starting_point()
	{
		while (!feof(Stream))
		{
			_fgetts(Main_Buffer, 256, Stream); //*CONTROL_POS_SAMPLE 0 0.0000 0.0000 0.0000
			if (!_tcsicmp(Main_Buffer, _T("}\n")))  //같으면 0반환.
			{
				break;
			}
		}
		return false; //다르면,
	}





	
	
	void Load_Scene()
	{
		_fgetts(Main_Buffer, 256, Stream); //*SCENE_FILENAME "Box.max"를 읽는다

		_fgetts(Main_Buffer, 256, Stream); //*SCENE_FIRSTFRAME 0을 읽는다.
		_stscanf(Main_Buffer, _T("%s%d"), one_string_Buffer, &m_Scene.First_Frame);

		_fgetts(Main_Buffer, 256, Stream); //	*SCENE_LASTFRAME 100
		_stscanf(Main_Buffer, _T("%s%d"), one_string_Buffer, &m_Scene.Last_Frame);

		_fgetts(Main_Buffer, 256, Stream); //	*SCENE_FRAMESPEED 30
		_stscanf(Main_Buffer, _T("%s%d"), one_string_Buffer, &m_Scene.Frames_per_second);

		_fgetts(Main_Buffer, 256, Stream); //	*SCENE_TICKSPERFRAME 30
		_stscanf(Main_Buffer, _T("%s%d"), one_string_Buffer, &m_Scene.Tick_Per_Frame);
	}

	void Load_Material() 
	{
		int i_MATERIAL_COUNT = 0;

		_fgetts(Main_Buffer, 256, Stream);//*MATERIAL_COUNT 2 읽어야 합니다.
		_stscanf(Main_Buffer, _T("%s%d"), one_string_Buffer, &i_MATERIAL_COUNT);//*MATERIAL_COUNT 2 읽어야 합니다.


		for (int iCnt = 0; iCnt < i_MATERIAL_COUNT; iCnt++) // 여기서는 총 2번 돕니다
		{
			Material yang_Material;// 여기서 저장되지만, 다 전역 벡터에 저장되어 사용됨.
			TextureMap yang_TextureMap; // 여기서 정의. 다 벡터에 저장됨.
		
			if (Find_one_specific_string(_T("*MATERIAL_CLASS")) == false) return; // 이미 이걸로 사실 메인버퍼, 원스트링에 저장되어 있다.

			TCHAR szClass[256] = { 0, };

			_stscanf(Main_Buffer, _T("%s%s"), one_string_Buffer, szClass); //*MATERIAL_CLASS "Standard"
			
			int iSubMtrl = 0;

			if (!_tcsicmp(szClass, _T("\"Multi/Sub-Object\""))) // 같으면 밑으로 들어갑니다. 처음엔 같지 않습니다.//같으면 0 반환, 다르면 0이 아닌 정수값 반환.	
			{
				if (Find_one_specific_string(_T("*NUMSUBMTLS")) == false) return; // 즉 이게 나오면,
				_stscanf(Main_Buffer, _T("%s%d"), one_string_Buffer,&iSubMtrl); //iSubMtrl에 5가 저장된다.
			}

			if (iSubMtrl == 0) // 처음 이것부터 합니다.
			{
				if (Find_one_specific_string(_T("*MAP_SUBNO")) == false) return; // *MATERIAL 0 { 이 박스에도 *MAP_SUBNO 1 이게 있어요.

				_stscanf(Main_Buffer, _T("%s%d"), one_string_Buffer, &yang_TextureMap.Type_number); // 좀더 상위 맵이랄까? 그런느낌인데.
				_fgetts(Main_Buffer, 256, Stream);//*MAP_AMOUNT 1.0000
				_fgetts(Main_Buffer, 256, Stream);//*BITMAP "D:\00_TBasisTutorials\Lecture_010\data\cncr25S.bmp"
				_stscanf(Main_Buffer, _T("%s%s"), one_string_Buffer, one_string_Buffer_2);

				TCHAR drive[256] = { 0, };
				TCHAR dir[256] = { 0, };
				TCHAR fname[256] = { 0, };
				TCHAR ext[256] = { 0, };

				_tsplitpath(one_string_Buffer_2, drive, dir, fname, ext);

				yang_TextureMap.name = fname; // 이게 좀 더 상위 텍스쳐 맵이에요. 일부러 구분했어요.
				ext[4] = 0;
				yang_TextureMap.name += ext;
				yang_Material.TextureMap_List.push_back(yang_TextureMap);
			}

			else // 5가 저장되어 있으니까,
			{
				for (int iSub = 0; iSub < iSubMtrl; iSub++) //5까지 순환하다.   // *SUBMATERIAL 0 { 의 이미 전줄까지 왔어요.                                
				{
					Material    Sub_Material;
					TextureMap  Sub_Map;

					if (Find_one_specific_string(_T("*MAP_SUBNO")) == false) return;

					//*SUBMATERIAL 0 { 안에	*MAP_SUBNO 1 있어요.
					//*SUBMATERIAL 1 { 안에 *MAP_SUBNO 1 있어요.

					_stscanf(Main_Buffer, _T("%s%d"), one_string_Buffer, &Sub_Map.Type_number);
					_fgetts(Main_Buffer, 256, Stream);//*MAP_AMOUNT 1.0000
					_fgetts(Main_Buffer, 256, Stream);//*BITMAP "D:\test\STAGE_02\st02_sc00\st02_sc00_map\0_st02_sc00_g00.dds"
					_stscanf(Main_Buffer, _T("%s%s"), one_string_Buffer, one_string_Buffer_2);

					TCHAR drive[256] = { 0, };
					TCHAR dir[256] = { 0, };
					TCHAR fname[256] = { 0, };
					TCHAR ext[256] = { 0, };
					_tsplitpath(one_string_Buffer_2, drive, dir, fname, ext);

					Sub_Map.name = fname;
					ext[4] = 0;
					Sub_Map.name += ext;
					Sub_Material.TextureMap_List.push_back(Sub_Map);
					yang_Material.Sub_Material_List.push_back(Sub_Material);
				}

			}
			Material_List.push_back(yang_Material);
		}
	}



	void Load_Geometry_Object()
	{

	}

	void Load_Helper_Object()
	{

	}




	void Load_Object()
	{
		while (!feof(Stream)) 
		{
			//int iNumFaces = 0; // 이걸 적극 사용할 것임
			//int iNumVertex = 0; // 전역으로 뺏는데 그게 옳은지 모르겠네요.

			GeometryMesh geomrtyMesh_Object; // 여기에 정의. 분명 어딘가 벡터 리스트에 저장하겠지? 응응!
			_fgetts(Main_Buffer, 256, Stream); //*NODE_NAME "st02_sc00_g" // 그 다음에 *NODE_NAME "Box01"
			_stscanf(Main_Buffer, _T("%s%s"), one_string_Buffer, one_string_Buffer_2);//"st02_sc00_g" // "Box01"
			geomrtyMesh_Object.node_name = one_string_Buffer_2; //"st02_sc00_g"

			_fgetts(Main_Buffer, 256, Stream);//*NODE_PARENT "Box001"
			_stscanf(Main_Buffer, _T("%s%s"), one_string_Buffer, one_string_Buffer_2);//*NODE_PARENT "Box001"
			if (!_tcsicmp(one_string_Buffer, _T("*NODE_PARENT")))//같으면 0반환.
			{
				geomrtyMesh_Object.node_parent_name = one_string_Buffer_2; //*NODE_PARENT "Box001"
			}

			D3DXMatrixIdentity(&geomrtyMesh_Object.m_matWorld); // 이 지오메트리 오브젝트 월드 행렬을 단위행렬化한다

			if (Find_one_specific_string(_T("*TM_ROW0")) == false) return; // 월드 행렬 시작을 찾는다. 이미 메인버퍼, ONE_STRING 버퍼에 저장되어 있다.
			Get_Data(&geomrtyMesh_Object.m_matWorld.m[0]); //*TM_ROW0 1.0000	0.0000	0.0000 쪼개 저장.

			_fgetts(Main_Buffer, 256, Stream); //*TM_ROW1 0.0000	1.0000	0.0000
			Get_Data(&geomrtyMesh_Object.m_matWorld.m[2]);

			_fgetts(Main_Buffer, 256, Stream);//	*TM_ROW2 0.0000	0.0000	1.0000
			Get_Data(&geomrtyMesh_Object.m_matWorld.m[1]);

			_fgetts(Main_Buffer, 256, Stream); //	*TM_ROW3 -0.0773 -0.1949 0.0000
			Get_Data(&geomrtyMesh_Object.m_matWorld.m[3]);

			// 세가지를 구하는 과정 : 단 의문은 임의의 축 회전을 쿼터니온 함수로 너무 쉽게 처리한 거 아닌가? 왜 역행렬 안해?
			//D3DXMATRIX    geomrtyMesh_Object.m_matWorldRotate; // 월드 회전 행렬
			//D3DXMATRIX    geomrtyMesh_Object.m_matWorldScale; // 월드 신축 행렬
			//D3DXMATRIX    geomrtyMesh_Object.m_matWorldTrans; // 월드 이동 행렬

			D3DXVECTOR3 David_Vector;
			float  David_Angle;
			D3DXQUATERNION qRotation;
			_fgetts(Main_Buffer, 256, Stream); //  *TM_POS 0.0000 0.0000 0.0000
			Get_Data(&David_Vector); //*TM_POS 0.0000 0.0000 0.0000쪼개 저장.
			D3DXMatrixTranslation(&geomrtyMesh_Object.m_matWorldTrans, David_Vector.x, David_Vector.y, David_Vector.z);
			_fgetts(Main_Buffer, 256, Stream); //   *TM_ROTAXIS 0.0000 0.0000 0.0000
			Get_Data(&David_Vector); // *TM_ROTAXIS 0.0000 0.0000 0.0000
			_fgetts(Main_Buffer, 256, Stream); //*TM_ROTANGLE 0.0000
			Get_Float_Data(&David_Angle); //*TM_ROTANGLE 0.0000
			// 임의의 축과 각을 쿼터니언으로 변환
			D3DXQuaternionRotationAxis(&qRotation, &David_Vector, David_Angle);
			D3DXMatrixRotationQuaternion(&geomrtyMesh_Object.m_matWorldRotate, &qRotation);
			//스케일축을 중심으로 회전하는 값이 스케일 값이므로 
			//스케일축의 회전만큼을 먼저 반대로 회전한 이후
			//스케일값을 적용시키고 그 다음에 
			//다시 스케일축만큼을 회전시켜 원상복귀 시킨다. = 임의의 축 스케일링.
			D3DXMATRIX matScl;
			_fgetts(Main_Buffer, 256, Stream); //  *TM_SCALE 1.0000 1.0000 1.0000
			Get_Data(&David_Vector); // *TM_SCALE 1.0000 1.0000 1.0000
			D3DXMatrixScaling(&geomrtyMesh_Object.m_matWorldScale, David_Vector.x, David_Vector.y, David_Vector.z);
			D3DXVECTOR3 David_Axis;
			_fgetts(Main_Buffer, 256, Stream); // *TM_SCALEAXIS 0.0000 0.0000 0.0000
			Get_Data(&David_Axis);// *TM_SCALEAXIS 0.0000 0.0000 0.0000
			_fgetts(Main_Buffer, 256, Stream); // *TM_SCALEAXISANG 0.0000
			Get_Float_Data(&David_Angle); // *TM_SCALEAXISANG 0.0000
			// 스케일축의 행렬과 그 역행렬을 구한다.
			D3DXMATRIX matRotation, matRotationInv;
			D3DXMatrixRotationAxis(&matRotation, &David_Axis, David_Angle);
			D3DXMatrixInverse(&matRotationInv, NULL, &matRotation);
			//D3DXMatrixMultiply( &pObject->m_matWorldScale, &matRotationInv, &pObject->m_matWorldScale);
			//D3DXMatrixMultiply( &pObject->m_matWorldScale, &pObject->m_matWorldScale, &matRotation );
			geomrtyMesh_Object.m_matWorldScale = matRotationInv * geomrtyMesh_Object.m_matWorldScale  * matRotation;




			if (Find_one_specific_string(_T("*MESH")) == false) return;
			
			{
				_fgetts(Main_Buffer, 256, Stream);//*TIMEVALUE 0
				_fgetts(Main_Buffer, 256, Stream); //*MESH_NUMVERTEX 2656
				_stscanf(Main_Buffer, _T("%s%d"), one_string_Buffer, &iNumVertex);//위치 기준 정점수 = 2656


				if (iNumVertex > 0)//위치 기준 정점수 = 2656 이니
				{
					_fgetts(Main_Buffer, 256, Stream); //*MESH_NUMFACES 4158 // 정확한 페이스수 = 4158
					_stscanf(Main_Buffer, _T("%s%d"), one_string_Buffer, &iNumFaces);  // 정확한 페이스수 = 4158
		
					_fgetts(Main_Buffer, 256, Stream);//*MESH_VERTEX_LIST {

					geomrtyMesh_Object.vertexList.resize(iNumVertex);//위치 기준 정점수 = 2656

					// 중요함. Vertex마다 돌아감.

					for (int iVertex = 0; iVertex < iNumVertex; iVertex++) // 버텍스 0부터 쭉 돌아감.
					{
						LoadVertexData(geomrtyMesh_Object.vertexList[iVertex], true);
					} // 	*MESH_VERTEX   0 -5.0773	-5.1949	0.0000 부터 *MESH_VERTEX 7 4.9227	4.8051	10.0000 다 돎.
	//기준 vertexList
					_fgetts(Main_Buffer, 256, Stream); // }
					_fgetts(Main_Buffer, 256, Stream); // 	*MESH_FACE_LIST {

					geomrtyMesh_Object.Position_Face_List.resize(iNumFaces); //앞에서 페이스수 = 4158  했음.

	//기준 Position_Face_List. 여기서 다 나눠짐

					for (int iFace = 0; iFace < iNumFaces; iFace++)
					{
						_fgetts(Main_Buffer, 256, Stream);		// *MESH_FACE    0:    A:    0 B: 2 C: 3 AB : 1 BC : 1 CA : 0 * MESH_SMOOTHING 2 * MESH_MTLID 1
						_stscanf(Main_Buffer, _T("%s%s %s%d %s%d %s%d %s%d %s%d %s %d %s%d %s%d"),

							one_string_Buffer, one_string_Buffer, one_string_Buffer,
							&geomrtyMesh_Object.Position_Face_List[iFace].ASE_Index[0], // 첫번째 버텍스
							one_string_Buffer,//B:
							&geomrtyMesh_Object.Position_Face_List[iFace].ASE_Index[2], // 시계 반시계 때문에 뒤짚었습니다
							one_string_Buffer,//C:
							&geomrtyMesh_Object.Position_Face_List[iFace].ASE_Index[1],
							one_string_Buffer, &Integer_Buffer,
							one_string_Buffer, &Integer_Buffer,
							one_string_Buffer, &Integer_Buffer,
							one_string_Buffer, &Integer_Buffer,
							one_string_Buffer, &geomrtyMesh_Object.Position_Face_List[iFace].Sub_Material_number);// 이건 뭔가 의미있는 정보야. 핵심적이지. 
					}
				}

				//일단 다 저장해. 그리고 Sub_Material_number로, 나중에 서브오브젝트들로 나눌건가봐.


				_fgetts(Main_Buffer, 256, Stream); // }
				_fgetts(Main_Buffer, 256, Stream);//	*MESH_NUMTVERTEX 3803
				_stscanf(Main_Buffer, _T("%s%d"), one_string_Buffer, &iNumVertex); //아까 8이였잖아. 지금은 12라고?

				if (iNumVertex > 0)
				{
					_fgetts(Main_Buffer, 256, Stream); // *MESH_NUMTVERTEX 3803 *MESH_TVERTLIST { // 여긴 이게 처음!
					geomrtyMesh_Object.Texture_List.resize(iNumVertex); // 3803. // vector<D3DXVECTOR3> Texture_List;

		//Texture_List 여긴 이게 처음!
					for (int iVertex = 0; iVertex < iNumVertex; iVertex++) //iNumVertex=3803 까지 간다.
					{
						LoadVertexData(geomrtyMesh_Object.Texture_List[iVertex], false); //_fgetts(Main_Buffer, 256, Stream); _stscanf(Main_Buffer, _T("%s%d%f%f%f"),	one_string_Buffer,	&Integer_Buffer,	&D_Vec.x,	&D_Vec.y,	&D_Vec.z);
						geomrtyMesh_Object.Texture_List[iVertex].y = 1.0f - geomrtyMesh_Object.Texture_List[iVertex].y; // 왜 1.0에서 뻇지? MAX는 아래왼쪽에서 텍스쳐가 시작하므로.
					}
	   //*MESH_NUMTVFACES 4158
					_fgetts(Main_Buffer, 256, Stream);//	}
					_fgetts(Main_Buffer, 256, Stream);//	*MESH_NUMTVFACES 4158
					_stscanf(Main_Buffer, _T("%s%d"), one_string_Buffer, &iNumFaces);//페이스수 = 4158 앞에서 했음.
					_fgetts(Main_Buffer, 256, Stream);//	*MESH_TFACELIST {

		//vector<Index> Texture_Face_List;는 처음임.
					geomrtyMesh_Object.Texture_Face_List.resize(iNumFaces);////페이스수 = 4158 앞에서 했음.

					for (int iFace = 0; iFace < iNumFaces; iFace++)//페이스수 = 4158 앞에서 했음. 페이스 0부터 돌면서.
					{ //	vector<Index> Texture_Face_List;
						LoadIndexData(geomrtyMesh_Object.Texture_Face_List[iFace]);//&D_Index.ASE_Index[0],	&D_Index.ASE_Index[2],	&D_Index.ASE_Index[1]) 에 저장.
					}
				}

				//이제부터는 칼라

	// *MESH_NUMCVERTEX 2656
				_fgetts(Main_Buffer, 256, Stream);//	}
				_fgetts(Main_Buffer, 256, Stream);//*MESH_NUMCVERTEX 2656

				// 칼라가 있다면, 이렇게 진행될 듯.

				_stscanf(Main_Buffer, _T("%s%d"), one_string_Buffer, &iNumVertex); //2656이 들어감. // 칼라가 있다면, 이렇게 진행될 듯.
				geomrtyMesh_Object.Color_List.resize(iNumVertex);//2656

				if (iNumVertex > 0) // 현재는 0 임.
				{

					_fgetts(Main_Buffer, 256, Stream);
					for (int iVertex = 0; iVertex < iNumVertex; iVertex++)
					{
						LoadVertexData(geomrtyMesh_Object.Color_List[iVertex], false); //vector<D3DXVECTOR3> Color_List;// RGB니까 순서 그대로 저장
					}
	//*MESH_NUMCVFACES 4158			
					_fgetts(Main_Buffer, 256, Stream);//}
					_fgetts(Main_Buffer, 256, Stream);//*MESH_NUMCVFACES 4158
					_stscanf(Main_Buffer, _T("%s%d"), one_string_Buffer, &iNumFaces);//iNumFaces=4158
	 //*MESH_CFACELIST 0

					//일부러 잘못쓴건가? 여기 한줄 빈다.

					geomrtyMesh_Object.Color_Face_List.resize(iNumFaces);

					for (int iFace = 0; iFace < iNumFaces; iFace++)
					{
						/**MESH_CFACE 0	0	1	2
						* MESH_CFACE 1	0	3	1
						* MESH_CFACE 2	0	4	3*/

						LoadIndexData(geomrtyMesh_Object.Color_Face_List[iFace]);
					}

				}

				//이제부터는 노멀
				//*MESH_NORMALS{

				geomrtyMesh_Object.normal_List.resize(iNumFaces * 3); // 정점노멀들만 넣을 것이기에 *3

				_fgetts(Main_Buffer, 256, Stream);//*MESH_NORMALS {

				for (int iFace = 0; iFace < iNumFaces; iFace++) // 12개.
				{
					_fgetts(Main_Buffer, 256, Stream); // 페이스 노멀 날린 거에요.
					LoadVertexData(geomrtyMesh_Object.normal_List[iFace * 3 + 0]); //페이스당 정점노멀들 3개.
					LoadVertexData(geomrtyMesh_Object.normal_List[iFace * 3 + 2]); // 왜 바꿨냐면, 시계 반시계
					LoadVertexData(geomrtyMesh_Object.normal_List[iFace * 3 + 1]);
				}

				////*MATERIAL_REF 0

				//if (Find_one_specific_string(_T("*MATERIAL_REF")) == false) return; // 메인버퍼, 원스트링 저장 끝.

				//_stscanf(Main_Buffer, _T("%s%d"), one_string_Buffer, &geomrtyMesh_Object.Material_Reference); // 지오메트리 메쉬 오브젝트도 함수 내에서 정의됐다가, 전역 벡터에 저장됨!
				//_fgetts(Main_Buffer, 256, Stream);
				//_fgetts(Main_Buffer, 256, Stream);
				//GeometryMesh_per_Object_List.push_back(geomrtyMesh_Object);

			}
			// 애니메이션 추가



			if (Find_one_specific_string(_T("*TM_ANIMATION")) == true)
			{





				if (Find_one_specific_string(_T("*CONTROL_POS_TRACK")) == true)
				{
					for (;;)
					{
						TAnimTrack David_Track; David_Track.Track_type_S1_R2_T3 = 3;

						_fgetts(Main_Buffer, 256, Stream); //*CONTROL_POS_SAMPLE 0	0.0000	0.0000	0.0000
						
						if (!_tcsicmp(Main_Buffer, _T("\t\t}\n")))  //같으면 0반환.
						{
							break;
						}
						_stscanf(Main_Buffer, _T("%s%d%f%f%f"),one_string_Buffer, &David_Track.iTick, &David_Track.Vector_in_Animation.x, &David_Track.Vector_in_Animation.z, &David_Track.Vector_in_Animation.y);
						
						geomrtyMesh_Object.m_PosTrack.push_back(David_Track);	
					}
				}




				if (Find_one_specific_string(_T("*CONTROL_ROT_TRACK")) == true)
				{
					for (;;)
					{
						TAnimTrack David_Track; David_Track.Track_type_S1_R2_T3 = 2;

						_fgetts(Main_Buffer, 256, Stream); //   *CONTROL_ROT_SAMPLE 160 0.0000 0.0000 -1.0000 0.0051

						if (!_tcsicmp(Main_Buffer, _T("\t\t}\n")))  //같으면 0반환.
						{
							break;
						}
						_stscanf(Main_Buffer, _T("%s%d%f%f%f%f"), one_string_Buffer, &David_Track.iTick,
							&David_Track.qRotate.x, &David_Track.qRotate.z, &David_Track.qRotate.y, &David_Track.qRotate.w);
					
						geomrtyMesh_Object.m_pRotTrack.push_back(David_Track);

					}
				}



				if (Find_one_specific_string(_T("*CONTROL_SCALE_TRACK")) == true)
				{
					for (;;)
					{
						TAnimTrack David_Track; David_Track.Track_type_S1_R2_T3 = 1;

						_fgetts(Main_Buffer, 256, Stream); //*CONTROL_POS_SAMPLE 0	0.0000	0.0000	0.0000

						if (!_tcsicmp(Main_Buffer, _T("\t\t}\n")))  //같으면 0반환.
						{
							break;
						}
						_stscanf(Main_Buffer, _T("%s%d %f%f%f %f%f%f%f"), one_string_Buffer, &David_Track.iTick, &David_Track.Vector_in_Animation.x, &David_Track.Vector_in_Animation.z, &David_Track.Vector_in_Animation.y,
							&David_Track.qRotate.x, &David_Track.qRotate.z, &David_Track.qRotate.y, &David_Track.qRotate.w);

						geomrtyMesh_Object.m_pSclTrack.push_back(David_Track);

					}
				}



			}

			if (Find_one_specific_string(_T("*MATERIAL_REF")) == false) 
				return; // 메인버퍼, 원스트링 저장 끝.
			_stscanf(Main_Buffer, _T("%s%d"), one_string_Buffer, &geomrtyMesh_Object.Material_Reference); // 지오메트리 메쉬 오브젝트도 함수 내에서 정의됐다가, 전역 벡터에 저장됨!
			_fgetts(Main_Buffer, 256, Stream);
			_fgetts(Main_Buffer, 256, Stream);
			GeometryMesh_per_Object_List.push_back(geomrtyMesh_Object);
		}
	}













	void Get_Data(void* pData) // 메인 버퍼에 담겨 있는 내용을, 예를 들어 *TM_ROW0 1.0000	0.0000	0.0000 을 각각 쪼개 넣는다. 단, 내가 뭔가 배열을 넣어줘야, 그곳 성분들 각각에 넣는다
	{

		_stscanf(Main_Buffer, _T("%s%f%f%f"),
			one_string_Buffer,
			&((D3DXVECTOR3*)pData)->x,
			&((D3DXVECTOR3*)pData)->z,
			&((D3DXVECTOR3*)pData)->y);

	}



	void Get_Float_Data(void* pData) // 메인 버퍼에 담겨 있는 내용을, 예를 들어 *TM_ROW0 1.0000	0.0000	0.0000 을 각각 쪼개 넣는다. 단, 내가 뭔가 배열을 넣어줘야, 그곳 성분들 각각에 넣는다
	{

		_stscanf(Main_Buffer, _T("%s%f"),one_string_Buffer, pData);

	}


	void LoadVertexData(D3DXVECTOR3& D_Vec, bool bSwap=true)
	{
		_fgetts(Main_Buffer, 256, Stream); //*MESH_VERTEX    0	-5.0773	-5.1949	0.0000

		if (bSwap)
		{
			_stscanf(Main_Buffer, _T("%s%d%f%f%f"),	one_string_Buffer,	&Integer_Buffer,	&D_Vec.x,	&D_Vec.z,	&D_Vec.y); //Y축 Z축 바꿨다.
		}
		else
		{
			_stscanf(Main_Buffer, _T("%s%d%f%f%f"),	one_string_Buffer,	&Integer_Buffer,	&D_Vec.x,	&D_Vec.y,	&D_Vec.z); // 안바꾸고 그대로다. 칼라값이다.
		}


	}
	void LoadIndexData(Index& D_Index)
	{
		_fgetts(Main_Buffer, 256, Stream);
		_stscanf(Main_Buffer, _T("%s%d%d%d%d"),	one_string_Buffer,&Integer_Buffer,	&D_Index.ASE_Index[0],	&D_Index.ASE_Index[2],	&D_Index.ASE_Index[1]); // 이건 YZ축이 아니라, 시계반시계아니야?
	}

public:
	AseMesh() {};
	virtual ~AseMesh() {}
};




