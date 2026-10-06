#pragma once

#include "Engine_Define.h"
#include "CEventDelegate.h"

namespace Engine
{
    class CCamera;
}

enum class CLIENT_CAMERA_TYPE
{
    FREE,
    PLAYER,
    CINEMATIC
};

enum class CAMERA_MODE;

class CClientCameraMgr
{
    DECLARE_SINGLETON(CClientCameraMgr)

private:
    CClientCameraMgr();
    ~CClientCameraMgr();

public:
    HRESULT         Add_Camera(CLIENT_CAMERA_TYPE eType, Engine::CCamera* pCamera);

    HRESULT         Select_Camera(CLIENT_CAMERA_TYPE eType);

    CCamera*        Find_Camera(CLIENT_CAMERA_TYPE eType) const;
    CCamera*        Get_ActiveCamera() const;

public:
    void            Update_Camera(_float fTimeDelta);
    void            LateUpdate_Camera(_float fTimeDelta);

    /* 성철 : 뷰 전환 이벤트 처리 */
    CEventDelegate<CAMERA_MODE> m_OnCameraViewChanged;

    // 씬 종료·재시작 시 카메라들을 정리합니다.
    void            Free();

private:
	void            Key_Input(_float fTimeDelta);
    void            CinematicToPlayer();

private:
    map<CLIENT_CAMERA_TYPE, CCamera*>   m_mapCamera;
    CCamera*                            m_pActiveCamera = nullptr;
    CLIENT_CAMERA_TYPE                  m_eCurrentType = CLIENT_CAMERA_TYPE::PLAYER;
};

