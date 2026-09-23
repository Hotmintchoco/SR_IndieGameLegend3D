#include "pch.h"
#include "CUI.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CRenderer.h"

CUI::CUI(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev), m_fFrame(0.f)
{
}


CUI::~CUI()
{
}

HRESULT CUI::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    return S_OK;
}

_int CUI::Update_GameObject(const _float& fTimeDelta)
{
    _int iExit = CGameObject::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

    return iExit;
}

void CUI::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);

    _vec3       vPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    CGameObject::Compute_ViewZ(&vPos);
}

void CUI::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

    if (nullptr != m_pTextureCom)
        m_pTextureCom->Set_Texture((_uint)m_fFrame);
	m_pBufferCom->Render_Buffer();
}

void CUI::Set_Pos(const _vec2& vPos)
{
    m_vPos = {vPos.x, vPos.y, 0.f}; /* 성철 : 형변환 경고 떠서 임의로 변경 */
    if (nullptr != m_pTransformCom)
		m_pTransformCom->Set_Pos(vPos.x - WINCX * 0.5f, -vPos.y + WINCY * 0.5f, 0.f);
}
void CUI::Set_Pos(_float fX, _float fY, _float fZ)
{
    if (nullptr != m_pTransformCom)
		m_pTransformCom->Set_Pos(fX - WINCX * 0.5f, -fY + WINCY * 0.5f, fZ);
}

void CUI::Set_Size(const _vec2& vSize)
{
    m_vSize = vSize;
    if (nullptr != m_pTransformCom)
        m_pTransformCom->Set_Scale(m_vSize.x, m_vSize.y, 1.f);
}

void CUI::Set_Texture(const _uint& iIndex)
{
    m_fFrame = (_float)iIndex;
}

HRESULT CUI::Add_Component()
{
    CComponent* pComponent = nullptr;

    // RcTex
    pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));
    if (nullptr == pComponent)
        return E_FAIL;
    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    // Transform
    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));
    if (nullptr == pComponent)
        return E_FAIL;
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    // Texture (태그가 있을 때만 생성)
    if (false == m_wstrTextureTag.empty())
    {
        pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(m_wstrTextureTag.c_str()));
        if (nullptr == pComponent)
            return E_FAIL;

        m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });
    }

    return S_OK;
}

CUI* CUI::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CUI* pGameUI = new CUI(pGraphicDev);

    if (FAILED(pGameUI->Ready_GameObject()))
    {
        Safe_Release(pGameUI);
        MSG_BOX("CUI Create Failed");
        return nullptr;
    }

    return pGameUI;
}

CUI* CUI::Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring& wstrTextureTag)
{
    CUI* pGameUI = new CUI(pGraphicDev);
    pGameUI->m_wstrTextureTag = wstrTextureTag;

    if (FAILED(pGameUI->Ready_GameObject()))
    {
        Safe_Release(pGameUI);
        MSG_BOX("CUI Create Failed");
        return nullptr;
    }

    return pGameUI;
}

void CUI::Free()
{
    CGameObject::Free();
}
