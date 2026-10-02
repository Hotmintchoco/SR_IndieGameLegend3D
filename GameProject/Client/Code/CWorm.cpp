#include "pch.h"
#include "CWorm.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CTimerMgr.h"
#include "CTerrain.h"
#include "CRoomLayer.h"
#include "CEffect.h"
#include "CGlubba.h"
#include "CShockwave.h"

CWorm::CWorm(LPDIRECT3DDEVICE9 pGraphicDev)
    : CMonster(pGraphicDev)
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

    if (m_iWormIndex < 10)
    {
        CGameObject* pGameObject;
        CScene* pScene = CManagement::GetInstance()->GetCurrentScene();

        pGameObject = CWorm::Create(m_pGraphicDev, m_iWormIndex + 1);
        if (nullptr == pGameObject) return E_FAIL;

		TCHAR		szFileName[128] = L"";
		wsprintf(szFileName, L"Worm_%d", m_iWormIndex + 1);
		if (FAILED(pScene->Add_GameObject(szFileName, pGameObject))) return E_FAIL;

        _vec3 vPos;
        m_pTransformCom->Get_Info(INFO_POS, &vPos);

    }
    else
    {
        return S_OK;
    }


    m_pTransformCom->Set_Scale(1.f, 1.f, 1.f);

    m_vRoomCenterLocation = static_cast<CRoomLayer*>(m_pOwner)->GetCenterPos();

    m_pColliderCom->Set_Radius(m_pTransformCom->m_vScale.x);

    m_iMaxHp = 1;
    m_iHp = m_iMaxHp;
    m_bCollision_WithMonster = false;
    return S_OK;
}

_int CWorm::Update_GameObject(const _float& fTimeDelta)
{
    _float _fTimeDelta = fTimeDelta;
    if (m_iHp <= 0)
    {
        m_eWormState = DEAD;

        m_bMoveFlag = false;
        m_bMoveFlag2 = false;
        m_pColliderCom->Set_IsActive(false);
        if (m_bDeadStart == false)
        {
            m_bDeadStart = true;
        }
    }
    else if (m_iHp <= m_iMaxHp / 2)
    {
        m_iPhase = 1;
        //_fTimeDelta *= 1.5f;
    }

    m_fFrame += fTimeDelta * 4.f;
    if (m_fFrame > 4.f)
        m_fFrame = 0.f;


    _int    iExit = CMonster::Update_GameObject(_fTimeDelta);



    Update_Motion(_fTimeDelta);

    switch (m_eWormState)
    {
    case IDLE:
        IDLE_Worm(_fTimeDelta);
        break;
    case SPAWN:
        Spawn_Monster(_fTimeDelta);
        break;
    case MOVE:
        Move_Worm(_fTimeDelta);
        break;
    case DEAD:
        Worm_Dead(_fTimeDelta);
        break;
    case OPENING:
        Opening_Worm(_fTimeDelta);
        break;
    }



    return iExit;
}

void CWorm::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CMonster::LateUpdate_GameObject(fTimeDelta);
}

void CWorm::Render_GameObject()
{
    if (m_bHitState == true) CMonster::Enable_HitRenderState();

    CMonster::Render_GameObject();

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
    m_pTextureCom->Set_Texture((_uint)m_fFrame);
    m_pBufferCom->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

    if (m_bHitState == true) CMonster::Disable_HitRenderState();

}

void CWorm::OnCollisionEnter(COLLINFO eCollInfo)
{
    CMonster::OnCollisionEnter(eCollInfo);
}

HRESULT CWorm::Add_Component()
{
	CComponent* pComponent = nullptr;
    if (m_iWormIndex == 1)
    {
        pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_worm_drillTexture"));
    }
    else if (m_iWormIndex == 10)
    {
        pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_worm_tailTexture"));
    }
    else
    {
        pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_worm_bobyTexture"));
    }

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

CWorm* CWorm::Create(LPDIRECT3DDEVICE9 pGraphicDev, _uint iIndex)
{
	CWorm* pMonster = new CWorm(pGraphicDev);
	pMonster->Set_WormIndex(iIndex);

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

void CWorm::Update_Motion(const _float& fTimeDelta)
{
    if (m_eWormState == DEAD || m_bOpening == true)return;

    m_fStateUpdateTime += fTimeDelta;

    if (m_fStateUpdateTime > m_fStateUpdateDuration)
    {
        m_fStateUpdateTime = 0.f;

        if (m_iPhase == 0)
        {
            m_eWormState = static_cast<WormSTATE>(rand() % 2);
            if (m_bMoveState == true)
            {
                m_eWormState = MOVE;
                m_bMoveState = false;
            }
        }
        else
        {
            m_eWormState = static_cast<WormSTATE>(rand() % 2);
        }
        //m_eWormState = SPAWN;
        m_eWormState = MOVE;
        if (m_eWormState == SPAWN)
        {
            Shuffle_Array(4);
            ZeroMemory(m_bSpawnFinish, sizeof(m_bSpawnFinish));
            ZeroMemory(m_bSpawnFinish2, sizeof(m_bSpawnFinish2));
            ZeroMemory(m_fSpawnStartTime, sizeof(m_fSpawnStartTime));
            m_fSpawnTime = 0.f;
            m_fStateUpdateDuration = 6.f;
            m_fSpawn_CoolDown = 1.f;

            m_iLandingCount = 0;
            m_bMoveFlag = false;
            m_bMoveFlag2 = false;

        }
        else if (m_eWormState == MOVE)
        {
            //Set_MovePosition();
            m_fStateUpdateDuration = 5.f;
            m_bMoveFlag = false;
            m_bMoveFlag2 = false;
            //m_bTrailStart = false;
            //m_fTrailTime2 = 0.f;

            m_iLandingCount = 0;
        }
        else if (m_eWormState == IDLE)
        {
            m_fStateUpdateDuration = 2.f;
        }
    }
}

void CWorm::Move_Worm(const _float& fTimeDelta)
{

    _vec3 vPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);

    CTransform* pPlayerTransformCom = dynamic_cast<CTransform*>(Engine::CManagement::GetInstance()
        ->Get_Component(ID_DYNAMIC, L"GameLogic_Layer", L"Player", L"Com_Transform"));

    if (nullptr == pPlayerTransformCom)
        return;

    _vec3   vPlayerPos;
    pPlayerTransformCom->Get_Info(INFO_POS, &vPlayerPos);

    _vec3   vPlayerLook;
    pPlayerTransformCom->Get_Info(INFO_LOOK, &vPlayerLook);



    if (m_bMoveFlag == false)
    {
        if (m_iLandingCount == 0 || m_iLandingCount == 1)
        {

            vPlayerLook.y = 0;
            D3DXVec3Normalize(&vPlayerLook, &vPlayerLook);
            m_vLandingDirection = vPlayerLook;
            m_vLandingDirection.y = 3.f;
            m_vLandingDirection.x /= 2.f;
            m_vLandingDirection.z /= 2.f;
            m_fLandingTime += fTimeDelta;
            m_vLandingDirection.y -= m_fLandingTime * 9.8f;

            _vec3 vDest = vPos + m_vLandingDirection * 3.f * 0.7f;
            _float fBlank = m_pTransformCom->m_vScale.y;
            if (vDest.x > m_vRoomCenterLocation.x + 6.5f - fBlank ||
                vDest.x < m_vRoomCenterLocation.x - 6.5f + fBlank ||
                vDest.z > m_vRoomCenterLocation.z + 5.0f - fBlank ||
                vDest.z < m_vRoomCenterLocation.z - 5.0f + fBlank)
            {
                _vec3 vVerticalDirection = { 0.f,m_vLandingDirection.y,0.f };
                m_pTransformCom->Move_Pos(&vVerticalDirection, 3.f, fTimeDelta);
            }
            else
            {
                m_pTransformCom->Move_Pos(&m_vLandingDirection, 3.f, fTimeDelta);
            }


            if (vPos.y <= m_pTransformCom->m_vScale.y && m_fLandingTime > 0.5f)
            {
                m_fLandingTime = 0.f;
                ++m_iLandingCount;

                //_vec3 vPlayerPos;
                pPlayerTransformCom->Get_Info(INFO_POS, &m_vMovePosition);

                _float fBlank = m_pTransformCom->m_vScale.y;
                if (m_vMovePosition.x > m_vRoomCenterLocation.x + 6.5f - fBlank)
                {
                    m_vMovePosition.x = m_vRoomCenterLocation.x + 6.5f - fBlank;
                }
                if (m_vMovePosition.x < m_vRoomCenterLocation.x - 6.5f + fBlank)
                {
                    m_vMovePosition.x = m_vRoomCenterLocation.x - 6.5f + fBlank;
                }
                if (m_vMovePosition.z > m_vRoomCenterLocation.z + 5.0f - fBlank)
                {
                    m_vMovePosition.z = m_vRoomCenterLocation.z + 5.0f - fBlank;
                }
                if (m_vMovePosition.z < m_vRoomCenterLocation.z - 5.0f + fBlank)
                {
                    m_vMovePosition.z = m_vRoomCenterLocation.z - 5.0f + fBlank;
                }


                m_vMovePosition.y = m_pTransformCom->m_vScale.y;
            }
            Look_AtPlayer();
        }
        else
        {
            Set_OnTerrain();


            _vec3 vec3 = vPos - m_vMovePosition;

            if (D3DXVec3Length(&vec3) < 0.1f)
            {
                m_bMoveFlag = true;
            }
            else
            {
                _vec3 vDir = m_vMovePosition - vPos;
                D3DXVec3Normalize(&vDir, &vDir);
                m_pTransformCom->Move_Pos(&vDir, 10.f, fTimeDelta);
            }
            Look_AtPlayer();
        }
    }
    else
    {
        Set_OnTerrain();
        Chase_Player_Worm(fTimeDelta);
    }



}

void CWorm::Spawn_Monster(const _float& fTimeDelta)
{
    if (m_bMoveFlag == false)
    {
        _vec3 vPos;
        m_pTransformCom->Get_Info(INFO_POS, &vPos);

        CTransform* pPlayerTransformCom = dynamic_cast<CTransform*>(Engine::CManagement::GetInstance()
            ->Get_Component(ID_DYNAMIC, L"GameLogic_Layer", L"Player", L"Com_Transform"));
        if (nullptr == pPlayerTransformCom) return;

        _vec3   vPlayerPos;
        pPlayerTransformCom->Get_Info(INFO_POS, &vPlayerPos);
        _vec3   vPlayerLook;
        pPlayerTransformCom->Get_Info(INFO_LOOK, &vPlayerLook);

        if (m_iLandingCount == 0 || m_iLandingCount == 1)
        {
            vPlayerLook.y = 0;
            D3DXVec3Normalize(&vPlayerLook, &vPlayerLook);
            m_vLandingDirection = vPlayerLook;
            m_vLandingDirection.y = 3.f;
            m_vLandingDirection.x /= 2.f;
            m_vLandingDirection.z /= 2.f;
            m_fLandingTime += fTimeDelta;
            m_vLandingDirection.y -= m_fLandingTime * 9.8f;

            _vec3 vDest = vPos + m_vLandingDirection * 3.f * 0.7f;
            _float fBlank = m_pTransformCom->m_vScale.y;
            if (vDest.x > m_vRoomCenterLocation.x + 6.5f - fBlank ||
                vDest.x < m_vRoomCenterLocation.x - 6.5f + fBlank ||
                vDest.z > m_vRoomCenterLocation.z + 5.0f - fBlank ||
                vDest.z < m_vRoomCenterLocation.z - 5.0f + fBlank)
            {
                _vec3 vVerticalDirection = { 0.f,m_vLandingDirection.y,0.f };
                m_pTransformCom->Move_Pos(&vVerticalDirection, 3.f, fTimeDelta);
            }
            else
            {
                m_pTransformCom->Move_Pos(&m_vLandingDirection, 3.f, fTimeDelta);
            }


            if (vPos.y <= m_pTransformCom->m_vScale.y && m_fLandingTime > 0.5f)
            {
                m_fLandingTime = 0.f;
                ++m_iLandingCount;
                if (m_iLandingCount == 2)
                    m_bMoveFlag = true;
            }
            Look_AtPlayer();
        }
    }

    else
    {
        Set_OnTerrain();
        m_fSpawnTime += fTimeDelta;
        _vec3 vPos, vVelocity;
        _int iFlag = 0;
        CGameObject* pGameObject = nullptr;

        if (m_fSpawnTime > m_fSpawn_CoolDown && m_bSpawnFinish[0] == false)
        {
            m_bSpawnFinish[0] = true;
            iFlag = 1;

            if (m_iPhase == 0)
            {
                pGameObject = CGlubba::Create(m_pGraphicDev, 0.75f);
            }
            else
            {
                pGameObject = CGlubba::Create(m_pGraphicDev, 0.75f);
            }
            if (nullptr == pGameObject) return;
            m_pSpawnMonster[iFlag - 1] = pGameObject;
        }
        else if (m_fSpawnTime > m_fSpawn_CoolDown * 2 && m_bSpawnFinish[1] == false)
        {
            m_bSpawnFinish[1] = true;
            iFlag = 2;

            if (m_iPhase == 0)
            {
                pGameObject = CGlubba::Create(m_pGraphicDev, 0.75f);
            }
            else
            {
                pGameObject = CGlubba::Create(m_pGraphicDev, 0.75f);
            }
            if (nullptr == pGameObject) return;
            m_pSpawnMonster[iFlag - 1] = pGameObject;
        }
        else if (m_fSpawnTime > m_fSpawn_CoolDown * 3 && m_bSpawnFinish[2] == false)
        {
            m_bSpawnFinish[2] = true;
            iFlag = 3;

            if (m_iPhase == 0)
            {
                pGameObject = CGlubba::Create(m_pGraphicDev, 0.75f);
            }
            else
            {
                pGameObject = CGlubba::Create(m_pGraphicDev, 0.75f);
            }
            if (nullptr == pGameObject) return;
            m_pSpawnMonster[iFlag - 1] = pGameObject;
        }
        else if (m_fSpawnTime > m_fSpawn_CoolDown * 4 && m_bSpawnFinish[3] == false)
        {
            m_bSpawnFinish[3] = true;
            iFlag = 4;

            if (m_iPhase == 0)
            {
                pGameObject = CGlubba::Create(m_pGraphicDev, 0.75f);
            }
            else
            {
                pGameObject = CGlubba::Create(m_pGraphicDev, 0.75f);
            }
            if (nullptr == pGameObject) return;

            m_pSpawnMonster[iFlag - 1] = pGameObject;
        }

        _float fSpawnLatency = 0.5f;
        if (m_fSpawnTime > m_fSpawn_CoolDown && m_bSpawnFinish2[0] == false)
        {
            m_fSpawnStartTime[0] += fTimeDelta;
            if (m_fSpawnStartTime[0] > fSpawnLatency)
            {
                m_pSpawnMonster[0]->Set_IsActive(true);
                m_bSpawnFinish2[0] = true;
            }
        }
        else if (m_fSpawnTime > m_fSpawn_CoolDown * 2 && m_bSpawnFinish2[1] == false)
        {
            m_fSpawnStartTime[1] += fTimeDelta;
            if (m_fSpawnStartTime[1] > fSpawnLatency)
            {
                m_pSpawnMonster[1]->Set_IsActive(true);
                m_bSpawnFinish2[1] = true;
            }
        }
        else if (m_fSpawnTime > m_fSpawn_CoolDown * 3 && m_bSpawnFinish2[2] == false)
        {
            m_fSpawnStartTime[2] += fTimeDelta;

            if (m_fSpawnStartTime[2] > fSpawnLatency)
            {
                m_pSpawnMonster[2]->Set_IsActive(true);
                m_bSpawnFinish2[2] = true;
            }
        }
        else if (m_fSpawnTime > m_fSpawn_CoolDown * 4 && m_bSpawnFinish2[3] == false)
        {
            m_fSpawnStartTime[3] += fTimeDelta;

            if (m_fSpawnStartTime[3] > fSpawnLatency)
            {
                m_pSpawnMonster[3]->Set_IsActive(true);
                m_bSpawnFinish2[3] = true;

                m_eWormState = IDLE;
            }
        }


        if (iFlag != 0)
        {
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

            vVelocity *= 2.f;
            vPos += vVelocity;
            static_cast<CMonster*>(pGameObject)->Set_Pos(vPos);

            CScene* pScene = CManagement::GetInstance()->GetCurrentScene();
            if (FAILED(pScene->Add_GameObject(L"Glubba", pGameObject))) return;

            CLayer* pGameLogicLayer = CManagement::GetInstance()->Get_Layer(L"GameLogic_Layer");
            _vec3 vScale;
            if (m_iPhase == 0)
            {
                if (iFlag == 1 || iFlag == 2)vScale = { 0.25f,0.25f, 0.25f };
                else if (iFlag == 3)vScale = { 0.35f,0.35f, 0.35f };
                else vScale = { 0.45f,0.45f, 0.45f };
            }
            else
            {
                if (iFlag == 1 || iFlag == 2)vScale = { 0.35f,0.35f, 0.35f };
                else vScale = { 0.45f,0.45f, 0.45f };
            }
            vScale *= 1.25f;
            vPos.y = vScale.y;

            _vec3 vEpsilon = vPlayerPos - vPos;
            vEpsilon.y = 0.f;
            D3DXVec3Normalize(&vEpsilon, &vEpsilon);
            vEpsilon *= (0.25f * 0.25f);
            vPos += vEpsilon;
            pGameObject = CShockwave::Create(m_pGraphicDev, vPos, vScale);
            if (nullptr == pGameObject) return;

            if (FAILED(pGameLogicLayer->Add_GameObject(L"Shockwave", pGameObject))) return;

        }
        Look_AtPlayer();
    }
}

void CWorm::IDLE_Worm(const _float& fTimeDelta)
{
    Set_OnTerrain();
    Chase_Player_Worm(fTimeDelta);
}

void CWorm::Opening_Worm(const _float& fTimeDelta)
{
    m_bElapsedOpeningTime += fTimeDelta;

    if (m_bOpeningMoveFlag == true)
    {
        m_bOpening = false;
        Look_AtPlayer();
        return;
    }
    if (m_bOpeningMoveFlag2 == false)
    {
        m_bOpeningMoveFlag2 = true;
        _vec3 vPos{ 0.f,m_pTransformCom->m_vScale.y,0.f };
        m_pTransformCom->Move_Pos(&vPos, 1.f, 1.f);
    }

    if (m_bElapsedOpeningTime > 2.f)
    {
        _vec3 vPos, vDir;
        m_pTransformCom->Get_Info(INFO_POS, &vPos);
        vDir = m_vOpeningMoveDirection[m_iOpeningMoveIndex];

        vDir.y = 3.f;
        m_fLandingTime += fTimeDelta;
        vDir.y -= m_fLandingTime * 9.8f;

        if (vPos.y <= m_pTransformCom->m_vScale.y && m_fLandingTime > 0.5f)
        {
            m_fLandingTime = 0.f;

            ++m_iOpeningMoveIndex;

            if (m_iOpeningMoveIndex == sizeof(m_vOpeningMoveDirection) / sizeof(m_vOpeningMoveDirection[0]))
            {
                m_bOpeningMoveFlag = true;
                //m_eWormState = IDLE;
                return;
            }
        }
        m_pTransformCom->Move_Pos(&vDir, 2.f, fTimeDelta);
    }
    Look_AtPlayer();
}

void CWorm::Look_AtPlayer()
{
    CTransform* pPlayerTransformCom = dynamic_cast<CTransform*>(Engine::CManagement::GetInstance()
        ->Get_Component(ID_DYNAMIC, L"GameLogic_Layer", L"Player", L"Com_Transform"));

    if (nullptr == pPlayerTransformCom)
        return;

    _vec3   vPlayerPos;
    pPlayerTransformCom->Get_Info(INFO_POS, &vPlayerPos);

    _vec3   vPlayerLook;
    pPlayerTransformCom->Get_Info(INFO_LOOK, &vPlayerLook);

    m_pTransformCom->LookAt_Player(&vPlayerPos, &vPlayerLook);
}

void CWorm::Look_AtDestination()
{
    _vec3 vPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    _vec3 vecLook = vPos - m_vMovePosition;
    m_pTransformCom->LookAt_Player(&m_vMovePosition, &vecLook);

}

void CWorm::Chase_Player_Worm(const _float& fTimeDelta)
{
    CTransform* pPlayerTransformCom = dynamic_cast<CTransform*>(Engine::CManagement::GetInstance()
        ->Get_Component(ID_DYNAMIC, L"GameLogic_Layer", L"Player", L"Com_Transform"));

    if (nullptr == pPlayerTransformCom)
        return;

    _vec3   vPlayerPos;
    pPlayerTransformCom->Get_Info(INFO_POS, &vPlayerPos);

    _vec3   vPlayerLook;
    pPlayerTransformCom->Get_Info(INFO_LOOK, &vPlayerLook);

    m_pTransformCom->Chase_Target(&vPlayerPos, &vPlayerLook, 2.f, fTimeDelta);
}

void CWorm::Shuffle_Array(_uint N)
{
    for (int i = 0; i < (int)N; ++i)
    {
        m_iSpawnOrderArr[i] = i;
    }

    for (int i = N - 1; i > 0; --i)
    {
        int j = rand() % (i + 1);
        int temp = m_iSpawnOrderArr[i];
        m_iSpawnOrderArr[i] = m_iSpawnOrderArr[j];
        m_iSpawnOrderArr[j] = temp;
    }
}

void CWorm::Worm_Dead(const _float& fTimeDelta)
{
    Worm_Dead_Effect();
    Look_AtPlayer();

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
}

void CWorm::Worm_Dead_Effect()
{
    if (m_bDead_Effect1 == false)
    {
        m_bDead_Effect1 = true;
        _vec3 vPos;
        m_pTransformCom->Get_Info(INFO_POS, &vPos);

        CLayer* pLayer = CManagement::GetInstance()->Get_Layer(L"GameLogic_Layer");
        CGameObject* pGameObject = nullptr;

        pGameObject = CEffect::Create(m_pGraphicDev, CEffect::BOSS1_DEAD_EFFECT, vPos);
        if (nullptr == pGameObject) return;
        if (FAILED(pLayer->Add_GameObject(L"Effect_Worm_Dead", pGameObject))) return;
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
        if (FAILED(pLayer->Add_GameObject(L"Effect_Worm_Explosion1", pGameObject))) return;
    }

    if (m_fElapsedDeadTime > m_fDeadTime)
    {
        _vec3 vPos;
        m_pTransformCom->Get_Info(INFO_POS, &vPos);
        CLayer* pLayer = CManagement::GetInstance()->Get_Layer(L"GameLogic_Layer");
        CGameObject* pGameObject = nullptr;

        pGameObject = CEffect::Create(m_pGraphicDev, CEffect::BOSS1_EXPLOSION2, vPos);
        if (nullptr == pGameObject) return;
        if (FAILED(pLayer->Add_GameObject(L"Effect_Worm_Explosion2", pGameObject))) return;
    }
}

void CWorm::Set_Angle()
{
}
