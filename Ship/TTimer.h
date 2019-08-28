#pragma once
#include "xStd.h"

/*
1) 1프레임의 경과 시간 = m_fSecPerFrame
2) 1초의 프레임 카운터	= m_dwFPS
3) 게임이 실행된 누적시간 = m_fGameTime
*/
class TTimer
{
public:
	LARGE_INTEGER	m_Frequency;		// 초당 주파수 
	LARGE_INTEGER	m_CurrentTime;		// 현재 시간 
	LARGE_INTEGER	m_FPS;				//(FPS 체크 타이머)
	LARGE_INTEGER	m_BeforeTick;		// 이전 프레임의 시간 체크 
	LARGE_INTEGER	m_Start;			// 이벤트 타이머 시작 시간

	int				m_iFramePerSecond;			// 2) 1초의 프레임 카운터
	int				m_iFrameCount;				// 초당 경과된 프레임 

	float			m_fSecPerFrame;				//1) 1프레임의 경과 시간
	float			m_fGameTime;				//3) 게임이 실행된 누적시간

	TCHAR	m_csBuffer[256];

public:
	bool Init();			//초기화
	bool Frame();			//계산
	bool Render();			//드로우
	bool Release();			//소멸, 삭제

	TTimer();
	virtual ~TTimer();
};

