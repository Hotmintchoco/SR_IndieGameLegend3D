#include "pch.h"
#include "CMagmamouth.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CGameStatusMgr.h"
#include "CTimerMgr.h"
#include "CTerrain.h"
#include "CSpeyeder.h"
#include "CFireball.h"
#include "CTrail.h"
#include "CParticle_Rectangle.h"
#include "CEffect.h"
#include "CParticle_Sphere.h"
#include "CRoomLayer.h"

CMagmamouth::CMagmamouth(LPDIRECT3DDEVICE9 pGraphicDev)
    : CMonster(pGraphicDev), m_fSpawn_CoolDown(0.25f), m_fStateUpdateTime(0.f), m_fStateUpdateDuration(2.f), 
    m_eMagmaMouthState(OPENING), m_vRoomCenterLocation{ 0.f,0.f,0.f }, m_vMovePosition{0.f,0.f,0.f},
    m_iMonsterX(0), m_iMonsterZ(0), m_iPlayerX(0), m_iPlayerZ(0), m_bMoveFlag(false), m_bMoveFlag2(false), m_bCloseMouth(false),
    m_fTrailTime(0.f), m_fTrailTime2(0.f), m_fTrailDuration(0.f), m_bTrailStart(false), m_bTrailFinish(false), m_fSpawnTime(0.f)
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
    
    m_vRoomCenterLocation = { 30.f,0.f,60.f };
    m_pTransformCom->Get_Info(INFO_POS, &m_vMovePosition);
    m_vMovePosition = m_vOpeningMoveDirection[m_iOpeningMoveIndex] + m_vRoomCenterLocation;
    m_vMovePosition.y = 2.f;

    m_fTrailDuration = 0.5f * 0.5f * 0.5f;

    m_iHp = 6;
    m_fFrame = 3.f;
    return S_OK;
}
    
_int CMagmamouth::Update_GameObject(const _float& fTimeDelta)
{
    _int    iExit = CMonster::Update_GameObject(fTimeDelta);

    if (m_iHp <= 0)
    {
        m_eMagmaMouthState = DEAD;
        m_fFrame = 3.f;
        m_bMoveFlag = false;
        m_bMoveFlag2 = false;
        m_pColliderCom->Set_IsActive(false);
    }
    else if (m_iHp < 3)
    {
        m_iPhase = 1;
    }
    Update_Motion(fTimeDelta);
   
    switch (m_eMagmaMouthState)
    {
    case IDLE:
        Set_Motion_CloseOpenMouth(fTimeDelta);
        break;
    case SPAWN:
        Spawn_Speyeder(fTimeDelta);
        break;
    case FIREBALL:
        Throw_Fireball(fTimeDelta);
        break;
    case MOVE:
        Move_Magmamouth(fTimeDelta);
        MagmaMouth_Trail(fTimeDelta);
        break;
    case DEAD:
        MagmaMouth_Dead(fTimeDelta);
        break;
    case OPENING:
        Opening_MagmaMouth(fTimeDelta);
        //MagmaMouth_Trail(fTimeDelta);
        break;
    }

    return iExit;
}

void CMagmamouth::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CMonster::LateUpdate_GameObject(fTimeDelta);
    Set_Motion();
}

void CMagmamouth::Render_GameObject()
{
    if (m_bHitState == true) CMonster::Enable_HitRenderState();
    CMonster::Render_GameObject();
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pTextureCom->Set_Texture((_uint) m_fFrame);
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
    if (m_fFrame < 3.f)
    {
        Set_Motion_OpenMouth(fTimeDelta);
        return;
    }

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
        m_eMagmaMouthState = IDLE;

    }

    if (iFlag != 0)
    {
        CGameObject* pGameObject = CSpeyeder::Create(m_pGraphicDev);
        if (nullptr == pGameObject) return;
        pGameObject->Set_IsActive(true);

        CTransform* pPlayerTransformCom = dynamic_cast<CTransform*>(Engine::CManagement::GetInstance()
            ->Get_Component(ID_DYNAMIC, L"GameLogic_Layer", L"Player", L"Com_Transform"));
        if (nullptr == pPlayerTransformCom) return;
        _vec3   vPlayerPos;
        pPlayerTransformCom->Get_Info(INFO_POS, &vPlayerPos);
        m_pTransformCom->Get_Info(INFO_POS, &vPos);
        vVelocity = vPlayerPos - vPos;
        vVelocity.y = 0.f;
        D3DXVec3Normalize(&vVelocity, &vVelocity);

        _matrix matRot;
        D3DXMatrixRotationY(&matRot, D3DXToRadian(45.f) - D3DXToRadian(30.f) * m_iSpawnOrderArr[iFlag - 1]);

        D3DXVec3TransformNormal(&vVelocity, &vVelocity, &matRot);

        vVelocity *= 4.f;
        vVelocity.y = 3.f;

        static_cast<CMonster*>(pGameObject)->Set_Pos(vPos);
        static_cast<CSpeyeder*>(pGameObject)->Set_Velocity(vVelocity);

        CRoomLayer* pLayer = CGameStatusMgr::GetInstance()->GetCurrentRoomLayer();
        if (FAILED(pLayer->Add_GameObject(L"Speyeder", pGameObject))) return;
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

void CMagmamouth::Opening_MagmaMouth(const _float& fTimeDelta)
{
    m_bElapsedOpeningTime += fTimeDelta;
    if (m_bElapsedOpeningTime > 15.f)
    {
        m_bOpening = false;
    }

    if (m_bElapsedOpeningTime<3.f)
    {
        Set_Motion_CloseOpenMouth(fTimeDelta);
    }
    else if (m_bElapsedOpeningTime < 4.f)
    {
        //Set_Motion_CloseMouth(fTimeDelta);
        Set_Motion_OpenMouth(fTimeDelta);
    }
    else if (m_bElapsedOpeningTime < 12.f)
    {
        _vec3 vPos, vDir;
        m_pTransformCom->Get_Info(INFO_POS, &vPos);
        vDir = m_vOpeningMoveDirection[m_iOpeningMoveIndex];

        if (m_bOpeningMoveFlag == true)
        {
            Set_Motion_CloseMouth(fTimeDelta);
            if (m_fFrame <= 0.f)
            {
                m_bOpening = false;
            }
        }
        else
        {
            _vec3 vDist = vPos - m_vMovePosition;
            if (D3DXVec3Length(&vDist) < 0.1f)
            {
                m_vMovePosition += m_vOpeningMoveDirection[++m_iOpeningMoveIndex];
                
                ////////
                //m_fTrailTime2 = 1.25f;
                ////////

                if (m_iOpeningMoveIndex == sizeof(m_vOpeningMoveDirection) / sizeof(m_vOpeningMoveDirection[0]))
                {
                    m_bOpeningMoveFlag = true;
                    return;
                }
            }
            D3DXVec3Normalize(&vDir, &vDir);
            m_pTransformCom->Move_Pos(&vDir, 10.f, fTimeDelta);
        }

    }
}

void CMagmamouth::Throw_Fireball(const _float& fTimeDelta)
{
    if (m_fFrame < 3.f)
    {
        Set_Motion_OpenMouth(fTimeDelta);
        return;
    }

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
        m_eMagmaMouthState = IDLE;
    }

    if (iFlag != 0)
    {
        CGameObject* pGameObject = CFireball::Create(m_pGraphicDev);
        if (nullptr == pGameObject) return;
        pGameObject->Set_IsActive(true);

        CTransform* pPlayerTransformCom = dynamic_cast<CTransform*>(Engine::CManagement::GetInstance()
            ->Get_Component(ID_DYNAMIC, L"GameLogic_Layer", L"Player", L"Com_Transform"));
        if (nullptr == pPlayerTransformCom) return;
        _vec3   vPlayerPos;
        pPlayerTransformCom->Get_Info(INFO_POS, &vPlayerPos);
        m_pTransformCom->Get_Info(INFO_POS, &vPos);
        vVelocity = vPlayerPos - vPos;
        vVelocity.y = 0.f;
        D3DXVec3Normalize(&vVelocity, &vVelocity);

        _matrix matRot;
        D3DXMatrixRotationY(&matRot, D3DXToRadian(10.f) - D3DXToRadian(10.f) * m_iSpawnOrderArr[iFlag - 1]);

        D3DXVec3TransformNormal(&vVelocity, &vVelocity, &matRot);

        vVelocity *= 6.f;
        vVelocity.y = 4.f;

        static_cast<CMonster*>(pGameObject)->Set_Pos(vPos);
        static_cast<CFireball*>(pGameObject)->Set_Velocity(vVelocity);

        CRoomLayer* pLayer = CGameStatusMgr::GetInstance()->GetCurrentRoomLayer();
        if (FAILED(pLayer->Add_GameObject(L"Fireball", pGameObject))) return;
    }
}

void CMagmamouth::Move_Magmamouth(const _float& fTimeDelta)
{
    if (m_fFrame < 3.f)
    {
        Set_Motion_OpenMouth(fTimeDelta);
        return;
    }
    
    _vec3 vPos, vDir, vDir2;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    vDir = m_vMovePosition - vPos;

    vDir2 = m_vMovePosition - vPos;
    vDir2.y = -0.75f;

    if (D3DXVec3Length(&vDir) < 0.1f && m_bMoveFlag2==false)
    {
        m_pTransformCom->Set_Pos(m_vMovePosition);
        m_bMoveFlag2 = true;
        Find_BackPoint();
        vDir = m_vMovePosition - vPos;
        return;
    }
    else if (D3DXVec3Length(&vDir) < 3.f && m_bMoveFlag2 == false)
    {
        m_bMoveFlag = true;
    }
    else if (m_bMoveFlag2 == true && D3DXVec3Length(&vDir) < 0.1f)
    {
        m_eMagmaMouthState = IDLE;
        return;
    }
    if (m_bMoveFlag2 == false)
    {
        if (m_bMoveFlag == true)
        {
            D3DXVec3Normalize(&vDir, &vDir);
            m_pTransformCom->Move_Pos(&vDir, 6.f, fTimeDelta);
        }
        else
        {
            D3DXVec3Normalize(&vDir2, &vDir2);
            m_pTransformCom->Move_Pos(&vDir2, 12.f, fTimeDelta);
        }
    }
    else
    {
        D3DXVec3Normalize(&vDir, &vDir);
        m_pTransformCom->Move_Pos(&vDir, 3.f, fTimeDelta);
    }
}

void CMagmamouth::Update_Motion(const _float& fTimeDelta)
{
    if (m_eMagmaMouthState == DEAD || m_bOpening == true)return;

    m_fStateUpdateTime += fTimeDelta;

    if (m_fStateUpdateTime > m_fStateUpdateDuration)
    {
        m_fStateUpdateTime = 0.f;
        m_bCloseMouth = false;

        //if (m_iPhase == -1)
        //{
        //    m_eMagmaMouthState = MOVE;
        //    ++m_iPhase;
        //}
        if (m_iPhase == 0)
        {
            m_eMagmaMouthState = static_cast<MAGMAMOUTHSTATE>(rand() % 3);
            if (m_bMoveState == true)
            {
                m_eMagmaMouthState = MOVE;
                m_bMoveState = false;

            }
            //m_bMoveState = !m_bMoveState;
        }
        else
        {
            m_eMagmaMouthState = static_cast<MAGMAMOUTHSTATE>(rand() % 3);
        }
        //m_eMagmaMouthState = SPAWN;
        //m_eMagmaMouthState = FIREBALL;
        //m_eMagmaMouthState = MOVE;
        if (m_eMagmaMouthState == SPAWN)
        {
            Shuffle_Array(4);
            ZeroMemory(m_bSpawnFinish, sizeof(m_bSpawnFinish));
            m_fSpawnTime = 0.f;
            m_fStateUpdateDuration = 2.f;
            m_fSpawn_CoolDown = 0.25f;
        }
        else if (m_eMagmaMouthState == FIREBALL)
        {
            Shuffle_Array(3);
            ZeroMemory(m_bFireballFinish, sizeof(m_bFireballFinish));
            m_fSpawnTime = 0.f;
            m_fStateUpdateDuration = 2.f;
            m_fSpawn_CoolDown = 0.5f;
        }
        else if (m_eMagmaMouthState == MOVE)
        {
            Set_MovePosition();
            m_fStateUpdateDuration = 4.f;
            m_bMoveFlag = false;
            m_bMoveFlag2 = false;
            m_bTrailStart = false;
            m_fTrailTime2 = 0.f;
            m_fFrame = 3.f;
        }
        else if (m_eMagmaMouthState == IDLE)
        {
            m_fFrame = 3.f;
        }
    }
}

void CMagmamouth::Set_MovePosition()
{
    Set_Position();
    if (m_iPlayerX == 1 && m_iPlayerZ == 1)
    {
        if (m_iMonsterX == 2)m_iMonsterX = 0;
        else if (m_iMonsterX == 0)m_iMonsterX = 2;
        if (m_iMonsterZ == 2)m_iMonsterZ = 0;
        else if (m_iMonsterZ == 0)m_iMonsterZ = 2;
    }
    else
    {
        m_iMonsterX = m_iPlayerX;
        m_iMonsterZ = m_iPlayerZ;
    }

    m_vMovePosition.x = m_iMonsterX * MAPX / 3.f - MAPX / 2.f + MAPX / 6.f;
    m_vMovePosition.y = 2.f;
    m_vMovePosition.z = m_iMonsterZ * MAPZ / 3.f - MAPZ / 2.f + MAPZ / 6.f;

    m_vMovePosition += m_vRoomCenterLocation;
}

void CMagmamouth::Set_Position()
{
    _vec3 vMonsterPos, vPlayerPos;
    CTransform* pPlayerTransformCom = dynamic_cast<CTransform*>(Engine::CManagement::GetInstance()
        ->Get_Component(ID_DYNAMIC, L"GameLogic_Layer", L"Player", L"Com_Transform"));
    if (nullptr == pPlayerTransformCom) return;

    pPlayerTransformCom->Get_Info(INFO_POS, &vPlayerPos);
    m_pTransformCom->Get_Info(INFO_POS, &vMonsterPos);

    vMonsterPos -= m_vRoomCenterLocation;
    vPlayerPos -= m_vRoomCenterLocation;

	if (vPlayerPos.x < -MAPX / 2.f || vPlayerPos.x > MAPX / 2.f ||
        vPlayerPos.z < -MAPZ / 2.f || vPlayerPos.z > MAPZ / 2.f)
		return;

	if (vMonsterPos.x < -MAPX / 2.f + MAPX / 3.f)
        m_iMonsterX = 0;
	else if (vMonsterPos.x < -MAPX / 2.f + MAPX / 3.f * 2.f)
        m_iMonsterX = 1;
	else
        m_iMonsterX = 2;

	if (vMonsterPos.z < -MAPZ / 2.f + MAPZ / 3.f)
        m_iMonsterZ = 0;
	else if (vMonsterPos.z < -MAPZ / 2.f + MAPZ / 3.f * 2.f)
        m_iMonsterZ = 1;
	else
        m_iMonsterZ = 2;

    if (vPlayerPos.x < -MAPX / 2.f + MAPX / 3.f)
        m_iPlayerX = 0;
    else if (vPlayerPos.x < -MAPX / 2.f + MAPX / 3.f * 2.f)
        m_iPlayerX = 1;
    else
        m_iPlayerX = 2;

    if (vPlayerPos.z < -MAPZ / 2.f + MAPZ / 3.f)
        m_iPlayerZ = 0;
    else if (vPlayerPos.z < -MAPZ / 2.f + MAPZ / 3.f * 2.f)
        m_iPlayerZ = 1;
    else
        m_iPlayerZ = 2;
    
}

void CMagmamouth::Find_BackPoint()
{
    Set_Position();
    if (m_iMonsterX == 0)
    {
        if (m_iMonsterZ == 0)
        {
            if (m_iPlayerZ == 0)
            {
                ++m_iMonsterZ;
            }
            else
            {
                ++m_iMonsterX;
            }
        }
        else if (m_iMonsterZ == 1)
        {
            if (m_iPlayerZ == 0)
            {
                ++m_iMonsterZ;
            }
            else
            {
                --m_iMonsterZ;
            }
        }
        else
        {
            if (m_iPlayerZ == 2)
            {
                --m_iMonsterZ;
            }
            else
            {
                ++m_iMonsterX;
            }
        }
    }
    else if (m_iMonsterX == 1)
    {
        if (m_iMonsterZ == 0)
        {
            if (m_iPlayerZ == 0)
            {
                if (m_iPlayerX == 2)
                {
                    --m_iMonsterX;
                }
                else
                {
                    ++m_iMonsterX;
                }
            }
            else
            {
                if (m_iPlayerX == 2)
                {
                    --m_iMonsterX;
                }
                else
                {
                    ++m_iMonsterX;
                }

            }
        }
        else if (m_iMonsterZ == 1)
        {
            if (m_iPlayerX == 1 && m_iPlayerZ == 1)
            {
                --m_iMonsterX;
                --m_iMonsterZ;
            }
            else
            {
                if (m_iPlayerX == 2)m_iMonsterX = 0;
                else if (m_iPlayerX == 0)m_iMonsterX = 2;
                if (m_iPlayerZ == 2)m_iMonsterZ = 0;
                else if (m_iPlayerZ == 0)m_iMonsterZ = 2;
            }
        }
        else
        {
            if (m_iPlayerZ == 2)
            {
                if (m_iPlayerX == 2)
                {
                    --m_iMonsterX;
                }
                else
                {
                    ++m_iMonsterX;
                }
            }
            else
            {
                if (m_iPlayerX == 2)
                {
                    --m_iMonsterX;
                }
                else
                {
                    ++m_iMonsterX;
                }

            }
        }
    }
    else
    {
        if (m_iMonsterZ == 0)
        {
            if (m_iPlayerZ == 0)
            {
                ++m_iMonsterZ;
            }
            else
            {
                --m_iMonsterX;
            }
        }
        else if (m_iMonsterZ == 1)
        {
            if (m_iPlayerZ == 0)
            {
                ++m_iMonsterZ;
            }
            else
            {
                --m_iMonsterZ;
            }
        }
        else
        {
            if (m_iPlayerZ == 2)
            {
                --m_iMonsterZ;
            }
            else
            {
                --m_iMonsterX;
            }
        }
    }

    m_vMovePosition.x = m_iMonsterX * MAPX / 3.f - MAPX / 2.f + MAPX / 6.f;
    m_vMovePosition.y = 2.f;
    m_vMovePosition.z = m_iMonsterZ * MAPZ / 3.f - MAPZ / 2.f + MAPZ / 6.f;

    m_vMovePosition += m_vRoomCenterLocation;
}

void CMagmamouth::Set_Motion()
{
    CTransform* pPlayerTransformCom = dynamic_cast<CTransform*>(Engine::CManagement::GetInstance()
        ->Get_Component(ID_DYNAMIC, L"GameLogic_Layer", L"Player", L"Com_Transform"));

    if (nullptr == pPlayerTransformCom)
        return;

    _vec3   vPlayerPos;
    pPlayerTransformCom->Get_Info(INFO_POS, &vPlayerPos);

    _vec3   vPlayerLook;
    pPlayerTransformCom->Get_Info(INFO_LOOK, &vPlayerLook);

    if (m_eMagmaMouthState == MOVE)
    {
        _vec3 vMonsterLook, vPlayerLook,vPlayerPos, vPos;

        CTransform* pPlayerTransformCom = dynamic_cast<CTransform*>(Engine::CManagement::GetInstance()
            ->Get_Component(ID_DYNAMIC, L"GameLogic_Layer", L"Player", L"Com_Transform"));

        if (nullptr == pPlayerTransformCom)
            return;

        pPlayerTransformCom->Get_Info(INFO_LOOK, &vPlayerLook);
        pPlayerTransformCom->Get_Info(INFO_POS, &vPlayerPos);
        m_pTransformCom->Get_Info(INFO_POS, &vPos);

        vMonsterLook = m_vMovePosition - vPos;
        
        vPlayerLook.y = 0;
        vMonsterLook.y = 0;

        _vec3 vAxis = { 0.f, 1.f, 0.f };
        _vec3 vCross;

        float fAngle = acosf(D3DXVec3Dot(D3DXVec3Normalize(&vPlayerLook, &vPlayerLook), D3DXVec3Normalize(&vMonsterLook, &vMonsterLook)));

        if (D3DXToDegree(fAngle) > 135.f)
        {
            m_fFrame = 3.f;
            m_pTransformCom->LookAt_Player(&vPlayerPos, &vPlayerLook);

        }
        else if (D3DXToDegree(fAngle) > 45.f)
        {
            _vec3 vecA = vPlayerPos - vPos;
            m_pTransformCom->LookAt_Player(&vPlayerPos, &vPlayerLook);
            if (D3DXVec3Dot(D3DXVec3Cross(&vCross, &vecA, &vMonsterLook), &vAxis) > 0.f)
                m_fFrame = 6.f;
            else
                m_fFrame = 4.f;
        }
        else
        {
            m_fFrame = 5.f;
            m_pTransformCom->LookAt_Player(&vPlayerPos, &vPlayerLook);

        }
    }
    else
    {
        m_pTransformCom->LookAt_Player(&vPlayerPos, &vPlayerLook);
    }
}

void CMagmamouth::Set_Motion_OpenMouth(const _float& fTimeDelta)
{
    m_fFrame += fTimeDelta * 6.f;
    if (m_fFrame > 4.f)
        m_fFrame = 3.f;
}

void CMagmamouth::Set_Motion_CloseMouth(const _float& fTimeDelta)
{
    m_fFrame -= fTimeDelta * 6.f;
    if (m_fFrame < 0.f)
    {
        m_fFrame = 0.f;
        m_bCloseMouth = true;
    }
}

void CMagmamouth::Set_Motion_CloseOpenMouth(const _float& fTimeDelta)
{
    if (m_bCloseMouth == false)
    {
        m_fFrame -= fTimeDelta * 6.f;

        if (m_fFrame < 0.f)
        {
            m_fFrame = 0.f;
            m_bCloseMouth = true;
        }
        else if (m_fFrame > 4.f)
        {
            m_fFrame = 4.f - 0.5f * 0.5f * 0.5f;
        }

    }
    else
    {
        m_fFrame += fTimeDelta * 4.f;
        if (m_fFrame > 4.f)
        {
            m_fFrame = 3.f;
            m_bCloseMouth = false;
        }
    }
}

void CMagmamouth::MagmaMouth_Trail(const _float& fTimeDelta)
{
    m_fTrailTime2 += fTimeDelta;

	if (m_fTrailTime2>1.5f || m_bMoveFlag2 == true)return;
    _vec3 vPos, vUp, vDown;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    vUp = vPos;
    vDown = vPos;
    
    _int Upx = rand() % 128 - 64;
    _int Upy = rand() % 128 - 64;
    _int Upz = rand() % 128 - 64;

    _int Downx = rand() % 128 - 64;
    _int Downy = rand() % 128 - 64;
    _int Downz = rand() % 128 - 64;

    vUp.x += (_float)Upx / 1024.f;
    vUp.y += (0.75f + (_float)Upy / 1024.f);
    vUp.z += (_float)Upz / 1024.f;

    vDown.x += (_float)Downx / 1024.f;
    vDown.y -= (0.75f + (_float)Downy / 1024.f);
    vDown.z += (_float)Downz / 1024.f;



    if (m_bTrailStart == false)
    {
        m_fTrailTime = 0.f;
        m_bTrailStart = true;

        m_fTrailPoint[0] = vUp;
        m_fTrailPoint[3] = vDown;
    }

    m_fTrailTime += fTimeDelta;
    if (m_fTrailTime > m_fTrailDuration)
    {
        m_fTrailTime = 0.f;

        m_fTrailPoint[1] = vUp;
        m_fTrailPoint[2] = vDown;

        CGameObject* pGameObject = nullptr;
        CLayer* pLayer = CManagement::GetInstance()->Get_Layer(L"GameLogic_Layer");

		D3DXCOLOR eColor[4];
		for (int i = 0; i < 4; ++i)
		{
            eColor[i] = { 1.f, 1.f, 1.f, 0.25f };
		}

        _float fLifeTime = 0.75f - m_fTrailTime2 * 0.5f;
        if (fLifeTime < 0.f) fLifeTime = 0.f;
        
        pGameObject = CTrail::Create(m_pGraphicDev, m_fTrailPoint, eColor, fLifeTime);

        if (nullptr == pGameObject)
            return;

        if (FAILED(pLayer->Add_GameObject(L"MagmaMouthTrail", pGameObject)))
            return;

        m_fTrailPoint[0] = m_fTrailPoint[1];
        m_fTrailPoint[3] = m_fTrailPoint[2];

        return;
    }
}

void CMagmamouth::MagmaMouth_Dead(const _float& fTimeDelta)
{
    MagmaMouth_Dead_Effect();

    m_fElapsedDeadTime += fTimeDelta;
    m_fElapsedDeadTime2 += fTimeDelta;

    if (m_fElapsedDeadTime > m_fDeadTime)
    {
        m_bDelete = true;
    }
    if (m_fElapsedDeadTime2 > 0.5f)
    {
        m_fElapsedDeadTime2 = 0.f;
		m_bHitState = !m_bHitState;
        m_fHitEffectElapsedTime = 0.f;
    }
    //if (m_bMoveFlag == false)
    //{
    //    if (m_bMoveFlag2 == false)
    //    {
    //        m_bMoveFlag2 = true;
    //        _vec3 vPos;
    //        m_pTransformCom->Get_Info(INFO_POS, &vPos);
    //        int iRandx = rand() % 128 - 64;
    //        int iRandy = rand() % 128 - 64;
    //        int iRandz = rand() % 128 - 64;
    //        _vec3 vRandPos = { iRandx / 64.f * 64.f, iRandy / 64.f * 64.f, iRandz / 64.f * 64.f };
    //        m_vMovePosition = vPos + vRandPos;
    //    }
    //}
    //else
    //{

    //}
    for (int j = 0; j < 3; ++j)
    {
        if (m_fElapsedDeadTime > m_fDeadTime - 1.25f + 0.5f * j && m_DeadFireball[j] == false)
        {
            m_DeadFireball[j] = true;
            _vec3 vPos;
            _matrix matRot;
            _float fDegreeInterval = 30.f;
            _float fVelocityY;
            fVelocityY = 2.f +4.f * j;

            m_pTransformCom->Get_Info(INFO_POS, &vPos);

            for (int i = 0; i < 360.f / fDegreeInterval; ++i)
            {
                CGameObject* pGameObject = CFireball::Create(m_pGraphicDev);
                if (nullptr == pGameObject) return;
                pGameObject->Set_IsActive(true);

                _vec3 vVelocity = { 0.f,0.f,1.f };
                D3DXMatrixRotationY(&matRot, D3DXToRadian(fDegreeInterval) * i);
                D3DXVec3TransformNormal(&vVelocity, &vVelocity, &matRot);
                vVelocity *= 4.f - 0.5f * j;
                vVelocity.y = fVelocityY;

                static_cast<CMonster*>(pGameObject)->Set_Pos(vPos);
                static_cast<CFireball*>(pGameObject)->Set_Velocity(vVelocity);

                CRoomLayer* pLayer = CGameStatusMgr::GetInstance()->GetCurrentRoomLayer();
                if (FAILED(pLayer->Add_GameObject(L"Fireball", pGameObject))) return;
            }
        }
    }
}

void CMagmamouth::MagmaMouth_Dead_Effect()
{
    if (m_bDead_Effect1 == false)
    {
        m_bDead_Effect1 = true;
        _vec3 vPos;
        m_pTransformCom->Get_Info(INFO_POS, &vPos);
        CLayer* pLayer = CManagement::GetInstance()->Get_Layer(L"GameLogic_Layer");
        CGameObject* pGameObject = nullptr;

        pGameObject = CEffect::Create(m_pGraphicDev, CEffect::MAGMA_DEAD_EFFECT, vPos);
        if (nullptr == pGameObject) return;
        if (FAILED(pLayer->Add_GameObject(L"Effect_Magma_Dead", pGameObject))) return;
    }

    if (m_bDead_Effect2 == false)
    {
        m_bDead_Effect2 = true;
        _vec3 vPos;
        m_pTransformCom->Get_Info(INFO_POS, &vPos);
        CLayer* pLayer = CManagement::GetInstance()->Get_Layer(L"GameLogic_Layer");
        CGameObject* pGameObject = nullptr;

        pGameObject = CEffect::Create(m_pGraphicDev, CEffect::MAGMA_EXPLOSION1, vPos);
        if (nullptr == pGameObject) return;
        if (FAILED(pLayer->Add_GameObject(L"Effect_Magma_Explosion1", pGameObject))) return;
    }

    if (m_fElapsedDeadTime > m_fDeadTime)
    {
        _vec3 vPos;
        m_pTransformCom->Get_Info(INFO_POS, &vPos);
        CLayer* pLayer = CManagement::GetInstance()->Get_Layer(L"GameLogic_Layer");
        CGameObject* pGameObject = nullptr;

        pGameObject = CEffect::Create(m_pGraphicDev, CEffect::MAGMA_EXPLOSION2, vPos);
        if (nullptr == pGameObject) return;
        if (FAILED(pLayer->Add_GameObject(L"Effect_Magma_Explosion2", pGameObject))) return;
    }
}

void CMagmamouth::Free()
{
    CMonster::Free();
}