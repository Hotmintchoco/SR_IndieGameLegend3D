#include "pch.h"
#include "CLiminalGunUltimateEffect.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

CLiminalGunUltimateEffect::CLiminalGunUltimateEffect(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CLiminalGunUltimateEffect::~CLiminalGunUltimateEffect()
{
}

HRESULT CLiminalGunUltimateEffect::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	if (FAILED(CGameObject::Ready_GameObject()))
		return E_FAIL;

	SetSize(128, 128);

	m_iRingTextureIndex = 0;
	m_iRingTextureFrameCount = m_pRingTexture->GetCount();

	m_vecTargetInfo.reserve(32);
	m_vecRenderInfo.reserve(32);

	return S_OK;
}

_int CLiminalGunUltimateEffect::Update_GameObject(_float fTimeDelta)
{
	if (!Get_IsActive()) return 0;

	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	UpdateRenderInfo();

	if (!m_vecRenderInfo.empty())
	{
		CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);
	}

	return iExit;
}

void CLiminalGunUltimateEffect::UpdateRenderInfo()
{
	m_vecRenderInfo.clear();

	for (const auto& [vWorldPos, fDmgRatio] : m_vecTargetInfo)
	{
		_vec3 vScreenPos = GetScreenPos(vWorldPos);

		// GetScreenPos가 z < 0을 반환하면 카메라 뒤 / 화면 밖
		if (vScreenPos.z < 0.f)
			continue;

		float fRatio = clamp(fDmgRatio, 0.f, 1.f);

		int iIndex = static_cast<int>(fRatio * m_iRingTextureFrameCount);
		if (iIndex >= m_iRingTextureFrameCount)
			iIndex = m_iRingTextureFrameCount - 1;

		m_vecRenderInfo.push_back({ vScreenPos, iIndex, fRatio >= 1.f });
	}

	m_vecTargetInfo.clear();
}

void CLiminalGunUltimateEffect::LateUpdate_GameObject(_float fTimeDelta)
{
	if (!Get_IsActive()) return;

	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CLiminalGunUltimateEffect::Render_GameObject()
{
	if (!Get_IsActive()) return;

	for (const auto& [vScreenPos, iIndex, bMaxCharged] : m_vecRenderInfo)
	{
		m_pTransform->Set_Pos(vScreenPos);
		m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransform->Get_World());

		m_pFixedRingTexture->Set_Texture(0);
		m_pBuffer->Render_Buffer();

		m_pRingTexture->Set_Texture(iIndex);
		m_pBuffer->Render_Buffer();

		if (bMaxCharged)
		{
			m_pSkullTexture->Set_Texture(0);
			m_pBuffer->Render_Buffer();
		}
	}
}

void CLiminalGunUltimateEffect::SetSize(int iX, int iY)
{
	m_pTransform->Set_Scale(_vec3{ iX * 0.5f, iY * 0.5f, 1.f });
}

void CLiminalGunUltimateEffect::AddTargetInfo(const TLiminalUltimateTargetInfo& tInfo)
{
	m_vecTargetInfo.push_back(tInfo);
}

_vec3 CLiminalGunUltimateEffect::GetScreenPos(const _vec3& vWorldPos)
{
	const _vec3 vInvalid{ 0.f, 0.f, -1.f };

	_matrix matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	// 카메라 뒤면 투영 결과가 뒤집히므로 뷰 공간 z로 먼저 거름
	_vec3 vViewPos;
	D3DXVec3TransformCoord(&vViewPos, &vWorldPos, &matView);
	if (vViewPos.z <= 0.f)
		return vInvalid;

	// 뷰 -> NDC (-1 ~ 1)
	_vec3 vNDC;
	D3DXVec3TransformCoord(&vNDC, &vViewPos, &matProj);

	if (vNDC.x < -1.f || vNDC.x > 1.f || vNDC.y < -1.f || vNDC.y > 1.f)
		return vInvalid;

	// NDC -> 화면 중앙 원점 픽셀 좌표
	D3DVIEWPORT9 tViewport;
	m_pGraphicDev->GetViewport(&tViewport);

	return _vec3{
		vNDC.x * tViewport.Width * 0.5f,
		vNDC.y * tViewport.Height * 0.5f,
		0.1f };
}

HRESULT CLiminalGunUltimateEffect::Add_Component()
{
	CComponent* pComponent = nullptr;

	// Transform
	pComponent = m_pTransform = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	// Buffer
	pComponent = m_pBuffer = dynamic_cast<CRcTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	// Fixed Ring Texture
	pComponent = m_pFixedRingTexture = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Ultimate_LiminalGun_FixedRing_Texture"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_FixedRingTexture", pComponent });

	// Ring Texture
	pComponent = m_pRingTexture = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Ultimate_LiminalGun_Ring_Texture"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_RingTexture", pComponent });

	// Skull Texture
	pComponent = m_pSkullTexture = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Ultimate_LiminalGun_Skull_Texture"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_SkullTexture", pComponent });

	return S_OK;
}

CLiminalGunUltimateEffect* CLiminalGunUltimateEffect::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CLiminalGunUltimateEffect* pEffect = new CLiminalGunUltimateEffect(pGraphicDev);

	if (FAILED(pEffect->Ready_GameObject()))
	{
		Safe_Release(pEffect);
		MSG_BOX("CLiminalGunUltimateEffect Create Failed");
		return nullptr;
	}

	return pEffect;
}

void CLiminalGunUltimateEffect::Free()
{
	CGameObject::Free();
}