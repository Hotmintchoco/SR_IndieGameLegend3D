#include "pch.h"
#include "CBoss1.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CTimerMgr.h"
#include "CTerrain.h"
#include "CRoomLayer.h"
#include "CEffect.h"
#include "CSprnub1.h"
#include "CSprnub2.h"
#include "CSprnub3.h"
#include "CShockwave.h"
#include "CUIMgr.h"
#include "CClientCameraMgr.h"
#include "CCinematicCamera.h"
#include "CPlayerCamera.h"

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

    m_pTransformCom->Set_Scale(1.5f, 1.5f, 1.5f);

    if (m_pOwner == nullptr)
    {
        m_vRoomCenterLocation = s_vRoomCenter;
    }
    else
    {
        m_vRoomCenterLocation = static_cast<CRoomLayer*>(m_pOwner)->GetCenterPos();
        s_vRoomCenter = m_vRoomCenterLocation;
    }


    m_pTransformCom2->Set_Scale(0.75f, 0.75f, 0.75f);
    m_pTransformCom2->Set_Pos(m_pTransformCom->m_vInfo[INFO_POS].x, m_pTransformCom->m_vInfo[INFO_POS].y, m_pTransformCom->m_vInfo[INFO_POS].z);

    m_pColliderCom->Set_Radius(m_pTransformCom->m_vScale.x);

    m_iMaxHp = 10;
    m_iHp = m_iMaxHp;
    m_bCollision_WithMonster = false;

    return S_OK;
}

_int CBoss1::Update_GameObject(_float fTimeDelta)
{
    _float _fTimeDelta = fTimeDelta;
    if (m_iHp <= 0)
    {
        m_eBoss1State = DEAD;

        m_bMoveFlag = false;
        m_bMoveFlag2 = false;
        m_pColliderCom->Set_IsActive(false);
        if (m_bDeadStart == false)
        {
            m_bDeadStart = true;
            m_bStand = false;
        }

        for (int i = 0; i < 4; ++i)
        {
            if (m_bSpawnFinish[i] == true && m_bSpawnFinish2[i] == false)
            {
				m_pSpawnMonster[i]->Set_Dead(true);
            }
        }
    }
    else if (m_iHp <= m_iMaxHp / 2)
    {
        m_iPhase = 1;
        _fTimeDelta *= 1.5f;
    }

    _int    iExit = CMonster::Update_GameObject(_fTimeDelta);



    Update_Motion(_fTimeDelta);

    switch (m_eBoss1State)
    {
    case IDLE:
        IDLE_Boss1(_fTimeDelta);
        break;
    case SPAWN:
        Spawn_Spn(_fTimeDelta);
        break;
    case MOVE:
        Move_Boss1(_fTimeDelta);
        break;
    case DEAD:
        Boss1_Dead(_fTimeDelta);
        break;
    case OPENING:
        Opening_Boss1(_fTimeDelta);
        break;
    }

    // 정민 : Boss HP 처리
    CUIMgr::GetInstance()->Set_BossHp(m_iHp / _float(m_iMaxHp));

    return iExit;
}

void CBoss1::LateUpdate_GameObject(_float fTimeDelta)
{
    

    //Angry버전 Transform->chase업데이트
    if (m_iPhase == 1)
    {
        const TBillBoardInfo& tInfo = m_pBillBoardCamera->GetBillBoardInfo();

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
        _vec3 vDst = -tInfo.vLook;

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
    CMonster::LateUpdate_GameObject(fTimeDelta);
}

void CBoss1::Render_GameObject()
{
    if (m_bHitState == true) CMonster::Enable_HitRenderState();

    CMonster::Render_GameObject();

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
    m_pTextureCom->Set_Texture((_uint)m_fFrame);
    m_pBufferCom->Render_Buffer();

    if (m_iPhase == 1)
    {
        if(m_eBoss1State!=DEAD)
			m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, FALSE);
        m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom2->Get_World());
        m_pTextureCom2->Set_Texture((_uint)m_fFrame / 2);
        m_pBufferCom->Render_Buffer();
        if (m_eBoss1State != DEAD)
            m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, TRUE);
    }
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

    if (m_bHitState == true) CMonster::Disable_HitRenderState();

}

void CBoss1::OnCollisionEnter(COLLINFO eCollInfo)
{
    CMonster::OnCollisionEnter(eCollInfo);
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

void CBoss1::Update_Motion(const _float& fTimeDelta)
{
    if (m_eBoss1State == DEAD || m_bOpening == true)return;

    m_fStateUpdateTime += fTimeDelta;

    if (m_fStateUpdateTime > m_fStateUpdateDuration)
    {
        m_fStateUpdateTime = 0.f;


        if (m_iPhase == 0)
        {
            m_eBoss1State = static_cast<BOSS1STATE>(rand() % 2);
            if (m_bMoveState == true)
            {
                m_eBoss1State = MOVE;
                m_bMoveState = false;
            }
        }
        else
        {
            m_eBoss1State = static_cast<BOSS1STATE>(rand() % 2);
        }
        //m_eBoss1State = SPAWN;
        //m_eBoss1State = MOVE;
        if (m_eBoss1State == SPAWN)
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

            m_bStand = false;

        }
        else if (m_eBoss1State == MOVE)
        {
            //Set_MovePosition();
            m_fStateUpdateDuration = 5.f;
            m_bMoveFlag = false;
            m_bMoveFlag2 = false;
            //m_bTrailStart = false;
            //m_fTrailTime2 = 0.f;

            m_iLandingCount = 0;
            m_bStand = false;
        }
        else if (m_eBoss1State == IDLE)
        {
            m_fStateUpdateDuration = 2.f;
        }
    }
}

void CBoss1::Move_Boss1(const _float& fTimeDelta)
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
            Set_Stand(fTimeDelta);

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
            Set_Walking(fTimeDelta);


            _vec3 vec3 = vPos - m_vMovePosition;

            if (D3DXVec3Length(&vec3) < 0.1f)
            {
                //Look_AtPlayer();
                m_bMoveFlag = true;
            }
            else
            {
                _vec3 vDir = m_vMovePosition - vPos;
                D3DXVec3Normalize(&vDir, &vDir);
                m_pTransformCom->Move_Pos(&vDir, 10.f, fTimeDelta);
                //Look_AtDestination();
            }
			Look_AtPlayer();
        }
    }
    else
    {
        Set_OnTerrain();
        Set_Walking(fTimeDelta);
        Chase_Player_Boss1(fTimeDelta);
    }
    
    
   
}

void CBoss1::Spawn_Spn(const _float& fTimeDelta)
{
    Set_Stand(fTimeDelta);

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
                pGameObject = CSprnub1::Create(m_pGraphicDev, 0.75f);
            }
            else
            {
                pGameObject = CSprnub2::Create(m_pGraphicDev, 0.75f);
            }
            if (nullptr == pGameObject) return;
            //pGameObject->Set_IsActive(true);
            m_pSpawnMonster[iFlag - 1] = pGameObject;
        }
        else if (m_fSpawnTime > m_fSpawn_CoolDown * 2 && m_bSpawnFinish[1] == false)
        {
            m_bSpawnFinish[1] = true;
            iFlag = 2;

            if (m_iPhase == 0)
            {
                pGameObject = CSprnub1::Create(m_pGraphicDev, 0.75f);
            }
            else
            {
                pGameObject = CSprnub2::Create(m_pGraphicDev, 0.75f);
            }
            if (nullptr == pGameObject) return;
            //pGameObject->Set_IsActive(true);
            m_pSpawnMonster[iFlag - 1] = pGameObject;
        }
        else if (m_fSpawnTime > m_fSpawn_CoolDown * 3 && m_bSpawnFinish[2] == false)
        {
            m_bSpawnFinish[2] = true;
            iFlag = 3;

            if (m_iPhase == 0)
            {
                pGameObject = CSprnub2::Create(m_pGraphicDev, 0.75f);
            }
            else
            {
                pGameObject = CSprnub3::Create(m_pGraphicDev, 0.75f);
            }
            if (nullptr == pGameObject) return;
            //pGameObject->Set_IsActive(true);
            m_pSpawnMonster[iFlag - 1] = pGameObject;
        }
        else if (m_fSpawnTime > m_fSpawn_CoolDown * 4 && m_bSpawnFinish[3] == false)
        {
            m_bSpawnFinish[3] = true;
            iFlag = 4;

            if (m_iPhase == 0)
            {
                pGameObject = CSprnub3::Create(m_pGraphicDev, 0.75f);
            }
            else
            {
                pGameObject = CSprnub3::Create(m_pGraphicDev, 0.75f);
            }
            if (nullptr == pGameObject) return;
            //pGameObject->Set_IsActive(true);

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

                m_eBoss1State = IDLE;
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
            if (FAILED(pScene->Add_GameObject(L"Sprnub", pGameObject))) return;


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

void CBoss1::IDLE_Boss1(const _float& fTimeDelta)
{
    Set_OnTerrain();
    Chase_Player_Boss1(fTimeDelta);
    Set_Walking(fTimeDelta);

}

void CBoss1::Opening_Boss1(const _float& fTimeDelta)
{
	m_fElapsedOpeningTime += fTimeDelta;

    //오프닝 무브 끝
    if (m_bOpeningMoveFlag == true)
	{
        //오프닝 종료
		m_bOpening = false;
		Look_AtPlayer();
		return;
    }
    //Y값 최초 조정
    if (m_bInit_YPos == false)
    {
        m_bInit_YPos = true;
        _vec3 vPos{ 0.f,m_pTransformCom->m_vScale.y,0.f };
        m_pTransformCom->Move_Pos(&vPos, 1.f, 1.f);

		_vec3 vBossPos;
		m_pTransformCom->Get_Info(INFO_POS, &vBossPos);

        // 정민 : 컷신 테스트 용
        auto* pCameraMgr = CClientCameraMgr::GetInstance();

        auto* pCinematic = dynamic_cast<CCinematicCamera*>(
            pCameraMgr->Find_Camera(CLIENT_CAMERA_TYPE::CINEMATIC));

        if (pCinematic)
        {
            pCinematic->Set_StartFromCurrent(true); // 연속적 동작 설정

            // 연출 1
            CINEMATIC_DESC desc;

            desc.vEyeFrom = { vBossPos.x, 3.f, vBossPos.z - 6.f };
            desc.vEyeTo = { vBossPos.x, 2.f, vBossPos.z - 2.f };
            desc.vLookAt = { vBossPos.x, 2.f, vBossPos.z };

            desc.fDuration = 2.f;
            desc.fFovFrom = D3DXToRadian(60.f);
            desc.fFovTo = D3DXToRadian(47.5f);
            pCinematic->Add_Shot(desc);

            // 연출 2
            desc.vEyeTo = { vBossPos.x, 4.5f, vBossPos.z - 7.f };
            desc.vLookAt = { vBossPos.x, 2.f, vBossPos.z };

            desc.fDuration = 0.2f;
            desc.fFovTo = D3DXToRadian(60.f);
            pCinematic->Add_Shot(desc);

            // 연출 3
            desc.vEyeTo = { vBossPos.x + 3.f, 4.5f, vBossPos.z - 7.f };
            desc.vLookAt = { vBossPos.x, 2.f, vBossPos.z };

            desc.fDuration = 0.5f;
            desc.fFovTo = D3DXToRadian(60.f);
            pCinematic->Add_Shot(desc);

            // 연출 4
            desc.vEyeTo = { vBossPos.x - 3.f, 4.5f, vBossPos.z - 7.f };
            desc.vLookAt = { vBossPos.x, 2.f, vBossPos.z };

            desc.fDuration = 0.5f;
            desc.fFovTo = D3DXToRadian(60.f);
            pCinematic->Add_Shot(desc);

            // 연출 5
            desc.vEyeTo = { vBossPos.x, 4.5f, vBossPos.z - 7.f };
            desc.vLookAt = { vBossPos.x, 2.f, vBossPos.z };

            desc.fDuration = 0.25f;
            desc.fFovTo = D3DXToRadian(60.f);
            pCinematic->Add_Shot(desc);

            // 연출 6
            desc.vEyeTo = { vBossPos.x, 4.5f, vBossPos.z - 7.f };
            desc.vLookAt = { vBossPos.x, 2.f, vBossPos.z };

            desc.fDuration = 1.5f;
            desc.fFovTo = D3DXToRadian(60.f);
            pCinematic->Add_Shot(desc);

            pCameraMgr->Select_Camera(CLIENT_CAMERA_TYPE::CINEMATIC);
        }
    }

    //뛰어다니기
    if (m_fElapsedOpeningTime > 2.f)
    {
        _vec3 vPos, vDir;
        m_pTransformCom->Get_Info(INFO_POS, &vPos);
        vDir = m_vOpeningMoveDirection[m_iOpeningMoveIndex];

        vDir.y = 3.f;
        m_fLandingTime += fTimeDelta;
        vDir.y -= m_fLandingTime * 9.8f;

        //착지했을때, 다음 착지위치 설정
        if (vPos.y <= m_pTransformCom->m_vScale.y && m_fLandingTime > 0.5f)
        {
            m_fLandingTime = 0.f;

            ++m_iOpeningMoveIndex;

            //다음 착지위치가 없을때, 오프닝 무브 종료
            if (m_iOpeningMoveIndex == sizeof(m_vOpeningMoveDirection) / sizeof(m_vOpeningMoveDirection[0]))
            {
                m_bOpeningMoveFlag = true;
                return;
            }
        }
        m_pTransformCom->Move_Pos(&vDir, 2.f, fTimeDelta);
    }
    Look_AtPlayer();

    // 정민 : Boss Hp UI 처리
    CUIMgr::GetInstance()->Active_Boss(true);
}

void CBoss1::Set_Stand(const _float& fTimeDelta)
{
    if (m_bStand == false)
    {
        m_fFrame += fTimeDelta * 6.f;
        if (m_fFrame > 4.f)
        {
            m_fFrame = 0.f;
			m_bStand = true;
        }
    }
}

void CBoss1::Set_Walking(const _float& fTimeDelta)
{
    m_fFrame += fTimeDelta * 6.f;
    if (m_fFrame > 4.f)
        m_fFrame = 0.f;
}

void CBoss1::Look_AtPlayer()
{
    const TBillBoardInfo& tInfo = m_pBillBoardCamera->GetBillBoardInfo();

    _vec3   vPlayerPos;
    vPlayerPos = tInfo.vPosition;

    _vec3   vPlayerLook;
    vPlayerLook = tInfo.vLook;

    m_pTransformCom->LookAt_Player(&vPlayerPos, &vPlayerLook);
}

void CBoss1::Look_AtDestination()
{
    _vec3 vPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    _vec3 vecLook = vPos - m_vMovePosition;
    m_pTransformCom->LookAt_Player(&m_vMovePosition, &vecLook);

}

void CBoss1::Chase_Player_Boss1(const _float& fTimeDelta)
{
	CTransform* pPlayerTransformCom = dynamic_cast<CTransform*>(Engine::CManagement::GetInstance()
		->Get_Component(ID_DYNAMIC, L"GameLogic_Layer", L"Player", L"Com_Transform"));

	if (nullptr == pPlayerTransformCom)
		return;

	const TBillBoardInfo& tInfo = m_pBillBoardCamera->GetBillBoardInfo();

	_vec3   vPlayerPos;
	pPlayerTransformCom->Get_Info(INFO_POS, &vPlayerPos);

	_vec3   vPlayerLook;
	vPlayerLook = tInfo.vLook;

	m_pTransformCom->Chase_Target(&vPlayerPos, &vPlayerLook, 2.f, fTimeDelta);
}

void CBoss1::Shuffle_Array(_uint N)
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

void CBoss1::Boss1_Dead(const _float& fTimeDelta)
{
    Boss1_Dead_Effect();
    Set_Stand(fTimeDelta);
    Look_AtPlayer();

    m_fElapsedDeadTime += fTimeDelta;
    m_fElapsedDeadTime2 += fTimeDelta;

    if (m_fElapsedDeadTime > m_fDeadTime)
    {
        CUIMgr::GetInstance()->Active_Boss(false);
        m_bDelete = true;
    }
    if (m_fElapsedDeadTime2 > 0.5f)
    {
        m_fElapsedDeadTime2 = 0.f;
        m_bHitState = !m_bHitState;
        m_fHitEffectElapsedTime = 0.f;
    }
}

void CBoss1::Boss1_Dead_Effect()
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
        if (FAILED(pLayer->Add_GameObject(L"Effect_Boss1_Dead", pGameObject))) return;
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
        if (FAILED(pLayer->Add_GameObject(L"Effect_Boss1_Explosion1", pGameObject))) return;
    }

    if (m_fElapsedDeadTime > m_fDeadTime)
    {
        _vec3 vPos;
        m_pTransformCom->Get_Info(INFO_POS, &vPos);
        CLayer* pLayer = CManagement::GetInstance()->Get_Layer(L"GameLogic_Layer");
        CGameObject* pGameObject = nullptr;

        pGameObject = CEffect::Create(m_pGraphicDev, CEffect::BOSS1_EXPLOSION2, vPos);
        if (nullptr == pGameObject) return;
        if (FAILED(pLayer->Add_GameObject(L"Effect_Boss1_Explosion2", pGameObject))) return;
    }
}
