#pragma once

#include "xStd.h"

class Canon_Camera
{
public:

	D3DXMATRIX			 m_matView; 
	
public:

	D3DXMATRIX			 m_matProj;


	//카메라 회전

	float m_fCameraYawAngle;
	float m_fCameraPitchAngle;
	float m_fCameraRollAngle;


	// 뷰 행렬


	D3DXVECTOR3   camera_position;   D3DXVECTOR3 camera_look;
	D3DXVECTOR3   fake_up;			 D3DXVECTOR3 camera_up;
	D3DXVECTOR3   Target_at;         D3DXVECTOR3 camera__Right;

	// 카메라 이동

	float m_fSpeed;


	bool Speed_auto_Lessen()
	{
		m_fSpeed -= g_fSecPerFrame;
		if (m_fSpeed < 0.5f) m_fSpeed = 0.5f;
		
		return true;
	}

	// 투영 행렬

	float Vertical_Fov_Angle;
	float Aspect_Ratio;//가로세로비
	float Near_Plane_Distance;
	float Far_Plane_Distance;


	void Set_View_Matrix(
		D3DXVECTOR3 camera_position_in = D3DXVECTOR3(10.0f, 10.0f, -10.0f),
		D3DXVECTOR3 fake_up_in = D3DXVECTOR3(0.0f, 1.0f, 0.0f),
		D3DXVECTOR3 Target_at_in = D3DXVECTOR3(0.0f, 0.0f, 0.0f))
	
	{
		camera_position = camera_position_in;
		fake_up = fake_up_in;
		Target_at = Target_at_in;

		D3DXMatrixLookAtLH(&m_matView,
			&camera_position,
			&Target_at,
			&fake_up); // 뷰 행렬 반환

		camera_Coordinates_System_from_ViewMatrix_and_YawPitch_Roll_from_LookVector(); // ViewMatrix가 나오면, 그걸로 CoordinateSystem과 요피치롤 업데이트한다.

	}





		bool ViewMatrix_Update_From_Keyboard_Mouse(D3DXVECTOR4 vValue) // 이게 핵심이다. 벡터4 vValue가 넘어오는 게 시작.
		{
			// yaw, pitch,roll, radius
			D3DXMATRIX matRotation;
			D3DXQUATERNION qRotation;

			D3DXQuaternionRotationYawPitchRoll(&qRotation, // 그걸 가지고, 쿼터니온을 만든다.
				m_fCameraYawAngle += vValue.y,
				m_fCameraPitchAngle += vValue.x,
				m_fCameraRollAngle += vValue.z); // 마우스 z좌표 할 게 없다.

			D3DXMatrixAffineTransformation(&matRotation, 1.0f, NULL, &qRotation, &camera_position);
			D3DXMatrixInverse(&m_matView, NULL, &matRotation); // 분명히 카메라와 물체들은 정 반대로 움직인다. 역행렬이다.
	
			camera_Coordinates_System_from_ViewMatrix_and_YawPitch_Roll_from_LookVector();

			return true;
		}




	void Set_Projection_Matrix(float Vertical_Fov_Angle_in, float Aspect_Ratio_in, float Near_Plane_Distance_in, float Far_Plane_Distance_in)
	{
		Vertical_Fov_Angle = Vertical_Fov_Angle_in;
		Aspect_Ratio = Aspect_Ratio_in;
		Near_Plane_Distance = Near_Plane_Distance_in;
		Far_Plane_Distance = Far_Plane_Distance_in;

		D3DXMatrixPerspectiveFovLH(&m_matProj,
			(float)D3DX_PI * Vertical_Fov_Angle,
			Aspect_Ratio,
			Near_Plane_Distance,
			Far_Plane_Distance);
	}

	void Move_on_Look_line(float fValue)
	{
		camera_position += camera_look * fValue*m_fSpeed; // 카메라를 Look쪽으로 이동.
	}
	
	void Move_on_Right_line(float fValue)
	{
		camera_position += camera__Right * fValue*m_fSpeed; // 카메라를 Right 벡터쪽으로 이동.
	}

	void Move_on_Up_line(float fValue)
	{
		camera_position += camera_up * fValue*m_fSpeed; // 카메라를 UP 벡터쪽으로 이동.
	}

	void SetSpeed(float fValue)
	{
		m_fSpeed += fValue;
	}



	bool camera_Coordinates_System_from_ViewMatrix_and_YawPitch_Roll_from_LookVector() // ViewMatix가 업데이트 되어서 넘어오면, 카메라 Coordinate System 과 요피치롤을 실시간 업데이트 합니다.
	{
		camera__Right.x = m_matView._11; camera_up.x = m_matView._12;   camera_look.x = m_matView._13;
		camera__Right.y = m_matView._21; camera_up.y = m_matView._22;   camera_look.y = m_matView._23;
		camera__Right.z = m_matView._31; camera_up.z = m_matView._32;   camera_look.z = m_matView._33;

		D3DXMATRIX m_Inverse_ViewMatrix;
		D3DXMatrixInverse(&m_Inverse_ViewMatrix, NULL, &m_matView);
		D3DXVECTOR3* camera_look_vectors_ptr = (D3DXVECTOR3*)&m_Inverse_ViewMatrix._31; 

		m_fCameraYawAngle = atan2f(camera_look_vectors_ptr->x, camera_look_vectors_ptr->z);
		float fLen = sqrtf(camera_look_vectors_ptr->z * camera_look_vectors_ptr->z + camera_look_vectors_ptr->x * camera_look_vectors_ptr->x);
		m_fCameraPitchAngle = -atan2f(camera_look_vectors_ptr->y, fLen);

		return true;
	}




	Canon_Camera()
	{
		m_fCameraYawAngle = 0.0f;
		m_fCameraPitchAngle = 0.0f;
		m_fCameraRollAngle = 0.0f;
		m_fSpeed = 1.0f;
	}
};