#include "pch.h"
#include "CBow.h"
#include "CImGuiTool.h"
#include "CProtoMgr.h"
#include "CDefaultBullet.h"
#include "CSoundMgr.h"
#include "CManagement.h"
#include "CRoomLayer.h"
#include "CVoxelBuffer.h"
#include "CRenderer.h"
#include "CArrow.h"
#include "CBombardArrowSpawner.h"
#include "Client_Enum.h"
#include "Client_Struct.h"

CBow::CBow(LPDIRECT3DDEVICE9 pGraphicDev)
    : CWeapon(pGraphicDev)
{
}

CBow::~CBow()
{
}

HRESULT CBow::Ready_GameObject()
{
    if (FAILED(CWeapon::Ready_GameObject()))
        return E_FAIL;

    if (FAILED(Add_Component()))
        return E_FAIL;

    m_fSpecialAtkInterval = 0.1f;

    m_tLocalFView = { {0.05f, 0.05f, 0.05f}, {35.f, -15.f, -10.f}, {0.3f, -0.4f, 0.75f} };
    m_tLocalTView = { {0.05f, 0.05f, 0.05f}, {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f} };
    m_vMuzzlePositionLocal = _vec3{ 0.0f, 5.f, 5.f };
    UpdateLocalTransform(m_tLocalFView);

    return S_OK;
}

_int CBow::Update_GameObject(_float fTimeDelta)
{
    _int iExit = CWeapon::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHATEST, this);

    if (m_bOnCharging)
    {
        m_fChargeTime += fTimeDelta;
        m_fChargeTime = clamp(m_fChargeTime, 0.f, m_fFullChargeTime);
        m_iRenderIdx = (int)ceil(m_fChargeTime / (m_fFullChargeTime / (float)m_iChargeLevel));
        m_iRenderIdx = clamp(m_iRenderIdx, 0, m_iChargeLevel);
    }
    else
    {
        m_iRenderIdx = 0;
    }

    return iExit;
}

void CBow::LateUpdate_GameObject(_float fTimeDelta)
{
    CWeapon::LateUpdate_GameObject(fTimeDelta);
}

void CBow::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

    m_pTextureCom[m_iRenderIdx]->Set_Texture(0);
    
    m_pBufferCom[m_iRenderIdx]->Render_Buffer();

    // RenderEditorPanel();
}

void CBow::RenderEditorPanel()
{
    bool bTransformUpdated = false;

    ImGui::Begin("Gun");

    ImGui::SeparatorText("Transform");
    bTransformUpdated |= ImGui::DragFloat3("Scale", &m_tLocalFView.vPosition.x, 0.01f, 0.001f, 100.f);
    bTransformUpdated |= ImGui::DragFloat3("Position", &m_tLocalFView.vPosition.x, 0.01f);
    bTransformUpdated |= ImGui::DragFloat3("Rotation", &m_tLocalFView.vPosition.x, 0.5f, -360.f, 360.f);

    ImGui::SeparatorText("Animation");
    ImGui::DragFloat("Move Cycle", &m_fMoveAnimationFrequency, 0.01f, 0.05f, 5.f, "%.2f s");
    ImGui::DragFloat("Horizontal Move", &m_fHorizontalMove, 0.001f, 0.f, 1.f);
    ImGui::DragFloat("Quadratic A", &m_fQuadraticA, 0.001f, 0.f, 1.f);
    ImGui::DragFloat("Max Recoil Angle", &m_fMaxRecoilAngle, 0.5f, -90.f, 0.f);
    ImGui::DragFloat("Recoil Damping", &m_fRecoilDamping, 0.05f, 0.f, 20.f);

    ImGui::End();

    if (bTransformUpdated)
    {
        UpdateLocalTransform(m_tLocalFView);
    }
}

TWeaponOutput CBow::DefaultAttack(EInputState ePri, EInputState eSec)
{
    switch (eSec)
    {
    case EInputState::Pressed:
    {
        m_bOnCharging = true;
        return { false, EWeaponAnimEvent::BOW_CHARGE_START };
        break;
    }
    case EInputState::Released:
    {
        ShootArrow();

        m_bOnCharging = false;
        m_fChargeTime = 0.f;
        return { true, EWeaponAnimEvent::BOW_CHARGE_END };
        break;
    }
    default:
        break;
    }

    return { false, EWeaponAnimEvent::NONE };
}

TWeaponOutput CBow::SpecialAttack(EInputState ePri, EInputState eSec)
{
    return { false, EWeaponAnimEvent::NONE };
}

TWeaponOutput CBow::StartUltimateAttack(EInputState ePri, EInputState eSec)
{
    CBombardArrowSpawner* pSpawner = CBombardArrowSpawner::Create(m_pGraphicDev);
    if (!pSpawner) return { false, EWeaponAnimEvent::NONE };

    CManagement::GetInstance()->GetCurrentScene()->Add_GameObject(L"BombardArrowSpawner", pSpawner);

    return { true, EWeaponAnimEvent::NONE };
}

TWeaponOutput CBow::UpdateUltimateAttack(EInputState ePri, EInputState eSec)
{
    return { false, EWeaponAnimEvent::NONE };
}

TWeaponOutput CBow::EndUltimateAttack(EInputState ePri, EInputState eSec)
{
    return { false, EWeaponAnimEvent::NONE };
}

void CBow::ShootArrow()
{
    _vec3 vDir = m_vBulletTo - m_vBulletFrom;
    D3DXVec3Normalize(&vDir, &vDir);

    CProjectile* pProjectile = CArrow::Create(m_pGraphicDev, m_vBulletFrom, vDir, m_fChargeTime / m_fFullChargeTime);
    CManagement::GetInstance()->GetCurrentScene()->Add_GameObject(L"Projectile_" + to_wstring(pProjectile->GetProjectileID()), pProjectile);
}

HRESULT CBow::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Mesh
    pComponent = m_pBufferCom[0] = dynamic_cast<CVoxelBuffer*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Bow_0_Vertex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer0", pComponent });

    pComponent = m_pBufferCom[1] = dynamic_cast<CVoxelBuffer*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Bow_1_Vertex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer1", pComponent });

    pComponent = m_pBufferCom[2] = dynamic_cast<CVoxelBuffer*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Bow_2_Vertex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer2", pComponent });

    pComponent = m_pBufferCom[3] = dynamic_cast<CVoxelBuffer*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Bow_3_Vertex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer3", pComponent });

    // Texture
    pComponent = m_pTextureCom[0] = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Bow_0_Texture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture0", pComponent });

    pComponent = m_pTextureCom[1] = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Bow_1_Texture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture1", pComponent });

    pComponent = m_pTextureCom[2] = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Bow_2_Texture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture2", pComponent });

    pComponent = m_pTextureCom[3] = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Bow_3_Texture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture3", pComponent });

    return S_OK;
}

CBow* CBow::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CBow* pWeapon = new CBow(pGraphicDev);

    if (FAILED(pWeapon->Ready_GameObject()))
    {
        Safe_Release(pWeapon);
        MSG_BOX("CBow Create Failed");
        return nullptr;
    }

    return pWeapon;
}

void CBow::Free()
{
    CWeapon::Free();
}
