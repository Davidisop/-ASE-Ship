#include "TScene.h"

class Sample : public xCore
{
public:

	TScene*  m_pGameScene;
	TScene*  m_pLobbyScene;
	TScene*  m_pEndScene;
	TScene*  m_pCurrentScene;

	int		 m_iLevel;

public:
	bool Init()
	{
		xCore::Init();

		m_pLobbyScene->Init();
		m_pGameScene->Init();
		m_pEndScene->Init();

		return true;
	}
	bool Frame()
	{
		xCore::Frame();

		switch (m_pCurrentScene->m_iSceneID)
		{
			case 0:
			{
				if (m_pCurrentScene->m_bNextSceneStart == true)
				{
					m_pCurrentScene->m_bNextSceneStart = false;
					m_pCurrentScene = m_pGameScene;
				}
			}break;

			case 1:

			{
				if (m_pCurrentScene->m_bNextSceneStart == true)
				{
					{
						m_pCurrentScene = m_pEndScene;
					}
				}
			}break;

			case 2:
			{
				if (m_pCurrentScene->m_bNextSceneStart == true)
				{
					m_pCurrentScene->m_bNextSceneStart = false;
					m_pCurrentScene = m_pGameScene;
				}
			}break;

		}
		m_pCurrentScene->Frame();

		return true;
	}
	bool Render()
	{
		xCore::Render();
		m_pCurrentScene->Render();
		return true;
	}
	
	bool Release()
	{
		xCore::Release();
		m_pGameScene->Release();
		m_pLobbyScene->Release();
		m_pEndScene->Release();
		return true;
	}
public:
	Sample() 
	{
		m_iLevel = 0;

		m_pLobbyScene = new TSceneLobby();
		m_pGameScene = new TSceneGame();
		m_pEndScene = new TSceneEnd();

		m_pCurrentScene = m_pLobbyScene;

	}
	~Sample() {}
};

GAMERUN("Texture3D", 900, 500);