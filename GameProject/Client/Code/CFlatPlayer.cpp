#include "pch.h"
#include "CFlatPlayer.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CTransform.h"
#include "CBoxCollider.h"
#include "CTexture.h"
#include "CFlatPlayerMovement.h"
#include "CCollisionMgr.h"
#include "Client_Enum.h"
#include "CDInputMgr.h"
#include "CFlatPlayerTex.h"
#include "CTimerMgr.h"

CFlatPlayer::CFlatPlayer(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CFlatPlayer::~CFlatPlayer()
{
}

HRESULT CFlatPlayer::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	if (FAILED(CGameObject::Ready_GameObject()))
		return E_FAIL;
	
	for (int i = 0; i < FLAT_END; ++i)
	{
		m_iTextureFrameCount[i] = m_pTexture[i]->GetCount();
	}

	m_pBoxCollider->Set_Extents(_vec3{ m_vFlatSize.x * 0.5f, 0.5f, m_vFlatSize.y * 0.5f });
	m_pBoxCollider->Set_DiffPos(_vec3{ 0.f, 0.f, m_vFlatSize.y * 0.5f });

	return S_OK;
}

_int CFlatPlayer::Update_GameObject(_float fTimeDelta)
{
	fTimeDelta = CTimerMgr::GetInstance()->GetGroupTimeDelta(CTG_FLATPLAYER);

	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHATEST, this);
	CCollisionMgr::GetInstance()->Add_Collider(COLL_FLATPLAYER, m_pBoxCollider);

	UpdateInput();
	UpdateAnimationStatus(fTimeDelta);

	return iExit;
}

void CFlatPlayer::LateUpdate_GameObject(_float fTimeDelta)
{
	fTimeDelta = CTimerMgr::GetInstance()->GetGroupTimeDelta(CTG_FLATPLAYER);

	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CFlatPlayer::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransform->Get_World());

	m_pTexture[m_eFSM]->Set_Texture(m_iCurrentTextureIndex);

	m_pBuffer->Render_Buffer();
}

HRESULT CFlatPlayer::Add_Component()
{
	CComponent* pComponent = nullptr;

	// Buffer
	pComponent = m_pBuffer = dynamic_cast<CFlatPlayerTex*>(
		CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_FlatPlayerTex"));
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Buffer", pComponent });

	// Transform
	pComponent = m_pTransform = dynamic_cast<CTransform*>(
		CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	// BoxCollider
	pComponent = m_pBoxCollider = dynamic_cast<CBoxCollider*>(
		CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_BoxCollider"));
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_BoxCollider", pComponent });

	// Textures (FSM 상태별)
	const _tchar* szProtoTags[FLAT_END] = {
		L"Proto_FlatPlayer_Idle_Texture",
		L"Proto_FlatPlayer_Walk_Texture",
		L"Proto_FlatPlayer_Jump_Texture"
	};
	const _tchar* szComTags[FLAT_END] = {
		L"Com_Texture_Idle",
		L"Com_Texture_Walk",
		L"Com_Texture_Jump"
	};

	for (_int i = 0; i < FLAT_END; ++i)
	{
		pComponent = m_pTexture[i] = dynamic_cast<CTexture*>(
			CProtoMgr::GetInstance()->Clone_Prototype(szProtoTags[i]));
		if (nullptr == pComponent)
			return E_FAIL;
		m_mapComponent[ID_STATIC].insert({ szComTags[i], pComponent });
	}

	// Movement
	m_pMovement = CFlatPlayerMovement::Create(m_pGraphicDev);
	if (nullptr == m_pMovement)
		return E_FAIL;
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Movement", m_pMovement });
	m_pMovement->AttachTransform(m_pTransform);

	return S_OK;
}

void CFlatPlayer::UpdateInput()
{
	if (!m_bInputEnabled) return;

	float fCommand = 0.f;
	if (CDInputMgr::GetInstance()->Key_Press(DIK_D))
	{
		fCommand += 1.f;
	}
	if (CDInputMgr::GetInstance()->Key_Press(DIK_A))
	{
		fCommand -= 1.f;
	}
	m_pMovement->Walk(fCommand);

	bool bJumpFlag = false;
	if (CDInputMgr::GetInstance()->Key_Press(DIK_SPACE))
	{
		m_pMovement->Jump();
		bJumpFlag = true;
	}

	if (!m_pMovement->IsOnGround())
	{
		SetFSM(FLAT_JUMP);
	}
	else if (fabsf(fCommand) > 0.f)
	{
		SetFSM(FLAT_WALK);
	}
	else
	{
		SetFSM(FLAT_IDLE);
	}
}

void CFlatPlayer::SetFSM(FLAT_FSM e)
{
	if (e == m_eFSM) return;

	m_eFSM = e;
	m_fTimeAfterPlay = 0.f;
}

void CFlatPlayer::UpdateAnimationStatus(float fTimeDelta)
{
	m_fTimeAfterPlay += fTimeDelta;

	if (m_fTimeAfterPlay >= m_fFrameInterval[m_eFSM])
	{
		m_fTimeAfterPlay -= m_fFrameInterval[m_eFSM];
		m_iCurrentTextureIndex = (m_iCurrentTextureIndex + 1) % m_iTextureFrameCount[m_eFSM];
	}
}

CFlatPlayer* CFlatPlayer::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CFlatPlayer* pInstance = new CFlatPlayer(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MSG_BOX("CFlatPlayer Create Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CFlatPlayer::Free()
{
	CGameObject::Free();
}