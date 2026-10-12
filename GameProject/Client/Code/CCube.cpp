#include "pch.h"
#include "CCube.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CTransform.h"
#include "CRayCaster.h"
#include "CManagement.h"
#include "CCollisionMgr.h"
#include "Client_Enum.h"

CCube::CCube(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pTextureProtoTag)
	: CGameObject(pGraphicDev), m_pTextureProtoTag(pTextureProtoTag)
{
}

CCube::~CCube()
{
}

HRESULT CCube::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	__super::Ready_GameObject();

	m_pColliderCom->Set_CollisionID(COLL_OBSTACLE);

	return S_OK;
}

_int CCube::Update_GameObject(_float fTimeDelta)
{
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);
	CCollisionMgr::GetInstance()->Add_Collider(COLL_OBSTACLE, m_pColliderCom);

	return iExit;
}

void CCube::LateUpdate_GameObject(_float fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CCube::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pTextureCom->Set_Texture(m_iTextureIdx);
	m_pBufferCom->Render_Buffer();
}

vector<pair<Engine::CVIBuffer*, Engine::CTransform*>> CCube::GetRayTestTargetInfo()
{
	return vector<pair<Engine::CVIBuffer*, Engine::CTransform*>>{ { m_pBufferCom, m_pTransformCom } };
}

float CCube::SampleTerrainHeight(const _vec3& vRayStart)
{
	CRayCaster* pRayCaster = static_cast<CRayCaster*>(CManagement::GetInstance()->Get_GameObject(L"GameLogic_Layer", L"RayCaster"));

	THitInfo tHit{};
	_vec3 vDir{ 0.f, -1.f, 0.f };

	pRayCaster->RayTest(tHit, vRayStart, vDir, m_pBufferCom, m_pTransformCom->Get_World());

	if (tHit.bHit)
	{
		return tHit.fHitPoint.y;
	}
	else
	{
		return -FLT_MAX;
	}
}

void CCube::OnCollisionStay(COLLINFO eCollInfo)
{
	switch (eCollInfo.iOtherID)
	{
	case COLLISIONID::COLL_PLAYER:
		Obstacle_Collision(eCollInfo.pOtherCollider, m_pColliderCom);
		break;
	default:
		break;
	}

}

HRESULT CCube::Add_Component()
{
	CComponent* pComponent = nullptr;

	// Buffer (렌더 + 판정 공용, 윗면 있음)
	pComponent = m_pBufferCom = dynamic_cast<CPlyTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Cube_Vertex"));
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	// Texture
	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(m_pTextureProtoTag));
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	// Transform
	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	// Collider (큐브와 같은 크기, 중심 = 원점)
	pComponent = m_pColliderCom = dynamic_cast<CCollider*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_BoxCollider"));
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider", pComponent });

	CBoxCollider* pBoxCollider = static_cast<CBoxCollider*>(m_pColliderCom);
	pBoxCollider->Set_Extents(0.5f, 0.5f, 0.5f);
	pBoxCollider->Set_DiffPos({ 0.f, 0.f, 0.f });
	m_pColliderCom->Set_CollisionID(COLL_OBSTACLE);


	return S_OK;
}

CCube* CCube::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pTextureProtoTag)
{
	CCube* pInstance = new CCube(pGraphicDev, pTextureProtoTag);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CCube Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CCube::Free()
{
	CGameObject::Free();
}