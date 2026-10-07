#include "pch.h"
#include "CClientCameraMgr.h"
#include "CCamera.h"
#include "CDInputMgr.h"
#include "CCinematicCamera.h"
#include "CPlayerCamera.h"

IMPLEMENT_SINGLETON(CClientCameraMgr)

CClientCameraMgr::CClientCameraMgr()
{
}

CClientCameraMgr::~CClientCameraMgr()
{
    Free();
}

HRESULT CClientCameraMgr::Add_Camera(CLIENT_CAMERA_TYPE eType, CCamera* pCamera)
{
    if (nullptr == pCamera)
        return E_FAIL;

    // 같은 종류의 중복 등록 방지
    if (m_mapCamera.find(eType) != m_mapCamera.end())
        return E_FAIL;

    // 같은 객체를 다른 키로 등록하면 종료 시 중복 해제될 수 있습니다.
    for (const auto& pair : m_mapCamera)
    {
        if (pair.second == pCamera)
            return E_FAIL;
    }

    m_mapCamera.emplace(eType, pCamera);
    return S_OK;
}

HRESULT CClientCameraMgr::Select_Camera(CLIENT_CAMERA_TYPE eType)
{
    CCamera* pCamera = Find_Camera(eType);

    if (nullptr == pCamera)
        return E_FAIL;

    m_pActiveCamera = pCamera;
    m_eCurrentType = eType;

    if (eType == CLIENT_CAMERA_TYPE::CINEMATIC)
    {
        auto pCinematicCamera = static_cast<CCinematicCamera*>(pCamera);

        if (FAILED(pCinematicCamera->Play()))
            return E_FAIL;
	}

    return S_OK;
}

CCamera* CClientCameraMgr::Find_Camera(CLIENT_CAMERA_TYPE eType) const
{
    auto iter = m_mapCamera.find(eType);

    if (iter == m_mapCamera.end())
        return nullptr;

    return iter->second;
}

CCamera* CClientCameraMgr::Get_ActiveCamera() const
{
    return m_pActiveCamera;
}

void CClientCameraMgr::Update_Camera(_float fTimeDelta)
{
    if (nullptr == m_pActiveCamera)
        return;

    // 카메라 설정
	Key_Input(fTimeDelta);

    // CDynamicCamera라면 여기서 키보드·마우스 입력 처리
    m_pActiveCamera->Update_GameObject(fTimeDelta);

    // 연출 카메라가 종료되면 플레이어 카메라로 전환
    CinematicToPlayer();
}

void CClientCameraMgr::LateUpdate_Camera(_float fTimeDelta)
{
    if (nullptr == m_pActiveCamera)
        return;

    // 추적 카메라 등에서 필요한 최종 위치 보정
    m_pActiveCamera->LateUpdate_GameObject(fTimeDelta);

    // 모든 동작을 마친 뒤 행렬 계산 및 화면 적용
    m_pActiveCamera->Update_Matrices();
    m_pActiveCamera->Apply_Transform();
}

void CClientCameraMgr::SetPlayerCameraMode(CAMERA_MODE eMode)
{
    Select_Camera(CLIENT_CAMERA_TYPE::PLAYER);
    static_cast<CPlayerCamera*>(m_pActiveCamera)->Set_CameraMode(eMode);
    m_OnCameraViewChanged.Broadcast(eMode);
}

void CClientCameraMgr::Free()
{
    // 선택 포인터는 소유권이 없으므로 비우기만 합니다.
    m_pActiveCamera = nullptr;

    for (auto& pair : m_mapCamera)
    {
        if (nullptr != pair.second)
        {
            pair.second->Release();
            pair.second = nullptr;
        }
    }

    m_mapCamera.clear();
}

void CClientCameraMgr::Key_Input(_float fTimeDelta)
{
    // 일반 카메라 일 때만 카메라 전환 키 허용
    if (m_eCurrentType != CLIENT_CAMERA_TYPE::CINEMATIC)
    {
        if (CDInputMgr::GetInstance()->Key_Down(DIK_1))
        {
            Select_Camera(CLIENT_CAMERA_TYPE::PLAYER);
            static_cast<CPlayerCamera*>(m_pActiveCamera)->Set_CameraMode(CAMERA_MODE::FIRST_PERSON);
            m_OnCameraViewChanged.Broadcast(CAMERA_MODE::FIRST_PERSON);
        }
        else if (CDInputMgr::GetInstance()->Key_Down(DIK_2))
        {
            Select_Camera(CLIENT_CAMERA_TYPE::FREE);
        }
        else if (CDInputMgr::GetInstance()->Key_Down(DIK_3))
        {
            Select_Camera(CLIENT_CAMERA_TYPE::PLAYER);
            static_cast<CPlayerCamera*>(m_pActiveCamera)->Set_CameraMode(CAMERA_MODE::THIRD_PERSON);
            m_OnCameraViewChanged.Broadcast(CAMERA_MODE::THIRD_PERSON);
        }
    }
    else
    {
        if (CDInputMgr::GetInstance()->Key_Down(DIK_RETURN))
        {
            auto pCinematicCamera = static_cast<CCinematicCamera*>(m_pActiveCamera);
            pCinematicCamera->Skip();
        }
    }
}

void CClientCameraMgr::CinematicToPlayer()
{
    if (m_pActiveCamera != Find_Camera(CLIENT_CAMERA_TYPE::CINEMATIC))
        return;

	auto pCinematicCamera = static_cast<CCinematicCamera*>(m_pActiveCamera);
    if (pCinematicCamera->Is_Finished())
        Select_Camera(CLIENT_CAMERA_TYPE::PLAYER);
}
