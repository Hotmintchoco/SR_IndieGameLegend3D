#include "pch.h"
#include "CWorm.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CTimerMgr.h"
//#include "CDInputMgr.h"
#include "CTerrain.h"
#include "CWorm_Boby.h"
#include "CWorm_Tail.h"
#include "CWorm_Face.h"
#include "CLayer.h"


CWorm::CWorm(LPDIRECT3DDEVICE9 pGraphicDev)
    : CMonster(pGraphicDev), m_iWormIndex(0), m_pFrontPart(nullptr)
{
}

CWorm::CWorm(LPDIRECT3DDEVICE9 pGraphicDev, _uint iIndex)
    : CMonster(pGraphicDev), m_iWormIndex(iIndex), m_pFrontPart(nullptr)
{
}


CWorm::~CWorm()
{
}

HRESULT CWorm::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;
    CMonster::Ready_GameObject();

    m_pTransformCom->Set_Scale(1.f, 1.f, 1.f);
    m_pTransformCom->Set_Pos(63.f, 0.f, 58.f);
    m_pColliderCom->Set_Radius(D3DXVec3Length(&m_pTransformCom->m_vScale));

    m_iHp = 10;
    if (m_iWormIndex < 9)
    {
        CGameObject* pGameObject;
        auto iter = m_pmapLayer->find(L"GameLogic_Layer");

        pGameObject = CWorm::Create(m_pGraphicDev, m_iWormIndex, m_pmapLayer);


        if (nullptr == pGameObject)
            return E_FAIL;

        if (iter != m_pmapLayer->end())
        {
            TCHAR		szFileName[128] = L"";
            wsprintf(szFileName, L"Worm_Boby_%d", m_iWormIndex + 1);
            iter->second->Add_GameObject(szFileName, pGameObject);
        }
    }
    //else if (m_iWormIndex == 8)
    //{

    //}
    else
    {
        return S_OK;
    }

    return S_OK;
}

_int CWorm::Update_GameObject(const _float& fTimeDelta)
{
    _int    iExit = CMonster::Update_GameObject(fTimeDelta);
    Set_OnTerrain();
   // m_pTransformCom->Rotation(ROT_Y, 3.f * m_iWormIndex * fTimeDelta);
    if (m_iWormIndex == 0)
    {
        _vec3 v(0, 0, 1);
        m_pTransformCom->Move_Pos(&v, 1.f, fTimeDelta);
    }
    else
    {
        TCHAR		szFileName[128] = L"";
        wsprintf(szFileName, L"Worm_Boby_%d", m_iWormIndex - 1);

        CTransform* pFrontWormTransformCom = dynamic_cast<CTransform*>(Engine::CManagement::GetInstance()
            ->Get_Component(ID_DYNAMIC, L"GameLogic_Layer", szFileName, L"Com_Transform"));
        _vec3 vInfo;
        _vec3 vFrontInfo;
        m_pTransformCom->Get_Info(INFO_POS, &vInfo);
        pFrontWormTransformCom->Get_Info(INFO_POS, &vFrontInfo);
        _vec3 vDir = vFrontInfo - vInfo;
        if (D3DXVec3Length(&vDir) > 2.f)
        {
            m_pTransformCom->Move_Pos(&vDir, 1.f, fTimeDelta);
        }
    }

    return iExit;
}

void CWorm::LateUpdate_GameObject(const _float& fTimeDelta)
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

    //m_pTransformCom->Chase_Target(&vPlayerPos, &vPlayerLook, 3.f, fTimeDelta);

}

void CWorm::Render_GameObject()
{
    if (m_iMotion == 40) m_iMotion = 0;
    WORMDIR eDir = FRONT;
    //eDir = SIDE;
    //eDir = TOP;

    if (m_iWormIndex == 0)
    {
        if (Angle_To_Worm() < D3DXToRadian(135.f))
        {
            eDir = SIDE;

            _matrix	matScale, matRot, matTrans;

            D3DXMatrixScaling(&matScale, m_pTransformCom->m_vScale.x, m_pTransformCom->m_vScale.y, m_pTransformCom->m_vScale.z);

            D3DXMatrixTranslation(&matTrans,
                m_pTransformCom->m_vInfo[INFO_POS].x,
                m_pTransformCom->m_vInfo[INFO_POS].y,
                m_pTransformCom->m_vInfo[INFO_POS].z);

            D3DXMatrixRotationAxis(&matRot, &m_pTransformCom->m_vInfo[INFO_UP], D3DXToRadian(-90.f));

            m_pTransformCom->m_matWorld = matScale * matRot * matTrans;

        }
    }
    else if (m_iWormIndex == 9)
    {
        _float fAngle = Angle_To_Worm();
        if (fAngle <= D3DXToRadian(45.f))
        {
            eDir = FRONT;
        }
        else
        {
            eDir = SIDE;
        }
        if (eDir != FRONT)
        {
            _matrix	matScale, matRot, matTrans;

            D3DXMatrixScaling(&matScale, m_pTransformCom->m_vScale.x, m_pTransformCom->m_vScale.y, m_pTransformCom->m_vScale.z);

            D3DXMatrixTranslation(&matTrans,
                m_pTransformCom->m_vInfo[INFO_POS].x,
                m_pTransformCom->m_vInfo[INFO_POS].y,
                m_pTransformCom->m_vInfo[INFO_POS].z);

            D3DXMatrixRotationAxis(&matRot, &m_pTransformCom->m_vInfo[INFO_UP], D3DXToRadian(-90.f));

            m_pTransformCom->m_matWorld = matScale * matRot * matTrans;
        }
    }
    else
    {
        _float fAngle = Angle_To_Worm();
        if (D3DXToRadian(135.f) < fAngle && fAngle <= D3DXToRadian(160.f))
        {
            eDir = SIDE45;
        }
        else if (D3DXToRadian(45.f) < fAngle && fAngle <= D3DXToRadian(135.f))
        {
            eDir = SIDE;
        }
        else if (fAngle <= D3DXToRadian(45.f))
        {
            eDir = SIDE45;
        }
        if (eDir != FRONT)
        {
            _matrix	matScale, matRot, matTrans;

            D3DXMatrixScaling(&matScale, m_pTransformCom->m_vScale.x, m_pTransformCom->m_vScale.y, m_pTransformCom->m_vScale.z);

            D3DXMatrixTranslation(&matTrans,
                m_pTransformCom->m_vInfo[INFO_POS].x,
                m_pTransformCom->m_vInfo[INFO_POS].y,
                m_pTransformCom->m_vInfo[INFO_POS].z);

            D3DXMatrixRotationAxis(&matRot, &m_pTransformCom->m_vInfo[INFO_UP], D3DXToRadian(-90.f));

            m_pTransformCom->m_matWorld = matScale * matRot * matTrans;
        }
        
    }

    CMonster::Render_GameObject();

    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    if (m_iWormIndex == 0)
    {
        m_pTextureCom->Set_Texture(eDir * 4 + m_iMotion / 10);
    }
    else if (m_iWormIndex == 9)
    {
        m_pTextureCom->Set_Texture(eDir);
    }
    else
    {
        m_pTextureCom->Set_Texture(eDir);
    }
    m_pBufferCom->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

    ++m_iMotion;
}

_float CWorm::Angle_To_Worm()
{
    WORMDIR eDir = FRONT;
    _vec3 pLook;
    m_pTransformCom->Get_Info(INFO_LOOK, &pLook);

    CTransform* pPlayerTransformCom = dynamic_cast<CTransform*>(Engine::CManagement::GetInstance()
        ->Get_Component(ID_DYNAMIC, L"GameLogic_Layer", L"Player", L"Com_Transform"));

    if (nullptr == pPlayerTransformCom)
        return -1.f;

    //_vec3   vPlayerPos;
    //pPlayerTransformCom->Get_Info(INFO_POS, &vPlayerPos);

    _vec3   vPlayerLook;
    pPlayerTransformCom->Get_Info(INFO_LOOK, &vPlayerLook);

    _float fAngle = acosf(D3DXVec3Dot(D3DXVec3Normalize(&vPlayerLook, &vPlayerLook), D3DXVec3Normalize(&pLook, &pLook)));

    //if (fAngle > D3DXToRadian(135.f))
    //{

    //}
    return fAngle;
}

HRESULT CWorm::Add_Component()
{
    CComponent* pComponent = nullptr;
    if (m_iWormIndex == 0)
    {
        pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_worm_drillTexture"));
    }
    else if (m_iWormIndex == 9)
    {
        pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_worm_tailTexture"));
    }
    else
    {
        pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_worm_bobyTexture"));
        //pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_worm_drillTexture"));
    }
    // Texture
    
    //pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_body_45Texture"));
    //pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_body_frontTexture"));
    //pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_body_sideTexture"));
    //pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_body_topTexture"));
    //pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_connectorTexture"));
    //pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_tail_backTexture"));
    //pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_tail_sideTexture"));
    //pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_tail_topTexture"));
    //pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_worm_drill_sideTexture"));
    //pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_worm_drill_topTexture"));
    //pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_worm_faceTexture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}


CWorm* CWorm::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CWorm* pMonster = new CWorm(pGraphicDev);

    if (FAILED(pMonster->Ready_GameObject()))
    {
        Safe_Release(pMonster);
        MSG_BOX("CWorm Create Failed");
        return nullptr;
    }

    return pMonster;
}

CWorm* CWorm::Create(LPDIRECT3DDEVICE9 pGraphicDev, map<const _tchar*, CLayer*>* pmap)
{
    CWorm* pMonster = new CWorm(pGraphicDev);
    pMonster->m_pmapLayer = pmap;
    if (FAILED(pMonster->Ready_GameObject()))
    {
        Safe_Release(pMonster);
        MSG_BOX("CWorm Create Failed");
        return nullptr;
    }
    
    return pMonster;
}


CWorm* CWorm::Create(LPDIRECT3DDEVICE9 pGraphicDev, _uint iIndex)
{
    CWorm* pMonster = new CWorm(pGraphicDev, ++iIndex);
    if (FAILED(pMonster->Ready_GameObject()))
    {
        Safe_Release(pMonster);
        MSG_BOX("CWorm Create Failed");
        return nullptr;
    }

    return pMonster;
}

CWorm* CWorm::Create(LPDIRECT3DDEVICE9 pGraphicDev, _uint iIndex, map<const _tchar*, CLayer*>* pmap)
{
    CWorm* pMonster = new CWorm(pGraphicDev, ++iIndex);
    pMonster->m_pmapLayer = pmap;
    //pMonster->Set
    if (FAILED(pMonster->Ready_GameObject()))
    {
        Safe_Release(pMonster);
        MSG_BOX("CWorm Create Failed");
        return nullptr;
    }

    return pMonster;
}



void CWorm::Free()
{
    CMonster::Free();
}