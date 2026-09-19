#include "pch.h"
#include "CMagmamouth.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CTimerMgr.h"
#include "CTerrain.h"
#include "CSpeyeder.h"
#include "CFireball.h"

CMagmamouth::CMagmamouth(LPDIRECT3DDEVICE9 pGraphicDev)
    : CMonster(pGraphicDev), m_fSpawn_CoolDown(0.25f), m_fStateUpdateTime(0.f), m_fStateUpdateDuration(2.f), m_eMagmaMouthState(IDLE)
{
    ZeroMemory(m_bSpawnFinish, sizeof(m_bSpawnFinish));
    ZeroMemory(m_bFireballFinish, sizeof(m_bFireballFinish));
    for (int i = 0; i < 4; ++i)
    {
        m_iSpawnOrderArr[i] = i;
    }
}


CMagmamouth::~CMagmamouth()
{
}

HRESULT CMagmamouth::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;
    CMonster::Ready_GameObject();

    m_pTransformCom->Set_Scale(1.f, 1.f, 1.f);
    m_pColliderCom->Set_Radius(m_pTransformCom->m_vScale.x);
    //m_pColliderCom->Set_Radius(0.2f);
    m_iHp = 6;
    return S_OK;
}
    
_int CMagmamouth::Update_GameObject(const _float& fTimeDelta)
{
    _int    iExit = CMonster::Update_GameObject(fTimeDelta);

    CTransform* pPlayerTransformCom = dynamic_cast<CTransform*>(Engine::CManagement::GetInstance()
        ->Get_Component(ID_DYNAMIC, L"GameLogic_Layer", L"Player", L"Com_Transform"));

    if (nullptr == pPlayerTransformCom)
        return 0;

    _vec3   vPlayerPos;
    pPlayerTransformCom->Get_Info(INFO_POS, &vPlayerPos);

    _vec3   vPlayerLook;
    pPlayerTransformCom->Get_Info(INFO_LOOK, &vPlayerLook);

    m_pTransformCom->LookAt_Player(&vPlayerPos, &vPlayerLook);

    m_fStateUpdateTime += fTimeDelta;

    if (m_fStateUpdateTime > m_fStateUpdateDuration)
    {
        m_fStateUpdateTime = 0.f;
        m_eMagmaMouthState = static_cast<MAGMAMOUTHSTATE>(rand() % 2);
        //m_eMagmaMouthState = SPAWN;
        m_eMagmaMouthState = FIREBALL;
        if (m_eMagmaMouthState == SPAWN)
        {
            Shuffle_Array(4);
            ZeroMemory(m_bSpawnFinish, sizeof(m_bSpawnFinish));
            m_fSpawnTime = 0.f;
            m_fStateUpdateDuration = 5.f;
            m_fSpawn_CoolDown = 0.25f;
        }
        else if (m_eMagmaMouthState == FIREBALL)
        {
            Shuffle_Array(3);
            ZeroMemory(m_bFireballFinish, sizeof(m_bFireballFinish));
            m_fSpawnTime = 0.f;
            m_fStateUpdateDuration = 5.f;
            m_fSpawn_CoolDown = 0.5f;

        }
    }
    switch (m_eMagmaMouthState)
    {
    case IDLE:
        break;
    case SPAWN:
        Spawn_Speyeder(fTimeDelta);
        break;
    case FIREBALL:
        Throw_Fireball(fTimeDelta);
        break;
    case MOVE:
        break;
    }
    //m_fFrame += fTimeDelta * 6.f;
    //if (m_fFrame > 4.f)
    //    m_fFrame = 0.f;





    return iExit;
}

void CMagmamouth::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CMonster::LateUpdate_GameObject(fTimeDelta);
}

void CMagmamouth::Render_GameObject()
{
    if (m_bHitState == true) CMonster::Enable_HitRenderState();
    CMonster::Render_GameObject();
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pTextureCom->Set_Texture(0);
    m_pBufferCom->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
    if (m_bHitState == true) CMonster::Disable_HitRenderState();
}

void CMagmamouth::OnCollisionEnter(CGameObject* pOther)
{
    CMonster::OnCollisionEnter(pOther);
}

HRESULT CMagmamouth::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_magmamouthTexture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}


CMagmamouth* CMagmamouth::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CMagmamouth* pMonster = new CMagmamouth(pGraphicDev);

    if (FAILED(pMonster->Ready_GameObject()))
    {
        Safe_Release(pMonster);
        MSG_BOX("CMagmamouth Create Failed");
        return nullptr;
    }

    return pMonster;
}

void CMagmamouth::Spawn_Speyeder(const _float& fTimeDelta)
{
    m_fSpawnTime += fTimeDelta;
    _vec3 vPos, vVelocity;
    _int iFlag = 0;
    if (m_fSpawnTime> m_fSpawn_CoolDown && m_bSpawnFinish[0] == false)
    {
        m_bSpawnFinish[0] = true;
        iFlag = 1;
    }
    else if (m_fSpawnTime > m_fSpawn_CoolDown * 2 && m_bSpawnFinish[1] == false)
    {
        m_bSpawnFinish[1] = true;
        iFlag = 2;
    }
    else if (m_fSpawnTime > m_fSpawn_CoolDown * 3 && m_bSpawnFinish[2] == false)
    {
        m_bSpawnFinish[2] = true;
        iFlag = 3;
    }
    else if (m_fSpawnTime > m_fSpawn_CoolDown * 4 && m_bSpawnFinish[3] == false)
    {
        m_bSpawnFinish[3] = true;
        iFlag = 4;
    }

    if (iFlag != 0)
    {
        CGameObject* pGameObject = CSpeyeder::Create(m_pGraphicDev);

        m_pTransformCom->Get_Info(INFO_POS, &vPos);
        m_pTransformCom->Get_Info(INFO_LOOK, &vVelocity);
        vVelocity.y = 0.f;
        D3DXVec3Normalize(&vVelocity, &vVelocity);

        _matrix matRot;
        D3DXMatrixRotationY(&matRot, D3DXToRadian(60.f) - D3DXToRadian(40.f) * m_iSpawnOrderArr[iFlag - 1]);

        D3DXVec3TransformNormal(&vVelocity, &vVelocity, &matRot);

        vVelocity *= 2.f;
        vVelocity.y = 3.f;

        static_cast<CMonster*>(pGameObject)->Set_Pos(vPos);
        static_cast<CSpeyeder*>(pGameObject)->Set_Velocity(vVelocity);
        if (nullptr == pGameObject)
            return;
        CLayer* pLayer = CManagement::GetInstance()->Get_Layer(L"GameLogic_Layer");
        if (FAILED(pLayer->Add_GameObject(L"Speyeder", pGameObject)))
            return;
    }
}

void CMagmamouth::Shuffle_Array(_uint N)
{
    for (int i = 0; i < (int)N; ++i)
    {
        m_iSpawnOrderArr[i] = i;
    }

	for (int i = N-1; i > 0; --i)
	{
		int j = rand() % (i + 1);
		int temp = m_iSpawnOrderArr[i];
        m_iSpawnOrderArr[i] = m_iSpawnOrderArr[j];
        m_iSpawnOrderArr[j] = temp;
	}
}

void CMagmamouth::Throw_Fireball(const _float& fTimeDelta)
{
    m_fSpawnTime += fTimeDelta;
    _vec3 vPos, vVelocity;
    _int iFlag = 0;
    if (m_fSpawnTime > m_fSpawn_CoolDown && m_bFireballFinish[0] == false)
    {
        m_bFireballFinish[0] = true;
        iFlag = 1;
    }
    else if (m_fSpawnTime > m_fSpawn_CoolDown * 2 && m_bFireballFinish[1] == false)
    {
        m_bFireballFinish[1] = true;
        iFlag = 2;
    }
    else if (m_fSpawnTime > m_fSpawn_CoolDown * 3 && m_bFireballFinish[2] == false)
    {
        m_bFireballFinish[2] = true;
        iFlag = 3;
    }

    if (iFlag != 0)
    {
        CGameObject* pGameObject = CFireball::Create(m_pGraphicDev);

        //_uint x = rand() % 100;
        //_uint y = rand() % 100;
        CTransform* pPlayerTransformCom = dynamic_cast<CTransform*>(Engine::CManagement::GetInstance()
            ->Get_Component(ID_DYNAMIC, L"GameLogic_Layer", L"Player", L"Com_Transform"));
        if (nullptr == pPlayerTransformCom) return;
        _vec3   vPlayerPos;
        pPlayerTransformCom->Get_Info(INFO_POS, &vPlayerPos);
        m_pTransformCom->Get_Info(INFO_POS, &vPos);
        vVelocity = vPlayerPos - vPos;
        //m_pTransformCom->Get_Info(INFO_LOOK, &vLook);
        vVelocity.y = 0.f;
        D3DXVec3Normalize(&vVelocity, &vVelocity);

        _matrix matRot;
        D3DXMatrixRotationY(&matRot, D3DXToRadian(30.f) - D3DXToRadian(30.f) * m_iSpawnOrderArr[iFlag - 1]);

        D3DXVec3TransformNormal(&vVelocity, &vVelocity, &matRot);

        vVelocity *= 3.f;
        vVelocity.y = 4.f;

        static_cast<CMonster*>(pGameObject)->Set_Pos(vPos);
        static_cast<CFireball*>(pGameObject)->Set_Velocity(vVelocity);
        if (nullptr == pGameObject)
            return;
        CLayer* pLayer = CManagement::GetInstance()->Get_Layer(L"GameLogic_Layer");
        if (FAILED(pLayer->Add_GameObject(L"Speyeder", pGameObject)))
            return;
    }

}

void CMagmamouth::Free()
{
    CMonster::Free();
}