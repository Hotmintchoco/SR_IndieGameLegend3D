#include "CTransform.h"

CTransform::CTransform()
	: m_vScale(1.f, 1.f, 1.f), m_vAngle(0.f, 0.f, 0.f), m_fAccumulatedTime(0.f)
{
	ZeroMemory(m_vInfo, sizeof(_vec3) * INFO_END);
	D3DXMatrixIdentity(&m_matWorld);
}

CTransform::CTransform(LPDIRECT3DDEVICE9 pGraphicDev)
	: CComponent(pGraphicDev)
	, m_vScale(1.f, 1.f, 1.f), m_vAngle(0.f, 0.f, 0.f), m_fAccumulatedTime(0.f)
{
	ZeroMemory(m_vInfo, sizeof(_vec3) * INFO_END);
	D3DXMatrixIdentity(&m_matWorld);
}

CTransform::CTransform(const CTransform& rhs)
	: CComponent(rhs)
	, m_vScale(rhs.m_vScale), m_vAngle(rhs.m_vAngle), m_fAccumulatedTime(0.f)
{
	for (_uint i = 0; i < INFO_END; ++i)
		m_vInfo[i] = rhs.m_vInfo[i];

	m_matWorld = rhs.m_matWorld;
}

CTransform::~CTransform()
{
	
}

HRESULT CTransform::Ready_Transform()
{
	D3DXMatrixIdentity(&m_matWorld);

	for (_uint i = 0; i < INFO_END; ++i)
		memcpy(&m_vInfo[i], &m_matWorld.m[i][0], sizeof(_vec3));

	return S_OK;
}

_int CTransform::Update_Component(const _float& fTimeDelta)
{
	D3DXMatrixIdentity(&m_matWorld);

	// 크기
	for (_uint i = 0; i < INFO_POS; ++i)
	{
		memcpy(&m_vInfo[i], &m_matWorld.m[i][0], sizeof(_vec3));
	}

	for (_uint i = 0; i < INFO_POS; ++i)
	{
		D3DXVec3Normalize(&m_vInfo[i], &m_vInfo[i]);
		m_vInfo[i] *= *(((_float*)&m_vScale) + i);
	}

	// 회전

	_matrix		matRot[ROT_END];

	D3DXMatrixRotationX(&matRot[ROT_X], D3DXToRadian(m_vAngle.x));
	D3DXMatrixRotationY(&matRot[ROT_Y], D3DXToRadian(m_vAngle.y));
	D3DXMatrixRotationZ(&matRot[ROT_Z], D3DXToRadian(m_vAngle.z));

	for (_uint i = 0; i < INFO_POS; ++i)
	{
		for (_uint j = 0; j < ROT_END; ++j)
		{
			D3DXVec3TransformNormal(&m_vInfo[i], &m_vInfo[i], &matRot[j]);
		}
	}

	// 월드 행렬 구성
	for (_uint i = 0; i < INFO_END; ++i)
	{
		memcpy(&m_matWorld.m[i][0], &m_vInfo[i], sizeof(_vec3));
	}


	return 0;
}

void CTransform::LateUpdate_Component()
{
}

void CTransform::Chase_Target(const _vec3* pPos, const _vec3* pLook, const _float& fSpeed, const _float& fTimeDelta)
{
	_vec3	vDir = *pPos - m_vInfo[INFO_POS];

	//if (D3DXVec3Length(&vDir) > 5.f)
	{
		m_vInfo[INFO_POS] += *D3DXVec3Normalize(&vDir, &vDir) * fSpeed * fTimeDelta;
	}

	_matrix	matScale, matRot, matTrans;

	D3DXMatrixScaling(&matScale, m_vScale.x, m_vScale.y, m_vScale.z);

	D3DXMatrixTranslation(&matTrans,
							m_vInfo[INFO_POS].x,
							m_vInfo[INFO_POS].y,
							m_vInfo[INFO_POS].z);

	matRot = *Compute_LookAtTarget(pPos, pLook);

	m_matWorld = matScale * matRot * matTrans;
}

void CTransform::Chase_Target2(const _vec3* pPos, const _vec3* pLook, const _float& fSpeed, const _float& fTimeDelta)
{
	_vec3	vDir = *pPos - m_vInfo[INFO_POS];
	
	m_fAccumulatedTime += fTimeDelta;
	if (m_fAccumulatedTime < 2.f)
	{

	}
	else
	{
		m_fAccumulatedTime = 0.f;
		m_vInfo[INFO_POS] += *D3DXVec3Normalize(&vDir, &vDir) * fSpeed * fTimeDelta;
	}

	//m_vInfo[INFO_POS] += *D3DXVec3Normalize(&vDir, &vDir) * fSpeed * fTimeDelta;

	_matrix	matScale, matRot, matTrans;

	D3DXMatrixScaling(&matScale, m_vScale.x, m_vScale.y, m_vScale.z);

	D3DXMatrixTranslation(&matTrans,
		m_vInfo[INFO_POS].x,
		m_vInfo[INFO_POS].y,
		m_vInfo[INFO_POS].z);

	matRot = *Compute_LookAtTarget(pPos, pLook);

	m_matWorld = matScale * matRot * matTrans;
}

void CTransform::LookAt_Player(const _vec3* pPos, const _vec3* pLook)
{
	_vec3	vDir = *pPos - m_vInfo[INFO_POS];

	_matrix	matScale, matRot, matTrans;

	D3DXMatrixScaling(&matScale, m_vScale.x, m_vScale.y, m_vScale.z);

	D3DXMatrixTranslation(&matTrans,
		m_vInfo[INFO_POS].x,
		m_vInfo[INFO_POS].y,
		m_vInfo[INFO_POS].z);

	matRot = *Compute_LookAtTarget(pPos, pLook);

	m_matWorld = matScale * matRot * matTrans;
}



_matrix* CTransform::Compute_LookAtTarget(const _vec3* pPos, const _vec3* pLook)
{
	_vec3 vSrc = m_vInfo[INFO_LOOK];
	_vec3 vDst = -(*pLook); //플레이어랑 평행하게 정렬
	//_vec3 vDst = (*pPos) - m_vInfo[INFO_POS]; //플레이어 시선 기준 정렬

	D3DXMATRIX	matRot;
	_vec3 vAxis = { 0.f, 1.f, 0.f };
	_vec3 vCross;

	vSrc.y = 0;
	vDst.y = 0;

	float fAngle = acosf(D3DXVec3Dot(D3DXVec3Normalize(&vSrc, &vSrc), D3DXVec3Normalize(&vDst, &vDst)));

	if (D3DXVec3Dot(D3DXVec3Cross(&vCross, &vSrc, &vDst), &vAxis) < 0.f)
		fAngle *= -1;

	return D3DXMatrixRotationAxis(&matRot, &m_vInfo[INFO_UP], fAngle);
}

CTransform* CTransform::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTransform* pTransform = new CTransform(pGraphicDev);

	if (FAILED(pTransform->Ready_Transform()))
	{
		Safe_Release(pTransform);
		MSG_BOX("Transform Create Failed");
		return nullptr;
	}

	return pTransform;
}

CComponent* CTransform::Clone()
{
	return new CTransform(*this);
}

void CTransform::Free()
{
	CComponent::Free();
}
