#include "CManagement.h"


IMPLEMENT_SINGLETON(CManagement)

CManagement::CManagement() : m_pScene(nullptr)
{
}

CManagement::~CManagement()
{
    Free();
}

CComponent* CManagement::Get_Component(COMPONENTID eID, const _tchar* pLayerTag, 
                                    const _tchar* pObjTag, const _tchar* pComponentTag)
{
    if (nullptr == m_pScene)
        return nullptr;

    return m_pScene->Get_Component(eID, pLayerTag, pObjTag, pComponentTag);
}

CGameObject* CManagement::Get_GameObject(const _tchar* pLayerTag, const _tchar* pObjTag)
{
    if (nullptr == m_pScene)
        return nullptr;

    return m_pScene->Get_GameObject(pLayerTag, pObjTag);
}

CLayer* CManagement::Get_Layer(const _tchar* pLayerTag)
{
    if (nullptr == m_pScene)
        return nullptr;

    return m_pScene->Get_Layer(pLayerTag);
}

HRESULT CManagement::Set_Scene(CScene* pScene)
{
    if (nullptr == pScene)
        return  E_FAIL;

    Safe_Release(m_pScene);

    m_pScene = pScene;

    return S_OK;
}

HRESULT CManagement::Change_Scene(_int iSceneIdx, CScene* pNewScene, bool bDestoryOld)
{
    // 1. 기존 씬이 있다면 퇴장(Exit) 처리
    if (nullptr != m_pScene)
    {
        m_pScene->OnExit();
        // 핵심: 이전 씬을 지워야 한다면 맵에서 제거하고 메모리 해제
        if (bDestoryOld)
        {
            auto iter = m_mapScene.find(iSceneIdx);
            if (iter != m_mapScene.end())
            {
                Safe_Release(iter->second); // CLogo 메모리 해제
                m_mapScene.erase(iter);    // 맵에서 CLogo 삭제
            }
            m_pScene = nullptr;
        }
    }

    // 2. 맵에서 씬 검색
    auto iter = m_mapScene.find(iSceneIdx);
    if (iter != m_mapScene.end())
    {
        // 3-A. 씬이 이미 존재하면 재사용 및 입장(Enter) 처리
        m_pScene = iter->second;
        m_pScene->OnEnter();
    }
    else
    {
        // 3-B. 씬이 없으면 맵에 추가하고 최초 입장(Enter) 처리
        if (nullptr == pNewScene)
            return E_FAIL;

        m_mapScene.emplace(iSceneIdx, pNewScene);
        m_pScene = pNewScene;
        m_pScene->OnEnter();
    }

    m_iSceneIdx = iSceneIdx;
    return S_OK;
}

_int CManagement::Update_Scene(const _float& fTimeDelta)
{
    if (nullptr == m_pScene)
        return -1;

    return m_pScene->Update_Scene(fTimeDelta);
}

void CManagement::LateUpdate_Scene(const _float& fTimeDelta)
{
    m_pScene->LateUpdate_Scene(fTimeDelta);
}

void CManagement::Render_Scene(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CRenderer::GetInstance()->Render(pGraphicDev);

    m_pScene->Render_Scene();
}

void CManagement::Free()
{
    Safe_Release(m_pScene);
}
