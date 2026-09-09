#include "pch.h"
#include "CGun.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include <ctime>

CGun::CGun(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}


CGun::~CGun()
{
}

HRESULT CGun::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    /* [DEBUG] Start 초기값 세팅 */
    m_pTransformCom->m_vInfo[INFO_POS] = _vec3{ 0.f, 0.f, 0.f };
    /* [DEBUG] End 초기값 세팅 */

    return S_OK;
}

_int CGun::Update_GameObject(const _float& fTimeDelta)
{
    _int    iExit = CGameObject::Update_GameObject(fTimeDelta);

    /* [DEBUG] Start 확인용 움직임 */
    m_pTransformCom->Rotation(ROT_Y, 30.f * fTimeDelta);
    _vec3 tmp{ 0.f, 1.f, 0.f };
    float t = (float)clock() / CLOCKS_PER_SEC;
    m_pTransformCom->Move_Pos(&tmp, 0.2 * sinf(t), fTimeDelta);
    /* [DEBUG] End 확인용 움직임 */

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_PRIORITY, this);


    return iExit;
}

void CGun::LateUpdate_GameObject(const _float& fTimeDelta)
{

    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CGun::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

    m_pTextureCom->Set_Texture(0);

    m_pBufferCom->Render_Buffer();
}

HRESULT CGun::Add_Component()
{
    CComponent* pComponent = nullptr;

    // TerrainTex
    pComponent = m_pBufferCom = dynamic_cast<CPlyTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_GunVertex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_GunTexture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    // Transform
    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });


    return S_OK;
}

CGun* CGun::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CGun* pGun = new CGun(pGraphicDev);

    if (FAILED(pGun->Ready_GameObject()))
    {
        Safe_Release(pGun);
        MSG_BOX("CGun Create Failed");
        return nullptr;
    }

    return pGun;
}

void CGun::Free()
{
    CGameObject::Free();
}
