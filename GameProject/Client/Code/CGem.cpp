#include "pch.h"
#include "CGem.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CGameStatusMgr.h"
#include "CManagement.h"
#include "CTransform.h"
#include "CUI.h"

CGem::CGem(LPDIRECT3DDEVICE9 pGraphicDev)
    : CItem(pGraphicDev)
{
}

CGem::CGem(LPDIRECT3DDEVICE9 pGraphicDev, Engine::CGameObject* pSpawner)
    : CItem(pGraphicDev)
{
    CTransform* pTransform = static_cast<CTransform*>(pSpawner->Get_Component(ID_DYNAMIC, L"Com_Transform"));
    pTransform->Get_Info(INFO_POS, &m_vSpawnPos);
}

CGem::~CGem()
{
}

HRESULT CGem::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    if (FAILED(CItem::Ready_GameObject()))
        return E_FAIL;

    m_pTransformCom->Set_Pos(m_vSpawnPos + _vec3{0.f, -m_vSpawnPos.y +0.25f, 0.f});
    m_pTransformCom->Set_Scale(0.12f, 0.12f, 1.f);

    return S_OK;
}

_int CGem::Update_GameObject(const _float& fTimeDelta)
{
    _int    iExit = CItem::Update_GameObject(fTimeDelta);

    m_fFrame += fTimeDelta * 6.f;
    if (m_fFrame > 6.f)
        m_fFrame = 0.f;

    return iExit;
}

void CGem::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CItem::LateUpdate_GameObject(fTimeDelta);
}

void CGem::Render_GameObject()
{
    if (m_bBlinkStart == true && m_bVisible == false) return;
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

    m_pTextureCom->Set_Texture((_uint)m_fFrame);
    m_pBufferCom->Render_Buffer();
}

HRESULT CGem::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Item_Gem_Texture"));
    if (nullptr == pComponent)
        return E_FAIL;
    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}

void CGem::Consume()
{
    CGameStatusMgr::GetInstance()->UpdateGem(1);

    // Update Gem Count UI
    Update_GemCountUI();

    Set_Dead(true);
}

void CGem::Update_GemCountUI()
{
    int iCnt = CGameStatusMgr::GetInstance()->GetGemCount();
    int iDiv = 100;
    // 100으로 나누고..

    for (int i = 0; i < 3; ++i)
    {
        int iNum = iCnt / iDiv;
        wstring wstrTag = L"GemNum_" + to_wstring(i);

        CUI* pUI = static_cast<CUI*>(CManagement::GetInstance()->Get_GameObject(L"UI_Layer", wstrTag.c_str()));
        pUI->Set_Texture(iNum);
        iCnt = iCnt % iDiv;
        iDiv /= 10;
    }
}

CGem* CGem::Create(LPDIRECT3DDEVICE9 pGraphicDev, Engine::CGameObject* pSpawner)
{
    CGem* pGem = new CGem(pGraphicDev, pSpawner);

    if (FAILED(pGem->Ready_GameObject()))
    {
        Safe_Release(pGem);
        MSG_BOX("CGem Create Failed");
        return nullptr;
    }

    return pGem;
}

void CGem::Free()
{
    CItem::Free();
}
