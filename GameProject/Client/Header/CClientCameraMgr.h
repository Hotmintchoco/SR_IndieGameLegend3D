#pragma once

#include "Engine_Define.h"

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

class CClientCameraMgr
{
    DECLARE_SINGLETON(CClientCameraMgr)

private:
    CClientCameraMgr();
    ~CClientCameraMgr();

public:
    // 성공하면 매니저가 카메라의 소유권을 넘겨받습니다.
    // 실패하면 호출자가 카메라를 해제해야 합니다.
    HRESULT         Add_Camera(CLIENT_CAMERA_TYPE eType, Engine::CCamera* pCamera);

    HRESULT         Select_Camera(CLIENT_CAMERA_TYPE eType);

    CCamera*        Find_Camera(CLIENT_CAMERA_TYPE eType) const;
    CCamera*        Get_ActiveCamera() const;

public:
    void            Update_Camera(const _float& fTimeDelta);
    void            LateUpdate_Camera(const _float& fTimeDelta);

    // 씬 종료·재시작 시 카메라들을 정리합니다.
    void Free();

private:
    map<CLIENT_CAMERA_TYPE, CCamera*>   m_mapCamera;
    CCamera*                            m_pActiveCamera = nullptr;
};

