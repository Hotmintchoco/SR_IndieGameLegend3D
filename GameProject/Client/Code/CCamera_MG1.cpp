#include "pch.h"
#include "CCamera_MG1.h"
#include "CDInputMgr.h"
#include "CProtoMgr.h"
#include "CRayCaster.h"
#include "CManagement.h"
#include "CStage.h"
#include "IRayTestable.h"
#include "CRoomLayer.h"
#include "CCursorPolicyMgr.h"

CCamera_MG1::CCamera_MG1(LPDIRECT3DDEVICE9 pGraphicDev)
	: CCamera(pGraphicDev), m_pTarget(nullptr),
	m_vEyeOffset({ 0.f, 1.f, 0.f }), m_fDistance(3.5f)
{
}

CCamera_MG1::CCamera_MG1(const CCamera_MG1& rhs)
	: CCamera(rhs), m_pTarget(rhs.m_pTarget),
	m_vEyeOffset(rhs.m_vEyeOffset), m_fDistance(rhs.m_fDistance)
{
}

CCamera_MG1::~CCamera_MG1()
{
}

HRESULT CCamera_MG1::Ready_GameObject(Engine::CTransform* pTarget)
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

_int CCamera_MG1::Update_GameObject(_float fTimeDelta)
{
	//Mouse_Move();

	//Update_LookFromAngles();

	_vec3 vPos; m_pTarget->Get_Info(INFO_POS, &vPos);
	_vec3 vLook; m_pTarget->Get_Info(INFO_LOOK, &vLook);
	D3DXVec3Normalize(&vLook, &vLook);

	m_vEye = vPos;
	m_vAt = vPos+vLook;
	m_vUp = { 0.f,1.f,0.f };

	return 0;
}

void CCamera_MG1::LateUpdate_GameObject(_float fTimeDelta)
{
	D3DXMatrixLookAtLH(&m_matView, &m_vEye, &m_vAt, &m_vUp);
	D3DXMatrixPerspectiveFovLH(&m_matProj, m_fFov, m_fAspect, m_fNear, m_fFar);

	m_pGraphicDev->SetTransform(D3DTS_VIEW, &m_matView);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_matProj);
}

void CCamera_MG1::SetPseudoScale(float fScale)
{
	m_vEyeOffset = m_vEyeOffsetRaw * fScale;
	m_fNear = m_fNearRaw * fScale;
	m_fFar = m_fFarRaw * fScale;
}

void CCamera_MG1::Mouse_Move()
{
	if (!CCursorPolicyMgr::GetInstance()->IsCursorFixed()) return;

	const _long mouseX = CDInputMgr::GetInstance()->Get_DIMouseMove(DIMS_X);
	const _long mouseY = CDInputMgr::GetInstance()->Get_DIMouseMove(DIMS_Y);
	Rotate(D3DXToRadian(mouseX / 10.f), D3DXToRadian(mouseY / 10.f));
	Update_LookFromAngles();
}

void CCamera_MG1::Follow_Target()
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


	// 플레이어 눈 위치에서 정면을 바라봄 (1인칭)
	m_vEye = vPivot;
	m_vAt = m_vEye + vLook;
	

	m_vUp = { 0.f, 1.f, 0.f };
}

void CCamera_MG1::UpdateBillBoardInfo()
{
	const _float fCosPitch = cosf(m_fPitch);
	const _vec3 vLook{ sinf(m_fYaw) * fCosPitch, -sinf(m_fPitch), cosf(m_fYaw) * fCosPitch };
	m_tBillBoardInfo = { m_vEye , vLook };
}

float CCamera_MG1::CalculateSpringArmLength()
{
	CStage* pStage = static_cast<CStage*>(CManagement::GetInstance()->GetCurrentScene());
	if (!pStage) return FLT_MAX;

	const vector<IRayTestable*>& mapObject = pStage->GetCurrentRoomLayer()->GetRayTestableList();
	CRayCaster* pRayCaster = static_cast<CRayCaster*>(CManagement::GetInstance()->Get_GameObject(L"GameLogic_Layer", L"RayCaster"));

	_vec3 vPivot = m_pTarget->Get_Info_Value(INFO_POS) + m_vEyeOffset;
	_vec3 vDir = m_vEye - m_vAt;
	D3DXVec3Normalize(&vDir, &vDir);

	THitInfo t{};

	for (auto& pObj : mapObject)
	{
		vector<pair<CVIBuffer*, CTransform*>> vecInfo = pObj->GetRayTestTargetInfo();
		for (auto& [pBuffer, pTransform] : vecInfo)
		{
			/* 피벗(캐릭터 눈) - 카메라 방향으로 레이 발사 */
			pRayCaster->RayTest(t, vPivot, vDir, pBuffer, pTransform->Get_World());
		}
	}

	return t.fDist;
}

CCamera_MG1* CCamera_MG1::Create(LPDIRECT3DDEVICE9 pGraphicDev, CTransform* pTarget)
{
	if (!pGraphicDev || !pTarget)
		return nullptr;

	CCamera_MG1* pCamera = new CCamera_MG1(pGraphicDev);

	if (FAILED(pCamera->Ready_GameObject(pTarget)))
	{
		pCamera->Release();
		return nullptr;
	}

	return pCamera;
}

void CCamera_MG1::Free()
{
	CCamera::Free();
}
