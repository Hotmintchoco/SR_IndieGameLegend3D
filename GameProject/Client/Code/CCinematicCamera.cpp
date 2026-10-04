#include "pch.h"
#include "CCinematicCamera.h"
#include "CManagement.h"

CCinematicCamera::CCinematicCamera(LPDIRECT3DDEVICE9 pGraphicDev)
	: CCamera(pGraphicDev), m_bFinished(false), m_bPlaying(false), m_fElapsedTime(0.f), m_bStartFromCurrent(false)
{
}

CCinematicCamera::~CCinematicCamera()
{
    
}

HRESULT CCinematicCamera::Ready_GameObject()
{
    m_fAspect = static_cast<_float>(WINCX) / WINCY;
    m_fNear = 0.1f;
    m_fFar = 1000.f;

    Evaluate(0.f);

    return CCamera::Ready_GameObject();
}

HRESULT CCinematicCamera::Play()
{
    if (m_queueDesc.empty())
		return E_FAIL;

    m_tCurrentDesc = m_queueDesc.front();
    m_queueDesc.pop();

    m_fElapsedTime = 0.f;
    m_bPlaying = true;
    m_bFinished = false;

    // UI Layer 및 Player 비활성화
	CLayer* pUILayer = CManagement::GetInstance()->Get_Layer(L"UI_Layer");
    if (pUILayer)
        pUILayer->Set_IsActive(false);

    // 이전 연출의 마지막 자세가 남지 않도록 초기 상태 설정
    Evaluate(0.f);
    Update_Matrices();

    return S_OK;
}

_int CCinematicCamera::Update_GameObject(_float fTimeDelta)
{
    if (!m_bPlaying)
        return 0;

    m_fElapsedTime += max(0.f, fTimeDelta);

    const _float fRatio = min(
        m_fElapsedTime / m_tCurrentDesc.fDuration, 1.f);

    Evaluate(fRatio);

    if (fRatio >= 1.f)
        Start_NextShot();

    if (m_bFinished)
    {
        // UI Layer 및 Player 활성화
        CLayer* pUILayer = CManagement::GetInstance()->Get_Layer(L"UI_Layer");
        if (pUILayer)
            pUILayer->Set_IsActive(true);
	}

    return 0;
}

void CCinematicCamera::LateUpdate_GameObject(_float fTimeDelta)
{
    
}

void CCinematicCamera::Evaluate(_float fRatio)
{
    const _float t = max(0.f, min(fRatio, 1.f));

    // Smoothstep: 부드럽게 출발하고 멈춤
    const _float u = t * t * (3.f - 2.f * t);

    D3DXVec3Lerp(
        &m_vEye,
        &m_tCurrentDesc.vEyeFrom,
        &m_tCurrentDesc.vEyeTo,
        u);

    m_vAt = m_tCurrentDesc.vLookAt;

    m_fFov = m_tCurrentDesc.fFovFrom
        + (m_tCurrentDesc.fFovTo - m_tCurrentDesc.fFovFrom) * u;

    // Eye와 At이 일치하면 View 행렬을 만들 수 없으므로 보정
    _vec3 vLook = m_vAt - m_vEye;

    if (D3DXVec3LengthSq(&vLook) < 0.00001f)
    {
        vLook = { 0.f, 0.f, 1.f };
        m_vAt = m_vEye + vLook;
    }

    D3DXVec3Normalize(&vLook, &vLook);

    // 시선이 수직에 가까울 때 Up과 평행해지는 상황 방지
    m_vUp = fabsf(vLook.y) > 0.999f ? _vec3{ 0.f, 0.f, 1.f } : _vec3{ 0.f, 1.f, 0.f };
}

void CCinematicCamera::Start_NextShot()
{
    if (m_queueDesc.empty())
    {
        m_bPlaying = false;
        m_bFinished = true;
        return;
    }

    m_tCurrentDesc = m_queueDesc.front();
    m_queueDesc.pop();

    m_fElapsedTime = 0.f;

    Evaluate(0.f);
}

void CCinematicCamera::Stop()
{
    // 현재 위치에서 재생 취소
    m_bPlaying = false;
    m_bFinished = false;
}

void CCinematicCamera::Skip()
{
    if (!m_bPlaying)
        return;

    m_fElapsedTime = m_tCurrentDesc.fDuration;
    Evaluate(1.f);

    m_bPlaying = false;
    m_bFinished = true;
}

void CCinematicCamera::Add_Shot(const CINEMATIC_DESC& tDesc)
{
	m_queueDesc.push(tDesc);
}

CCinematicCamera* CCinematicCamera::Create(
    LPDIRECT3DDEVICE9 pGraphicDev)
{
    if (!pGraphicDev)
        return nullptr;

    CCinematicCamera* pCamera = new CCinematicCamera(pGraphicDev);

    if (FAILED(pCamera->Ready_GameObject()))
    {
        pCamera->Release();
        return nullptr;
    }

    return pCamera;
}

void CCinematicCamera::Free()
{
    CCamera::Free();
}