#include "pch.h"
#include "CMG0_DancerPart.h"
#include "CProtoMgr.h"
#include "CTransform.h"
#include "CPlayerPartTex.h"

CMG0_DancerPart::CMG0_DancerPart(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}

CMG0_DancerPart::~CMG0_DancerPart()
{
}

HRESULT CMG0_DancerPart::Ready_GameObject(const _tchar* pProtoTag)
{
    if (FAILED(Add_Component(pProtoTag)))
        return E_FAIL;

    Set_Parent(nullptr);

    return CGameObject::Ready_GameObject();
}

_int CMG0_DancerPart::Update_GameObject(_float fTimeDelta)
{
    return CGameObject::Update_GameObject(fTimeDelta);
}

void CMG0_DancerPart::LateUpdate_GameObject(_float fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CMG0_DancerPart::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
    m_pBufferCom->Render_Buffer();
}

void CMG0_DancerPart::Set_Parent(CMG0_DancerPart* pParent)
{
    m_pParent = pParent;

    if (m_pParent)
        m_pTransformCom->Set_Pos(m_vPivot - m_pParent->Get_Pivot());
    else
        m_pTransformCom->Set_Pos(m_vPivot);
}

void CMG0_DancerPart::Update_Part(const MINIGAME0::MGDANCEBEAT& tBeat, _float fTimeDelta, const _matrix& matRoot)
{
    _float fTarget = 0.f;

    if (tBeat.bDance)
    {
        _float fFrom = Get_PoseAngle(tBeat.iPoseFrom);
        _float fTo = Get_PoseAngle(tBeat.iPoseTo);

        fTarget = fFrom + (fTo - fFrom) * tBeat.fRate;
    }

    m_fAngle += (fTarget - m_fAngle) * min(1.f, 18.f * fTimeDelta);

    m_pTransformCom->Set_Rotation_Raw(_vec3(tBeat.bSit ? Get_SitAngle() : 0.f, 0.f, m_fAngle));

    if (m_pParent)
        m_pTransformCom->WorldMatrixPropagation(*m_pParent->Get_Transform()->Get_World());
    else
        m_pTransformCom->WorldMatrixPropagation(matRoot);
}

HRESULT CMG0_DancerPart::Add_Component(const _tchar* pProtoTag)
{
    CComponent* pComponent = nullptr;

    pComponent = m_pBufferCom = dynamic_cast<CPlayerPartTex*>(CProtoMgr::GetInstance()->Clone_Prototype(pProtoTag));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    m_pTransformCom->SetUseLocal(true);

    m_vPivot = m_pBufferCom->GetPivot();

    return S_OK;
}

void CMG0_DancerPart::Free()
{
    CGameObject::Free();
}
