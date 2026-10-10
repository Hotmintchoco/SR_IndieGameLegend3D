#include "pch.h"
#include "CTest1.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CTimerMgr.h"
#include "CTerrain.h"
#include "CPlayerCamera.h"

CTest1::CTest1(LPDIRECT3DDEVICE9 pGraphicDev)
    : CMonster(pGraphicDev)
{
}


CTest1::~CTest1()
{
}

HRESULT CTest1::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;
    CMonster::Ready_GameObject();

    m_pTransformCom->Set_Scale(0.5f, 0.5f, 0.5f);
    _vec3 vScale = m_pTransformCom->Get_Scale();
    m_pColliderCom->Set_Radius(vScale.x);

    _vec3 vPos = { 0.f,0.f,10.5f };
    m_pTransformCom->Set_Pos(vPos);

    m_iMaxHp = 3;
    m_iHp = m_iMaxHp;

    _vec3 vDir{0.f,0.f,-1.f};
    D3DXVec3Normalize(&vDir, &vDir);

    _vec3 vAngle;
    vAngle.x = D3DXToDegree(-asinf(vDir.y));
    vAngle.y = D3DXToDegree(atan2f(vDir.x, vDir.z));
    vAngle.z = 0.f;
    m_pTransformCom->Set_Angle(vAngle);

    return S_OK;
}

_int CTest1::Update_GameObject(_float fTimeDelta)
{
    //_matrix matView;
    //_matrix matCameraWorld;

    //m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
    //D3DXMatrixInverse(&matCameraWorld, nullptr, &matView);
    //
    //matView;
    //matCameraWorld;



    //_vec3 vPos = { 0.f, 0.f, -10.f };
    //_vec3 vLook = { 0.f, 0.f, 1.f };
    //_vec3 vUp = { 0.f, 1.f, 0.f };

    //_vec3 vAt = vPos + vLook;


    //D3DXMatrixLookAtLH(&matView, &vPos, &vAt, &vUp);

    //m_pGraphicDev->SetTransform(D3DTS_VIEW, &matView);





    //_matrix matProj;

    //D3DXMatrixPerspectiveFovLH(
    //    &matProj,
    //    D3DXToRadian(60.f), // 세로 시야각(FOV)
    //    800.f / 600.f,      // 화면 가로/세로 비율
    //    0.1f,               // Near
    //    1000.f              // Far
    //);

    //m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &matProj);









    Check_Hp(fTimeDelta);
    m_pTransformCom;
    _int    iExit = CMonster::Update_GameObject(fTimeDelta);

    Animation_Monster(fTimeDelta);

    return iExit;
}

void CTest1::LateUpdate_GameObject(_float fTimeDelta)
{
    CMonster::LateUpdate_GameObject(fTimeDelta);
}

void CTest1::Render_GameObject()
{
    if (m_bHitState == true) CMonster::Enable_HitRenderState();
    CMonster::Render_GameObject();
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pTextureCom->Set_Texture(0);
    m_pBufferCom->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
    if (m_bHitState == true) CMonster::Disable_HitRenderState();
}

void CTest1::OnCollisionEnter(COLLINFO eCollInfo)
{
    CMonster::OnCollisionEnter(eCollInfo);
}

void CTest1::Check_Hp(_float& fTimedelta)
{
    if (m_iHp <= 0)
    {
        m_bDelete = true;
    }
}

HRESULT CTest1::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_skull3Texture"));
    if (nullptr == pComponent) return E_FAIL;
    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}

CTest1* CTest1::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CTest1* pMonster = new CTest1(pGraphicDev);

    if (FAILED(pMonster->Ready_GameObject()))
    {
        Safe_Release(pMonster);
        MSG_BOX("CTest1 Create Failed");
        return nullptr;
    }

    return pMonster;
}

void CTest1::Free()
{
    CMonster::Free();
}