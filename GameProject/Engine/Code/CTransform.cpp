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

void CTransform::UpdateWorldMatrix()
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
}

void CTransform::WorldMatrixDecompose()
{
	/* 실패할 수 있는 경우
	* 스케일이 0인 축 존재
	* 스케일이 음수
	*/

	// 1. 위치 & 축 벡터 (행 벡터 기준: 각 행이 Right / Up / Look / Pos)
	for (_uint i = 0; i < INFO_END; ++i)
		memcpy(&m_vInfo[i], &m_matWorld.m[i][0], sizeof(_vec3));

	// 2. 스케일 = 각 축의 길이
	m_vScale.x = D3DXVec3Length(&m_vInfo[INFO_RIGHT]);
	m_vScale.y = D3DXVec3Length(&m_vInfo[INFO_UP]);
	m_vScale.z = D3DXVec3Length(&m_vInfo[INFO_LOOK]);

	const _float fEpsilon = 1e-6f;
	if (m_vScale.x < fEpsilon || m_vScale.y < fEpsilon || m_vScale.z < fEpsilon)
		return; // 스케일 0인 축이 있으면 회전을 구할 수 없음

	// 음수 스케일(반전) 처리: 좌표계가 뒤집혔으면 x 스케일에 부호를 줌
	_vec3 vCross;
	D3DXVec3Cross(&vCross, &m_vInfo[INFO_RIGHT], &m_vInfo[INFO_UP]);
	if (D3DXVec3Dot(&vCross, &m_vInfo[INFO_LOOK]) < 0.f)
		m_vScale.x = -m_vScale.x;

	// 3. 순수 회전 축 (스케일 제거)
	_vec3 vRight = m_vInfo[INFO_RIGHT] / m_vScale.x;
	_vec3 vUp = m_vInfo[INFO_UP] / m_vScale.y;
	_vec3 vLook = m_vInfo[INFO_LOOK] / m_vScale.z;

	// 4. 오일러 각 추출 (R = Rx * Ry * Rz, UpdateWorldMatrix와 같은 순서)
	//    R._13 = -sin(y)
	//    R._23 = sin(x)cos(y), R._33 = cos(x)cos(y)
	//    R._12 = cos(y)sin(z), R._11 = cos(y)cos(z)
	_float fSinY = -vRight.z;
	fSinY = max(-1.f, min(1.f, fSinY)); // asinf 범위 보호

	_float fX, fY, fZ;
	fY = asinf(fSinY);

	if (fabsf(fSinY) < 0.9999f)
	{
		fX = atan2f(vUp.z, vLook.z);
		fZ = atan2f(vRight.y, vRight.x);
	}
	else
	{
		// 짐벌락 (y = ±90도): x와 z가 겹치므로 z를 0으로 고정
		fX = atan2f(-vLook.y, vUp.y);
		fZ = 0.f;
	}

	// UpdateWorldMatrix가 D3DXToRadian을 쓰므로 도(degree) 단위로 저장
	m_vAngle = _vec3(D3DXToDegree(fX), D3DXToDegree(fY), D3DXToDegree(fZ));
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
	UpdateWorldMatrix();

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
