#include "pch.h"
#include "CPlayer.h"
#include "CProtoMgr.h"
#include "CTimerMgr.h"
#include "CStage.h"
#include "CManagement.h"
#include "CPlayerAnimator.h"
#include "CMovement.h"
#include "CPlayerPartTex.h"
#include "CDInputMgr.h"

CPlayer::CPlayer(LPDIRECT3DDEVICE9 pGraphicDev)
{
}

CPlayer::~CPlayer()
{
}

HRESULT CPlayer::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	if (FAILED(CGameObject::Ready_GameObject()))
		return E_FAIL;

	m_pColliderCom->Set_Scale(m_fColliderScale);
	m_pColliderCom->Set_CollisionID(COLL_PLAYER);

	return S_OK;
}

_int CPlayer::Update_GameObject(_float fTimeDelta)
{
	/* 캐릭터 타임스케일 */
	fTimeDelta *= CTimerMgr::GetInstance()->GetGroupTimeScale(TG_PLAYER);

	KeyInput();

	CScene* pScene = CManagement::GetInstance()->GetCurrentScene();
	if (CStage* pStage = dynamic_cast<CStage*>(pScene))
	{
		pStage->UpdatePlayerPosition(m_pTransformCom->Get_Info_Value(INFO_POS));
	}

	return S_OK;
}

void CPlayer::LateUpdate_GameObject(_float fTimeDelta)
{
}

void CPlayer::Render_GameObject()
{
}

HRESULT CPlayer::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Transform
    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));
    if (nullptr == pComponent)
        return E_FAIL;
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    // Collider
    pComponent = m_pColliderCom = dynamic_cast<CCollider*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_SphereCollider"));
    if (nullptr == pComponent)
        return E_FAIL;
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider", pComponent });

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Player_Texture"));
    if (nullptr == pComponent)
        return E_FAIL;
    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    array<TPlayerBuffer, PP_END> arrBuffer;

    for (int i = 0; i < PP_END; ++i)
    {
        wstring wstrBodyPart;
        switch (i)
        {
        case PP_HEAD:
            wstrBodyPart = L"Head";
            break;
        case PP_BODY:
            wstrBodyPart = L"Body";
            break;
        case PP_LARM:
            wstrBodyPart = L"LArm";
            break;
        case PP_RARM:
            wstrBodyPart = L"RArm";
            break;
        case PP_LLEG:
            wstrBodyPart = L"LLeg";
            break;
        case PP_RLEG:
            wstrBodyPart = L"RLeg";
            break;
        default:
            assert(0);
            break;
        }
        wstring wstrName = L"Proto_Player_" + wstrBodyPart + L"_Vertex";
        CPlayerPartTex* pBuffer = m_pBufferCom[i] = dynamic_cast<CPlayerPartTex*>(CProtoMgr::GetInstance()->Clone_Prototype(wstrName.c_str()));

        if (nullptr == pBuffer)
            return E_FAIL;

        wstrName = L"Com_Buffer_" + wstrBodyPart;
        m_mapComponent[ID_STATIC].insert({ wstrName.c_str(), pBuffer });

        CTransform* pTransform = m_pBufferTransformCom[i] = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));
        pTransform->SetUseLocal(true);

        if (nullptr == pTransform)
            return E_FAIL;

        wstrName = L"Com_BufferTransform_" + wstrBodyPart;
        m_mapComponent[ID_DYNAMIC].insert({ wstrName.c_str(), pTransform });

        arrBuffer[i] = TPlayerBuffer{ pBuffer, pTransform };
    }

    m_pAnimator = CPlayerAnimator::Create(m_pGraphicDev);
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_PlayerAnimator", m_pAnimator });

    m_pAnimator->SetBuffer(arrBuffer);

    return S_OK;
}

void CPlayer::KeyInput()
{
    /* 이동 */
    if (CDInputMgr::GetInstance()->Key_Press(DIK_W))
    {
        m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vLook, &vLook), fSpeed, fTimeDelta);
    }

    if (CDInputMgr::GetInstance()->Key_Press(DIK_S))
    {
        m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vLook, &vLook), -fSpeed, fTimeDelta);
    }

    if (CDInputMgr::GetInstance()->Key_Press(DIK_A))
    {
        m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vRight, &vRight), -fSpeed, fTimeDelta);
    }

    if (CDInputMgr::GetInstance()->Key_Press(DIK_D))
    {
        m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vRight, &vRight), fSpeed, fTimeDelta);
    }
}


void CPlayer::OnCollisionEnter(COLLINFO eCollInfo)
{
}

void CPlayer::OnCollisionStay(COLLINFO eCollInfo)
{
}

void CPlayer::OnHit()
{
}

void CPlayer::Respawn()
{
}

void CPlayer::OnDead()
{
}

void CPlayer::RestoreHP(int iAmount)
{
}

CPlayer* CPlayer::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	return nullptr;
}

void CPlayer::Free()
{
}
