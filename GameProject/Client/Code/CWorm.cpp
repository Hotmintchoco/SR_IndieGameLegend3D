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

    m_pTransformCom->Set_Scale(0.75f, 0.5f, 0.5f);

    if (m_iWormIndex == 1)
        m_vRoomCenterLocation = static_cast<CRoomLayer*>(m_pOwner)->GetCenterPos();

    m_pColliderCom->Set_Radius(m_pTransformCom->m_vScale.x);

    m_iMaxHp = 5;
    m_iHp = m_iMaxHp;
    m_bCollision_WithMonster = false;

    _vec3 vLook = { 0.f,1.f,0.f };
    _vec3 vAngle;
    vAngle.x = D3DXToDegree(-asinf(vLook.y));
    vAngle.y = D3DXToDegree(atan2f(vLook.x, vLook.z));
    vAngle.z = 0.f;
    //vAngle.z = 90.f;
    m_pTransformCom->Set_Angle(vAngle);

    return S_OK;
}

_int CWorm::Update_GameObject(_float fTimeDelta)
{
    Set_Init_Worm();
    m_iWormIndex;
    m_pNextWorm;
    _float _fTimeDelta = fTimeDelta;
    if (m_iHp <= 0)
	{

		if (m_bDeadStart == false)
		{
			m_bDeadStart = true;

            m_eWormState = DEAD;

            m_bMoveFlag = false;
            m_bMoveFlag2 = false;
            m_pColliderCom->Set_IsActive(false);

			if (m_pNextWorm != nullptr)
			{
				static_cast<CWorm*>(m_pNextWorm)->Set_Damage(static_cast<CWorm*>(m_pNextWorm)->Get_Hp());
				m_pHeadWorm = nullptr;
			}
		}
    }
    else if (m_iHp <= m_iMaxHp / 2)
    {
        m_iPhase = 1;
        //_fTimeDelta *= 1.5f;
    }

    if (m_iWormIndex == 1)
    {
        m_fFrame += fTimeDelta * 8.f;
        if (m_fFrame > 4.f)
            m_fFrame = 0.f;
    }

    m_pTransformCom->Set_Scale(0.75f, 0.5f, 0.5f);



    _int    iExit = CMonster::Update_GameObject(_fTimeDelta);

    if (m_iWormIndex == 1)
    {
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
            Move_WormHead(_fTimeDelta);
            break;
        case DEAD:
            Worm_Dead(_fTimeDelta);
            break;
        case OPENING:
            Opening_Worm(_fTimeDelta);
            break;
	    }
    }
    else
    {
        if (m_pHeadWorm != nullptr && static_cast<CWorm*>(m_pHeadWorm)->Get_WormState() != DEAD)
            Update_WormBoby(_fTimeDelta);
        else
            Worm_Dead(_fTimeDelta);
    }

    return iExit;
}

void CWorm::LateUpdate_GameObject(_float fTimeDelta)
{
    CMonster::LateUpdate_GameObject(fTimeDelta);
    Set_Motion_FromAngle();
}

void CWorm::Render_GameObject()
{
    if (m_bHitState == true) CMonster::Enable_HitRenderState();

    CMonster::Render_GameObject();

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
    if (m_iWormIndex == 1)
    {
        m_pTextureCom->Set_Texture((_uint)m_fFrame + (_int)m_eDir * 4);
    }
    else
    {
        m_pTextureCom->Set_Texture((_int)m_eDir);
        //m_pTextureCom->Set_Texture(0);
    }
    m_pBufferCom->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

    if (m_bHitState == true) CMonster::Disable_HitRenderState();

}

void CWorm::OnCollisionEnter(COLLINFO eCollInfo)
{
    //CMonster::OnCollisionEnter(eCollInfo);
    CCollider* pCollider = eCollInfo.pOtherCollider;

    if (pCollider && pCollider->Get_CollisionID() == COLL_PROJECTILE)
    {
        m_bHitState = true;
        m_fHitEffectElapsedTime = 0.f;

        if (m_pHeadWorm == nullptr)return;
        if (static_cast<CWorm*>(m_pHeadWorm)->Get_Hp() <= 0) return;
        static_cast<CWorm*>(m_pHeadWorm)->Set_Damage(1);

        //if (static_cast<CWorm*>(m_pHeadWorm)->Get_Hp() <= 0)
        //    m_pHeadWorm = nullptr;

    }
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

CWorm* CWorm::Create(LPDIRECT3DDEVICE9 pGraphicDev, _uint iIndex, CWorm* pFront)
{
	CWorm* pMonster = new CWorm(pGraphicDev);
	pMonster->Set_WormIndex(iIndex);
    pMonster->Set_Prev_Worm(pFront);
    pMonster->Set_Head_Worm(static_cast<CWorm*>(pFront->Get_Head_Worm()));
    pFront->Set_Next_Worm(pMonster);
	if (FAILED(pMonster->Ready_GameObject()))
	{
		Safe_Release(pMonster);
		MSG_BOX("CWorm Create Failed");
		return nullptr;
	}
    _vec3 vPos;
    pFront->Get_Pos(&vPos);
    vPos.y -= 1.5f;
    pMonster->Set_Pos(vPos);
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

        Clear_MoveDest();

        if (m_iPhase == 0)
        {
            m_eWormState = static_cast<WORMSTATE>(rand() % 2);
            if (m_bMoveState == true)
            {
                m_eWormState = MOVE;
                m_bMoveState = false;
            }
        }
        else
        {
            m_eWormState = static_cast<WORMSTATE>(rand() % 2);
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

            m_bMoveFlag = false;
            //m_fSpeed = 5.f;
            Set_Speed_Worm(5.f);

            //m_bMoveFlag2 = false;
        }
        else if (m_eWormState == MOVE)
        {
            Set_MoveDest();
            m_fStateUpdateDuration = 4.f;
            m_bMoveFlag = false;
            //m_bMoveFlag2 = false;
            //m_fSpeed = 7.f;
            Set_Speed_Worm(7.f);
        }
        else if (m_eWormState == IDLE)
        {
            m_fStateUpdateDuration = 2.f;
        }
    }
}

void CWorm::Set_MoveDest()
{
    CTransform* pPlayerTransformCom = dynamic_cast<CTransform*>(Engine::CManagement::GetInstance()
        ->Get_Component(ID_DYNAMIC, L"GameLogic_Layer", L"Player", L"Com_Transform"));
    if (nullptr == pPlayerTransformCom) return;
    _vec3 vPlayerPos; pPlayerTransformCom->Get_Info(INFO_POS, &vPlayerPos);
    _vec3 vPos; m_pTransformCom->Get_Info(INFO_POS, &vPos);

    _vec3 vDir = m_vRoomCenterLocation - vPlayerPos;
    vDir.y = 0.f;
    D3DXVec3Normalize(&vDir, &vDir);
    _matrix matRotY;
    _int iRand = rand() % 180;
    D3DXMatrixRotationY(&matRotY, D3DXToRadian(-90.f + (_float)iRand));
    D3DXVec3TransformNormal(&vDir, &vDir, &matRotY);

    //iRand = rand() % 4;
    //vDir *= (iRand + 1);
    vDir *= 4.f;
    
    _vec3 vInitPos = m_vRoomCenterLocation + vDir;
    vInitPos.y = -2.f;////////////

    Set_Pos_Worm(vInitPos);

    vDir = vPlayerPos - vInitPos;
    vDir.y = 0.f;
    D3DXVec3Normalize(&vDir, &vDir);
    //vDir *= 2.f;
    //vDir *= 5.f;
    vDir.y = 1.5f + m_MoveHeight;
    _vec3 vDest = vInitPos + vDir;
    Push_Back_MoveDest(vDest);
    m_pNextWorm;
    //vDest = m_vRoomCenterLocation;
    //vDest.y = 1.5f;
    //m_vMoveDest.push_back(vDest);



}

void CWorm::Set_Pos_Worm(_vec3 vPos)
{
    Set_Pos(vPos);
    if (m_pNextWorm != nullptr)
    {
        static_cast<CWorm*>(m_pNextWorm)->Set_Pos_Worm(vPos);
    }
}

void CWorm::Set_Speed_Worm(_float fSpeed)
{
    m_fSpeed = fSpeed;
    if (m_pNextWorm != nullptr)
    {
        static_cast<CWorm*>(m_pNextWorm)->Set_Speed_Worm(fSpeed);
    }
}

void CWorm::Push_Back_MoveDest(const _vec3& vDest)
{
    m_vMoveDest.push_back(vDest);
    if (m_pNextWorm != nullptr)
    {
        static_cast<CWorm*>(m_pNextWorm)->Push_Back_MoveDest(vDest);
    }
}

void CWorm::Clear_MoveDest()
{
    m_vMoveDest.clear();
    if (m_pNextWorm != nullptr)
    {
        static_cast<CWorm*>(m_pNextWorm)->Clear_MoveDest();
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


		vPlayerLook.y = 0;
		D3DXVec3Normalize(&vPlayerLook, &vPlayerLook);
		m_vSpawnDirection = vPlayerLook;
		m_vSpawnDirection.y = 3.f;
		m_vSpawnDirection.x /= 2.f;
		m_vSpawnDirection.z /= 2.f;


		_vec3 vDest = vPos + m_vSpawnDirection * 3.f * 0.7f;
		_float fBlank = m_pTransformCom->m_vScale.y;
		if (vDest.x > m_vRoomCenterLocation.x + 6.5f - fBlank ||
			vDest.x < m_vRoomCenterLocation.x - 6.5f + fBlank ||
			vDest.z > m_vRoomCenterLocation.z + 5.0f - fBlank ||
			vDest.z < m_vRoomCenterLocation.z - 5.0f + fBlank)
		{
			_vec3 vVerticalDirection = { 0.f,m_vSpawnDirection.y,0.f };
			m_pTransformCom->Move_Pos(&vVerticalDirection, 3.f, fTimeDelta);
		}
		else
		{
			m_pTransformCom->Move_Pos(&m_vSpawnDirection, 3.f, fTimeDelta);
		}


		if (vPos.y <= m_pTransformCom->m_vScale.y)
		{
			m_bMoveFlag = true;
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
    }
}

void CWorm::IDLE_Worm(const _float& fTimeDelta)
{
}

void CWorm::Opening_Worm(const _float& fTimeDelta)
{
	m_bElapsedOpeningTime += fTimeDelta;

    if (m_bElapsedOpeningTime > 1.f && m_bElapsedOpeningTime<=3.f)
    {
        //오프닝 도착지점 세팅
        if (m_bOpeningStart == false)
        {
            //m_fSpeed = 5.f;
            Set_Speed_Worm(5.f);
            m_bOpeningStart = true;

            CTransform* pPlayerTransformCom = dynamic_cast<CTransform*>(Engine::CManagement::GetInstance()
                ->Get_Component(ID_DYNAMIC, L"GameLogic_Layer", L"Player", L"Com_Transform"));
            if (nullptr == pPlayerTransformCom) return;
            _vec3 vPlayerPos; pPlayerTransformCom->Get_Info(INFO_POS, &vPlayerPos);
            _vec3 vPos; m_pTransformCom->Get_Info(INFO_POS, &vPos);
            _vec3 vDest;
            //_vec3 vDir = vPlayerPos - vPos;


            _vec3 vLook = { 0.f,1.f,0.f };
            _vec3 vAngle;
            vAngle.x = D3DXToDegree(-asinf(vLook.y));
            vAngle.y = D3DXToDegree(atan2f(vLook.x, vLook.z));
            vAngle.z = 0.f;
            //vAngle.z = 90.f;
            m_pTransformCom->Set_Angle(vAngle);

            _vec3 vUp; m_pTransformCom->Get_Info(INFO_UP, &vUp);
            _vec3 vDir = -vUp;

            vDir.y = 0;
            D3DXVec3Normalize(&vDir, &vDir);
            vDir *= 1.f;
            vDir.y = 0.5f;
            vDest = vPos + vDir;
            //m_vMoveDest.push_back(vDest);
            Push_Back_MoveDest(vDest);

            vDir.y = 0;
            vDest = vDest + vDir;
            //m_vMoveDest.push_back(vDest);
            Push_Back_MoveDest(vDest);

            vDir.y = -0.5;
            vDest = vDest + vDir;
            //m_vMoveDest.push_back(vDest);
            Push_Back_MoveDest(vDest);

            vDir = { 0.f, -20.f, 0.f };
            vDest = vDest + vDir;
            //m_vMoveDest.push_back(vDest);
            Push_Back_MoveDest(vDest);
        }
    }
    //3초뒤 움직임
    else if (m_bElapsedOpeningTime > 3.f)
    {
        if (!m_vMoveDest.empty())
        {
            _vec3 vPos, vDir;
            m_pTransformCom->Get_Info(INFO_POS, &vPos);

            vDir = m_vMoveDest.front() - vPos;
            if (D3DXVec3Length(&vDir) < 0.125f)
            {
                if (!m_vMoveDest.empty())
                    m_vMoveDest.erase(m_vMoveDest.begin());

                if (m_vMoveDest.empty())
                {
                    m_bOpening = false;
                }
            }
            else
            {
                D3DXVec3Normalize(&vDir, &vDir);
                m_pTransformCom->Move_Pos(&vDir, m_fSpeed, fTimeDelta);

                _vec3 vAngle;
                vAngle.x = D3DXToDegree(-asinf(vDir.y));
                vAngle.y = D3DXToDegree(atan2f(vDir.x, vDir.z));
                vAngle.z = 0.f;
                m_pTransformCom->Set_Angle(vAngle);
            }
        }
    }
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

void CWorm::Set_Init_Worm()
{

    if (/*m_pHeadWorm == this && *//*m_pNextWorm != nullptr && */m_bSet_InitPos == false)
    {
        m_bSet_InitPos = true;
        if (m_iWormIndex == 1)
        {
            _vec3 vPos = m_vRoomCenterLocation;
            vPos.y = 3.5f;
            Set_Pos(vPos);
        }

        if (m_iWormIndex < 10)
        {
            CMonster* pMonster;
            CScene* pScene = CManagement::GetInstance()->GetCurrentScene();
            //CLayer* pLayer = CManagement::GetInstance()->Get_Layer(L"GameLogic_Layer");


            pMonster = CWorm::Create(m_pGraphicDev, m_iWormIndex + 1, this);
            pMonster->Set_IsActive(true);
            //_vec3 vPos;
            //m_pTransformCom->
            //pMonster->Set_Pos()
            if (nullptr == pMonster) return;
            //Set_Next_Worm(pMonster);

            TCHAR		szFileName[128] = L"";
            wsprintf(szFileName, L"Worm_%d", m_iWormIndex + 1);
            if (FAILED(pScene->Add_GameObject(szFileName, pMonster))) return;
            //if (FAILED(pLayer->Add_Monster(szFileName, pMonster)))return E_FAIL;
        }
        else
        {
            return;
        }

        //m_pNextWorm->Set_IsActive(true);

    }
}

void CWorm::Set_Motion_FromAngle()
{

    _matrix* matWorld;
    matWorld = m_pTransformCom->Get_World();
    _vec3 vUp = { matWorld->_21, matWorld->_22, matWorld->_23 };
    _vec3 vRight = { matWorld->_11, matWorld->_12, matWorld->_13 };
    _vec3 vLook = { matWorld->_31, matWorld->_32, matWorld->_33 };
    _vec3 vPos = { matWorld->_41, matWorld->_42,  matWorld->_43 };

    matWorld->_41 = 0.f;
    matWorld->_42 = 0.f;
    matWorld->_43 = 0.f;

    D3DXVec3Normalize(&vUp, &vUp);
    D3DXVec3Normalize(&vLook, &vLook);

    _vec3 vPlayerLook;
    CTransform* pPlayerTransformCom = dynamic_cast<CTransform*>(Engine::CManagement::GetInstance()
        ->Get_Component(ID_DYNAMIC, L"GameLogic_Layer", L"Player", L"Com_Transform"));
    if (nullptr == pPlayerTransformCom) return;
    pPlayerTransformCom->Get_Info(INFO_LOOK, &vPlayerLook);
    D3DXVec3Normalize(&vPlayerLook, &vPlayerLook);

    _vec3 vProj = vPlayerLook - D3DXVec3Dot(&vPlayerLook, &vUp) * vUp;
    D3DXVec3Normalize(&vProj, &vProj);

    _float fDot = D3DXVec3Dot(&vProj, &vLook);
    fDot = max(-1.f, min(1.f, fDot));
    _float fDegree = D3DXToDegree(acosf(fDot));

    _vec3 vProj2 = vPlayerLook - D3DXVec3Dot(&vPlayerLook, &vLook) * vLook;
    D3DXVec3Normalize(&vProj2, &vProj2);
    
    _float fDot2 = D3DXVec3Dot(&vProj2, &vUp);
    fDot2 = max(-1.f, min(1.f, fDot2));
    _float fDegree2 = D3DXToDegree(acosf(fDot2));


    if (m_iWormIndex == 1 || m_iWormIndex == 10)
    {
        //정면
        if (fDegree > 135.f)
        {
            _vec3 vScale = { 0.5f,0.5f,0.5f };
            m_pTransformCom->Set_Scale(vScale);

            m_eDir = FRONT;
        }
        //옆면, 후면
        else
        {
            //옆면
            if (fDegree2 > 45.f - 22.5f && fDegree2 < 135.f + 22.5f)
            {
                m_eDir = SIDE;

				_matrix matRot;
				D3DXMatrixRotationAxis(&matRot, &vUp, D3DXToRadian(-90.f));
				*matWorld = (*matWorld) * matRot;
  
            }
            //탑
            else
            {
                m_eDir = TOP;

                _matrix matRot;
				D3DXMatrixRotationAxis(&matRot, &vUp, D3DXToRadian(-90.f));
				*matWorld = (*matWorld) * matRot;
				D3DXMatrixRotationAxis(&matRot, &vLook, D3DXToRadian(90.f));
				*matWorld = (*matWorld) * matRot;
            }
        }
    }
    else
    {
        //정면
        if (fDegree > 135.f + 22.5f)
        {
            _vec3 vScale = { 0.5f,0.5f,0.5f };
            m_pTransformCom->Set_Scale(vScale);

            m_eDir = FRONT;
        }
        //45도
        else if (fDegree > 135.f - 22.5f)
        {
            //옆면
            if (fDegree2 > 45.f - 22.5f && fDegree2 < 135.f + 22.5f)
            {
                m_eDir = SIDE45;

                _matrix matRot;
                D3DXMatrixRotationAxis(&matRot, &vUp, D3DXToRadian(-90.f));
                *matWorld = (*matWorld) * matRot;
            }
            //탑
            else
            {
                m_eDir = TOP;

                _matrix matRot;
                D3DXMatrixRotationAxis(&matRot, &vUp, D3DXToRadian(-90.f));
                *matWorld = (*matWorld) * matRot;
                D3DXMatrixRotationAxis(&matRot, &vLook, D3DXToRadian(90.f));
                *matWorld = (*matWorld) * matRot;
            }
        }
        //옆면
        else if (fDegree > 45.f + 22.5f)
        {
            //옆면
            if (fDegree2 > 45.f- 22.5f && fDegree2 < 135.f + 22.5f)
            {
                m_eDir = SIDE;

                _matrix matRot;
                D3DXMatrixRotationAxis(&matRot, &vUp, D3DXToRadian(-90.f));
                *matWorld = (*matWorld) * matRot;
            }
            //탑
            else
            {
                m_eDir = TOP;

                _matrix matRot;
                D3DXMatrixRotationAxis(&matRot, &vUp, D3DXToRadian(-90.f));
                *matWorld = (*matWorld) * matRot;
                D3DXMatrixRotationAxis(&matRot, &vLook, D3DXToRadian(90.f));
                *matWorld = (*matWorld) * matRot;
            }
        }
        //45도
        else if (fDegree > 45.f - 22.5f)
        {
            //옆면
            if (fDegree2 > 45.f - 22.5f && fDegree2 < 135.f + 22.5f)
            {
                m_eDir = SIDE45;

                _matrix matRot;
                D3DXMatrixRotationAxis(&matRot, &vUp, D3DXToRadian(-90.f));
                *matWorld = (*matWorld) * matRot;
            }
            //탑
            else
            {
                m_eDir = TOP;

                _matrix matRot;
                D3DXMatrixRotationAxis(&matRot, &vUp, D3DXToRadian(-90.f));
                *matWorld = (*matWorld) * matRot;
                D3DXMatrixRotationAxis(&matRot, &vLook, D3DXToRadian(90.f));
                *matWorld = (*matWorld) * matRot;
            }
        }
        //후면
        else
        {
            m_eDir = TOP;

            _matrix matRot;
            D3DXMatrixRotationAxis(&matRot, &vUp, D3DXToRadian(-90.f));
            *matWorld = (*matWorld) * matRot;
            D3DXMatrixRotationAxis(&matRot, &vLook, D3DXToRadian(90.f));
            *matWorld = (*matWorld) * matRot;

        }
    }
    matWorld->_41 = vPos.x;
    matWorld->_42 = vPos.y;
    matWorld->_43 = vPos.z;
}

void CWorm::Move_WormHead(const _float& fTimeDelta)
{
    _vec3 vDir, vPos, vAngle, vDest;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    if (m_vMoveDest.empty())return;

    vDest = m_vMoveDest.front();
    vDir = vDest - vPos;
    if (D3DXVec3Length(&vDir) > 0.125f)
    {
        D3DXVec3Normalize(&vDir, &vDir);
        m_pTransformCom->Move_Pos(&vDir, m_fSpeed, fTimeDelta);

        _vec3 vAngle;
        vAngle.x = D3DXToDegree(-asinf(vDir.y));
        vAngle.y = D3DXToDegree(atan2f(vDir.x, vDir.z));
        vAngle.z = 0.f;
        m_pTransformCom->Set_Angle(vAngle);
    }
    else
    {
        Set_Pos(vDest);
        if (!m_vMoveDest.empty())
            m_vMoveDest.erase(m_vMoveDest.begin());

        if (m_vMoveDest.empty())
        {
            if (m_bMoveFlag == false)
            {
                m_bMoveFlag = true;
                //////플레이어 추적
                CTransform* pPlayerTransformCom = dynamic_cast<CTransform*>(Engine::CManagement::GetInstance()
                    ->Get_Component(ID_DYNAMIC, L"GameLogic_Layer", L"Player", L"Com_Transform"));
                if (nullptr == pPlayerTransformCom) return;
                _vec3 vPlayerPos; pPlayerTransformCom->Get_Info(INFO_POS, &vPlayerPos);
                _vec3 vPos; m_pTransformCom->Get_Info(INFO_POS, &vPos);

                vDir = vPlayerPos - vPos;
                if (D3DXVec3Length(&vDir) > 1.f)
                {
                    _vec3 vDirN;
                    D3DXVec3Normalize(&vDirN, &vDir);
                    vDir -= vDirN;

                    vDir.y = 0.f;
                    _vec3 vDest = vPos + vDir;
                    //m_vMoveDest.push_back(vDest);
                    Push_Back_MoveDest(vDest);

                    vDirN.y = -1.f;
                    vDest = vDest + vDirN * 15.f;
                    //m_vMoveDest.push_back(vDest);
                    Push_Back_MoveDest(vDest);

                }
                else
                {
                    D3DXVec3Normalize(&vDir, &vDir);
                    vDir.y = -1.f;
                    _vec3 vDest = vPos + vDir * 15.f;
                    //m_vMoveDest.push_back(vDest);
                    Push_Back_MoveDest(vDest);
                }

            }

        }
    }
}

void CWorm::Update_WormBoby(const _float& fTimeDelta)
{
    if (m_vMoveDest.empty())return;
	_vec3 vDir, vDist, vPrevWormPos, vPos, vAngle;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);

	m_pPrevWorm->Get_Pos(&vPrevWormPos);
    vDist = vPrevWormPos - vPos;
    vDir = m_vMoveDest.front() - vPos;

	if (D3DXVec3Length(&vDist) > 1.5f && D3DXVec3Length(&vDir)>0.125f)
	{
        _vec3 vDirN;
		D3DXVec3Normalize(&vDirN, &vDir);
		m_pTransformCom->Move_Pos(&vDirN, m_fSpeed, fTimeDelta);
        D3DXVec3Normalize(&vDir, &vDir);
        vAngle.x = D3DXToDegree(-asinf(vDir.y));
        vAngle.y = D3DXToDegree(atan2f(vDir.x, vDir.z));
        vAngle.z = 0.f;
        m_pTransformCom->Set_Angle(vAngle);
	}
    if (D3DXVec3Length(&vDir) <= 0.125f)
    {
        Set_Pos(m_vMoveDest.front());
        m_vMoveDest.erase(m_vMoveDest.begin());
    }

}

//기존
//void CWorm::Update_WormBoby(const _float& fTimeDelta)
//{
//    _vec3 vDir, vPrevWormPos, vPos, vAngle;
//    m_pTransformCom->Get_Info(INFO_POS, &vPos);
//
//    m_pPrevWorm->Get_Pos(&vPrevWormPos);
//    vDir = vPrevWormPos - vPos;
//
//    if (D3DXVec3Length(&vDir) > 1.5f)
//    {
//        m_pTransformCom->Move_Pos(&vDir, m_fSpeed, fTimeDelta);
//    }
//    D3DXVec3Normalize(&vDir, &vDir);
//    vAngle.x = D3DXToDegree(-asinf(vDir.y));
//    vAngle.y = D3DXToDegree(atan2f(vDir.x, vDir.z));
//    vAngle.z = 0.f;
//    m_pTransformCom->Set_Angle(vAngle);
//}