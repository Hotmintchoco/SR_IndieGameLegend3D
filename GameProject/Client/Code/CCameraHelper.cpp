#include "pch.h"
#include "CCameraHelper.h"

CCameraHelper::CCameraHelper(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}

CCameraHelper::~CCameraHelper()
{
}

HRESULT CCameraHelper::Ready_GameObject()
{
    if (FAILED(CGameObject::Ready_GameObject()))
        return E_FAIL;

    return S_OK;
}

_int CCameraHelper::Update_GameObject(_float fTimeDelta)
{
    _int iExit = CGameObject::Update_GameObject(fTimeDelta);

    return iExit;
}

void CCameraHelper::LateUpdate_GameObject(_float fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CCameraHelper::Render_GameObject()
{
}

HRESULT CCameraHelper::Add_Component()
{
    return S_OK;
}

CCameraHelper* CCameraHelper::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    return nullptr;
}

void CCameraHelper::Free()
{
    CGameObject::Free();
}
