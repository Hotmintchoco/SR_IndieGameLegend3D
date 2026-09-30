#include "pch.h"
#include "CBow.h"
#include "CImGuiTool.h"
#include "CProtoMgr.h"
#include "CDefaultBullet.h"
#include "CSoundMgr.h"
#include "CGameStatusMgr.h"
#include "CRoomLayer.h"
#include "CVoxelBuffer.h"
#include "CRenderer.h"
#include "CArrow.h"

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

    m_vScaleLocal = _vec3{ 0.05f, 0.05f, 0.05f };
    m_vPositionLocal = _vec3{ 0.25f, -0.4f, 0.75f };
    m_vRotationLocal = _vec3{ 45.f, -20.f, -15.f };
    m_vMuzzlePositionLocal = _vec3{ 0.0f, 5.f, 5.f };
    UpdateLocalTransform(m_vScaleLocal, m_vRotationLocal, m_vPositionLocal);

    return S_OK;
}

_int CBow::Update_GameObject(const _float& fTimeDelta)
{
    _int iExit = CWeapon::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHATEST, this);

    if (m_bOnCharging)
    {
        m_fChargeTime += fTimeDelta;
        m_iRenderIdx = (int)ceil(m_fChargeTime / (m_fFullChargeTime / (float)m_iChargeLevel));
        m_iRenderIdx = clamp(m_iRenderIdx, 0, m_iChargeLevel);
    }
    else
    {
        m_iRenderIdx = 0;
    }

    return iExit;
}

void CBow::LateUpdate_GameObject(const _float& fTimeDelta)
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

void CBow::ChargeStart()
{
    m_bOnCharging = true;
}

void CBow::ChargeEnd()
{
    if (!m_bSpecialAttackSwitchOn)
    {
        DefaultAttack();
    }

}

void CBow::RenderEditorPanel()
{
    ImGui::Begin("Bow");

    ImGui::SeparatorText("Transform");
    ImGui::DragFloat3("Scale", &m_vScaleLocal.x, 0.01f, 0.001f, 100.f);
    ImGui::DragFloat3("Position", &m_vPositionLocal.x, 0.01f);
    ImGui::DragFloat3("Rotation", &m_vRotationLocal.x, 0.5f, -360.f, 360.f);

    ImGui::SeparatorText("Animation");
    ImGui::DragFloat("Move Cycle", &m_fMoveAnimationFrequency, 0.01f, 0.05f, 5.f, "%.2f s");
    ImGui::DragFloat("Horizontal Move", &m_fHorizontalMove, 0.001f, 0.f, 1.f);
    ImGui::DragFloat("Quadratic A", &m_fQuadraticA, 0.001f, 0.f, 1.f);
    ImGui::DragFloat("Max Recoil Angle", &m_fMaxRecoilAngle, 0.5f, -90.f, 0.f);
    ImGui::DragFloat("Recoil Damping", &m_fRecoilDamping, 0.05f, 0.f, 20.f);

    ImGui::End();

    UpdateLocalTransform(m_vScaleLocal, m_vRotationLocal, m_vPositionLocal);
}

void CBow::DefaultAttack()
{
    _vec3 vDir = m_vBulletTo - m_vBulletFrom;
    D3DXVec3Normalize(&vDir, &vDir);

    CProjectile* pProjectile = CArrow::Create(m_pGraphicDev, m_vBulletFrom, vDir, m_iRenderIdx);
    cout << m_iChargeLevel;
    CGameStatusMgr::GetInstance()->GetCurrentRoomLayer()->Add_GameObject(L"Projectile_" + to_wstring(pProjectile->GetProjectileID()), pProjectile);

    m_bOnCharging = false;
    m_fChargeTime = 0.f;
}

void CBow::SpecialAttack()
{
}

void CBow::UltimateAttack()
{
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
