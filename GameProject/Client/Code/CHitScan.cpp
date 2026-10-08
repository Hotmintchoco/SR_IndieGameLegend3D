#include "pch.h"
#include "CHitScan.h"
#include "CProtoMgr.h"
#include "CLaserBuffer.h"
#include "CRenderer.h"
#include "CMonster.h"

CHitScan::CHitScan(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vStart, CMonster* pTarget, const float fDamage)
    : CGameObject(pGraphicDev), m_vStart(vStart), m_pTarget(pTarget), m_fDamage(fDamage)
{
}

CHitScan::~CHitScan()
{
}

HRESULT CHitScan::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

    if (FAILED(CGameObject::Ready_GameObject()))
        return E_FAIL;

    CalculateLength();
    
    /* TODO */
    m_pTarget->Set_Dead(true);

	return S_OK;
}

_int CHitScan::Update_GameObject(_float fTimeDelta)
{
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

    if (m_fTimeAfterBirth >= m_fLifeTime)
    {
        Set_Dead(true);
    }
    else
    {
        m_fTimeAfterBirth += fTimeDelta;
        m_iOpacity = clamp((int)((1.f - m_fTimeAfterBirth / m_fLifeTime) * 100.f), 0, 100);
    }

    /* TODO 임시 */
    m_fViewZ = 0.f;

	return iExit;
}

void CHitScan::LateUpdate_GameObject(_float fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CHitScan::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransform->Get_World());

    m_pTexture->Set_Texture(m_iOpacity);

    m_pBuffer->Render_Buffer();
}

HRESULT CHitScan::Add_Component()
{
    // Mesh
    m_pBuffer = dynamic_cast<CLaserBuffer*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Laser_Buffer"));

    if (nullptr == m_pBuffer)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", m_pBuffer });

    // Transform
    m_pTransform = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == m_pTransform)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Transform", m_pTransform });

    // Texture
    m_pTexture = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_White_Texture"));

    if (nullptr == m_pTexture)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", m_pTexture });

    return S_OK;
}

void CHitScan::CalculateLength()
{
    CTransform* pTransform = dynamic_cast<CTransform*>(m_pTarget->Get_Component(ID_DYNAMIC, L"Com_Transform"));
    
    _vec3 vTargetPos = pTransform->Get_Info_Value(INFO_POS);
    _vec3 vDisplacement = vTargetPos - m_vStart;

    D3DXVec3Normalize(&m_vDir, &vDisplacement);
    m_fLength = D3DXVec3Length(&vDisplacement);

    m_pTransform->Set_Scale(_vec3{ m_fWidth, 1.f, m_fLength });
    m_pTransform->Set_Pos(vTargetPos);

    /* 회전 정하기 */
    _vec3 vLook, vRight;
    _vec3 vUp{ 0.f, 1.f, 0.f };
    D3DXVec3Normalize(&vLook, &m_vDir);
    D3DXVec3Cross(&vRight, &vUp, &vLook);
    D3DXVec3Normalize(&vRight, &vRight);
    D3DXVec3Cross(&vUp, &vLook, &vRight);
    D3DXVec3Normalize(&vUp, &vUp);

    _matrix* pWorld = m_pTransform->Get_World();
    _vec3 vScale = m_pTransform->Get_Scale();

    _vec3 vR = vRight * vScale.x;
    _vec3 vU = vUp * vScale.y;
    _vec3 vL = vLook * vScale.z;
    memcpy(&pWorld->m[0][0], &vR, sizeof(_vec3));
    memcpy(&pWorld->m[1][0], &vU, sizeof(_vec3));
    memcpy(&pWorld->m[2][0], &vL, sizeof(_vec3));

    m_pTransform->WorldMatrixDecompose();
}

CHitScan* CHitScan::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vStart, CMonster* pTarget, const float fDamage)
{
    CHitScan* pObject = new CHitScan(pGraphicDev, vStart, pTarget, fDamage);

    if (FAILED(pObject->Ready_GameObject()))
    {
        Safe_Release(pObject);
        MSG_BOX("CHitScan Create Failed");
        return nullptr;
    }

    return pObject;
}

void CHitScan::Free()
{
    CGameObject::Free();
}
