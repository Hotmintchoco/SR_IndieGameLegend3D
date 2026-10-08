#include "pch.h"
#include "CParticle.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CManagement.h"
#include <ctime>
#include "Client_Struct.h"
#include "CPlayerCamera.h"
#include "CClientCameraMgr.h"


CParticle::CParticle(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev), m_fFrame(0.f)
{
}


CParticle::~CParticle()
{
}

HRESULT CParticle::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    return S_OK;
}

_int CParticle::Update_GameObject(_float fTimeDelta)
{
    _int    iExit = CGameObject::Update_GameObject(fTimeDelta);
    if (!m_pBillBoardCamera)
    {
        m_pBillBoardCamera = dynamic_cast<CPlayerCamera*>(CClientCameraMgr::GetInstance()->Find_Camera(CLIENT_CAMERA_TYPE::PLAYER));
        assert(m_pBillBoardCamera);
    }

    m_fElapsedTime += fTimeDelta;

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHATEST, this);

    return iExit;
}

void CParticle::LateUpdate_GameObject(_float fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CParticle::Render_GameObject()
{
}

HRESULT CParticle::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Transform
    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    return S_OK;
}


CParticle* CParticle::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CParticle* pEffect = new CParticle(pGraphicDev);

    if (FAILED(pEffect->Ready_GameObject()))
    {
        Safe_Release(pEffect);
        MSG_BOX("CEffect Create Failed");
        return nullptr;
    }

    return pEffect;
}

void CParticle::LookAtPlayer()
{
    const TBillBoardInfo& tInfo = m_pBillBoardCamera->GetBillBoardInfo();

    _vec3   vPlayerPos; vPlayerPos = tInfo.vPosition;
    _vec3   vPlayerLook; vPlayerLook = tInfo.vLook;

    m_pTransformCom->LookAt_Player(&vPlayerPos, &vPlayerLook);
}

void CParticle::LookAtPlayer2()
{
    const TBillBoardInfo& tInfo = m_pBillBoardCamera->GetBillBoardInfo();

    _vec3   vPlayerPos; vPlayerPos = tInfo.vPosition;

    _vec3 vPos; m_pTransformCom->Get_Info(INFO_POS, &vPos);
    _vec3 vDir = vPlayerPos - vPos;
    vDir.y = 0.f;
    D3DXVec3Normalize(&vDir, &vDir);

    _vec3 vAngle;
    vAngle.x = D3DXToDegree(-asinf(vDir.y));
    vAngle.y = D3DXToDegree(atan2f(vDir.x, vDir.z));
    vAngle.z = 0.f;
    m_pTransformCom->Set_Angle(vAngle);
}

void CParticle::Free()
{
    CGameObject::Free();
}

void CParticle::Set_Pos(_vec3 vPos)
{
    m_pTransformCom->Set_Pos(vPos);
}
void CParticle::Set_Pos(_float fX, _float fY, _float fZ)
{
    m_pTransformCom->Set_Pos(fX, fY, fZ);
}

void CParticle::Set_Scale(_vec3 vPos)
{
    m_pTransformCom->Set_Scale(vPos);
}

void CParticle::Set_Scale(_float fX, _float fY, _float fZ)
{
    m_pTransformCom->Set_Scale(fX, fY, fZ);
}