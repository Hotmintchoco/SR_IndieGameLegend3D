#include "CSphereCollider.h"
#include "CBoxCollider.h"
#include "CGameObject.h"
#include "CTransform.h"
#include "CProtoMgr.h"
#include "CDebugMgr.h"
#include "CRenderer.h"

CSphereCollider::CSphereCollider() 
{
	m_eColliderType = CT_SPHERE;
}

CSphereCollider::CSphereCollider(LPDIRECT3DDEVICE9 pGraphicDev)
	: CCollider(pGraphicDev)
{
	m_eColliderType = CT_SPHERE;
	/* 디버그 용이니깐 그냥 클라에서 뜯기... */
	CComponent* pComp = CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Sphere_Vertex");
	m_pDebugSphereTex = dynamic_cast<CPlyTex*>(pComp);
}

CSphereCollider::CSphereCollider(const CSphereCollider& rhs)
	: CCollider(rhs), m_pDebugSphereTex(rhs.m_pDebugSphereTex), m_tSphere(rhs.m_tSphere)
{
	if (nullptr != m_pDebugSphereTex)
		m_pDebugSphereTex->AddRef();
}

CSphereCollider::~CSphereCollider()
{
}

_bool CSphereCollider::Intersect(CCollider* pOther)
{
	if (pOther->Get_ColliderType() == CT_SPHERE)
	{
		CSphereCollider* pTargetSphere = static_cast<CSphereCollider*>(pOther);

		// 내 구(Sphere)와 상대방 구(Sphere)의 충돌 검사
		return m_tSphere.Intersects(pTargetSphere->m_tSphere);
	}

	if (pOther->Get_ColliderType() == CT_BOX)
	{
		CBoxCollider* pTargetBox = static_cast<CBoxCollider*>(pOther);
		return m_tSphere.Intersects(pTargetBox->m_tBox);
	}

	return false;
}

void CSphereCollider::Render(LPDIRECT3DDEVICE9& pGraphicDev)
{
	if (nullptr == m_pDebugSphereTex || nullptr == m_pGraphicDev)
		return;

	_matrix matScale, matTrans, matWorld;
	D3DXMatrixScaling(&matScale, m_tSphere.Radius, m_tSphere.Radius, m_tSphere.Radius);
	D3DXMatrixTranslation(&matTrans, m_tSphere.Center.x, m_tSphere.Center.y, m_tSphere.Center.z);
	matWorld = matScale * matTrans;

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);
	m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR,
		m_bIsCollided ? D3DCOLOR_XRGB(255, 0, 0) : D3DCOLOR_XRGB(0, 255, 0));

	m_pDebugSphereTex->Render_Buffer();
}

_int CSphereCollider::Update_Component(const _float& fTimeDelta)
{
	if (CDebugMgr::GetInstance()->GetShowCollider())
	{
		CRenderer::GetInstance()->Add_RenderGroup(RENDER_DEBUG_COLLIDER, this);
	}

	return 0;
}

void CSphereCollider::LateUpdate_Component()
{
	if (!m_pOwner) return;
	CTransform* pOwnerTransformCom = dynamic_cast<CTransform*>(m_pOwner->Get_Component(ID_DYNAMIC, L"Com_Transform"));

	_vec3   vOwnerPos;
	pOwnerTransformCom->Get_Info(INFO_POS, &vOwnerPos);
	m_tSphere.Center = { vOwnerPos.x, vOwnerPos.y, vOwnerPos.z };
}

CCollider* CSphereCollider::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	return new CSphereCollider(pGraphicDev);
}

CComponent* CSphereCollider::Clone()
{
	return new CSphereCollider(*this);
}

void CSphereCollider::Set_Radius(const _float& fRadius)
{
	m_fRadius = fRadius;
	m_tSphere.Radius = fRadius;
}

void CSphereCollider::Free()
{
	Safe_Release(m_pDebugSphereTex);
	CCollider::Free();
}
