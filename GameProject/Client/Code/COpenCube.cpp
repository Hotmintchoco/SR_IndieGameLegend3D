#include "pch.h"
#include "COpenCube.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CTransform.h"
#include "COpenCubeTex.h"
#include "CRayCaster.h"
#include "CManagement.h"

COpenCube::COpenCube(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pTextureProtoTag)
	: CGameObject(pGraphicDev), m_pTextureProtoTag(pTextureProtoTag)
{
}

COpenCube::~COpenCube()
{
}

HRESULT COpenCube::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	__super::Ready_GameObject();

	return S_OK;
}

_int COpenCube::Update_GameObject(_float fTimeDelta)
{
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);

	return iExit;
}

void COpenCube::LateUpdate_GameObject(_float fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void COpenCube::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pTextureCom->Set_Texture(m_iTextureIdx);
	m_pBufferCom->Render_Buffer();	// 판정용 m_pRayBufferCom은 그리지 않음
}

vector<pair<Engine::CVIBuffer*, Engine::CTransform*>> COpenCube::GetRayTestTargetInfo()
{
	return vector<pair<Engine::CVIBuffer*, Engine::CTransform*>>{ { m_pRayBufferCom, m_pTransformCom } };
}

float COpenCube::SampleTerrainHeight(const _vec3& vRayStart)
{
	CRayCaster* pRayCaster = static_cast<CRayCaster*>(CManagement::GetInstance()->Get_GameObject(L"GameLogic_Layer", L"RayCaster"));

	THitInfo tHit{};
	_vec3 vDir{ 0.f, -1.f, 0.f };

	const vector<pair<CVIBuffer*, CTransform*>>& vecInfo = GetRayTestTargetInfo();
	for (auto& [pBuffer, pTransform] : vecInfo)
	{
		pRayCaster->RayTest(tHit, vRayStart, vDir, pBuffer, pTransform->Get_World());
	}

	if (tHit.bHit)
	{
		return tHit.fHitPoint.y;
	}
	else
	{
		return -FLT_MAX;
	}
}

HRESULT COpenCube::Add_Component()
{
	CComponent* pComponent = nullptr;

	// Buffer (렌더용, 윗면 없음)
	pComponent = m_pBufferCom = dynamic_cast<COpenCubeTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_OpenCubeTex"));
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	// Ray Buffer (판정용, 윗면 있음)
	pComponent = m_pRayBufferCom = dynamic_cast<CPlyTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Cube_Vertex"));
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[ID_STATIC].insert({ L"Com_RayBuffer", pComponent });

	// Texture (큐브 텍스처)
	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(m_pTextureProtoTag));
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	// Transform
	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	return S_OK;
}

COpenCube* COpenCube::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pTextureProtoTag)
{
	COpenCube* pInstance = new COpenCube(pGraphicDev, pTextureProtoTag);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("COpenCube Create Failed");
		return nullptr;
	}

	return pInstance;
}

void COpenCube::Free()
{
	CGameObject::Free();
}