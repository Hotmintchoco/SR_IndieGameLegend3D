#include "CScene.h"

CScene::CScene(LPDIRECT3DDEVICE9 pGraphicDev)
    : m_pGraphicDev(pGraphicDev)
{
    m_pGraphicDev->AddRef();
}

CScene::~CScene()
{
}

CComponent* CScene::Get_Component(COMPONENTID eID, const wstring& wstrLayerTag, const _tchar* pObjTag, const _tchar* pComponentTag)
{
    auto    iter = m_mapLayer.find(wstrLayerTag);

    if (iter == m_mapLayer.end())
        return nullptr;

    return iter->second->Get_Component(eID, pObjTag, pComponentTag);
}

CGameObject* CScene::Get_GameObject(const wstring& wstrLayerTag, const _tchar* pObjTag)
{
    auto    iter = m_mapLayer.find(wstrLayerTag);

    if (iter == m_mapLayer.end())
        return nullptr;

    return iter->second->Get_GameObject(pObjTag);
}

CLayer* CScene::Get_Layer(const wstring& wstrLayerTag)
{
    auto    iter = m_mapLayer.find(wstrLayerTag);

    if (iter == m_mapLayer.end())
        return nullptr;

    return iter->second;
}

HRESULT CScene::Ready_Scene()
{
    return S_OK;
}

_int CScene::Update_Scene(const _float& fTimeDelta)
{
    for (auto& pLayer : m_mapLayer)
        pLayer.second->Update_Layer(fTimeDelta);

    return 0;
}

void CScene::LateUpdate_Scene(const _float& fTimeDelta)
{
    for (auto& pLayer : m_mapLayer)
        pLayer.second->LateUpdate_Layer(fTimeDelta);
}


void CScene::Free()
{
    for_each(m_mapLayer.begin(), m_mapLayer.end(), CDeleteMap());
    m_mapLayer.clear();

    Safe_Release(m_pGraphicDev);
}
