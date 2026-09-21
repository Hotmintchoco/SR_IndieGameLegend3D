#include "pch.h"
#include "CBoss1.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CTimerMgr.h"
//#include "CDInputMgr.h"
#include "CTerrain.h"

CBoss1::CBoss1(LPDIRECT3DDEVICE9 pGraphicDev)
    : CMonster(pGraphicDev)
{
}


CBoss1::~CBoss1()
{
}

HRESULT CBoss1::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;
    CMonster::Ready_GameObject();

    m_pTransformCom->Set_Scale(2.f, 2.f, 2.f);

    m_pTransformCom2->Set_Scale(1.f, 1.f, 1.f);
    m_pTransformCom2->Set_Pos(m_pTransformCom->m_vInfo[INFO_POS].x, m_pTransformCom->m_vInfo[INFO_POS].y, m_pTransformCom->m_vInfo[INFO_POS].z);

    m_pColliderCom->Set_Radius(m_pTransformCom->m_vScale.x);

    m_iHp = 3;
    return S_OK;
}

_int CBoss1::Update_GameObject(const _float& fTimeDelta)
{
    _int    iExit = CMonster::Update_GameObject(fTimeDelta);
    Set_OnTerrain();
    m_fFrame += fTimeDelta * 6.f;
    if (m_fFrame > 4.f)
        m_fFrame = 0.f;

    return iExit;
}

void CBoss1::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CMonster::LateUpdate_GameObject(fTimeDelta);

    CTransform* pPlayerTransformCom = dynamic_cast<CTransform*>(Engine::CManagement::GetInstance()
        ->Get_Component(ID_DYNAMIC, L"GameLogic_Layer", L"Player", L"Com_Transform"));

    if (nullptr == pPlayerTransformCom)
        return;

    _vec3   vPlayerPos;
    pPlayerTransformCom->Get_Info(INFO_POS, &vPlayerPos);

    _vec3   vPlayerLook;
    pPlayerTransformCom->Get_Info(INFO_LOOK, &vPlayerLook);

    //m_pTransformCom->Chase_Target(&vPlayerPos, &vPlayerLook, 0.8f, fTimeDelta);
    m_pTransformCom->LookAt_Player(&vPlayerPos, &vPlayerLook);
    //m_pTransformCom2->Chase_Target(&vPlayerPos, &vPlayerLook, 0.8f, fTimeDelta);
    _vec3 vSrc = m_pTransformCom->m_vInfo[INFO_LOOK];
    _vec3 vSr2222;
    _matrix a;
    m_pTransformCom->Get_Info(INFO_LOOK, &vSr2222);

    _vec3 vSrc2 = pPlayerTransformCom->m_vInfo[INFO_LOOK];
    //Angry버전 Transform->chase업데이트
    if (m_iHp < 4)
    {
        _matrix	matWorld, matScale, matRot, matTrans;

        D3DXMatrixScaling(&matScale, m_pTransformCom2->m_vScale.x, m_pTransformCom2->m_vScale.y, m_pTransformCom2->m_vScale.z);

        if ((_uint)m_fFrame % 2 == 0)
        {
            D3DXMatrixTranslation(&matTrans,
                m_pTransformCom->m_vInfo[INFO_POS].x,
                m_pTransformCom->m_vInfo[INFO_POS].y,
                m_pTransformCom->m_vInfo[INFO_POS].z);
        }
        else
        {
            D3DXMatrixTranslation(&matTrans,
                m_pTransformCom->m_vInfo[INFO_POS].x,
                m_pTransformCom->m_vInfo[INFO_POS].y + 0.1f,
                m_pTransformCom->m_vInfo[INFO_POS].z);
        }
        _vec3 vSrc = m_pTransformCom->m_vInfo[INFO_LOOK];
        _vec3 vDst = -pPlayerTransformCom->m_vInfo[INFO_LOOK]; //플레이어랑 평행하게 정렬
        //_vec3 vDst = pPlayerTransformCom->m_vInfo[INFO_POS] - m_pTransformCom->m_vInfo[INFO_POS]; //플레이어 시선 기준 정렬

        _vec3 vAxis = { 0.f, 1.f, 0.f };
        _vec3 vCross;

        vSrc.y = 0;
        vDst.y = 0;

        float fAngle = acosf(D3DXVec3Dot(D3DXVec3Normalize(&vSrc, &vSrc), D3DXVec3Normalize(&vDst, &vDst)));

        if (D3DXVec3Dot(D3DXVec3Cross(&vCross, &vSrc, &vDst), &vAxis) < 0.f)
            fAngle *= -1;

        D3DXMatrixRotationAxis(&matRot, &m_pTransformCom->m_vInfo[INFO_UP], fAngle);

        //D3DXMatrixIdentity(&matRot);

        matWorld = matScale * matRot * matTrans;
        m_pTransformCom2->Set_World(&matWorld);
    }
}

void CBoss1::Render_GameObject()
{
    if (m_bHitState == true) CMonster::Enable_HitRenderState();

    CMonster::Render_GameObject();

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
    m_pTextureCom->Set_Texture((_uint)m_fFrame);
    m_pBufferCom->Render_Buffer();

    if (m_iHp < 3)
    {
        m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom2->Get_World());
        m_pTextureCom2->Set_Texture((_uint)m_fFrame / 2);
        m_pBufferCom->Render_Buffer();
    }
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

    if (m_bHitState == true) CMonster::Disable_HitRenderState();

}

void CBoss1::OnCollisionEnter(CGameObject* pOther)
{
    CMonster::OnCollisionEnter(pOther);

}

HRESULT CBoss1::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_boss1Texture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    // Texture2
    pComponent = m_pTextureCom2 = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_boss1_angryTexture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture2", pComponent });

    // Transform2
    pComponent = m_pTransformCom2 = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform2", pComponent });


    return S_OK;
}


CBoss1* CBoss1::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CBoss1* pMonster = new CBoss1(pGraphicDev);

    if (FAILED(pMonster->Ready_GameObject()))
    {
        Safe_Release(pMonster);
        MSG_BOX("CBoss1 Create Failed");
        return nullptr;
    }

    return pMonster;
}

void CBoss1::Free()
{
    CMonster::Free();
}