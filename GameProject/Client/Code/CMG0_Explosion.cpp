#include "pch.h"
#include "CMG0_Explosion.h"
#include "CProtoMgr.h"

CMG0_Explosion::CMG0_Explosion(LPDIRECT3DDEVICE9 pGraphicDev)
    : CMG0_Sprite(pGraphicDev)
{
}

CMG0_Explosion::~CMG0_Explosion()
{
}

HRESULT CMG0_Explosion::Ready_GameObject(const _vec3& vPos, _float fScale)
{
    if (FAILED(Add_Component()))
        return E_FAIL;
    if (FAILED(CMG0_Sprite::Ready_GameObject()))
        return E_FAIL;

    m_vPos = vPos;
    m_fScale = fScale;

    return S_OK;
}

_int CMG0_Explosion::Update_GameObject(_float fTimeDelta)
{
    m_fElapsedTime += fTimeDelta;
    if (m_fElapsedTime >= m_fLifeTime)
    {
        Set_Dead(true);
        return 0;
    }

    m_vPos.z -= MGBG_SCROLL_SPEED * 0.3f * fTimeDelta;

    _int iCount = max(1, m_pTextureCom->GetCount());
    _int iFrame = (_int)(m_fElapsedTime / m_fLifeTime * iCount);
    m_iFrame = (_uint)max(0, min(iCount - 1, iFrame));

    return CMG0_Sprite::Update_GameObject(fTimeDelta);
}

void CMG0_Explosion::LateUpdate_GameObject(_float fTimeDelta)
{
    CMG0_Sprite::LateUpdate_GameObject(fTimeDelta);
}

void CMG0_Explosion::Render_GameObject()
{
    CMG0_Sprite::Render_GameObject();
}

HRESULT CMG0_Explosion::Add_Component()
{
    CComponent* pComponent = nullptr;

    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_smallexplodeTexture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}

CMG0_Explosion* CMG0_Explosion::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vPos, _float fScale)
{
    CMG0_Explosion* pExplosion = new CMG0_Explosion(pGraphicDev);

    if (FAILED(pExplosion->Ready_GameObject(vPos, fScale)))
    {
        Safe_Release(pExplosion);
        MSG_BOX("CMG0_Explosion Create Failed");
        return nullptr;
    }

    return pExplosion;
}

void CMG0_Explosion::Free()
{
    CMG0_Sprite::Free();
}
