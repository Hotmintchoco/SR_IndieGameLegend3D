#include "pch.h"
#include "CPlayerCamera.h"
#include "CDInputMgr.h"
#include "CProtoMgr.h"

CPlayerCamera::CPlayerCamera(LPDIRECT3DDEVICE9 pGraphicDev)
	: CCamera(pGraphicDev), m_pTarget(nullptr),
	m_vEyeOffset({ 0.f, 1.f, 0.f }), m_fDistance(3.5f)
{
}

CPlayerCamera::CPlayerCamera(const CPlayerCamera& rhs)
	: CCamera(rhs), m_pTarget(rhs.m_pTarget),
	m_vEyeOffset(rhs.m_vEyeOffset), m_fDistance(rhs.m_fDistance)
{
}

CPlayerCamera::~CPlayerCamera()
{
}

HRESULT CPlayerCamera::Ready_GameObject(Engine::CTransform* pTarget)
{
	if (!pTarget)
		return E_FAIL;

	m_pTarget = pTarget;

	m_fFov = D3DXToRadian(60.f);
	m_fAspect = static_cast<_float>(WINCX) / WINCY;
	m_fNear = 0.1f;
	m_fFar = 1000.f;

	_vec3 vPlayerPos;
	_vec3 vPlayerLook;

	m_pTarget->Get_Info(INFO_POS, &vPlayerPos);
	m_pTarget->Get_Info(INFO_LOOK, &vPlayerLook);

	// 플레이어 Transform의 기준점에 맞춰 보정
	m_vEye = vPlayerPos + m_vEyeOffset;
	m_vAt = m_vEye + vPlayerLook;
	m_vUp = { 0.f, 1.f, 0.f };

	Sync_AnglesFromLook();
	Update_LookFromAngles();

	// 초기 View·Projection 계산
	return CCamera::Ready_GameObject();
}

_int CPlayerCamera::Update_GameObject(_float fTimeDelta)
{
	// 마우스 이동량을 읽어 공통 Rotate() 호출
	Mouse_Move();

	// 현재 Eye에서 시선 방향 갱신
	Update_LookFromAngles();

	return 0;
}

void CPlayerCamera::LateUpdate_GameObject(_float fTimeDelta)
{
	// 플레이어 Transform을 따라가도록 Eye 위치 갱신
	Follow_Target();
}

void CPlayerCamera::SetPseudoScale(float fScale)
{
	m_vEyeOffset = m_vEyeOffsetRaw * fScale;
	m_fNear = m_fNearRaw * fScale;
	m_fFar = m_fFarRaw * fScale;
}

void CPlayerCamera::Mouse_Move()
{
	const _long mouseX = CDInputMgr::GetInstance()->Get_DIMouseMove(DIMS_X);
	const _long mouseY = CDInputMgr::GetInstance()->Get_DIMouseMove(DIMS_Y);
	Rotate(D3DXToRadian(mouseX / 10.f), D3DXToRadian(mouseY / 10.f));
	Update_LookFromAngles();

	m_pTarget->Rotation(ROT_Y, mouseX / 10.f); /* 플레이어 회전 */
}

void CPlayerCamera::Follow_Target()
{
	if (!m_pTarget)
		return;

	_vec3 vPlayerPos;
	m_pTarget->Get_Info(INFO_POS, &vPlayerPos);

	_vec3 vPivot = vPlayerPos + m_vEyeOffset;

	// Eye가 바뀌었으므로 At도 새 Eye 기준으로 다시 계산
	Update_LookFromAngles();

	_vec3 vLook = m_vAt - m_vEye;
	D3DXVec3Normalize(&vLook, &vLook);

	if (m_eCameraMode == CAMERA_MODE::THIRD_PERSON)
	{
		// 시선 방향의 반대쪽으로 물러나서 플레이어를 바라봄 (3인칭)
		m_vEye = vPivot - vLook * m_fDistance;
		m_vAt = vPivot;
	}
	else
	{
		// 플레이어 눈 위치에서 정면을 바라봄 (1인칭)
		m_vEye = vPivot;
		m_vAt = m_vEye + vLook;
	}

	m_vUp = { 0.f, 1.f, 0.f };
}

CPlayerCamera* CPlayerCamera::Create(LPDIRECT3DDEVICE9 pGraphicDev, CTransform* pTarget)
{
	if (!pGraphicDev || !pTarget)
		return nullptr;

	CPlayerCamera* pCamera = new CPlayerCamera(pGraphicDev);

	if (FAILED(pCamera->Ready_GameObject(pTarget)))
	{
		pCamera->Release();
		return nullptr;
	}

	return pCamera;
}

void CPlayerCamera::Free()
{
	CCamera::Free();
}
