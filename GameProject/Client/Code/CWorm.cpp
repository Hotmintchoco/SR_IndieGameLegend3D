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
#include "CPlayerCamera.h"

CWorm::CWorm(LPDIRECT3DDEVICE9 pGraphicDev)
    : CMonster(pGraphicDev)
{
    ZeroMemory(&m_matConnector, sizeof(m_matConnector));
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
    if (m_iWormIndex == 1)
        Set_RoomCenterLocation();

    Set_Init_Worm();

    _float _fTimeDelta = fTimeDelta;
    if (m_iHp <= 0)
	{
        m_fElapsedDeadTime3 += fTimeDelta;
		if (m_bDeadStart == false)
		{
			m_bDeadStart = true;

            m_eWormState = DEAD;

            m_bMoveFlag = false;
            m_bMoveFlag2 = false;
            m_pColliderCom->Set_IsActive(false);
            if (m_iWormIndex == 1)
            {
                Set_HeadWorm_Null();
            }
		}
        if (m_pNextWorm != nullptr)
        {
            if (m_fElapsedDeadTime3 > 0.25f)
            {
                static_cast<CWorm*>(m_pNextWorm)->Set_Damage(static_cast<CWorm*>(m_pNextWorm)->Get_Hp());
                m_pNextWorm = nullptr;
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
        if (m_fFrame >= 4.f)
            m_fFrame -= 4.f;
    }

    if (m_iWormIndex == 1 || m_iWormIndex == 10)
    {
        m_pTransformCom->Set_Scale(0.75f, 0.5f, 0.5f);
    }
    else
    {
        m_pTransformCom->Set_Scale(0.5f, 0.5f, 0.5f);
    }
    _int    iExit = CMonster::Update_GameObject(_fTimeDelta);

    Check_Sandburst(fTimeDelta);
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
        case ATTACK:
            Attack_Worm(_fTimeDelta);
            break;
	    }
    }
    else
    {
		if (m_eWormState == DEAD)
		{
			Worm_Dead(_fTimeDelta);    
		}
        else
        {
            Update_WormBoby(_fTimeDelta);
        }
        //if (m_pHeadWorm != nullptr && static_cast<CWorm*>(m_pHeadWorm)->Get_WormState() != DEAD)
    }

    return iExit;
}

void CWorm::LateUpdate_GameObject(_float fTimeDelta)
{
    CMonster::LateUpdate_GameObject(fTimeDelta);
    Set_Motion_FromAngle();

    Update_Connector();
}

void CWorm::Render_GameObject()
{
    if (m_bHitState == true) CMonster::Enable_HitRenderState();

    CMonster::Render_GameObject();

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
    if (m_iWormIndex == 1)
    {
        m_pTextureCom->Set_Texture((_int)m_fFrame + (_int)m_eDir * 4);
    }
    else
    {
        m_pTextureCom->Set_Texture((_int)m_eDir);
        m_pBufferCom->Render_Buffer();

		m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom2->Get_World());
		m_pTextureCom2->Set_Texture(4);
    }
    m_pBufferCom->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

    if (m_bHitState == true) CMonster::Disable_HitRenderState();

}

void CWorm::OnCollisionEnter(COLLINFO eCollInfo)
{
    CCollider* pCollider = eCollInfo.pOtherCollider;

    if (pCollider && pCollider->Get_CollisionID() == COLL_PROJECTILE)
    {
        m_bHitState = true;
        m_fHitEffectElapsedTime = 0.f;

        if (m_pHeadWorm == nullptr)return;
        if (static_cast<CWorm*>(m_pHeadWorm)->Get_Hp() <= 0) return;
        static_cast<CWorm*>(m_pHeadWorm)->Set_Damage(1);
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

    //Connector
    if (m_iWormIndex != 1)
    {
        pComponent = m_pTextureCom2 = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_worm_bobyTexture"));
    }

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture2", pComponent });

    // Transform
    pComponent = m_pTransformCom2 = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform2", pComponent });

    return S_OK;
}

void CWorm::Check_Sandburst(_float fTimeDelta)
{
    m_fElapsedTime2 += fTimeDelta;
    m_fElapsedTime3 += fTimeDelta;
    if (m_iWormIndex == 1)
    {
        _vec3 vPos; m_pTransformCom->Get_Info(INFO_POS, &vPos);
        if (0.f < vPos.y && vPos.y < 0.125f)
        {
            if (m_fElapsedTime2 > 0.5f)
            {
                _float fTime = 0.f;
                if (m_eWormState == SPAWN || m_eWormState == ATTACK)
                {
                    if (m_bMoveFlag == false)
                        fTime = 0.4f;
                    else
                        fTime = 0.9f;
                }
                else
                    fTime = 1.1f;
                Effect_Sandburst(fTime);
                m_fElapsedTime2 = 0.f;
            }
        }
    }
    if (m_iWormIndex == 9)
    {
        _vec3 vPos; m_pTransformCom->Get_Info(INFO_POS, &vPos);
        if (0.f < vPos.y && vPos.y < 0.125f)
        {
            if (m_fElapsedTime3 > 0.5f)
            {
                Effect_Sandburst2();
                m_fElapsedTime3 = 0.f;
            }
        }
    }
}

void CWorm::Effect_Sandburst(_float fLifeTime)
{
    _vec3 vPos; m_pTransformCom->Get_Info(INFO_POS, &vPos);
    CLayer* pLayer = CManagement::GetInstance()->Get_Layer(L"GameLogic_Layer");

    CGameObject* pGameObject = nullptr;
    pGameObject = CEffect::Create(m_pGraphicDev, CEffect::SANDBURST, vPos, fLifeTime);
    if (nullptr == pGameObject) return;

    if (FAILED(pLayer->Add_GameObject(L"Sandburst", pGameObject))) return;
}

void CWorm::Effect_Sandburst2()
{
    _vec3 vPos; m_pTransformCom->Get_Info(INFO_POS, &vPos);
    CLayer* pLayer = CManagement::GetInstance()->Get_Layer(L"GameLogic_Layer");

    CGameObject* pGameObject = nullptr;
    pGameObject = CEffect::Create(m_pGraphicDev, CEffect::SANDBURST2, vPos);
    if (nullptr == pGameObject) return;

    if (FAILED(pLayer->Add_GameObject(L"Sandburst", pGameObject))) return;
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
    if (pMonster->m_iWormIndex == 2)
    {
        vPos.y -= 1.25f;                   
    }
    else
    {
        vPos.y -= 1.f;
    }
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
        m_bMotionEnd = false;
        Clear_MoveDest();

        m_eWormState = static_cast<WORMSTATE>(Get_MotionState());

        if (m_eWormState == SPAWN)
        {
            Set_MoveDest();
            m_fStateUpdateDuration = 6.f;
            m_bSpawnStart = false;
            m_bMoveFlag = false;
            m_bMoveFlag2 = false;
            Set_Speed_Worm(9.f);
            m_fSpawnTime = 0.f;
            m_fSpawnTime2 = 0.f;
        }
        else if (m_eWormState == MOVE)
        {
            Set_MoveDest();
            m_fStateUpdateDuration = 5.f;
            m_bMoveFlag = false;
            Set_Speed_Worm(9.f);
        }
        else if (m_eWormState == IDLE)
        {
            m_fStateUpdateDuration = 2.f;
        }
        else if (m_eWormState == ATTACK)
        {
            Set_MoveDest();
            m_fStateUpdateDuration = 12.f;
            m_bAttackStart = false;
            m_bMoveFlag = false;
            m_bMoveFlag2 = false;
            Set_Speed_Worm(9.f);
            m_fAttackTime = 0.f;
            m_fAttackTime2 = 0.f;
        }
    }
}

_int CWorm::Get_MotionState()
{
    if (m_listState.empty() == true)
    {
        m_listState.push_back(0);
        return m_listState.back();
    }

    if (m_listState.size() == 3)
    {
        for (int i = 0; i < 3; ++i)
        {
            if (find(m_listState.begin(), m_listState.end(), i) == m_listState.end())
            {
                m_listState.pop_front();
                m_listState.push_back(i);
                return m_listState.back();
            }
        }
    }
    _int iRand;
    do
    {
        iRand = rand() % 3;
    } while (m_listState.size() >= 2 && *m_listState.rbegin() == iRand && *next(m_listState.rbegin()) == iRand);

    if (m_listState.size() == 3)
        m_listState.pop_front();

    m_listState.push_back(iRand);
    return m_listState.back();
}

void CWorm::Set_MoveDest()
{
    const TBillBoardInfo& tInfo = m_pBillBoardCamera->GetBillBoardInfo();

    _vec3   vPlayerPos; vPlayerPos = tInfo.vPosition;
    _vec3 vPos; m_pTransformCom->Get_Info(INFO_POS, &vPos);

    _vec3 vDir = m_vRoomCenterLocation - vPlayerPos;
    vDir.y = 0.f;
    D3DXVec3Normalize(&vDir, &vDir);
    _matrix matRotY;
    _int iRand = rand() % 180;
    D3DXMatrixRotationY(&matRotY, D3DXToRadian(-90.f + (_float)iRand));
    D3DXVec3TransformNormal(&vDir, &vDir, &matRotY);

    if (m_eWormState == ATTACK)
    {
        vDir *= 3.5f;
    }
    else
    {
        vDir *= 4.f;
    }

    _vec3 vInitPos = m_vRoomCenterLocation + vDir;
    vInitPos.y = -2.f;

    Set_Pos_Worm(vInitPos);

    if (m_eWormState == MOVE)
    {
        vDir = vPlayerPos - vInitPos;
        vDir.y = 0.f;
        D3DXVec3Normalize(&vDir, &vDir);
        vDir *= 0.25f;
        vDir.y = 2.f + m_fMoveHeight;
        _vec3 vDest = vInitPos + vDir;
        Push_Back_MoveDest(vDest);
    }
    else if (m_eWormState == SPAWN)
    {
        vDir = { 0.f,5.f,0.f };
        _vec3 vDest = vInitPos + vDir;
        Push_Back_MoveDest(vDest);
    }
    else if (m_eWormState == ATTACK)
    {
        const TBillBoardInfo& tInfo = m_pBillBoardCamera->GetBillBoardInfo();

        _vec3   vPlayerPos; vPlayerPos = tInfo.vPosition;
        _vec3 vPos; m_pTransformCom->Get_Info(INFO_POS, &vPos);
        _vec3 vDir2 = vPlayerPos - vPos;
        vDir2.y = 0.f;
        D3DXVec3Normalize(&vDir2, &vDir2);

        vDir = { 0.f,1.f,0.f };

        _vec3 vDest = vInitPos + vDir * 3.f - vDir2 * 0.5f;
        Push_Back_MoveDest(vDest);

        vDest += vDir * 1.5f - vDir2 * 0.5f;
        Push_Back_MoveDest(vDest);

        vDest += vDir2 + vDir * 0.5f;
        Push_Back_MoveDest(vDest);

        vDest += vDir2*0.5f;
        Push_Back_MoveDest(vDest);

        vDest += vDir2 - vDir*0.5f;
        Push_Back_MoveDest(vDest);

    }
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
        static_cast<CWorm*>(m_pNextWorm)->Set_Speed_Worm(fSpeed+m_iWormIndex/20.f);
    }
}

void CWorm::Set_HeadWorm_Null()
{
    m_pHeadWorm = nullptr;
    if (m_pNextWorm != nullptr)
    {
        static_cast<CWorm*>(m_pNextWorm)->Set_HeadWorm_Null();
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
        Move_WormHead_BeforeSpawn(fTimeDelta);
    }
    else if (m_bSpawnStart == false)
    {
        m_fSpawnTime += fTimeDelta;
        if (m_fSpawnTime < 1.f)return;

        m_bSpawnStart = true;

        _vec3 vPos; m_pTransformCom->Get_Info(INFO_POS, &vPos);

        const TBillBoardInfo& tInfo = m_pBillBoardCamera->GetBillBoardInfo();

        _vec3   vPlayerPos; vPlayerPos = tInfo.vPosition;
        _vec3   vPlayerLook; vPlayerLook = tInfo.vLook;

        _vec3 vVelocity;

        CGameObject* pGameObject = nullptr;

        vVelocity = vPlayerPos - vPos;
        vVelocity.y = 0.f;
        D3DXVec3Normalize(&vVelocity, &vVelocity);
        //vVelocity *= 0.5f;

        _int iRand = rand()%20 + 20;
        _int iAngle[4] = { -60, -60 + iRand, -60 + iRand * 2, 60 };

        _vec3 vOriginVelocity = vVelocity;
        vPos.y += 0.5f;
        for (int i = 0; i < 4; ++i)
        {
            _matrix matRot;
            D3DXMatrixRotationY(&matRot, D3DXToRadian(iAngle[i]));

            D3DXVec3TransformNormal(&vVelocity, &vVelocity, &matRot);

            pGameObject = CGlubba::Create(m_pGraphicDev);
            vVelocity.y = 0.f;

            static_cast<CMonster*>(pGameObject)->Set_Pos(vPos + vVelocity * 0.25f);
            vVelocity.y = 4.f;
            static_cast<CGlubba*>(pGameObject)->Set_Velocity(vVelocity);
            vVelocity = vOriginVelocity;
            pGameObject->Set_IsActive(true);
            CScene* pScene = CManagement::GetInstance()->GetCurrentScene();
            if (FAILED(pScene->Add_GameObject(L"Glubba", pGameObject))) return;
        }

    }
    else
    {
        Move_WormHead_AfterSpawn(fTimeDelta);   
    }
}

void CWorm::IDLE_Worm(const _float& fTimeDelta)
{
}

void CWorm::Attack_Worm(const _float& fTimeDelta)
{
    if (m_bMoveFlag == false)
    {
        Move_WormHead_BeforeAttack(fTimeDelta);
    }
    else if (m_bAttackStart == false)
    {
        m_fAttackTime += fTimeDelta;
        if (m_fAttackTime > 1.5f)
        {
            m_bAttackStart = true;
        }
    }
    else
    {
        Move_WormHead_AfterAttack(fTimeDelta);
    }
}

void CWorm::Opening_Worm(const _float& fTimeDelta)
{
	m_bElapsedOpeningTime += fTimeDelta;

    if (m_bElapsedOpeningTime > 1.f && m_bElapsedOpeningTime<=1.1f)
    {
        //오프닝 도착지점 세팅
        if (m_bOpeningStart == false)
        {
            Set_Speed_Worm(9.f);
            m_bOpeningStart = true;

            const TBillBoardInfo& tInfo = m_pBillBoardCamera->GetBillBoardInfo();

            _vec3   vPlayerPos; vPlayerPos = tInfo.vPosition;
            _vec3 vPos; m_pTransformCom->Get_Info(INFO_POS, &vPos);
            _vec3 vDest;

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
            vDest = vPos;
			vDir *= 0.5f;

			vDir.y = 0.75f;
			vDest = vDest + vDir;
			Push_Back_MoveDest(vDest);

			vDir.y = 0.5f;
			vDest = vDest + vDir;
			Push_Back_MoveDest(vDest);

			vDir.y = 0.25f;
			vDest = vDest + vDir;
			Push_Back_MoveDest(vDest);

            vDir.y = 0.f;
            vDest = vDest + vDir;
            Push_Back_MoveDest(vDest);

            vDir.y = -0.25f;
            vDest = vDest + vDir;
            Push_Back_MoveDest(vDest);

            vDir.y = -0.5f;
            vDest = vDest + vDir;
            Push_Back_MoveDest(vDest);

            vDir.y = -0.75f;
            vDest = vDest + vDir;
            Push_Back_MoveDest(vDest);

            vDir = { 0.f, -20.f, 0.f };
			vDest = vDest + vDir;
			Push_Back_MoveDest(vDest);

            //버전1
            //vDir *= 1.f;
            //vDir.y = 0.5f;
            //vDest = vPos + vDir;
            //Push_Back_MoveDest(vDest);

            //vDir.y = 0;
            //vDest = vDest + vDir;
            //Push_Back_MoveDest(vDest);

            //vDir.y = -0.5;
            //vDest = vDest + vDir;
            //Push_Back_MoveDest(vDest);

            //vDir = { 0.f, -20.f, 0.f };
            //vDest = vDest + vDir;
            //Push_Back_MoveDest(vDest);
        }
    }
    //3초뒤 움직임
    else if (m_bElapsedOpeningTime > 1.f)
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

void CWorm::Worm_Dead(const _float& fTimeDelta)
{
    Worm_Dead_Effect();

    m_fElapsedDeadTime += fTimeDelta;

    if (m_fElapsedDeadTime > m_fDeadTime)
    {
        if (m_bDelete == false && m_iWormIndex == 10)
            DropItem_Boss();
        m_bDelete = true;
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

        pGameObject = CEffect::Create(m_pGraphicDev, CEffect::WORM_DEAD_EFFECT, vPos);
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

        pGameObject = CEffect::Create(m_pGraphicDev, CEffect::WORM_EXPLOSION1, vPos);
        if (nullptr == pGameObject) return;
        if (FAILED(pLayer->Add_GameObject(L"Effect_Worm_Explosion1", pGameObject))) return;
    }

    if (m_fElapsedDeadTime > m_fDeadTime)
    {
        _vec3 vPos;
        m_pTransformCom->Get_Info(INFO_POS, &vPos);
        CLayer* pLayer = CManagement::GetInstance()->Get_Layer(L"GameLogic_Layer");
        CGameObject* pGameObject = nullptr;

        pGameObject = CEffect::Create(m_pGraphicDev, CEffect::WORM_EXPLOSION2, vPos);
        if (nullptr == pGameObject) return;
        if (FAILED(pLayer->Add_GameObject(L"Effect_Worm_Explosion2", pGameObject))) return;
    }
}

void CWorm::Set_Init_Worm()
{

    if (m_bSet_InitPos == false)
    {
        m_bSet_InitPos = true;
        if (m_iWormIndex == 1)
        {
            _vec3 vPos = m_vRoomCenterLocation;
            vPos.y = 2.5f;
            Set_Pos(vPos);
        }

        if (m_iWormIndex < 10)
        {
            CMonster* pMonster;
            CScene* pScene = CManagement::GetInstance()->GetCurrentScene();

            pMonster = CWorm::Create(m_pGraphicDev, m_iWormIndex + 1, this);
            pMonster->Set_IsActive(true);
            if (nullptr == pMonster) return;

            TCHAR		szFileName[128] = L"";
            wsprintf(szFileName, L"Worm_%d", m_iWormIndex + 1);
            if (FAILED(pScene->Add_GameObject(szFileName, pMonster))) return;
        }
        else
        {
            return;
        }
    }
}

void CWorm::Set_Motion_FromAngle()
{

    _matrix* matWorld;
    matWorld = m_pTransformCom->Get_World();
    _vec3 vRight = { matWorld->_11, matWorld->_12, matWorld->_13 };
    _vec3 vUp = { matWorld->_21, matWorld->_22, matWorld->_23 };
    _vec3 vLook = { matWorld->_31, matWorld->_32, matWorld->_33 };
    _vec3 vPos = { matWorld->_41, matWorld->_42,  matWorld->_43 };
    _vec3 vLookXZ = vLook;
    _vec3 vRightXZ = vRight;
    matWorld->_41 = 0.f;
    matWorld->_42 = 0.f;
    matWorld->_43 = 0.f;

    D3DXVec3Normalize(&vUp, &vUp);
    D3DXVec3Normalize(&vLook, &vLook);

    _vec3 vPlayerLook, vPlayerPos;
    const TBillBoardInfo& tInfo = m_pBillBoardCamera->GetBillBoardInfo();
    vPlayerLook = tInfo.vLook;
    vPlayerPos = tInfo.vPosition;
    _vec3 vPlayerLookXZ = vPlayerLook;
    //////////////////////
    vPlayerLook = vPos - vPlayerPos;
    vPlayerLookXZ = vPlayerLook;
    //////////////////////
    D3DXVec3Normalize(&vPlayerLook, &vPlayerLook);

    _vec3 vProj = vPlayerLook - D3DXVec3Dot(&vPlayerLook, &vUp) * vUp;
    D3DXVec3Normalize(&vProj, &vProj);

    _float fDot = D3DXVec3Dot(&vProj, &vLook);
    fDot = max(-1.f, min(1.f, fDot));
    _float fDegree = D3DXToDegree(acosf(fDot));

    _vec3 vProj2 = vPlayerLook - D3DXVec3Dot(&vPlayerLook, &vLook) * vLook;
    D3DXVec3Normalize(&vProj2, &vProj2);

    //_float fDot2 = D3DXVec3Dot(&vProj2, &vUp);
    //fDot2 = max(-1.f, min(1.f, fDot2));
    //_float fDegree2 = D3DXToDegree(acosf(fDot2));

    _float fDot2 = D3DXVec3Dot(&vPlayerLook, &vUp);
    fDot2 = max(-1.f, min(1.f, fDot2));
    _float fDegree2 = D3DXToDegree(acosf(fDot2));

    vPlayerLookXZ.y = 0;
    vLookXZ.y = 0;
    vRightXZ.y = 0;
    D3DXVec3Normalize(&vPlayerLookXZ, &vPlayerLookXZ);
    D3DXVec3Normalize(&vLookXZ, &vLookXZ);
    D3DXVec3Normalize(&vRightXZ, &vRightXZ);

    //_float fThreshold = 22.5f;
    _float fThreshold = 32.5f;
    if (fDegree2 < fThreshold || fDegree2 > 180.f - fThreshold)
    {
        m_eDir = TOP;
    }
    else
    {
        if (m_iWormIndex == 1 || m_iWormIndex == 10)
        {
            //정면
            if (fDegree > 135.f)
            {
                _vec3 vScale = { 0.5f,0.5f,0.5f };
                m_pTransformCom->Set_Scale(vScale);

                m_eDir = FRONT;
            }
            //옆면
            else if (fDegree > 45.f)
            {
                m_eDir = SIDE;
            }
            //후면
            else
            {
                if (m_iWormIndex == 10)
                {
					_vec3 vScale = { 0.5f,0.5f,0.5f };
					m_pTransformCom->Set_Scale(vScale);

					m_eDir = FRONT;
                }
                else
                {
					m_eDir = SIDE;
                }
            }
        }
        else
        {
            _float fThreshold2 = 15.f;
            _float fThreshold3 = 10.f;

            //정면
            if (fDegree > 135.f + fThreshold2 + fThreshold3)
            {
                _vec3 vScale = { 0.5f,0.5f,0.5f };
                m_pTransformCom->Set_Scale(vScale);

                m_eDir = FRONT;
            }
            //45도
            else if (fDegree > 135.f - fThreshold2)
            {
                m_eDir = SIDE45;
            }
            //옆면
            else if (fDegree > 45.f + fThreshold2 - fThreshold3)
            {
                m_eDir = SIDE;
            }
            //45도
            else if (fDegree > 45.f - fThreshold2 - fThreshold3)
            {
                m_eDir = SIDE45;
            }
            //후면
            else
            {
                m_eDir = SIDE45;
            }
        }
    }

    if (m_eDir == FRONT)
    {
        matWorld->_41 = vPos.x;
        matWorld->_42 = vPos.y;
        matWorld->_43 = vPos.z;
        return;
    }
    _matrix matRot;
    D3DXMatrixRotationAxis(&matRot, &vUp, D3DXToRadian(-90.f));
    *matWorld = (*matWorld) * matRot;
    if (m_eDir == TOP)
    {
        D3DXMatrixRotationAxis(&matRot, &vLook, D3DXToRadian(90.f));
        *matWorld = (*matWorld) * matRot;
    }
    _matrix matTrans;
    vRight = { matWorld->_11, matWorld->_12, matWorld->_13 };
    vUp = { matWorld->_21, matWorld->_22, matWorld->_23 };
    vLook = { matWorld->_31, matWorld->_32, matWorld->_33 };


    _float fAngleY = 0.f;
    vLookXZ = vLook;
    vLookXZ.y = 0.f;
    D3DXVec3Normalize(&vLookXZ, &vLookXZ);
    _float fDot_WormLook_PlayerLook = D3DXVec3Dot(&vLookXZ, &vPlayerLookXZ);
    if (fDot_WormLook_PlayerLook <= 0.f)
    {
        vLookXZ *= -1;
    }
    _float fCrossY = vLookXZ.z * vPlayerLookXZ.x - vLookXZ.x * vPlayerLookXZ.z;

    _float fDot3 = vLookXZ.x * vPlayerLookXZ.x + vLookXZ.z * vPlayerLookXZ.z;

    fAngleY = atan2f(fCrossY, fDot3);

    _vec3 vY{ 0.f,1.f,0.f };
    D3DXMatrixRotationAxis(&matRot, &vY, fAngleY);
    *matWorld = (*matWorld) * matRot;

    matWorld->_41 = vPos.x;
    matWorld->_42 = vPos.y;
    matWorld->_43 = vPos.z;
 //   if (fDot_WormLook_PlayerLook >= 0.f)
 //   {
 //       vLook *= -1;
	//}
	//D3DXMatrixTranslation(&matTrans, vLook.x, vLook.y, vLook.z);
	//(*matWorld) = (*matWorld) * matTrans;
}

void CWorm::Update_Connector()
{
    if (m_eWormState == DEAD)
    {
        m_pTransformCom2->Set_World(&m_matConnector);
        return;
    }
    if (m_iWormIndex != 1)
    {
        _matrix* matpWorldCurrWorm = m_pTransformCom->Get_World();
        _matrix matWorld = *matpWorldCurrWorm;
        _vec3 vPrevWormPos;
        m_pPrevWorm->Get_Pos(&vPrevWormPos);

        _vec3 vCurrWormPos;
        memcpy(&vCurrWormPos, &matWorld.m[INFO_POS][0], sizeof(_vec3));

        _vec3 vPos = (vPrevWormPos + vCurrWormPos) / 2.f;

        memcpy(&matWorld.m[INFO_POS][0], &vPos, sizeof(_vec3));


        _vec3 vRight(matWorld._11, matWorld._12, matWorld._13);
        _vec3 vUp(matWorld._21, matWorld._22, matWorld._23);
        _vec3 vLook(matWorld._31, matWorld._32, matWorld._33);

        vRight *= 0.75f;
        vUp *= 0.75f;
        vLook *= 0.75f;
        //vRight *= 1.f;
        //vUp *=    1.f;
        //vLook *=  1.f;

        memcpy(&matWorld._11, &vRight, sizeof(_vec3));
        memcpy(&matWorld._21, &vUp, sizeof(_vec3));
        memcpy(&matWorld._31, &vLook, sizeof(_vec3));

        D3DXVec3Normalize(&vLook, &vLook);

        const TBillBoardInfo& tInfo = m_pBillBoardCamera->GetBillBoardInfo();
        _vec3 vPlayerPos; vPlayerPos = tInfo.vPosition;
        vPos; m_pTransformCom->Get_Info(INFO_POS, &vPos);
        _vec3 vDir = vPos - vPlayerPos;
        D3DXVec3Normalize(&vDir, &vDir);

        _float fDot = D3DXVec3Dot(&vDir, &vLook);

        if (fDot < 0.f)
        {
            vLook *= -1;
        }
        vLook *= 0.5f;
        _matrix matTrans;

        vDir *= 0.35f;

        D3DXMatrixTranslation(&matTrans, vDir.x, vDir.y, vDir.z);
        matWorld = matWorld * matTrans;
        m_matConnector = matWorld;
        m_pTransformCom2->Set_World(&m_matConnector);
    }
}

void CWorm::Move_WormHead(const _float& fTimeDelta)
{
    _vec3 vDir, vPos, vAngle, vDest;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    if (m_vMoveDest.empty())
    {
        if (m_bMotionEnd == false)
        {
            m_bMotionEnd = true;
            m_fStateUpdateDuration = m_fStateUpdateTime + 1.f;
        }
        return;
    }
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
                const TBillBoardInfo& tInfo = m_pBillBoardCamera->GetBillBoardInfo();

                _vec3   vPlayerPos; vPlayerPos = tInfo.vPosition;
                _vec3 vPos; m_pTransformCom->Get_Info(INFO_POS, &vPos);

                vDir = vPlayerPos - vPos;
                vDir.y = 0.f;
                if (D3DXVec3Length(&vDir) > 1.f)
                {
                    _vec3 vDirN;
                    D3DXVec3Normalize(&vDirN, &vDir);
                    vDir -= vDirN *1.f;

                    vDir.y = 0.f;
                    _vec3 vDest = vPos + vDir;
                    Push_Back_MoveDest(vDest);

                    vDirN.y = -m_fMoveHeight*2.f;
                    vDest = vDest + vDirN*0.5f;
                    Push_Back_MoveDest(vDest);
                    vDest = vDest + _vec3{ 0.f,-15.f,0.f };
                    Push_Back_MoveDest(vDest);
                }
            }
        }
    }
}

void CWorm::Move_WormHead_BeforeSpawn(const _float& fTimeDelta)
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
        {
            m_vMoveDest.erase(m_vMoveDest.begin());
            if (m_vMoveDest.empty())
            {
                m_bMoveFlag = true;
            }
        }
    }
}

void CWorm::Move_WormHead_AfterSpawn(const _float& fTimeDelta)
{
    if (m_fSpawnTime2 < 1.f)
    {
        m_fSpawnTime2 += fTimeDelta;
    }
    else
    {
        if (m_bMoveFlag2 == false)
        {
            m_bMoveFlag2 = true;
            _vec3 vPos; m_pTransformCom->Get_Info(INFO_POS, &vPos);
            _vec3 vDir = m_vRoomCenterLocation - vPos;
            vDir.y = 0.f;
            D3DXVec3Normalize(&vDir, &vDir);
            _vec3 vDest = vPos;
            vDir *= 0.5f;

            vDir.y = 0;
            D3DXVec3Normalize(&vDir, &vDir);
            vDest = vPos;
            vDir *= 0.5f;

            vDir.y = 0.75f;
            vDest = vDest + vDir;
            Push_Back_MoveDest(vDest);

            vDir.y = 0.5f;
            vDest = vDest + vDir;
            Push_Back_MoveDest(vDest);

            vDir.y = 0.25f;
            vDest = vDest + vDir;
            Push_Back_MoveDest(vDest);

            vDir.y = 0.f;
            vDest = vDest + vDir;
            Push_Back_MoveDest(vDest);

            vDir.y = -0.25f;
            vDest = vDest + vDir;
            Push_Back_MoveDest(vDest);

            vDir.y = -0.5f;
            vDest = vDest + vDir;
            Push_Back_MoveDest(vDest);

            vDir.y = -0.75f;
            vDest = vDest + vDir;
            Push_Back_MoveDest(vDest);

            vDir = { 0.f, -22.f, 0.f };
            vDest = vDest + vDir;
            Push_Back_MoveDest(vDest);
        }
        else
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
                        if (m_bMotionEnd == false)
                        {
                            m_bMotionEnd = true;
                            m_fStateUpdateDuration = m_fStateUpdateTime + 1.f;
                        }
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
}

void CWorm::Move_WormHead_BeforeAttack(const _float& fTimeDelta)
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
        {
            m_vMoveDest.erase(m_vMoveDest.begin());
            if (m_vMoveDest.empty())
            {
                m_bMoveFlag = true;
            }
        }
    }
}

void CWorm::Move_WormHead_AfterAttack(const _float& fTimeDelta)
{
    if (m_fAttackTime2 < 0.5f)
    {
        m_fAttackTime2 += fTimeDelta;
    }
    else
    {
        if (m_bMoveFlag2 == false)
        {
            m_bMoveFlag2 = true;
            _vec3 vPos; m_pTransformCom->Get_Info(INFO_POS, &vPos);
            vPos.y = -18.f;
            Push_Back_MoveDest(vPos);

        }
        else
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
                        if (m_bMotionEnd == false)
                        {
                            m_bMotionEnd = true;
                            m_fStateUpdateDuration = m_fStateUpdateTime + 1.f;
                        }
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
}

void CWorm::Update_WormBoby(const _float& fTimeDelta)
{
    if (m_vMoveDest.empty())return;
	_vec3 vDir, vDist, vPrevWormPos, vPos, vAngle;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);

	m_pPrevWorm->Get_Pos(&vPrevWormPos);
    vDist = vPrevWormPos - vPos;
    vDir = m_vMoveDest.front() - vPos;
    _float fDist;
    _float f = +0.f;
    if (m_iWormIndex == 2 || m_iWormIndex == 10)fDist = 1.25f + f;
    else fDist = 1.f + f;

	if (D3DXVec3Length(&vDist) > fDist && D3DXVec3Length(&vDir)>0.125f)
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