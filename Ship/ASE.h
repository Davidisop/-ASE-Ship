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
	int First_Frame;// ���� ������
	int Last_Frame; // ������ ������
	int Frames_per_second;// 1�ʴ� ������ ����(30)
	int Tick_Per_Frame;// 1�������� ƽ ��(160)
};

struct TextureMap // STL���� MAP�� �����ϸ�, NUMBER - �̸� ����
{
	int Type_number;
	T_STR name;//3. TCHAR �ڵ� : ��Ȳ�� �°� �����ڵ�, ��Ƽ����Ʈ �˾Ƽ� ��ȯ�ؼ� �����Ϸ��� ���ư���. 
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



struct GeometryMesh  // �̷��� ���� �� ���� ���̾�?
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

public: //�ִϸ��̼��� ����. �ϴ��� �̵���.

	D3DXMATRIX    m_matWorld; // ���� ���
	D3DXMATRIX    m_matWorldRotate; // ���� ȸ�� ���
	D3DXMATRIX    m_matWorldScale; // ���� ���� ���
	D3DXMATRIX    m_matWorldTrans; // ���� �̵� ���

	vector<TAnimTrack>  m_pSclTrack; // ���� Ʈ��
	vector<TAnimTrack>  m_pRotTrack; // ȸ�� Ʈ��
	vector<TAnimTrack>  m_PosTrack;  // �̵� Ʈ�� 
	vector<TAnimTrack>	m_pVisTrack; // �̰� ����??

	D3DXMATRIX			m_matCalculation; // ���� ��� ���
	vector <D3DXMATRIX>	m_C_Matrix_List; //�����Ӹ��� ��� ����� �̸� ������ ���ΰ�...? ��¶�� �����س��� ���� ���� �ʳ�.

	D3DXMATRIX			m_matCalculation_acculated; // ���� ��� ���


public: // �̰� ���� ������

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


public: // �� ������ ����

	Scene		  m_Scene;


public: //  Load_Geometry_Object() �Լ� �ȿ� ��� ������ �־��. �̰�, �׳� �������� ���Կ� ������ 

	int iNumFaces = 0; // �̰� ���� ����� ����
	int iNumVertex = 0;

public:
	bool Load(T_STR name) // ��ü ����
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

			if (!_tcsicmp(one_string_Buffer, string_aimed.c_str()))  //������ 0��ȯ.
			{
				return true;
			}
		}
		return false; //�ٸ���,
	}

	bool Find_new_Object_starting_point()
	{
		while (!feof(Stream))
		{
			_fgetts(Main_Buffer, 256, Stream); //*CONTROL_POS_SAMPLE 0 0.0000 0.0000 0.0000
			if (!_tcsicmp(Main_Buffer, _T("}\n")))  //������ 0��ȯ.
			{
				break;
			}
		}
		return false; //�ٸ���,
	}





	
	
	void Load_Scene()
	{
		_fgetts(Main_Buffer, 256, Stream); //*SCENE_FILENAME "Box.max"�� �д´�

		_fgetts(Main_Buffer, 256, Stream); //*SCENE_FIRSTFRAME 0�� �д´�.
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

		_fgetts(Main_Buffer, 256, Stream);//*MATERIAL_COUNT 2 �о�� �մϴ�.
		_stscanf(Main_Buffer, _T("%s%d"), one_string_Buffer, &i_MATERIAL_COUNT);//*MATERIAL_COUNT 2 �о�� �մϴ�.


		for (int iCnt = 0; iCnt < i_MATERIAL_COUNT; iCnt++) // ���⼭�� �� 2�� ���ϴ�
		{
			Material yang_Material;// ���⼭ ���������, �� ���� ���Ϳ� ����Ǿ� ����.
			TextureMap yang_TextureMap; // ���⼭ ����. �� ���Ϳ� �����.
		
			if (Find_one_specific_string(_T("*MATERIAL_CLASS")) == false) return; // �̹� �̰ɷ� ��� ���ι���, ����Ʈ���� ����Ǿ� �ִ�.

			TCHAR szClass[256] = { 0, };

			_stscanf(Main_Buffer, _T("%s%s"), one_string_Buffer, szClass); //*MATERIAL_CLASS "Standard"
			
			int iSubMtrl = 0;

			if (!_tcsicmp(szClass, _T("\"Multi/Sub-Object\""))) // ������ ������ ���ϴ�. ó���� ���� �ʽ��ϴ�.//������ 0 ��ȯ, �ٸ��� 0�� �ƴ� ������ ��ȯ.	
			{
				if (Find_one_specific_string(_T("*NUMSUBMTLS")) == false) return; // �� �̰� ������,
				_stscanf(Main_Buffer, _T("%s%d"), one_string_Buffer,&iSubMtrl); //iSubMtrl�� 5�� ����ȴ�.
			}

			if (iSubMtrl == 0) // ó�� �̰ͺ��� �մϴ�.
			{
				if (Find_one_specific_string(_T("*MAP_SUBNO")) == false) return; // *MATERIAL 0 { �� �ڽ����� *MAP_SUBNO 1 �̰� �־��.

				_stscanf(Main_Buffer, _T("%s%d"), one_string_Buffer, &yang_TextureMap.Type_number); // ���� ���� ���̶���? �׷������ε�.
				_fgetts(Main_Buffer, 256, Stream);//*MAP_AMOUNT 1.0000
				_fgetts(Main_Buffer, 256, Stream);//*BITMAP "D:\00_TBasisTutorials\Lecture_010\data\cncr25S.bmp"
				_stscanf(Main_Buffer, _T("%s%s"), one_string_Buffer, one_string_Buffer_2);

				TCHAR drive[256] = { 0, };
				TCHAR dir[256] = { 0, };
				TCHAR fname[256] = { 0, };
				TCHAR ext[256] = { 0, };

				_tsplitpath(one_string_Buffer_2, drive, dir, fname, ext);

				yang_TextureMap.name = fname; // �̰� �� �� ���� �ؽ��� ���̿���. �Ϻη� �����߾��.
				ext[4] = 0;
				yang_TextureMap.name += ext;
				yang_Material.TextureMap_List.push_back(yang_TextureMap);
			}

			else // 5�� ����Ǿ� �����ϱ�,
			{
				for (int iSub = 0; iSub < iSubMtrl; iSub++) //5���� ��ȯ�ϴ�.   // *SUBMATERIAL 0 { �� �̹� ���ٱ��� �Ծ��.                                
				{
					Material    Sub_Material;
					TextureMap  Sub_Map;

					if (Find_one_specific_string(_T("*MAP_SUBNO")) == false) return;

					//*SUBMATERIAL 0 { �ȿ�	*MAP_SUBNO 1 �־��.
					//*SUBMATERIAL 1 { �ȿ� *MAP_SUBNO 1 �־��.

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
			//int iNumFaces = 0; // �̰� ���� ����� ����
			//int iNumVertex = 0; // �������� ���µ� �װ� ������ �𸣰ڳ׿�.

			GeometryMesh geomrtyMesh_Object; // ���⿡ ����. �и� ��� ���� ����Ʈ�� �����ϰ���? ����!
			_fgetts(Main_Buffer, 256, Stream); //*NODE_NAME "st02_sc00_g" // �� ������ *NODE_NAME "Box01"
			_stscanf(Main_Buffer, _T("%s%s"), one_string_Buffer, one_string_Buffer_2);//"st02_sc00_g" // "Box01"
			geomrtyMesh_Object.node_name = one_string_Buffer_2; //"st02_sc00_g"

			_fgetts(Main_Buffer, 256, Stream);//*NODE_PARENT "Box001"
			_stscanf(Main_Buffer, _T("%s%s"), one_string_Buffer, one_string_Buffer_2);//*NODE_PARENT "Box001"
			if (!_tcsicmp(one_string_Buffer, _T("*NODE_PARENT")))//������ 0��ȯ.
			{
				geomrtyMesh_Object.node_parent_name = one_string_Buffer_2; //*NODE_PARENT "Box001"
			}

			D3DXMatrixIdentity(&geomrtyMesh_Object.m_matWorld); // �� ������Ʈ�� ������Ʈ ���� ����� ����������Ѵ�

			if (Find_one_specific_string(_T("*TM_ROW0")) == false) return; // ���� ��� ������ ã�´�. �̹� ���ι���, ONE_STRING ���ۿ� ����Ǿ� �ִ�.
			Get_Data(&geomrtyMesh_Object.m_matWorld.m[0]); //*TM_ROW0 1.0000	0.0000	0.0000 �ɰ� ����.

			_fgetts(Main_Buffer, 256, Stream); //*TM_ROW1 0.0000	1.0000	0.0000
			Get_Data(&geomrtyMesh_Object.m_matWorld.m[2]);

			_fgetts(Main_Buffer, 256, Stream);//	*TM_ROW2 0.0000	0.0000	1.0000
			Get_Data(&geomrtyMesh_Object.m_matWorld.m[1]);

			_fgetts(Main_Buffer, 256, Stream); //	*TM_ROW3 -0.0773 -0.1949 0.0000
			Get_Data(&geomrtyMesh_Object.m_matWorld.m[3]);

			// �������� ���ϴ� ���� : �� �ǹ��� ������ �� ȸ���� ���ʹϿ� �Լ��� �ʹ� ���� ó���� �� �ƴѰ�? �� ����� ����?
			//D3DXMATRIX    geomrtyMesh_Object.m_matWorldRotate; // ���� ȸ�� ���
			//D3DXMATRIX    geomrtyMesh_Object.m_matWorldScale; // ���� ���� ���
			//D3DXMATRIX    geomrtyMesh_Object.m_matWorldTrans; // ���� �̵� ���

			D3DXVECTOR3 David_Vector;
			float  David_Angle;
			D3DXQUATERNION qRotation;
			_fgetts(Main_Buffer, 256, Stream); //  *TM_POS 0.0000 0.0000 0.0000
			Get_Data(&David_Vector); //*TM_POS 0.0000 0.0000 0.0000�ɰ� ����.
			D3DXMatrixTranslation(&geomrtyMesh_Object.m_matWorldTrans, David_Vector.x, David_Vector.y, David_Vector.z);
			_fgetts(Main_Buffer, 256, Stream); //   *TM_ROTAXIS 0.0000 0.0000 0.0000
			Get_Data(&David_Vector); // *TM_ROTAXIS 0.0000 0.0000 0.0000
			_fgetts(Main_Buffer, 256, Stream); //*TM_ROTANGLE 0.0000
			Get_Float_Data(&David_Angle); //*TM_ROTANGLE 0.0000
			// ������ ��� ���� ���ʹϾ����� ��ȯ
			D3DXQuaternionRotationAxis(&qRotation, &David_Vector, David_Angle);
			D3DXMatrixRotationQuaternion(&geomrtyMesh_Object.m_matWorldRotate, &qRotation);
			//���������� �߽����� ȸ���ϴ� ���� ������ ���̹Ƿ� 
			//���������� ȸ����ŭ�� ���� �ݴ�� ȸ���� ����
			//�����ϰ��� �����Ű�� �� ������ 
			//�ٽ� �������ุŭ�� ȸ������ ���󺹱� ��Ų��. = ������ �� �����ϸ�.
			D3DXMATRIX matScl;
			_fgetts(Main_Buffer, 256, Stream); //  *TM_SCALE 1.0000 1.0000 1.0000
			Get_Data(&David_Vector); // *TM_SCALE 1.0000 1.0000 1.0000
			D3DXMatrixScaling(&geomrtyMesh_Object.m_matWorldScale, David_Vector.x, David_Vector.y, David_Vector.z);
			D3DXVECTOR3 David_Axis;
			_fgetts(Main_Buffer, 256, Stream); // *TM_SCALEAXIS 0.0000 0.0000 0.0000
			Get_Data(&David_Axis);// *TM_SCALEAXIS 0.0000 0.0000 0.0000
			_fgetts(Main_Buffer, 256, Stream); // *TM_SCALEAXISANG 0.0000
			Get_Float_Data(&David_Angle); // *TM_SCALEAXISANG 0.0000
			// ���������� ��İ� �� ������� ���Ѵ�.
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
				_stscanf(Main_Buffer, _T("%s%d"), one_string_Buffer, &iNumVertex);//��ġ ���� ������ = 2656


				if (iNumVertex > 0)//��ġ ���� ������ = 2656 �̴�
				{
					_fgetts(Main_Buffer, 256, Stream); //*MESH_NUMFACES 4158 // ��Ȯ�� ���̽��� = 4158
					_stscanf(Main_Buffer, _T("%s%d"), one_string_Buffer, &iNumFaces);  // ��Ȯ�� ���̽��� = 4158
		
					_fgetts(Main_Buffer, 256, Stream);//*MESH_VERTEX_LIST {

					geomrtyMesh_Object.vertexList.resize(iNumVertex);//��ġ ���� ������ = 2656

					// �߿���. Vertex���� ���ư�.

					for (int iVertex = 0; iVertex < iNumVertex; iVertex++) // ���ؽ� 0���� �� ���ư�.
					{
						LoadVertexData(geomrtyMesh_Object.vertexList[iVertex], true);
					} // 	*MESH_VERTEX   0 -5.0773	-5.1949	0.0000 ���� *MESH_VERTEX 7 4.9227	4.8051	10.0000 �� ��.
	//���� vertexList
					_fgetts(Main_Buffer, 256, Stream); // }
					_fgetts(Main_Buffer, 256, Stream); // 	*MESH_FACE_LIST {

					geomrtyMesh_Object.Position_Face_List.resize(iNumFaces); //�տ��� ���̽��� = 4158  ����.

	//���� Position_Face_List. ���⼭ �� ������

					for (int iFace = 0; iFace < iNumFaces; iFace++)
					{
						_fgetts(Main_Buffer, 256, Stream);		// *MESH_FACE    0:    A:    0 B: 2 C: 3 AB : 1 BC : 1 CA : 0 * MESH_SMOOTHING 2 * MESH_MTLID 1
						_stscanf(Main_Buffer, _T("%s%s %s%d %s%d %s%d %s%d %s%d %s %d %s%d %s%d"),

							one_string_Buffer, one_string_Buffer, one_string_Buffer,
							&geomrtyMesh_Object.Position_Face_List[iFace].ASE_Index[0], // ù��° ���ؽ�
							one_string_Buffer,//B:
							&geomrtyMesh_Object.Position_Face_List[iFace].ASE_Index[2], // �ð� �ݽð� ������ ��¤�����ϴ�
							one_string_Buffer,//C:
							&geomrtyMesh_Object.Position_Face_List[iFace].ASE_Index[1],
							one_string_Buffer, &Integer_Buffer,
							one_string_Buffer, &Integer_Buffer,
							one_string_Buffer, &Integer_Buffer,
							one_string_Buffer, &Integer_Buffer,
							one_string_Buffer, &geomrtyMesh_Object.Position_Face_List[iFace].Sub_Material_number);// �̰� ���� �ǹ��ִ� ������. �ٽ�������. 
					}
				}

				//�ϴ� �� ������. �׸��� Sub_Material_number��, ���߿� ���������Ʈ��� �����ǰ���.


				_fgetts(Main_Buffer, 256, Stream); // }
				_fgetts(Main_Buffer, 256, Stream);//	*MESH_NUMTVERTEX 3803
				_stscanf(Main_Buffer, _T("%s%d"), one_string_Buffer, &iNumVertex); //�Ʊ� 8�̿��ݾ�. ������ 12���?

				if (iNumVertex > 0)
				{
					_fgetts(Main_Buffer, 256, Stream); // *MESH_NUMTVERTEX 3803 *MESH_TVERTLIST { // ���� �̰� ó��!
					geomrtyMesh_Object.Texture_List.resize(iNumVertex); // 3803. // vector<D3DXVECTOR3> Texture_List;

		//Texture_List ���� �̰� ó��!
					for (int iVertex = 0; iVertex < iNumVertex; iVertex++) //iNumVertex=3803 ���� ����.
					{
						LoadVertexData(geomrtyMesh_Object.Texture_List[iVertex], false); //_fgetts(Main_Buffer, 256, Stream); _stscanf(Main_Buffer, _T("%s%d%f%f%f"),	one_string_Buffer,	&Integer_Buffer,	&D_Vec.x,	&D_Vec.y,	&D_Vec.z);
						geomrtyMesh_Object.Texture_List[iVertex].y = 1.0f - geomrtyMesh_Object.Texture_List[iVertex].y; // �� 1.0���� �P��? MAX�� �Ʒ����ʿ��� �ؽ��İ� �����ϹǷ�.
					}
	   //*MESH_NUMTVFACES 4158
					_fgetts(Main_Buffer, 256, Stream);//	}
					_fgetts(Main_Buffer, 256, Stream);//	*MESH_NUMTVFACES 4158
					_stscanf(Main_Buffer, _T("%s%d"), one_string_Buffer, &iNumFaces);//���̽��� = 4158 �տ��� ����.
					_fgetts(Main_Buffer, 256, Stream);//	*MESH_TFACELIST {

		//vector<Index> Texture_Face_List;�� ó����.
					geomrtyMesh_Object.Texture_Face_List.resize(iNumFaces);////���̽��� = 4158 �տ��� ����.

					for (int iFace = 0; iFace < iNumFaces; iFace++)//���̽��� = 4158 �տ��� ����. ���̽� 0���� ���鼭.
					{ //	vector<Index> Texture_Face_List;
						LoadIndexData(geomrtyMesh_Object.Texture_Face_List[iFace]);//&D_Index.ASE_Index[0],	&D_Index.ASE_Index[2],	&D_Index.ASE_Index[1]) �� ����.
					}
				}

				//�������ʹ� Į��

	// *MESH_NUMCVERTEX 2656
				_fgetts(Main_Buffer, 256, Stream);//	}
				_fgetts(Main_Buffer, 256, Stream);//*MESH_NUMCVERTEX 2656

				// Į�� �ִٸ�, �̷��� ����� ��.

				_stscanf(Main_Buffer, _T("%s%d"), one_string_Buffer, &iNumVertex); //2656�� ��. // Į�� �ִٸ�, �̷��� ����� ��.
				geomrtyMesh_Object.Color_List.resize(iNumVertex);//2656

				if (iNumVertex > 0) // ����� 0 ��.
				{

					_fgetts(Main_Buffer, 256, Stream);
					for (int iVertex = 0; iVertex < iNumVertex; iVertex++)
					{
						LoadVertexData(geomrtyMesh_Object.Color_List[iVertex], false); //vector<D3DXVECTOR3> Color_List;// RGB�ϱ� ���� �״�� ����
					}
	//*MESH_NUMCVFACES 4158			
					_fgetts(Main_Buffer, 256, Stream);//}
					_fgetts(Main_Buffer, 256, Stream);//*MESH_NUMCVFACES 4158
					_stscanf(Main_Buffer, _T("%s%d"), one_string_Buffer, &iNumFaces);//iNumFaces=4158
	 //*MESH_CFACELIST 0

					//�Ϻη� �߸����ǰ�? ���� ���� ���.

					geomrtyMesh_Object.Color_Face_List.resize(iNumFaces);

					for (int iFace = 0; iFace < iNumFaces; iFace++)
					{
						/**MESH_CFACE 0	0	1	2
						* MESH_CFACE 1	0	3	1
						* MESH_CFACE 2	0	4	3*/

						LoadIndexData(geomrtyMesh_Object.Color_Face_List[iFace]);
					}

				}

				//�������ʹ� ���
				//*MESH_NORMALS{

				geomrtyMesh_Object.normal_List.resize(iNumFaces * 3); // ������ֵ鸸 ���� ���̱⿡ *3

				_fgetts(Main_Buffer, 256, Stream);//*MESH_NORMALS {

				for (int iFace = 0; iFace < iNumFaces; iFace++) // 12��.
				{
					_fgetts(Main_Buffer, 256, Stream); // ���̽� ��� ���� �ſ���.
					LoadVertexData(geomrtyMesh_Object.normal_List[iFace * 3 + 0]); //���̽��� ������ֵ� 3��.
					LoadVertexData(geomrtyMesh_Object.normal_List[iFace * 3 + 2]); // �� �ٲ�ĸ�, �ð� �ݽð�
					LoadVertexData(geomrtyMesh_Object.normal_List[iFace * 3 + 1]);
				}

				////*MATERIAL_REF 0

				//if (Find_one_specific_string(_T("*MATERIAL_REF")) == false) return; // ���ι���, ����Ʈ�� ���� ��.

				//_stscanf(Main_Buffer, _T("%s%d"), one_string_Buffer, &geomrtyMesh_Object.Material_Reference); // ������Ʈ�� �޽� ������Ʈ�� �Լ� ������ ���ǵƴٰ�, ���� ���Ϳ� �����!
				//_fgetts(Main_Buffer, 256, Stream);
				//_fgetts(Main_Buffer, 256, Stream);
				//GeometryMesh_per_Object_List.push_back(geomrtyMesh_Object);

			}
			// �ִϸ��̼� �߰�



			if (Find_one_specific_string(_T("*TM_ANIMATION")) == true)
			{





				if (Find_one_specific_string(_T("*CONTROL_POS_TRACK")) == true)
				{
					for (;;)
					{
						TAnimTrack David_Track; David_Track.Track_type_S1_R2_T3 = 3;

						_fgetts(Main_Buffer, 256, Stream); //*CONTROL_POS_SAMPLE 0	0.0000	0.0000	0.0000
						
						if (!_tcsicmp(Main_Buffer, _T("\t\t}\n")))  //������ 0��ȯ.
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

						if (!_tcsicmp(Main_Buffer, _T("\t\t}\n")))  //������ 0��ȯ.
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

						if (!_tcsicmp(Main_Buffer, _T("\t\t}\n")))  //������ 0��ȯ.
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
				return; // ���ι���, ����Ʈ�� ���� ��.
			_stscanf(Main_Buffer, _T("%s%d"), one_string_Buffer, &geomrtyMesh_Object.Material_Reference); // ������Ʈ�� �޽� ������Ʈ�� �Լ� ������ ���ǵƴٰ�, ���� ���Ϳ� �����!
			_fgetts(Main_Buffer, 256, Stream);
			_fgetts(Main_Buffer, 256, Stream);
			GeometryMesh_per_Object_List.push_back(geomrtyMesh_Object);
		}
	}













	void Get_Data(void* pData) // ���� ���ۿ� ��� �ִ� ������, ���� ��� *TM_ROW0 1.0000	0.0000	0.0000 �� ���� �ɰ� �ִ´�. ��, ���� ���� �迭�� �־����, �װ� ���е� ������ �ִ´�
	{

		_stscanf(Main_Buffer, _T("%s%f%f%f"),
			one_string_Buffer,
			&((D3DXVECTOR3*)pData)->x,
			&((D3DXVECTOR3*)pData)->z,
			&((D3DXVECTOR3*)pData)->y);

	}



	void Get_Float_Data(void* pData) // ���� ���ۿ� ��� �ִ� ������, ���� ��� *TM_ROW0 1.0000	0.0000	0.0000 �� ���� �ɰ� �ִ´�. ��, ���� ���� �迭�� �־����, �װ� ���е� ������ �ִ´�
	{

		_stscanf(Main_Buffer, _T("%s%f"),one_string_Buffer, pData);

	}


	void LoadVertexData(D3DXVECTOR3& D_Vec, bool bSwap=true)
	{
		_fgetts(Main_Buffer, 256, Stream); //*MESH_VERTEX    0	-5.0773	-5.1949	0.0000

		if (bSwap)
		{
			_stscanf(Main_Buffer, _T("%s%d%f%f%f"),	one_string_Buffer,	&Integer_Buffer,	&D_Vec.x,	&D_Vec.z,	&D_Vec.y); //Y�� Z�� �ٲ��.
		}
		else
		{
			_stscanf(Main_Buffer, _T("%s%d%f%f%f"),	one_string_Buffer,	&Integer_Buffer,	&D_Vec.x,	&D_Vec.y,	&D_Vec.z); // �ȹٲٰ� �״�δ�. Į���̴�.
		}


	}
	void LoadIndexData(Index& D_Index)
	{
		_fgetts(Main_Buffer, 256, Stream);
		_stscanf(Main_Buffer, _T("%s%d%d%d%d"),	one_string_Buffer,&Integer_Buffer,	&D_Index.ASE_Index[0],	&D_Index.ASE_Index[2],	&D_Index.ASE_Index[1]); // �̰� YZ���� �ƴ϶�, �ð�ݽð�ƴϾ�?
	}

public:
	AseMesh() {};
	virtual ~AseMesh() {}
};




